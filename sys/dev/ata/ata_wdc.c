/*	$NetBSD: ata_wdc.c,v 1.21 1999/08/09 09:43:11 bouyer Exp $	*/

/*
 * Copyright (c) 1998 Manuel Bouyer.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum, by Onno van der Linden and by Manuel Bouyer.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/device.h>
#include <sys/disklabel.h>
#include <sys/syslog.h>
#include <sys/proc.h>

#include <machine/intr.h>
#include <machine/bus.h>

#include <dev/ata/atareg.h>
#include <dev/ata/atavar.h>
#include <dev/ic/wdcreg.h>
#include <dev/ic/wdcvar.h>
#include <dev/ata/wdvar.h>

#define DEBUG_INTR   0x01
#define DEBUG_XFERS  0x02
#define DEBUG_STATUS 0x04
#define DEBUG_FUNCS  0x08
#define DEBUG_PROBE  0x10

#ifdef WDCDEBUG
int wdcdebug_wd_mask = 0x1f;
#define WDCDEBUG_PRINT(args, level) \
	if (wdcdebug_wd_mask & (level)) \
		printf args
#else
#define WDCDEBUG_PRINT(args, level)
#endif

#define ATA_DELAY 10000 /* 10s for a drive I/O */

#ifdef __OpenBSD__
struct cfdriver wdc_cd = {
	NULL, "wdc", DV_DULL
};
#endif

void  wdc_ata_bio_start  __P((struct channel_softc *,struct wdc_xfer *));
void  _wdc_ata_bio_start  __P((struct channel_softc *,struct wdc_xfer *));
int   wdc_ata_bio_intr   __P((struct channel_softc *, struct wdc_xfer *, int));
void  wdc_ata_bio_kill_xfer __P((struct channel_softc *,struct wdc_xfer *));
void  wdc_ata_bio_done   __P((struct channel_softc *, struct wdc_xfer *)); 
int   wdc_ata_ctrl_intr __P((struct channel_softc *, struct wdc_xfer *, int));
int   wdc_ata_err __P((struct ata_drive_datas *, struct ata_bio *));
#define WDC_ATA_NOERR 0x00 /* Drive doesn't report an error */
#define WDC_ATA_RECOV 0x01 /* There was a recovered error */
#define WDC_ATA_ERR   0x02 /* Drive reports an error */

/*
 * Handle block I/O operation. Return WDC_COMPLETE, WDC_QUEUED, or
 * WDC_TRY_AGAIN. Must be called at splio().
 */
int
wdc_ata_bio(drvp, ata_bio)
	struct ata_drive_datas *drvp;
	struct ata_bio *ata_bio;
{
	struct wdc_xfer *xfer;
	struct channel_softc *chp = drvp->chnl_softc;

	xfer = wdc_get_xfer(WDC_NOSLEEP);
	if (xfer == NULL)
		return WDC_TRY_AGAIN;
	if (ata_bio->flags & ATA_POLL)
		xfer->c_flags |= C_POLL;
#if !defined(PMON)||defined(IDE_DMA)
/*	if ((drvp->drive_flags & (DRIVE_DMA | DRIVE_UDMA)) &&
	    (ata_bio->flags & ATA_SINGLE) == 0)*/
		xfer->c_flags |= C_DMA;
#endif
	xfer->drive = drvp->drive;
	xfer->cmd = ata_bio;
	xfer->databuf = ata_bio->databuf;
	xfer->c_bcount = ata_bio->bcount;
	xfer->c_start = wdc_ata_bio_start;
	xfer->c_intr = wdc_ata_bio_intr;
	xfer->c_kill_xfer = wdc_ata_bio_kill_xfer;
	wdc_exec_xfer(chp, xfer);
	return (ata_bio->flags & ATA_ITSDONE) ? WDC_COMPLETE : WDC_QUEUED;
}

void
wdc_ata_bio_start(chp, xfer)
	struct channel_softc *chp;
	struct wdc_xfer *xfer;
{
#if !defined(PMON)||defined(IDE_DMA)
	struct ata_bio *ata_bio = xfer->cmd;
	WDCDEBUG_PRINT(("wdc_ata_bio_start %s:%d:%d\n",
	    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive),
	    DEBUG_XFERS);

