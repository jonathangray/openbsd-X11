#	$NetBSD: Makefile,v 1.3 1997/12/09 11:58:28 mrg Exp $
#	$OpenBSD: Makefile,v 1.2 1998/03/19 22:36:35 gene Exp $
#
# build and install X11

.if ($(MACHINE) == "mac68k")
BCFLAGS?=	-Dmac68k
.else
BCFLAGS?=
.endif

all:
	cd xc ; ${MAKE} BOOTSTRAPCFLAGS="$(BCFLAGS)" World
	${MAKE} all-contrib

all-contrib:
	cd contrib ; PATH=../xc/config/imake:$$PATH \
	    sh ../xc/config/util/xmkmf -a ../xc ../contrib
	cd contrib ; ${MAKE}

build: all
	${MAKE} install

install: install-xc install-contrib install-linkkit

install-xc:
	cd xc; ${MAKE} install && ${MAKE} install.man

install-contrib:
	cd contrib; ${MAKE} install && ${MAKE} install.man

install-linkkit:
.if (${MACHINE} == "i386") 
	cd xc; ${MAKE} install.linkkit
.endif

clean:
	cd xc; ${MAKE} clean
	cd contrib; ${MAKE} clean
