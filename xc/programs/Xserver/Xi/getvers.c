/* $XConsortium: getvers.c,v 1.8 94/04/17 20:33:13 rws Exp $ */
/* $XFree86: xc/programs/Xserver/Xi/getvers.c,v 3.0 1996/03/29 22:13:34 dawes Exp $ */

/************************************************************

Copyright (c) 1989  X Consortium

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

Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, California.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Hewlett-Packard not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
HEWLETT-PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * Extension function to return the version of the extension.
 *
 */

#define	 NEED_EVENTS
#define	 NEED_REPLIES
#include "X.h"				/* for inputstr.h    */
#include "Xproto.h"			/* Request macro     */
#include "inputstr.h"			/* DeviceIntPtr	     */
#include "XI.h"
#include "XIproto.h"
#include "extnsionst.h"
#include "extinit.h"			/* LookupDeviceIntRec */
#include "exglobals.h"

#include "getvers.h"

XExtensionVersion	AllExtensionVersions[128];

/***********************************************************************
 *
 * Handle a request from a client with a different byte order than us.
 *
 */

int
SProcXGetExtensionVersion(client)
    register ClientPtr client;
    {
    register char n;

    REQUEST(xGetExtensionVersionReq);
    swaps(&stuff->length, n);
    REQUEST_AT_LEAST_SIZE(xGetExtensionVersionReq);
    swaps(&stuff->nbytes, n);
    return(ProcXGetExtensionVersion(client));
    }

/***********************************************************************
 *
 * This procedure lists the input devices available to the server.
 *
 */

int
ProcXGetExtensionVersion (client)
    register ClientPtr client;
    {
    xGetExtensionVersionReply	rep;

    REQUEST(xGetExtensionVersionReq);
    REQUEST_AT_LEAST_SIZE(xGetExtensionVersionReq);

    if (stuff->length != (sizeof(xGetExtensionVersionReq) + 
	stuff->nbytes + 3)>>2)
	{
	SendErrorToClient(client, IReqCode, X_GetExtensionVersion, 0, 
		BadLength);
	return Success;
	}

    rep.repType = X_Reply;
    rep.RepType = X_GetExtensionVersion;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.major_version = 0;
    rep.minor_version = 0;

    rep.present = TRUE;
    if (rep.present)
	{
	rep.major_version = 
	    AllExtensionVersions[IReqCode-128].major_version;
	rep.minor_version = 
	    AllExtensionVersions[IReqCode-128].minor_version;
	}
    WriteReplyToClient (client, sizeof (xGetExtensionVersionReply), &rep);

    return Success;
    }

/***********************************************************************
 *
 * This procedure writes the reply for the XGetExtensionVersion function,
 * if the client and server have a different byte ordering.
 *
 */

void
SRepXGetExtensionVersion (client, size, rep)
    ClientPtr	client;
    int		size;
    xGetExtensionVersionReply	*rep;
    {
    register char n;

    swaps(&rep->sequenceNumber, n);
    swapl(&rep->length, n);
    swaps(&rep->major_version, n);
    swaps(&rep->minor_version, n);
    WriteToClient(client, size, (char *)rep);
    }