	/* start timeout machinery */
	if ((ata_bio->flags & ATA_POLL) == 0)
		timeout(wdctimeout, chp, ATA_DELAY / 1000 * hz);
#endif /*PMON*/
	_wdc_ata_bio_start(chp, xfer);
}

void
_wdc_ata_bio_start(chp, xfer)
	struct channel_softc *chp;
	struct wdc_xfer *xfer;
{
	struct ata_bio *ata_bio = xfer->cmd;
	struct ata_drive_datas *drvp = &chp->ch_drive[xfer->drive];
	u_int16_t cyl;
	u_int8_t head1, head, sect, cmd = 0;
	u_int8_t LBA1,LBA2,LBA3,LBA4,LBA5,LBA6,precomp;
	int nblks;
	int ata_delay;
#if !defined(PMON) || defined(IDE_DMA)
	int dma_flags = 0;
#endif
	
	WDCDEBUG_PRINT(("_wdc_ata_bio_start %s:%d:%d\n",
	    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive),
	    DEBUG_INTR | DEBUG_XFERS);
	/* Do control operations specially. */
	if (drvp->state < READY) {
		/*
		 * Actually, we want to be careful not to mess with the control
		 * state if the device is currently busy, but we can assume
		 * that we never get to this point if that's the case.
		 */
		/* at this point, we should only be in RECAL state */
		if (drvp->state != RECAL) {
			printf("%s:%d:%d: bad state %d in _wdc_ata_bio_start\n",
			    chp->wdc->sc_dev.dv_xname, chp->channel,
			    xfer->drive, drvp->state);
			panic("_wdc_ata_bio_start: bad state");
		}
		xfer->c_intr = wdc_ata_ctrl_intr;
		CHP_WRITE_REG(chp, wdr_sdh, WDSD_IBM | (xfer->drive << 4));
		if (wdcwait(chp, WDCS_DRDY, WDCS_DRDY, ATA_DELAY) != 0)
			goto timeout;
		wdccommandshort(chp, xfer->drive, WDCC_RECAL);
		drvp->state = RECAL_WAIT;
		if ((ata_bio->flags & ATA_POLL) == 0) {
			chp->ch_flags |= WDCF_IRQ_WAIT;
		} else {
			/* Wait for at last 400ns for status bit to be valid */
			DELAY(1);
			wdc_ata_ctrl_intr(chp, xfer, 0);
		}
		return;
	}

#if !defined(PMON) || defined(IDE_DMA)
	if (xfer->c_flags & C_DMA) {
		dma_flags = (ata_bio->flags & ATA_READ) ?  WDC_DMA_READ : 0;
		dma_flags |= (ata_bio->flags & ATA_POLL) ?  WDC_DMA_POLL : 0;
	}
#endif
	if (ata_bio->flags & ATA_SINGLE)
		ata_delay = ATA_DELAY;
	else
		ata_delay = ATA_DELAY;
 again:
	/*
	 *
	 * When starting a multi-sector transfer, or doing single-sector
	 * transfers...
	 */
	if (xfer->c_skip == 0 || (ata_bio->flags & ATA_SINGLE) != 0) {
		if (ata_bio->flags & ATA_SINGLE)
			nblks = 1;
		else 
			nblks = xfer->c_bcount / ata_bio->lp->d_secsize;
#ifdef DEBUG_IDE
		printf("nblks=%d\n",nblks);
#endif
		/* Check for bad sectors and adjust transfer, if necessary. */
		if ((ata_bio->lp->d_flags & D_BADSECT) != 0) {
			long blkdiff;
			int i;
			for (i = 0; (blkdiff = ata_bio->badsect[i]) != -1;
			     i++) {
#ifdef DEBUG_IDE
				printf("blkdiff=%ld\n",blkdiff);
                                printf("ata_bio->blkno=%lld\n",ata_bio->blkno);
#endif
				blkdiff -= ata_bio->blkno;
				if (blkdiff < 0)
					continue;
				if (blkdiff == 0) {
					/* Replace current block of transfer. */
#ifdef DEBUG_IDE
       		 		        printf("Replace current block of transfer.\n");
#endif
					ata_bio->blkno =
					    ata_bio->lp->d_secperunit -
					    ata_bio->lp->d_nsectors - i - 1;
				}
				if (blkdiff < nblks) {
#ifdef DEBUG_IDE
					printf("Bad block inside transfer.\n");
#endif		
					/* Bad block inside transfer. */
					ata_bio->flags |= ATA_SINGLE;
					nblks = 1;
				}
				break;
			}
			/* Transfer is okay now. */
		}
		if (ata_bio->flags & ATA_LBA) {
#ifdef DEBUG_IDE
			printf("mov right\n");
#endif
#ifdef DEBUG_IDE
			printf("ata_bio->blkno=%llx\n",ata_bio->blkno);
#endif
			if(!(drvp->drive_flags & DRIVE_LBA48)){
				sect = (ata_bio->blkno >> 0) & 0xff;
				cyl = (ata_bio->blkno >> 8) & 0xffff;
				head = (ata_bio->blkno >> 24) & 0x0f;
				head |= WDSD_LBA;
			}
			else
			{
				LBA1 = (ata_bio->blkno >> 0) & 0xff;
                LBA2 = (ata_bio->blkno >> 8) & 0xff;
                LBA3 = (ata_bio->blkno >> 16) & 0xff;
                LBA4 = (ata_bio->blkno >> 24) & 0xff;
                LBA5 = (ata_bio->blkno >> 32) & 0xff;
                LBA6 = (ata_bio->blkno >> 40) & 0xff;
                head1 = (ata_bio->blkno >> 44) & 0x0f;
                head1 |= WDSD_LBA;
#ifdef DEBUG_IDE
                printf("LBA1=%x\n",LBA1);
                printf("LBA2=%x\n",LBA2);
                printf("LBA3=%x\n",LBA3);
                printf("LBA4=%x\n",LBA4);
                printf("LBA5=%x\n",LBA5);
                printf("LBA6=%x\n",LBA6);
                printf("head1=%x\n",head1);
#endif
			}
		} else {
			int blkno = ata_bio->blkno;
#ifdef DEBUG_IDE
			printf("not move right\n");
#endif
			sect = blkno % ata_bio->lp->d_nsectors;
			sect++;    /* Sectors begin with 1, not 0. */
			blkno /= ata_bio->lp->d_nsectors;
			head = blkno % ata_bio->lp->d_ntracks;
			blkno /= ata_bio->lp->d_ntracks;
			cyl = blkno;
			head |= WDSD_CHS;
		}
#if !defined(PMON) || defined(IDE_DMA)
		if (xfer->c_flags & C_DMA) {
			ata_bio->nblks = nblks;
#ifdef DEBUG_IDE
			printf("1.ata_bio->nblks=%d,nblks=%d\n",ata_bio->nblks,nblks);
#endif
			ata_bio->nbytes = xfer->c_bcount;
#ifdef DEBUG_IDE
			printf("ata_bio->nbytes=%d\n",ata_bio->nbytes);
#endif
			if(drvp->drive_flags & DRIVE_LBA48)
				 cmd = (ata_bio->flags & ATA_READ) ? WDCC_READDMA_EXT : WDCC_WRITEDMA_EXT;
			else
				cmd = (ata_bio->flags & ATA_READ) ? WDCC_READDMA : WDCC_WRITEDMA;
			nblks = ata_bio->nblks;
#ifdef DEBUG_IDE
			 printf("2.ata_bio->nblks=%d,nblks=%d\n",ata_bio->nblks,nblks);
#endif
	    		/* Init the DMA channel. */
			if ((*chp->wdc->dma_init)(chp->wdc->dma_arg,
			    chp->channel, xfer->drive,
			    (char *)xfer->databuf + xfer->c_skip, 
			    ata_bio->nbytes, dma_flags) != 0) {
				ata_bio->error = ERR_DMA;
				ata_bio->r_error = 0;
				wdc_ata_bio_done(chp, xfer);
				return;
			}
			/* Initiate command */
			CHP_WRITE_REG(chp, wdr_sdh, 
			    WDSD_IBM | (xfer->drive << 4));
			if (wait_for_ready(chp, ata_delay) < 0)
				goto timeout;
			if(drvp->drive_flags & DRIVE_LBA48){
				precomp=(ata_bio->lp->d_type == DTYPE_ST506)?ata_bio->lp->d_precompcyl/4:0;
                CHP_WRITE_REG(chp, 1, 0);
                CHP_WRITE_REG(chp, wdr_seccnt, 0);
            	CHP_WRITE_REG(chp, wdr_sector, LBA4);
            	CHP_WRITE_REG(chp, wdr_cyl_lo, LBA5);
            	CHP_WRITE_REG(chp, wdr_cyl_hi, LBA6);
                DELAY(10);
            	CHP_WRITE_REG(chp, 1, 0);
            	CHP_WRITE_REG(chp, wdr_seccnt, nblks);
            	CHP_WRITE_REG(chp, wdr_sector, LBA1);
                CHP_WRITE_REG(chp, wdr_cyl_lo, LBA2);
                CHP_WRITE_REG(chp, wdr_cyl_hi, LBA3);
                CHP_WRITE_REG(chp, wdr_sdh, WDSD_IBM | (xfer->drive << 4) | head1);
                DELAY(10);
                CHP_WRITE_REG(chp, wdr_command, cmd);
			}
			else
				wdccommand(chp, xfer->drive, cmd, cyl, head, sect, nblks, 0);
			

			/* start the DMA channel */
			(*chp->wdc->dma_start)(chp->wdc->dma_arg,
			    chp->channel, xfer->drive, dma_flags);
			/* wait for irq */
			goto intr;
		} /* else not DMA */
#endif
#ifdef DEBUG_IDE
		printf("!defined(PMON) || defined(IDE_DMA)\n");
#endif
		ata_bio->nblks = min(nblks, ata_bio->multi);
#ifdef DEBUG_IDE
		printf("ata_bio->nblks=%d\n",ata_bio->nblks);
#endif
		ata_bio->nbytes = ata_bio->nblks * ata_bio->lp->d_secsize;
		if (ata_bio->nblks > 1 && (ata_bio->flags & ATA_SINGLE) == 0) {
			cmd = (ata_bio->flags & ATA_READ) ?
			    WDCC_READMULTI : WDCC_WRITEMULTI;
		} else {
			if(!(drvp->drive_flags & DRIVE_LBA48))
				cmd = (ata_bio->flags & ATA_READ) ? WDCC_READ : WDCC_WRITE;
			else
				cmd = (ata_bio->flags & ATA_READ) ? WDCC_READ_EXT : WDCC_WRITE_EXT;
		}
		/* Initiate command! */
		CHP_WRITE_REG(chp, wdr_sdh, WDSD_IBM | (xfer->drive << 4));
		if (wait_for_ready(chp, ata_delay) < 0)
			goto timeout;
		
		if(!(drvp->drive_flags & DRIVE_LBA48)){
			wdccommand(chp, xfer->drive, cmd, cyl,
		    		head, sect, nblks, 
		    		(ata_bio->lp->d_type == DTYPE_ST506) ?
		    		ata_bio->lp->d_precompcyl / 4 : 0);
		}
		else{
			precomp=(ata_bio->lp->d_type == DTYPE_ST506)?ata_bio->lp->d_precompcyl/4:0;
	   	 	CHP_WRITE_REG(chp, wdr_sdh, WDSD_IBM | (xfer->drive << 4) | head1);
		 	CHP_WRITE_REG(chp, wdr_seccnt, 0);
            CHP_WRITE_REG(chp, wdr_sector, LBA4);
            CHP_WRITE_REG(chp, wdr_cyl_lo, LBA5);
            CHP_WRITE_REG(chp, wdr_cyl_hi, LBA6);
		 	DELAY(1);
            CHP_WRITE_REG(chp, wdr_precomp, precomp);
			CHP_WRITE_REG(chp, wdr_seccnt, nblks);
			CHP_WRITE_REG(chp, wdr_sector, LBA1);
			CHP_WRITE_REG(chp, wdr_cyl_lo, LBA2);
			CHP_WRITE_REG(chp, wdr_cyl_hi, LBA3);
			CHP_WRITE_REG(chp, wdr_sdh, WDSD_IBM | (xfer->drive << 4) | head1);
			CHP_WRITE_REG(chp, wdr_command, cmd);
		}
#ifdef DEBUG_IDE
		 printf("cmd=%x\n",cmd);
#endif
	} else if (ata_bio->nblks > 1) {
		/* The number of blocks in the last stretch may be smaller. */
#ifdef DEBUG_IDE
		printf("The number of blocks in the last stretch may be smaller.\n");
#endif
		nblks = xfer->c_bcount / ata_bio->lp->d_secsize;
		if (ata_bio->nblks > nblks) {
			ata_bio->nblks = nblks;
			ata_bio->nbytes = xfer->c_bcount;
		}
	}
	/* If this was a write and not using DMA, push the data. */
	if ((ata_bio->flags & ATA_READ) == 0) {
		if (wait_for_drq(chp, ata_delay) != 0) {
			printf("%s:%d:%d: timeout waiting for DRQ, "
			    "st=0x%02x, err=0x%02x\n",
			    chp->wdc->sc_dev.dv_xname, chp->channel,
			    xfer->drive, chp->ch_status, chp->ch_error);
			if (wdc_ata_err(drvp, ata_bio) != WDC_ATA_ERR)
				ata_bio->error = TIMEOUT;
			wdc_ata_bio_done(chp, xfer);
			return;
		}
		if (wdc_ata_err(drvp, ata_bio) == WDC_ATA_ERR) {
			wdc_ata_bio_done(chp, xfer);
			return;
		}
		wdc_output_bytes(drvp, (char *)xfer->databuf + xfer->c_skip,
		    ata_bio->nbytes);
	}

#if !defined(PMON) || defined(IDE_DMA)
 intr:	/* Wait for IRQ (either real or polled) */
#endif
	if ((ata_bio->flags & ATA_POLL) == 0) {
		chp->ch_flags |= WDCF_IRQ_WAIT;
	} else {
		/* Wait for at last 400ns for status bit to be valid */
		delay(1);
		wdc_ata_bio_intr(chp, xfer, 0);
		if ((ata_bio->flags & ATA_ITSDONE) == 0)
			goto again;
	}
	return;
 timeout:
	printf("%s:%d:%d: not ready, st=0x%02x, err=0x%02x\n",
	    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive,
	    chp->ch_status, chp->ch_error);
	if (wdc_ata_err(drvp, ata_bio) != WDC_ATA_ERR)
		ata_bio->error = TIMEOUT;
	wdc_ata_bio_done(chp, xfer);
	return;
}

