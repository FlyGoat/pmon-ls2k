#ifndef _LS1A_H
#define _LS1A_H

#define LS1A_IO_REG_BASE				0xb2000000

/* CHIP CONFIG regs */
#define LS1A_CHIP_CFG_REG_BASE				(LS1A_IO_REG_BASE + 0x00d00000)
#define LS1A_MSI_PORT_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0)

#define LS1A_INT_REG_BASE				(LS1A_CHIP_CFG_REG_BASE + 0x0040)

#define LS1A_INT_ISR0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0040)
#define LS1A_INT_IEN0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0044)
#define LS1A_INT_SET0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0048)
#define LS1A_INT_CLR0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x004c)
#define LS1A_INT_POL0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0050)
#define LS1A_INT_EDGE0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0054)

#define LS1A_GPIO_CFG_REG				(LS1A_CHIP_CFG_REG_BASE + 0x00c0)
#define LS1A_GPIO_OUT_EN_REG				(LS1A_CHIP_CFG_REG_BASE + 0x00c4)
#define LS1A_GPIO_IN_REG				(LS1A_CHIP_CFG_REG_BASE + 0x00c8)
#define LS1A_GPIO_OUT_REG				(LS1A_CHIP_CFG_REG_BASE + 0x00cc)

#define LS1A_DMA_ORDER_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0100)
#define LS1A_CHIP_CFG0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0200)
#define LS1A_CHIP_CFG1_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0204)
#define LS1A_CHIP_CFG2_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0208)
#define LS1A_CHIP_CFG3_REG				(LS1A_CHIP_CFG_REG_BASE + 0x020c)
#define LS1A_CHIP_SAMP0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0210)
#define LS1A_CHIP_SAMP1_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0214)
#define LS1A_CHIP_SAMP2_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0218)
#define LS1A_CHIP_SAMP3_REG				(LS1A_CHIP_CFG_REG_BASE + 0x021c)
#define LS1A_CHIP_SAMP4_REG				(LS1A_CHIP_CFG_REG_BASE + 0x00d8)
#define LS1A_CHIP_SAMP5_REG				(LS1A_CHIP_CFG_REG_BASE + 0x00e8)

#define LS1A_CLK_CTRL0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0220)
#define LS1A_CLK_CTRL1_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0224)
#define LS1A_CLK_CTRL2_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0228)
#define LS1A_CLK_CTRL3_REG				(LS1A_CHIP_CFG_REG_BASE + 0x022c)

#define LS1A_PIXCLK0_CTRL0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0230)
#define LS1A_PIXCLK0_CTRL1_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0234)
#define LS1A_PIXCLK1_CTRL0_REG				(LS1A_CHIP_CFG_REG_BASE + 0x0238)
#define LS1A_PIXCLK1_CTRL1_REG				(LS1A_CHIP_CFG_REG_BASE + 0x023c)

#define LS1A_CLOCK_CTRL0_REG                            (LS1A_CHIP_CFG_REG_BASE + 0x0220)
#define LS1A_CLOCK_CTRL1_REG                            (LS1A_CHIP_CFG_REG_BASE + 0x0224)
#define LS1A_CLOCK_CTRL2_REG                            (LS1A_CHIP_CFG_REG_BASE + 0x0228)
#define LS1A_CLOCK_CTRL3_REG                            (LS1A_CHIP_CFG_REG_BASE + 0x022c)

#define LS1A_WIN_CFG_BASE				(LS1A_CHIP_CFG_REG_BASE + 0x80000)
#define LS1A_M4_WIN0_BASE_REG				(LS1A_WIN_CFG_BASE + 0x0400)
#define LS1A_M4_WIN0_MASK_REG				(LS1A_WIN_CFG_BASE + 0x0440)
#define LS1A_M4_WIN0_MMAP_REG				(LS1A_WIN_CFG_BASE + 0x0480)
#define LS1A_QOS_CFG6_REG				(LS1A_WIN_CFG_BASE + 0x0630)

/* USB regs */
#define LS1A_EHCI_REG_BASE				(LS1A_IO_REG_BASE + 0x00e00000)
#define LS1A_OHCI_REG_BASE				(LS1A_IO_REG_BASE + 0x00e08000)

/* GMAC regs */
#define LS1A_GMAC0_REG_BASE				(LS1A_IO_REG_BASE + 0x00e10000)
#define LS1A_GMAC1_REG_BASE				(LS1A_IO_REG_BASE + 0x00e18000)

/* HDA regs */
#define LS1A_HDA_REG_BASE				(LS1A_IO_REG_BASE + 0x00e20000)

/* SATAregs */
#define LS1A_SATA_REG_BASE				(LS1A_IO_REG_BASE + 0x00e30000)

/* GPU regs */
#define LS1A_GPU_REG_BASE				(LS1A_IO_REG_BASE + 0x00e40000)

