#include "macbsd.h"
#include "dixstruct.h"
#include "dix.h"
#include "opaque.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


Bool macbsdMonoSave(
	ScreenPtr	screen,
	int	on)
{
	/* if(on == SCREEN_SAVER_FORCER)
		Do something with event time */
	return(FALSE);
}



Bool macbsdMonoInit(
	int	index,
	ScreenPtr	screen,
	int	argc,
	char	**argv)
{
	struct grfmode *id;
	char scrstr[150];

	if(mac_fbs[index].added)
		return(TRUE);

	screen->SaveScreen = macbsdMonoSave;
	screen->whitePixel = 0;
	screen->blackPixel = 1;

	id = &mac_fbs[index].idata;
	printf("Calling ScreenInit to add screen %d...\n", index);
	sprintf(scrstr, "Screen %d at %#08x, %d by %d, rowB %d, fbbase %#x.\n",
		index, mac_fbs[index].vaddr, id->width,
		id->height, id->rowbytes, id->fbbase);
	ErrorF(scrstr);
	if(!mfbScreenInit(screen,
		mac_fbs[index].vaddr,		/* BARF */
		id->width,
		id->height,
		/* id->vRes >> 16 */ 75,	/* BARF */
		/* id->vRes >> 16 */ 75,	/* BARF */
		id->rowbytes*8))
			return(FALSE);
	mac_fbs[index].added = 1;
	return(macbsd_screeninit(screen) && mfbCreateDefColormap(screen));
}