int
wdc_ata_bio_intr(chp, xfer, irq)
	struct channel_softc *chp;
	struct wdc_xfer *xfer;
	int irq;
{
	struct ata_bio *ata_bio = xfer->cmd;
	struct ata_drive_datas *drvp = &chp->ch_drive[xfer->drive];
	int drv_err;
#if !defined(PMON) || defined(IDE_DMA)
	int dma_flags = 0;
#endif

	WDCDEBUG_PRINT(("wdc_ata_bio_intr %s:%d:%d\n",
	    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive),
	    DEBUG_INTR | DEBUG_XFERS);


	/* Is it not a transfer, but a control operation? */
	if (drvp->state < READY) {
		printf("%s:%d:%d: bad state %d in wdc_ata_bio_intr\n",
		    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive,
		    drvp->state);
		panic("wdc_ata_bio_intr: bad state\n");
	}

#if !defined(PMON)||defined(IDE_DMA)
	if (xfer->c_flags & C_DMA) {
		dma_flags = (ata_bio->flags & ATA_READ) ?  WDC_DMA_READ : 0;
		dma_flags |= (ata_bio->flags & ATA_POLL) ?  WDC_DMA_POLL : 0;
	}
#endif

	/*
	 * if we missed an interrupt in a PIO transfer, reset and restart.
	 * Don't try to continue transfer, we may have missed cycles.
	 */
	if ((xfer->c_flags & (C_TIMEOU | C_DMA)) == C_TIMEOU) {
		ata_bio->error = TIMEOUT;
		wdc_ata_bio_done(chp, xfer);
		return 1;
	}

	/* Ack interrupt done by wait_for_unbusy */
	if (wait_for_unbusy(chp,
	    (irq == 0) ? ATA_DELAY : 0) < 0) {
		if (irq && (xfer->c_flags & C_TIMEOU) == 0)
			return 0; /* IRQ was not for us */
		printf("%s:%d:%d: device timeout, c_bcount=%d, c_skip%d\n",
		    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive,
		    xfer->c_bcount, xfer->c_skip);
#if !defined(PMON)||defined(IDE_DMA)
		/* if we were using DMA, turn off DMA channel */
		if (xfer->c_flags & C_DMA) {
			(*chp->wdc->dma_finish)(chp->wdc->dma_arg,
			    chp->channel, xfer->drive, dma_flags);
			drvp->n_dmaerrs++;
		}
#endif
		ata_bio->error = TIMEOUT;
		wdc_ata_bio_done(chp, xfer);
		return 1;
	}
	
	drv_err = wdc_ata_err(drvp, ata_bio);

#if !defined(PMON)||defined(IDE_DMA)
	/* If we were using DMA, Turn off the DMA channel and check for error */
	if (xfer->c_flags & C_DMA) {
		if (ata_bio->flags & ATA_POLL) {
			/*
			 * IDE drives deassert WDCS_BSY before transfer is
			 * complete when using DMA. Polling for DRQ to deassert
			 * is not enouth DRQ is not required to be
			 * asserted for DMA transfers, so poll for DRDY.
			 */
			if (wdcwait(chp, WDCS_DRDY | WDCS_DRQ, WDCS_DRDY,
			    ATA_DELAY) < 0) {
				printf("%s:%d:%d: polled transfer timed out "
				    "(st=0x%x)\n", chp->wdc->sc_dev.dv_xname,
				    chp->channel, xfer->drive, chp->ch_status);
				ata_bio->error = TIMEOUT;
				drv_err = WDC_ATA_ERR;
			}
		}
		if ((*chp->wdc->dma_finish)(chp->wdc->dma_arg,
		    chp->channel, xfer->drive, dma_flags) != 0) {
			if (drv_err != WDC_ATA_ERR) {
				ata_bio->error = ERR_DMA;
				drv_err = WDC_ATA_ERR;
			}
		}
		if (chp->ch_status & WDCS_DRQ) {
			if (drv_err != WDC_ATA_ERR) {
				printf("%s:%d:%d: intr with DRQ (st=0x%x)\n",
				    chp->wdc->sc_dev.dv_xname, chp->channel,
				    xfer->drive, chp->ch_status);
				ata_bio->error = TIMEOUT;
				drv_err = WDC_ATA_ERR;
			}
		}
		if (drv_err != WDC_ATA_ERR)
			goto end;
		drvp->n_dmaerrs++;
	}
#endif

	/* if we had an error, end */
	if (drv_err == WDC_ATA_ERR) {
		wdc_ata_bio_done(chp, xfer);
		return 1;
	}

	/* If this was a read and not using DMA, fetch the data. */
	if ((ata_bio->flags & ATA_READ) != 0) {
		if ((chp->ch_status & WDCS_DRQ) != WDCS_DRQ) {
			printf("%s:%d:%d: read intr before drq\n",
			    chp->wdc->sc_dev.dv_xname, chp->channel,
			    xfer->drive);
			ata_bio->error = TIMEOUT;
			wdc_ata_bio_done(chp, xfer);
			return 1;
		}
		wdc_input_bytes(drvp, (char *)xfer->databuf + xfer->c_skip,
		    ata_bio->nbytes);
	}
#if !defined(PMON)||defined(IDE_DMA)
end:
#endif
	ata_bio->blkno += ata_bio->nblks;
	ata_bio->blkdone += ata_bio->nblks;
	xfer->c_skip += ata_bio->nbytes;
	xfer->c_bcount -= ata_bio->nbytes;
	/* See if this transfer is complete. */
	if (xfer->c_bcount > 0) {
		if ((ata_bio->flags & ATA_POLL) == 0) {
			/* Start the next operation */
			_wdc_ata_bio_start(chp, xfer);
		} else {
			/* Let _wdc_ata_bio_start do the loop */
			return 1;
		}
	} else { /* Done with this transfer */
		ata_bio->error = NOERROR;
		wdc_ata_bio_done(chp, xfer);
	}
	return 1;
}


