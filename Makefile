#	$NetBSD: Makefile,v 1.3 1997/12/09 11:58:28 mrg Exp $
#	$OpenBSD: Makefile,v 1.8 1998/06/26 01:36:33 todd Exp $
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

RELERROR=echo You must set DESTDIR and RELEASEDIR for a release.; exit 255

CP?= cp


all:
	cd xc ; ${MAKE} World
	${MAKE} all-contrib

all-contrib:
	cd contrib ; PATH=../xc/config/imake:$$PATH \
	    sh ../xc/config/util/xmkmf -a ../xc ../contrib
	cd contrib ; ${MAKE}

build: all
	${MAKE} install
	/usr/libexec/makewhatis ${DESTDIR}/usr/X11R6/man

release:
.if defined(DESTDIR)
.if defined(RELEASEDIR)
.if defined(MACHINE)
.if ${MACHINE} == hp300
	@if [ -e ${XHP} ]; then \
	  echo "${XHP} does not exist.  Please set XHP to the Xhp server.";\
	  exit 1;\
	fi
.endif
.endif
	rm -rf ${DESTDIR}/usr/X11R6/*
	@if [ "`cd ${DESTDIR};ls`" ]; then \
		echo "Files found in ${DESTDIR}. Cleanup before proceeding."; \
		exit 255; \
	fi
	${CP} ${HOSTDEF} ${CONFHOSTDEF}
	@${MKDIR} -p ${DESTDIR}/usr/X11R6
	@${CHOWN} ${BINOWN}.${BINGRP} ${DESTDIR}/usr/X11R6
	@make build
.if defined(MACHINE) && ${MACHINE} == hp300
	@${CP} ${XHP} ${DESTDIR}/usr/X11R6/bin
	@${CHMOD} 755 ${DESTDIR}/usr/X11R6/bin/Xhp
	@${LN} -s Xhp ${DESTDIR}/usr/X11R6/bin/X
	@${ECHO} /dev/grf0 > ${DESTDIR}/usr/X11R6/lib/X11/X0screens
.endif
	@cd distrib/sets && ./maketars && ./checkflist
.else
	@${RELERROR}
.endif
.else
	@${RELERROR}
.endif

install: install-xc install-contrib install-linkkit install-distrib

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