/* DC regs */
#define LS1A_DC_REG_BASE				(LS1A_IO_REG_BASE + 0x00e50000)

#define LS1A_FB_CFG_DVO_REG				(LS1A_DC_REG_BASE + 0x1240)
#define LS1A_FB_CFG_VGA_REG				(LS1A_DC_REG_BASE + 0x1250)
#define LS1A_FB_ADDR0_DVO_REG				(LS1A_DC_REG_BASE + 0x1260)
#define LS1A_FB_ADDR0_VGA_REG				(LS1A_DC_REG_BASE + 0x1270)
#define LS1A_FB_ADDR1_DVO_REG				(LS1A_DC_REG_BASE + 0x1580)
#define LS1A_FB_ADDR1_VGA_REG				(LS1A_DC_REG_BASE + 0x1590)
#define LS1A_FB_STRI_DVO_REG				(LS1A_DC_REG_BASE + 0x1280)
#define LS1A_FB_STRI_VGA_REG				(LS1A_DC_REG_BASE + 0x1290)

#define LS1A_FB_CUR_CFG_REG				(LS1A_DC_REG_BASE + 0x1520)
#define LS1A_FB_CUR_ADDR_REG				(LS1A_DC_REG_BASE + 0x1530)
#define LS1A_FB_CUR_LOC_ADDR_REG			(LS1A_DC_REG_BASE + 0x1540)
#define LS1A_FB_CUR_BACK_REG				(LS1A_DC_REG_BASE + 0x1550)
#define LS1A_FB_CUR_FORE_REG				(LS1A_DC_REG_BASE + 0x1560)

#define LS1A_FB_DAC_CTRL_REG				(LS1A_DC_REG_BASE + 0x1600)
/* OTG regs */
#define LS1A_OTG_REG_BASE				(LS1A_IO_REG_BASE + 0x00e60000)

/* SPI regs */
#define LS1A_SPI_REG_BASE				(LS1A_IO_REG_BASE + 0x00e70000)

/* UART regs */
#define LS1A_UART0_REG_BASE				(LS1A_IO_REG_BASE + 0x00e80000)
#define LS1A_UART1_REG_BASE				(LS1A_IO_REG_BASE + 0x00e81000)
#define LS1A_UART2_REG_BASE				(LS1A_IO_REG_BASE + 0x00e82000)
#define LS1A_UART3_REG_BASE				(LS1A_IO_REG_BASE + 0x00e83000)

/* I2C regs */
#define LS1A_I2C0_REG_BASE				(LS1A_IO_REG_BASE + 0x00e90000)
#define LS1A_I2C0_PRER_LO_REG				(LS1A_I2C0_REG_BASE + 0x0)
#define LS1A_I2C0_PRER_HI_REG				(LS1A_I2C0_REG_BASE + 0x1)
#define LS1A_I2C0_CTR_REG   				(LS1A_I2C0_REG_BASE + 0x2)
#define LS1A_I2C0_TXR_REG   				(LS1A_I2C0_REG_BASE + 0x3)
#define LS1A_I2C0_RXR_REG    				(LS1A_I2C0_REG_BASE + 0x3)
#define LS1A_I2C0_CR_REG     				(LS1A_I2C0_REG_BASE + 0x4)
#define LS1A_I2C0_SR_REG     				(LS1A_I2C0_REG_BASE + 0x4)

#define LS1A_I2C1_REG_BASE				(LS1A_IO_REG_BASE + 0x00e91000)
#define LS1A_I2C1_PRER_LO_REG				(LS1A_I2C1_REG_BASE + 0x0)
#define LS1A_I2C1_PRER_HI_REG				(LS1A_I2C1_REG_BASE + 0x1)
#define LS1A_I2C1_CTR_REG    				(LS1A_I2C1_REG_BASE + 0x2)
#define LS1A_I2C1_TXR_REG    				(LS1A_I2C1_REG_BASE + 0x3)
#define LS1A_I2C1_RXR_REG    				(LS1A_I2C1_REG_BASE + 0x3)
#define LS1A_I2C1_CR_REG     				(LS1A_I2C1_REG_BASE + 0x4)
#define LS1A_I2C1_SR_REG     				(LS1A_I2C1_REG_BASE + 0x4)

#define CR_START					0x80
#define CR_STOP						0x40
#define CR_READ						0x20
#define CR_WRITE					0x10
#define CR_ACK						0x8
#define CR_IACK						0x1

#define SR_NOACK					0x80
#define SR_BUSY						0x40
#define SR_AL						0x20
#define SR_TIP						0x2
#define	SR_IF						0x1

/* PWM regs */
#define LS1A_PWM_REG_BASE				(LS1A_IO_REG_BASE + 0x00ea0000)

