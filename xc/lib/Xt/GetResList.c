/* $TOG: GetResList.c /main/8 1997/05/15 17:29:43 kaleb $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
Copyright 1993 by Sun Microsystems, Inc. Mountain View, CA.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or Sun not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/
/* $XFree86: xc/lib/Xt/GetResList.c,v 3.0.4.2 1998/10/04 13:36:34 hohndel Exp $ */

/*

Copyright (c) 1987, 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

*/

#include "IntrinsicI.h"
#include "Intrinsic.h"

/*
 * XtGetResourceList(), XtGetConstraintResourceList()
 */

#define	TOXRMQUARK(p) ((XrmQuark)(long)(p))	/* avoid LP64 warnings */

void XtGetResourceList(widget_class, resources, num_resources)
	WidgetClass widget_class;
	XtResourceList *resources;
	Cardinal *num_resources;
{
	int size;
	register int i, dest = 0;
	register XtResourceList *list, dlist;

	LOCK_PROCESS;
	size = widget_class->core_class.num_resources * sizeof(XtResource);
	*resources = (XtResourceList) __XtMalloc((unsigned) size);

	if (!widget_class->core_class.class_inited) {
	    /* Easy case */

	    (void) memmove((char *) *resources, 
			   (char *)widget_class->core_class.resources, size);
	    *num_resources = widget_class->core_class.num_resources;
	    UNLOCK_PROCESS;
	    return;
	}

	/* Nope, it's the hard case */

	list = (XtResourceList *) widget_class->core_class.resources;
	dlist = *resources;
	for (i = 0; i < widget_class->core_class.num_resources; i++) {
	    if (list[i] != NULL) {
		dlist[dest].resource_name = (String)
			XrmQuarkToString(TOXRMQUARK(list[i]->resource_name));
		dlist[dest].resource_class = (String) 
			XrmQuarkToString(TOXRMQUARK(list[i]->resource_class));
		dlist[dest].resource_type = (String)
			XrmQuarkToString(TOXRMQUARK(list[i]->resource_type));
		dlist[dest].resource_size = list[i]->resource_size;
		/* trust that resource_offset isn't that big */
		dlist[dest].resource_offset = (Cardinal)
			-((int)(list[i]->resource_offset + 1));
		dlist[dest].default_type = (String)
			XrmQuarkToString(TOXRMQUARK(list[i]->default_type));
		dlist[dest].default_addr = list[i]->default_addr;
		dest++;
	    }
	}
	*num_resources = dest;
	UNLOCK_PROCESS;
}


static Boolean ClassIsSubclassOf(class, superclass)
    WidgetClass class, superclass;
{
    for (; class != NULL; class = class->core_class.superclass) {
	if (class == superclass) return True;
    }
    return False;
}

void XtGetConstraintResourceList(widget_class, resources, num_resources)
	WidgetClass widget_class;
	XtResourceList *resources;
	Cardinal *num_resources;
{
	int size;
	register int i, dest = 0;
	register XtResourceList *list, dlist;
	ConstraintWidgetClass class = (ConstraintWidgetClass)widget_class;

	LOCK_PROCESS;
	if (   (class->core_class.class_inited &&
		!(class->core_class.class_inited & ConstraintClassFlag))
	    || (!class->core_class.class_inited &&
		!ClassIsSubclassOf(widget_class, constraintWidgetClass))
	    || class->constraint_class.num_resources == 0) {

	    *resources = NULL;
	    *num_resources = 0;
	    UNLOCK_PROCESS;
	    return;
	}

	size = class->constraint_class.num_resources * sizeof(XtResource);
	*resources = (XtResourceList) __XtMalloc((unsigned) size);

	if (!class->core_class.class_inited) {
	    /* Easy case */

	    (void) memmove((char *) *resources, 
			   (char *)class->constraint_class.resources, size);
	    *num_resources = class->constraint_class.num_resources;
	    UNLOCK_PROCESS;
	    return;
	}

	/* Nope, it's the hard case */

	list = (XtResourceList *) class->constraint_class.resources;
	dlist = *resources;
	for (i = 0; i < class->constraint_class.num_resources; i++) {
	    if (list[i] != NULL) {
		dlist[dest].resource_name = (String)
			XrmQuarkToString(TOXRMQUARK(list[i]->resource_name));
		dlist[dest].resource_class = (String) 
			XrmQuarkToString(TOXRMQUARK(list[i]->resource_class));
		dlist[dest].resource_type = (String)
			XrmQuarkToString(TOXRMQUARK(list[i]->resource_type));
		dlist[dest].resource_size = list[i]->resource_size;
		/* trust that resource_offset isn't that big */
		dlist[dest].resource_offset = (Cardinal)
			-((int)(list[i]->resource_offset + 1));
		dlist[dest].default_type = (String)
			XrmQuarkToString(TOXRMQUARK(list[i]->default_type));
		dlist[dest].default_addr = list[i]->default_addr;
		dest++;
	    }
	}
	*num_resources = dest;
	UNLOCK_PROCESS;
}
