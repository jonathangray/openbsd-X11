#	$NetBSD: Makefile,v 1.3 1997/12/09 11:58:28 mrg Exp $
#	$OpenBSD: Makefile,v 1.13 1998/09/27 03:14:05 todd Exp $
#
# build and install X11, create release tarfiles
#
# You need to have set DESTDIR and RELEASEDIR in order to properly
# create a release
# 
XHP?=${.CURDIR}/Xhp
XMACH!= [ "${MACHINE}" = "i386" ] && echo "ix86" || echo "${MACHINE}"
HOSTDEF=xc/programs/Xserver/hw/xfree86/etc/bindist/OpenBSD-${XMACH}/host.def
CONFHOSTDEF=xc/config/cf/host.def


CP?= /bin/cp
MKDIR?= /bin/mkdir -p
LN?= /bin/ln
CHOWN?=/usr/sbin/chown
RM?= /bin/rm

MACHINE?=`uname -m`

all:
	${RM} -f ${CONFHOSTDEF}
	${CP} ${HOSTDEF} ${CONFHOSTDEF}
	cd xc ; ${MAKE} World
	${MAKE} all-contrib

all-contrib:
	cd contrib ; PATH=../xc/config/imake:$$PATH \
	    sh ../xc/config/util/xmkmf -a ../xc ../contrib
	cd contrib ; ${MAKE}

build: all
	${MAKE} install

release:
.if ! ( defined(DESTDIR) && defined(RELEASEDIR) )
	@echo You must set DESTDIR and RELEASEDIR for a release.; exit 255
.endif
.if ${MACHINE} == hp300
	@if [ -e ${XHP} ]; then \
	  echo "${XHP} does not exist.  Please set XHP to the Xhp server.";\
	  exit 1;\
	fi
.endif
	${RM} -rf ${DESTDIR}/usr/X11R6/*
	@if [ "`cd ${DESTDIR}/usr/X11R6;ls`" ]; then \
		echo "Files found in ${DESTDIR}/usr/X11R6." \
		echo "Cleanup before proceeding."; \
		exit 255; \
	fi
	@${MKDIR} -p ${DESTDIR}/usr/X11R6
	@${CHOWN} ${BINOWN}.${BINGRP} ${DESTDIR}/usr/X11R6
	@${MAKE} install
.if defined(MACHINE) && ${MACHINE} == hp300
	@${CP} ${XHP} ${DESTDIR}/usr/X11R6/bin
	@${CHMOD} 755 ${DESTDIR}/usr/X11R6/bin/Xhp
	@${LN} -s Xhp ${DESTDIR}/usr/X11R6/bin/X
	@${ECHO} /dev/grf0 > ${DESTDIR}/usr/X11R6/lib/X11/X0screens
.endif
	@${MAKE} dist

dist:
	cd distrib/sets && ./maketars ${OSrev} && ./checkflist

install: install-xc install-contrib install-linkkit install-distrib
	/usr/libexec/makewhatis ${DESTDIR}/usr/X11R6/man

install-xc:
	cd xc; ${MAKE} install && ${MAKE} install.man
.if (${MACHINE} == "hp300")
	echo /dev/grf0 > ${DESTDIR}/usr/X11R6/lib/X11/X0screens
	chown root.wheel ${DESTDIR}/usr/X11R6/lib/X11/X0screens
.endif

install-contrib:
	cd contrib; ${MAKE} install && ${MAKE} install.man

install-linkkit:
.if (${MACHINE} == "i386") 
	cd xc; ${MAKE} install.linkkit
.endif

install-distrib:
	cd distrib/notes; ${MAKE} install

clean:
	cd xc; ${MAKE} clean
.if exists(contrib/Makefile)
	cd contrib; ${MAKE} clean
.endif

distclean:
	${MAKE} clean
	find xc contrib -name Makefile -mindepth 2 -exec rm -f '{}' ';'
	rm -f xc/xmakefile contrib/Makefile