/* HPET regs */
#define LS1A_HPET_REG_BASE				(LS1A_IO_REG_BASE + 0x00ec0000)

/* AC97 regs */
#define LS1A_AC97_REG_BASE				(LS1A_IO_REG_BASE + 0x00ed0000)

/* NAND regs */
#define LS1A_NAND_REG_BASE				(LS1A_IO_REG_BASE + 0x00ee0000)
#define LS1A_NAND_CMD_REG				(LS1A_NAND_REG_BASE + 0x0000)
#define LS1A_NAND_ADDR_C_REG				(LS1A_NAND_REG_BASE + 0x0004)
#define LS1A_NAND_ADDR_R_REG				(LS1A_NAND_REG_BASE + 0x0008)
#define LS1A_NAND_TIMING_REG				(LS1A_NAND_REG_BASE + 0x000c)
#define LS1A_NAND_IDL_REG				(LS1A_NAND_REG_BASE + 0x0010)
#define LS1A_NAND_STA_IDH_REG				(LS1A_NAND_REG_BASE + 0x0014)
#define LS1A_NAND_PARAM_REG				(LS1A_NAND_REG_BASE + 0x0018)
#define LS1A_NAND_OP_NUM_REG				(LS1A_NAND_REG_BASE + 0x001c)
#define LS1A_NAND_CSRDY_MAP_REG				(LS1A_NAND_REG_BASE + 0x0020)
#define LS1A_NAND_DMA_ACC_REG				(LS1A_NAND_REG_BASE + 0x0040)

/* ACPI regs */
#define LS1A_ACPI_REG_BASE				(LS1A_IO_REG_BASE + 0x00ef0000)
#define LS1A_PM_SOC_REG					(LS1A_ACPI_REG_BASE + 0x0000)
#define LS1A_PM_RESUME_REG				(LS1A_ACPI_REG_BASE + 0x0004)
#define LS1A_PM_RTC_REG					(LS1A_ACPI_REG_BASE + 0x0008)
#define LS1A_PM1_STS_REG				(LS1A_ACPI_REG_BASE + 0x000c)
#define LS1A_PM1_EN_REG					(LS1A_ACPI_REG_BASE + 0x0010)
#define LS1A_PM1_CNT_REG				(LS1A_ACPI_REG_BASE + 0x0014)
#define LS1A_PM1_TMR_REG				(LS1A_ACPI_REG_BASE + 0x0018)
#define LS1A_P_CNT_REG					(LS1A_ACPI_REG_BASE + 0x001c)
#define LS1A_P_LVL2_REG					(LS1A_ACPI_REG_BASE + 0x0020)
#define LS1A_P_LVL3_REG					(LS1A_ACPI_REG_BASE + 0x0024)
#define LS1A_GPE0_STS_REG				(LS1A_ACPI_REG_BASE + 0x0028)
#define LS1A_GPE0_EN_REG				(LS1A_ACPI_REG_BASE + 0x002c)
#define LS1A_RST_CNT_REG				(LS1A_ACPI_REG_BASE + 0x0030)
#define LS1A_WD_SET_REG					(LS1A_ACPI_REG_BASE + 0x0034)
#define LS1A_WD_TIMER_REG				(LS1A_ACPI_REG_BASE + 0x0038)
#define LS1A_DVFS_CNT_REG				(LS1A_ACPI_REG_BASE + 0x003c)
#define LS1A_DVFS_STS_REG				(LS1A_ACPI_REG_BASE + 0x0040)
#define LS1A_MS_CNT_REG					(LS1A_ACPI_REG_BASE + 0x0044)
#define LS1A_MS_THT_REG					(LS1A_ACPI_REG_BASE + 0x0048)
#define	LS1A_THSENS_CNT_REG				(LS1A_ACPI_REG_BASE + 0x004c)
#define LS1A_GEN_RTC1_REG				(LS1A_ACPI_REG_BASE + 0x0050)
#define LS1A_GEN_RTC2_REG				(LS1A_ACPI_REG_BASE + 0x0054)

