#	$OpenBSD: bsd.lib.mk,v 1.35 2001/07/22 13:37:29 espie Exp $
#	$NetBSD: bsd.lib.mk,v 1.67 1996/01/17 20:39:26 mycroft Exp $
#	@(#)bsd.lib.mk	5.26 (Berkeley) 5/2/91

.include <pmon.own.mk>				# for 'NOPIC' definition

.if exists(${.CURDIR}/../Makefile.inc)
.include "${.CURDIR}/../Makefile.inc"
.endif

.if exists(${.CURDIR}/shlib_version)
.include "${.CURDIR}/shlib_version"
SHLIB_MAJOR=${major}
SHLIB_MINOR=${minor}
.endif

.MAIN: all

# prefer .S to a .c, add .po, remove stuff not used in the BSD libraries.
# .so used for PIC object files.  .ln used for lint output files.
# .m for objective c files.
.SUFFIXES:
.SUFFIXES: .out .o .go .po .so .S .s .c .cc .C .cxx .f .y .l .ln .m4 .m

.c.o:
	@echo "${COMPILE.c} ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} ${.IMPSRC}  -o ${.TARGET}.o
	@${LD} -x -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.c.go:
	@echo "${COMPILE.c} -g ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} -g ${.IMPSRC} -o ${.TARGET}.o
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.c.po:
	@echo "${COMPILE.c} -p ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} -p ${.IMPSRC} -o ${.TARGET}.o
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.c.so:
	@echo "${COMPILE.c} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.c} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}.o
	@${LD} -x -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.c.ln:
	${LINT} ${LINTFLAGS} ${CFLAGS:M-[IDU]*} -i ${.IMPSRC}

.cc.o .C.o .cxx.o:
	@echo "${COMPILE.cc} ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.cc} ${.IMPSRC} -o ${.TARGET}.o
	@${LD} -x -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.cc.go .C.go .cxx.go:
	@echo "${COMPILE.cc} -g ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.cc} -g ${.IMPSRC} -o ${.TARGET}.o
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.cc.po .C.po .cxx.po:
	@echo "${COMPILE.cc} -p ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.cc} -p ${.IMPSRC} -o ${.TARGET}.o
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.cc.so .C.so .cxx.so:
	@echo "${COMPILE.cc} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}"
	@${COMPILE.cc} ${PICFLAG} -DPIC ${.IMPSRC} -o ${.TARGET}.o
	@${LD} -x -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.S.o .s.o:
	@echo "${CPP} ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} | \
		${AS} -o ${.TARGET}"
	echo "${.CURDIR}"
	${CPP} ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} | \
	    ${AS} -o ${.TARGET}.o
	${LD} -x -r ${.TARGET}.o -o ${.TARGET}
	rm -f ${.TARGET}.o

.S.go .s.go:
	@echo "${CPP} ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} |\
	    ${AS} -o ${.TARGET}"
	@${CPP} ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} | \
	    ${AS} -o ${.TARGET}.o
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.S.po .s.po:
	@echo "${CPP} -DPROF ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} |\
	    ${AS} -o ${.TARGET}"
	@${CPP} -DPROF ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} | \
	    ${AS} -o ${.TARGET}.o
	@${LD} -X -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.S.so .s.so:
	@echo "${CPP} -DPIC ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} | \
	    ${AS} -k -o ${.TARGET}"
	@${CPP} -DPIC ${CPPFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} | \
	    ${AS} -k -o ${.TARGET}.o
	@${LD} -x -r ${.TARGET}.o -o ${.TARGET}
	@rm -f ${.TARGET}.o

.if ${WARNINGS:L} == "yes"
CFLAGS+=	${CDIAGFLAGS}
CXXFLAGS+=	${CXXDIAGFLAGS}
.endif
CFLAGS+=	${COPTS}
CXXFLAGS+=	${CXXOPTS}

.if !defined(PICFLAG) && (${MACHINE_ARCH} != "mips")
PICFLAG=-fpic
.if ${MACHINE_ARCH} == "m68k"
# Function CSE makes gas -k not recognize external function calls as lazily
# resolvable symbols, thus sometimes making ld.so report undefined symbol
# errors on symbols found in shared library members that would never be 
# called.  Ask niklas@openbsd.org for details.
PICFLAG+=-fno-function-cse
.endif
.endif

_LIBS=lib${LIB}.a
.if (${DEBUGLIBS:L} == "yes")
_LIBS+=lib${LIB}_g.a
.endif
.if !defined(NOPROFILE)
_LIBS+=lib${LIB}_p.a
.endif

.if !defined(NOPIC)
.if (${MACHINE_ARCH} != "mips")
_LIBS+=lib${LIB}_pic.a
.endif
.if defined(SHLIB_MAJOR) && defined(SHLIB_MINOR)
_LIBS+=lib${LIB}.so.${SHLIB_MAJOR}.${SHLIB_MINOR}
.endif
.endif

.if !defined(NOLINT)
_LIBS+=llib-l${LIB}.ln
.endif

all: ${_LIBS} _SUBDIRUSE

OBJS+=	${SRCS:N*.h:R:S/$/.o/g}

lib${LIB}.a:: ${OBJS}
	@echo building standard ${LIB} library
	@rm -f lib${LIB}.a
	@${AR} cq lib${LIB}.a `${LORDER} ${OBJS} | tsort -q`
	${RANLIB} lib${LIB}.a

GOBJS+=	${OBJS:.o=.go}
lib${LIB}_g.a:: ${GOBJS}
	@echo building debugging ${LIB} library
	@rm -f lib${LIB}_g.a
	@${AR} cq lib${LIB}_g.a `${LORDER} ${GOBJS} | tsort -q`
	${RANLIB} lib${LIB}_g.a