void
wdc_ata_bio_kill_xfer(chp, xfer)
	struct channel_softc *chp;
	struct wdc_xfer *xfer;
{
	struct ata_bio *ata_bio = xfer->cmd;

#if !defined(PMON)||defined(IDE_DMA)
	untimeout(wdctimeout, chp);
#endif
	/* remove this command from xfer queue */
	wdc_free_xfer(chp, xfer);

	ata_bio->flags |= ATA_ITSDONE;
	ata_bio->error = ERR_NODEV;
	ata_bio->r_error = WDCE_ABRT;
	if ((ata_bio->flags & ATA_POLL) == 0) {
		WDCDEBUG_PRINT(("wdc_ata_done: wddone\n"), DEBUG_XFERS);
		wddone(ata_bio->wd);
	}
}

void
wdc_ata_bio_done(chp, xfer)
	struct channel_softc *chp;
	struct wdc_xfer *xfer;
{
	struct ata_bio *ata_bio = xfer->cmd;
	int drive = xfer->drive;
	struct ata_drive_datas *drvp = &chp->ch_drive[drive];

	WDCDEBUG_PRINT(("wdc_ata_bio_done %s:%d:%d: flags 0x%x\n",
	    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive, 
	    (u_int)xfer->c_flags),
	    DEBUG_XFERS);

#if !defined(PMON)||defined(IDE_DMA)
	untimeout(wdctimeout, chp);
#endif
	if (ata_bio->error == NOERROR)
		drvp->n_dmaerrs = 0;
	else if (drvp->n_dmaerrs >= NERRS_MAX) {
		wdc_downgrade_mode(drvp);
	}

