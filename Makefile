#	$NetBSD: Makefile,v 1.3 1997/12/09 11:58:28 mrg Exp $
#	$OpenBSD: Makefile,v 1.1 1998/03/19 20:48:46 matthieu Exp $
#
# build and install X11

all:
	cd xc ; ${MAKE} World
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
