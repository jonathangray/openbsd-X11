/*-
 * idlefile.c --
 *	Idle file time stamping for xautolock
 *
 * Copyright (c) 1995 by Andreas Stolcke
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any non-commercial purpose and without fee is hereby
 * granted, provided that the above copyright notice appears in all copies.
 * Andreas Stolcke makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 */
#ifndef lint
static char *rcsid =
"$Id: idlefile.c,v 1.1.1.1 1998/03/28 20:28:35 matthieu Exp $ ICSI (Berkeley)";
#endif /* not lint */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#ifndef W_OK
#include <unistd.h>
#endif
#include <fcntl.h>
#include <utime.h>
#include <errno.h>

extern int errno;

static char *idlefile = NULL;

/*
 * Establish a file for idle time stamping
 */
int
IdleFileInit(path)
	char *path;
{
	int fd = -1;

	if (access(path, W_OK) < 0 &&
            (errno != ENOENT ||
	     /*
	      * Make file unreable/unwritable to prevent accidental access.
	      */
	     (fd = creat(path, 0000)) < 0)) {
		perror(path);
		return -1;
	} 
	
	if (fd >= 0) close(fd);

	idlefile = path;

	/*
	 * Check that we can actually set atimes on the thing
	 */
        return IdleFileStamp();
}

/*
 * Update the idle file time stamp
 */
int
IdleFileStamp()
{
	struct stat statb;
	struct utimbuf utimb;

#ifdef DEBUG
	fprintf(stderr, "stamp");
	fflush(stderr);
#endif
	if (!idlefile)
		return -1;

	if (stat(idlefile, &statb) < 0) {
		perror(idlefile);
		return -1;
	}

	/*
	 * leave mtime unchanged, set atime to now
	 */
	utimb.modtime = statb.st_mtime;
	utimb.actime = time((time_t *)0);

	if (utime(idlefile, &utimb) < 0) {
		perror(idlefile);
		return -1;
	}
	return 0;
}

/*
 * Stop using idle file
 */
int
IdleFileQuit()
{
	idlefile = NULL;
	return 0;
}