	/* feed back residual bcount to our caller */
	ata_bio->bcount = xfer->c_bcount;

	/* remove this command from xfer queue */
	wdc_free_xfer(chp, xfer);

	ata_bio->flags |= ATA_ITSDONE;
	if ((ata_bio->flags & ATA_POLL) == 0) {
		WDCDEBUG_PRINT(("wdc_ata_done: wddone\n"), DEBUG_XFERS);
		wddone(ata_bio->wd);
	}
	WDCDEBUG_PRINT(("wdcstart from wdc_ata_done, flags 0x%x\n",
	    chp->ch_flags), DEBUG_XFERS);
	wdcstart(chp);
}

/*
 * Implement operations needed before read/write.
 */
int
wdc_ata_ctrl_intr(chp, xfer, irq)
	struct channel_softc *chp;
	struct wdc_xfer *xfer;
	int irq;
{
	struct ata_bio *ata_bio = xfer->cmd;
	struct ata_drive_datas *drvp = &chp->ch_drive[xfer->drive];
	char *errstring = NULL;
	int delay = (irq == 0) ? ATA_DELAY : 0;

	WDCDEBUG_PRINT(("wdc_ata_ctrl_intr: state %d\n", drvp->state),
	    DEBUG_FUNCS);

again:
	switch (drvp->state) {
	case RECAL:    /* Should not be in this state here */
		panic("wdc_ata_ctrl_intr: state==RECAL");
		break;

	case RECAL_WAIT:
		errstring = "recal";
		if (wdcwait(chp, WDCS_DRDY, WDCS_DRDY, delay))
			goto timeout;
		if (chp->ch_status & (WDCS_ERR | WDCS_DWF))
			goto error;
	/* fall through */

	case PIOMODE:
		/* Don't try to set modes if controller can't be adjusted */
		if ((chp->wdc->cap & WDC_CAPABILITY_MODE) == 0)
			goto geometry;
		/* Also don't try if the drive didn't report its mode */
		if ((drvp->drive_flags & DRIVE_MODE) == 0)
			goto geometry;
		wdccommand(chp, drvp->drive, SET_FEATURES, 0, 0, 0,
		    0x08 | drvp->PIO_mode, WDSF_SET_MODE);
		drvp->state = PIOMODE_WAIT;
		break;

	case PIOMODE_WAIT:
		errstring = "piomode";
		if (wdcwait(chp, WDCS_DRDY, WDCS_DRDY, delay))
			goto timeout;
		if (chp->ch_status & (WDCS_ERR | WDCS_DWF))
			goto error;
	/* fall through */

	case DMAMODE:
		if (drvp->drive_flags & DRIVE_UDMA) {
			wdccommand(chp, drvp->drive, SET_FEATURES, 0, 0, 0,
			    0x40 | drvp->UDMA_mode, WDSF_SET_MODE);
		} else if (drvp->drive_flags & DRIVE_DMA) {
			wdccommand(chp, drvp->drive, SET_FEATURES, 0, 0, 0,
			    0x20 | drvp->DMA_mode, WDSF_SET_MODE);
		} else {
			goto geometry;
		}	
		drvp->state = DMAMODE_WAIT;
		break;
	case DMAMODE_WAIT:
		errstring = "dmamode";
		if (wdcwait(chp, WDCS_DRDY, WDCS_DRDY, delay))
			goto timeout;
		if (chp->ch_status & (WDCS_ERR | WDCS_DWF))
			goto error;
	/* fall through */

	case GEOMETRY:
	geometry:
		if (ata_bio->flags & ATA_LBA)
			goto multimode;
		wdccommand(chp, xfer->drive, WDCC_IDP,
		    ata_bio->lp->d_ncylinders,
		    ata_bio->lp->d_ntracks - 1, 0, ata_bio->lp->d_nsectors,
		    (ata_bio->lp->d_type == DTYPE_ST506) ?
			ata_bio->lp->d_precompcyl / 4 : 0);
		drvp->state = GEOMETRY_WAIT;
		break;

	case GEOMETRY_WAIT:
		errstring = "geometry";
		if (wdcwait(chp, WDCS_DRDY, WDCS_DRDY, delay))
			goto timeout;
		if (chp->ch_status & (WDCS_ERR | WDCS_DWF))
			goto error;
		/* fall through */

	case MULTIMODE:
	multimode:
		if (ata_bio->multi == 1)
			goto ready;
		wdccommand(chp, xfer->drive, WDCC_SETMULTI, 0, 0, 0,
		    ata_bio->multi, 0);
		drvp->state = MULTIMODE_WAIT;
		break;

	case MULTIMODE_WAIT:
		errstring = "setmulti";
		if (wdcwait(chp, WDCS_DRDY, WDCS_DRDY, delay))
			goto timeout;
		if (chp->ch_status & (WDCS_ERR | WDCS_DWF))
			goto error;
		/* fall through */

	case READY:
	ready:
		drvp->state = READY;
		/*
		 * The drive is usable now
		 */
		xfer->c_intr = wdc_ata_bio_intr;
		_wdc_ata_bio_start(chp, xfer); 
		return 1;
	}

	if ((ata_bio->flags & ATA_POLL) == 0) {
		chp->ch_flags |= WDCF_IRQ_WAIT;
	} else {
		goto again;
	}
	return 1;

timeout:
	if (irq && (xfer->c_flags & C_TIMEOU) == 0) {
		return 0; /* IRQ was not for us */
	}
	printf("%s:%d:%d: %s timed out\n",
	    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive, errstring);
	ata_bio->error = TIMEOUT;
	drvp->state = 0;
	wdc_ata_bio_done(chp, xfer);
	return 0;
error:
	printf("%s:%d:%d: %s ",
	    chp->wdc->sc_dev.dv_xname, chp->channel, xfer->drive,
	    errstring);
	if (chp->ch_status & WDCS_DWF) {
		printf("drive fault\n");
		ata_bio->error = ERR_DF;
	} else {
		printf("error (%x)\n", chp->ch_error);
		ata_bio->r_error = chp->ch_error;
		ata_bio->error = ERROR;
	}
	drvp->state = 0;
	wdc_ata_bio_done(chp, xfer);
	return 1;
}