POBJS+=	${OBJS:.o=.po}
lib${LIB}_p.a:: ${POBJS}
	@echo building profiled ${LIB} library
	@rm -f lib${LIB}_p.a
	@${AR} cq lib${LIB}_p.a `${LORDER} ${POBJS} | tsort -q`
	${RANLIB} lib${LIB}_p.a

SOBJS+=	${OBJS:.o=.so}
lib${LIB}_pic.a:: ${SOBJS}
	@echo building shared object ${LIB} library
	@rm -f lib${LIB}_pic.a
	@${AR} cq lib${LIB}_pic.a `${LORDER} ${SOBJS} | tsort -q`
	${RANLIB} lib${LIB}_pic.a

lib${LIB}.so.${SHLIB_MAJOR}.${SHLIB_MINOR}: ${SOBJS} ${DPADD}
	@echo building shared ${LIB} library \(version ${SHLIB_MAJOR}.${SHLIB_MINOR}\)
	@rm -f lib${LIB}.so.${SHLIB_MAJOR}.${SHLIB_MINOR}
	${CC} -shared ${PICFLAG} \
	    -o lib${LIB}.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
	    `${LORDER} ${SOBJS}|tsort -q` ${LDADD}

LOBJS+=	${LSRCS:.c=.ln} ${SRCS:M*.c:.c=.ln}
# the following looks XXX to me... -- cgd
LLIBS?=	-lc
llib-l${LIB}.ln: ${LOBJS}
	@echo building llib-l${LIB}.ln
	@rm -f llib-l${LIB}.ln
	@${LINT} -C${LIB} ${LOBJS} ${LLIBS}

.if !target(clean)
clean: _SUBDIRUSE
	rm -f a.out [Ee]rrs mklog core *.core ${CLEANFILES}
	rm -f lib${LIB}.a ${OBJS}
	rm -f lib${LIB}_g.a ${GOBJS}
	rm -f lib${LIB}_p.a ${POBJS}
	rm -f lib${LIB}_pic.a lib${LIB}.so.*.* ${SOBJS}
	rm -f llib-l${LIB}.ln ${LOBJS}
.endif

cleandir: _SUBDIRUSE clean

.if defined(SRCS)
afterdepend: .depend
	@(TMP=`mktemp -q /tmp/_dependXXXXXXXXXX`; \
	if [ $$? -ne 0 ]; then \
		echo "$$0: cannot create temp file, exiting..."; \
		exit 1; \
	fi; \
	sed -e 's/^\([^\.]*\).o[ ]*:/\1.o \1.po \1.so:/' \
	      < .depend > $$TMP; \
	mv $$TMP .depend)
.endif

.if !target(install)
.if !target(beforeinstall)
beforeinstall:
.endif

realinstall:
#	ranlib lib${LIB}.a
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m 600 lib${LIB}.a \
	    ${DESTDIR}${LIBDIR}
.if (${INSTALL_COPY} != "-p")
	${RANLIB} -t ${DESTDIR}${LIBDIR}/lib${LIB}.a
.endif
	chmod ${LIBMODE} ${DESTDIR}${LIBDIR}/lib${LIB}.a
.if (${DEBUGLIBS:L} == "yes")
#	ranlib lib${LIB}_g.a
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m 600 \
	    lib${LIB}_g.a ${DESTDIR}${LIBDIR}/debug/lib${LIB}.a
.if (${INSTALL_COPY} != "-p")
	${RANLIB} -t ${DESTDIR}${LIBDIR}/debug/lib${LIB}.a
.endif
	chmod ${LIBMODE} ${DESTDIR}${LIBDIR}/debug/lib${LIB}.a
.endif
.if !defined(NOPROFILE)
#	ranlib lib${LIB}_p.a
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m 600 \
	    lib${LIB}_p.a ${DESTDIR}${LIBDIR}
.if (${INSTALL_COPY} != "-p")
	${RANLIB} -t ${DESTDIR}${LIBDIR}/lib${LIB}_p.a
.endif
	chmod ${LIBMODE} ${DESTDIR}${LIBDIR}/lib${LIB}_p.a
.endif
.if !defined(NOPIC) && (${MACHINE_ARCH} != "mips") 
#	ranlib lib${LIB}_pic.a
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m 600 \
	    lib${LIB}_pic.a ${DESTDIR}${LIBDIR}
.if (${INSTALL_COPY} != "-p")
	${RANLIB} -t ${DESTDIR}${LIBDIR}/lib${LIB}_pic.a
.endif
	chmod ${LIBMODE} ${DESTDIR}${LIBDIR}/lib${LIB}_pic.a
.endif
.if !defined(NOPIC) && defined(SHLIB_MAJOR) && defined(SHLIB_MINOR)
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
	    lib${LIB}.so.${SHLIB_MAJOR}.${SHLIB_MINOR} ${DESTDIR}${LIBDIR}
.endif
.if !defined(NOLINT)
	${INSTALL} ${INSTALL_COPY} -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
	    llib-l${LIB}.ln ${DESTDIR}${LINTLIBDIR}
.endif
.if defined(LINKS) && !empty(LINKS)
.  for lnk file in ${LINKS}
	@l=${DESTDIR}${lnk}; \
	 t=${DESTDIR}${file}; \
	 echo $$t -\> $$l; \
	 rm -f $$t; ln $$l $$t
.  endfor
.endif

install: maninstall _SUBDIRUSE
maninstall: afterinstall
afterinstall: realinstall
realinstall: beforeinstall
.endif

.if !defined(NOMAN)
.include <pmon.man.mk>
.endif

.if !defined(NONLS)
.include <pmon.nls.mk>
.endif

.include <pmon.obj.mk>
.include <pmon.dep.mk>
.include <pmon.subdir.mk>
.include <pmon.sys.mk>
