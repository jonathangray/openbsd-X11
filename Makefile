#	$NetBSD: Makefile,v 1.3 1997/12/09 11:58:28 mrg Exp $
#	$OpenBSD: Makefile,v 1.26 2001/02/10 08:52:29 matthieu Exp $
#
# build and install X11, create release tarfiles
#
# You need to have set DESTDIR and RELEASEDIR in order to properly
# create a release
# 
XHP?=${.CURDIR}/XhpBSD
XMACH!= ([ "${MACHINE}" = "i386" ] && echo "ix86") || \
	([ "${MACHINE}" = "hp700" ] && echo "hppa") || echo "${MACHINE}"
HOSTDEF=xc/programs/Xserver/hw/xfree86/etc/bindist/OpenBSD-${XMACH}/host.def
CONFHOSTDEF=xc/config/cf/host.def


CP?= /bin/cp
MKDIR?= /bin/mkdir
LN?= /bin/ln
CHOWN?=/usr/sbin/chown
BINOWN?=root
BINGRP?=wheel
CHMOD?=/bin/chmod
ECHO?=/bin/echo
RM?= /bin/rm

MACHINE?=`uname -m`

# Some defaults.
RELEASEDIR?=${.CURDIR}/rel
DESTDIR?=${.CURDIR}/dest

LOCALAPPD=/usr/local/lib/X11/app-defaults
LOCALAPPX=/usr/local/lib/X11
REALAPPD=/var/X11/app-defaults

all:
	${RM} -f ${CONFHOSTDEF}
	${CP} ${HOSTDEF} ${CONFHOSTDEF}
	cd xc ; ${MAKE} World WORLDOPTS=
	${MAKE} all-contrib

all-contrib:
	cd contrib ; PATH=../xc/config/imake:$$PATH \
	    sh ../xc/config/util/xmkmf -a ../xc ../contrib
	cd contrib ; ${MAKE}

build: all
	${MAKE} install
# kludge, since Todd abuses DESTDIR
	${MAKE} fix-appd DESTDIR=`echo $$DESTDIR`

release:
.if ! ( defined(DESTDIR) && defined(RELEASEDIR) )
	@echo You must set DESTDIR and RELEASEDIR for a release.; exit 255
.endif
.if ${MACHINE} == hp300
	@if [ ! -e ${XHP} ]; then \
	  echo "${XHP} does not exist.  Please set XHP to the XhpBSD server.";\
	  exit 1;\
	fi
.endif
	${RM} -rf ${DESTDIR}/usr/X11R6/*
	${RM} -rf ${DESTDIR}/var/X11/*
	@if [ "`cd ${DESTDIR}/usr/X11R6;ls`" ]; then \
		echo "Files found in ${DESTDIR}/usr/X11R6." \
		echo "Cleanup before proceeding."; \
		exit 255; \
	fi
	@${MKDIR} -p ${DESTDIR}/usr/X11R6
	@${MKDIR} -p ${DESTDIR}/var/X11
	@${MKDIR} -p ${DESTDIR}/usr/local/lib/X11
	${MAKE} perms
	@${MAKE} install
.if defined(MACHINE) && ${MACHINE} == hp300
	@${CP} ${XHP} ${DESTDIR}/usr/X11R6/bin
	@${CHMOD} 755 ${DESTDIR}/usr/X11R6/bin/XhpBSD
	@${LN} -s XhpBSD ${DESTDIR}/usr/X11R6/bin/X
	@${ECHO} /dev/grf0 > ${DESTDIR}/usr/X11R6/lib/X11/X0screens
.endif
	@${MAKE} fix-appd
	@${MAKE} dist

perms:
	@${CHOWN} ${BINOWN}.${BINGRP} ${DESTDIR}/.
	@${CHOWN} ${BINOWN}.${BINGRP} ${DESTDIR}/usr
	ls -ld ${DESTDIR}/. ${DESTDIR}/usr
	@${CHOWN} ${BINOWN}.${BINGRP} ${DESTDIR}/usr/X11R6
	ls -al ${DESTDIR}/usr/X11R6/
	@${CHOWN} ${BINOWN}.${BINGRP} ${DESTDIR}/var
	ls -ld ${DESTDIR}/var
	@${CHOWN} ${BINOWN}.${BINGRP} ${DESTDIR}/var/X11
	ls -al ${DESTDIR}/var/X11

dist:
	${MAKE} perms
	cd distrib/sets && csh ./maketars ${OSrev} && csh ./checkflist

install: install-xc install-contrib install-linkkit install-distrib 
	/usr/libexec/makewhatis ${DESTDIR}/usr/X11R6/man

install-xc:
	cd xc; ${MAKE} install && ${MAKE} install.man
.if (${MACHINE} == "hp300")
	echo /dev/grf0 > ${DESTDIR}/usr/X11R6/lib/X11/X0screens
	chown root.wheel ${DESTDIR}/usr/X11R6/lib/X11/X0screens
.endif
	cd xc/programs/rstart; ${MAKE} install && ${MAKE} install.man

install-contrib:
	cd contrib; ${MAKE} install && ${MAKE} install.man

install-linkkit:
.if (${MACHINE} == "i386") 
	cd xc; ${MAKE} install.linkkit
.endif

install-distrib:
	cd distrib/notes; ${MAKE} install

fix-appd:
	# Make sure /usr/local/lib/X11/app-defaults is a link
	if [ ! -L $(DESTDIR)${LOCALAPPD} ]; then \
	    if [ -d $(DESTDIR)${LOCALAPPD} ]; then \
		mv $(DESTDIR)${LOCALAPPD}/* $(DESTDIR)${REALAPPD}; \
		rmdir $(DESTDIR)${LOCALAPPD}; \
	    fi; \
	    mkdir -p ${DESTDIR}${LOCALAPPX}; \
	    ln -s ${REALAPPD} ${DESTDIR}${LOCALAPPD}; \
	fi

clean:
	cd xc; ${MAKE} clean
.if exists(contrib/Makefile)
	cd contrib; ${MAKE} clean
.endif

distclean:
	${MAKE} clean
	find xc contrib -name Makefile -mindepth 2 -exec ${RM} -f '{}' ';'
	find xc contrib -name .depend -exec ${RM} -f '{}' ';'
	rm -f xc/xmakefile contrib/Makefile

b-r:
	@echo ${.CURDIR}/build-release
	@env DESTDIR=${DESTDIR} RELEASEDIR=${RELEASEDIR} ${.CURDIR}/build-release

.PHONY: all all-contrib build release dist install install-xc \
    install-contrib install-linkkit install-distrib clean distclean \
    fix-appd b-r