int
wdc_ata_err(drvp, ata_bio)
	struct ata_drive_datas *drvp;
	struct ata_bio *ata_bio;
{
	struct channel_softc *chp = drvp->chnl_softc;
	ata_bio->error = 0;
	if (chp->ch_status & WDCS_BSY) {
		ata_bio->error = TIMEOUT;
		return WDC_ATA_ERR;
	}

	if (chp->ch_status & WDCS_DWF) {
		ata_bio->error = ERR_DF;
		return WDC_ATA_ERR;
	}

	if (chp->ch_status & WDCS_ERR) {
		ata_bio->error = ERROR;
		ata_bio->r_error = chp->ch_error;
		if (drvp->drive_flags & DRIVE_UDMA &&
		    (ata_bio->r_error & WDCE_CRC)) {
			/*
			 * Record the CRC error, to avoid downgrading to
			 * multiword DMA
			 */
			drvp->drive_flags |= DRIVE_DMAERR;
		}
		if (ata_bio->r_error & (WDCE_BBK | WDCE_UNC | WDCE_IDNF |
		    WDCE_ABRT | WDCE_TK0NF | WDCE_AMNF))
			return WDC_ATA_ERR;
		return WDC_ATA_NOERR;
	}

	if (chp->ch_status & WDCS_CORR)
		ata_bio->flags |= ATA_CORR;
	return WDC_ATA_NOERR;
}

#if 0
int
wdc_ata_addref(drvp)
	struct ata_drive_datas *drvp;
{
	struct channel_softc *chp = drvp->chnl_softc;

	return (wdc_addref(chp));
}

void
wdc_ata_delref(drvp)
	struct ata_drive_datas *drvp;
{
	struct channel_softc *chp = drvp->chnl_softc;

	wdc_delref(chp);
}
#endif