/* RTC regs lxf modify*/
#define LS1A_RTC_REG_BASE				(LS1A_IO_REG_BASE + 0x00e64000)
#define	LS1A_TOY_TRIM_REG				(LS1A_RTC_REG_BASE + 0x0020)
#define	LS1A_TOY_WRITE0_REG				(LS1A_RTC_REG_BASE + 0x0024)
#define	LS1A_TOY_WRITE1_REG				(LS1A_RTC_REG_BASE + 0x0028)
#define	LS1A_TOY_READ0_REG				(LS1A_RTC_REG_BASE + 0x002c)
#define	LS1A_TOY_READ1_REG				(LS1A_RTC_REG_BASE + 0x0030)
#define	LS1A_TOY_MATCH0_REG				(LS1A_RTC_REG_BASE + 0x0034)
#define	LS1A_TOY_MATCH1_REG				(LS1A_RTC_REG_BASE + 0x0038)
#define	LS1A_TOY_MATCH2_REG				(LS1A_RTC_REG_BASE + 0x003c)
#define	LS1A_RTC_CTRL_REG				(LS1A_RTC_REG_BASE + 0x0040)
#define	LS1A_RTC_TRIM_REG				(LS1A_RTC_REG_BASE + 0x0060)
#define	LS1A_RTC_WRITE0_REG				(LS1A_RTC_REG_BASE + 0x0064)
#define	LS1A_RTC_READ0_REG				(LS1A_RTC_REG_BASE + 0x0068)
#define	LS1A_RTC_MATCH0_REG				(LS1A_RTC_REG_BASE + 0x006c)
#define	LS1A_RTC_MATCH1_REG				(LS1A_RTC_REG_BASE + 0x0070)
#define	LS1A_RTC_MATCH2_REG				(LS1A_RTC_REG_BASE + 0x0074)

/* LPC regs */
#define LS1A_LPC_IO_BASE				(LS1A_IO_REG_BASE + 0x00f00000)
#define LS1A_LPC_REG_BASE				(LS1A_IO_REG_BASE + 0x00f10000)
#define LS1A_LPC_CFG0_REG				(LS1A_LPC_REG_BASE + 0x0)
#define LS1A_LPC_CFG1_REG				(LS1A_LPC_REG_BASE + 0x4)
#define LS1A_LPC_CFG2_REG				(LS1A_LPC_REG_BASE + 0x8)
#define LS1A_LPC_CFG3_REG				(LS1A_LPC_REG_BASE + 0xc)


#define LS1A_PCIE_MAX_PORTNUM       3
#define LS1A_PCIE_PORT0             0
#define LS1A_PCIE_PORT1             1
#define LS1A_PCIE_PORT2             2
#define LS1A_PCIE_PORT3             3
#define LS1A_PCIE_GET_PORTNUM(sysdata) \
        ((((struct pci_controller *)(sysdata))->mem_resource->start \
                        & ~LS1A_PCIE_MEM0_DOWN_MASK) >> 25)

#define LS1A_CHIP_CFG_REG_CLK_CTRL3     0x22c
#define LS1A_CLK_CTRL3_BIT_PEREF_EN(portnum) (1 << (24 + portnum))

#define LS1A_PCIE_MEM0_BASE_PORT(portnum)       (0x10000000 + (portnum << 25))
#define LS1A_PCIE_IO_BASE_PORT(portnum)         (0x18100000 + (portnum << 22))

#define LS1A_PCIE_REG_BASE_PORT(portnum)        (0x18118000 + (portnum << 22))
#define LS1A_PCIE_PORT_REG_CTR0			0x0
#define LS1A_PCIE_REG_CTR0_BIT_LTSSM_EN			(1 << 3)
#define LS1A_PCIE_REG_CTR0_BIT_REQ_L1			(1 << 12)
#define LS1A_PCIE_REG_CTR0_BIT_RDY_L23			(1 << 13)
#define LS1A_PCIE_PORT_REG_STAT1		0xC
#define LS1A_PCIE_REG_STAT1_MASK_LTSSM		0x0000003f
#define LS1A_PCIE_REG_STAT1_BIT_LINKUP			(1 << 6)
#define LS1A_PCIE_PORT_REG_CFGADDR		0x24
#define LS1A_PCIE_PORT_REG_CTR_STAT		0x28
#define LS1A_PCIE_REG_CTR_STAT_BIT_ISX4			(1 << 26)
#define LS1A_PCIE_REG_CTR_STAT_BIT_ISRC			(1 << 27)

#define LS1A_PCIE_PORT_HEAD_BASE_PORT(portnum)  (0x18114000 + (portnum << 22))
#define LS1A_PCIE_DEV_HEAD_BASE_PORT(portnum)   (0x18116000 + (portnum << 22))

#define LIE_IN_WINDOW(addr,base,mask)   ((addr & mask) == base)
#define MAP_2_WINDOW(addr,mmap,mask)    ((addr & (~(mask))) | (mmap & mask))
#define LS1A_PCIE_MEM0_DOWN_BASE		0x10000000
#define LS1A_PCIE_MEM0_DOWN_MASK		0xf8000000
#define LS1A_PCIE_MEM0_UP_BASE			0x10000000
#define LS1A_PCIE_MEM0_UP_MASK			0xfe000000
#define LS1A_PCIE_IO_DOWN_BASE			0x18100000
#define LS1A_PCIE_IO_DOWN_MASK			0xff3f0000
#define LS1A_PCIE_IO_UP_BASE			0x0
#define LS1A_PCIE_IO_UP_MASK			0xffff0000

#endif /*_LS1A_H*/