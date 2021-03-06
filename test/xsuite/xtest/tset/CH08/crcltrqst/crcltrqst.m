/*
 
Copyright (c) 1990, 1991  X Consortium

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

 *
 * Copyright 1990, 1991 by UniSoft Group Limited.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of UniSoft not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  UniSoft
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * $XConsortium: crcltrqst.m,v 1.13 94/04/17 21:07:22 rws Exp $
 */
>>TITLE CirculateRequest CH08
>>EXTERN
#define	EVENT		CirculateRequest
#define	MASK		SubstructureRedirectMask
>>ASSERTION Good A
When a xname event is generated,
then
all clients having set
.S SubstructureRedirectMask
event mask bits on the parent of the window
for which the circulate request was issued are delivered
a xname event.
>>STRATEGY
Create clients client2 and client3.
Build and create window hierarchy.
Create inferiors with override-redirect set to True.
Change one inferior's override-redirect attribute to False.
Select for CirculateRequest events using SubstructureRedirectMask.
Select for no events with client3.
Circulate lowest window to top.
Initialize for expected events.
Verify that a CirculateRequest event is delivered.
Verify that no events are delivered to client3.
Verify members of event structure.
Verify that no events were delivered to client3.
Circulate highest window (override-redirect set to False) to bottom.
Initialize for expected events.
Verify that a CirculateRequest event is delivered.
Verify that no events are delivered to client3.
Verify members of event structure.
Verify that no events were delivered to client3.
>>CODE
Display	*display = Dsp;
Display	*client2, *client3;
Winh	*parent, *lastw, *winh;
Winhg	winhg;
XEvent	event;
int	i;
int	status;
int	numchildren = 4;
int	count;
XSetWindowAttributes attrs;
unsigned long valuemask;

/* Create clients client2 and client3. */
	if ((client2 = opendisplay()) == (Display *) NULL) {
		delete("Couldn't create client2.");
		return;
	}
	else
		CHECK;
	if ((client3 = opendisplay()) == (Display *) NULL) {
		delete("Couldn't create client3.");
		return;
	}
	else
		CHECK;
/* Build and create window hierarchy. */
	/* can't use winh() because the windows need to overlap */
	winhg.border_width = 1;
	winhg.area.x = 1;
	winhg.area.y = 1;
	winhg.area.width = 100;
	winhg.area.height = 100;
	parent = winh_adopt(display, (Winh *) NULL, 0L, (XSetWindowAttributes *) NULL, &winhg, WINH_NOMASK);
	if (parent == (Winh *) NULL) {
		report("Could not create parent");
		return;
	}
	else
		CHECK;
	winhg.area.x = 1;
	winhg.area.y = 1;
	winhg.area.width = 30;
	winhg.area.height = 30;
/* Create inferiors with override-redirect set to True. */
	attrs.override_redirect = True;
	attrs.border_pixel = W_FG;
	attrs.background_pixel = W_BG;
	valuemask = CWOverrideRedirect | CWBorderPixel | CWBackPixel;
	for (i=0; i<numchildren; i++) {
		if (!i)
			CHECK;
		lastw = winh_adopt(display, parent, valuemask, &attrs, &winhg, WINH_NOMASK);
		if (lastw == (Winh *) NULL) {
			report("Could not create child %d", i);
			return;
		}
		winhg.area.x += 10;
		winhg.area.y += 10;
	}
	if (winh_create(display, (Winh *) NULL, WINH_MAP))
		return;
	else
		CHECK;
/* Change one inferior's override-redirect attribute to False. */
	attrs.override_redirect = False;
	valuemask = CWOverrideRedirect;
	if (winh_changewindowattributes(display, lastw, valuemask, &attrs)) {
		report("Failed to change attribute for subwindow");
		return;
	}
/* Select for CirculateRequest events using SubstructureRedirectMask. */
	/*
	 * Selection with a single client because only one can select
	 * for this event at a time.
	 */
	if (winh_selectinput(display, parent, MASK)) {
		report("Selection failed.");
		return;
	}
	else
		CHECK;
/* Select for no events with client3. */
	if (winh_selectinput(client3, (Winh *) NULL, NoEventMask)) {
		report("Selection with client3 failed.");
		return;
	}
	else
		CHECK;
/* Circulate lowest window to top. */
	XSync(display, True);
	XSync(client2, True);
	XSync(client3, True);
	XCirculateSubwindows(client2, parent->window, RaiseLowest);
	XSync(client2, False);
	XSync(display, False);
	XSync(client3, False);
/* Initialize for expected events. */
	event.xany.type = EVENT;
	event.xany.window = parent->window;
	if (winh_plant(parent, &event, MASK, WINH_NOMASK)) {
		report("Could not plant events for parent");
		return;
	}
	else
		CHECK;
/* Verify that a CirculateRequest event is delivered. */
/* Verify that no events are delivered to client3. */
	if (winh_harvest(display, (Winh *) NULL)) {
		report("Could not harvest events");
		return;
	}
	else
		CHECK;
	if (winh_harvest(client3, (Winh *) NULL)) {
		report("Could not harvest events for client3");
		return;
	}
	else
		CHECK;
	status = winh_weed((Winh *) NULL, -1, WINH_WEED_IDENTITY);
	if (status == -1)
		return;
	else if (status) {
		report("Event delivery not as expected");
		FAIL;
	}
	else {
		XEvent	*event_return;
		XCirculateRequestEvent	good;

/* Verify members of event structure. */
		event_return = winh_qdel->event;
		good = event_return->xcirculaterequest;
		good.type = EVENT;
		good.send_event = False;
		good.display = display;
		good.parent = parent->window;
		good.window = parent->firstchild->window;
		good.place = PlaceOnTop;
		if (checkevent((XEvent *) &good, event_return)) {
			report("Unexpected values in delivered event");
			FAIL;
		}
		else
			CHECK;
	}
/* Verify that no events were delivered to client3. */
	count = XPending(client3);
	if (count != 0) {
		report("Got %d events, expected %d for client3", count, 0);
		FAIL;
	}
	else
		CHECK;
/* Circulate highest window (override-redirect set to False) to bottom. */
	XSync(display, True);
	XSync(client2, True);
	XSync(client3, True);
	XCirculateSubwindows(client2, parent->window, LowerHighest);
	XSync(client2, False);
	XSync(display, False);
	XSync(client3, False);
/* Initialize for expected events. */
	event.xany.type = EVENT;
	event.xany.window = parent->window;
	if (winh_plant(parent, &event, MASK, WINH_NOMASK)) {
		report("Could not plant events for parent");
		return;
	}
	else
		CHECK;
/* Verify that a CirculateRequest event is delivered. */
/* Verify that no events are delivered to client3. */
	if (winh_harvest(display, (Winh *) NULL)) {
		report("Could not harvest events");
		return;
	}
	else
		CHECK;
	if (winh_harvest(client3, (Winh *) NULL)) {
		report("Could not harvest events for client3");
		return;
	}
	else
		CHECK;
	status = winh_weed((Winh *) NULL, -1, WINH_WEED_IDENTITY);
	if (status == -1)
		return;
	else if (status) {
		report("Event delivery not as expected");
		FAIL;
	}
	else {
		XEvent	*event_return;
		XCirculateRequestEvent	good;

/* Verify members of event structure. */
		event_return = winh_qdel->event;
		good = event_return->xcirculaterequest;
		good.type = EVENT;
		good.send_event = False;
		good.display = display;
		good.parent = parent->window;
		good.window = lastw->window;
		good.place = PlaceOnBottom;
		if (checkevent((XEvent *) &good, event_return)) {
			report("Unexpected values in delivered event");
			FAIL;
		}
		else
			CHECK;
	}
/* Verify that no events were delivered to client3. */
	count = XPending(client3);
	if (count != 0) {
		report("Got %d events, expected %d for client3", count, 0);
		FAIL;
		return;
	}
	else
		CHECK;

	CHECKPASS(17);
>>ASSERTION def
>>#NOTE Tested for in previous test.
When a xname event is generated,
then
clients not having set
.S SubstructureRedirectMask
event mask bits on the event window are not delivered
a xname event.
>>#NOTEm >>ASSERTION
>>#NOTEm When a client calls
>>#NOTEm .F XCirculateSubwindows ,
>>#NOTEm .F XCirculateSubwindowsUp ,
>>#NOTEm or
>>#NOTEm .F XCirculateSubwindowsDown
>>#NOTEm and a subwindow actually needs to be restacked,
>>#NOTEm then ARTICLE xname event is generated.
>>#NOTEm >>ASSERTION
>>#NOTEm When ARTICLE xname event is generated
>>#NOTEm and a client has selected xname events on the child's parent
>>#NOTEm and the override-redirect attribute of the child window is set to
>>#NOTEm .S False ,
>>#NOTEm then
>>#NOTEm .M window
>>#NOTEm is not WINDOWTYPE.
>>#NOTEs >>ASSERTION
>>#NOTEs When ARTICLE xname event is delivered,
>>#NOTEs then
>>#NOTEs .M type
>>#NOTEs is set to
>>#NOTEs xname.
>>#NOTEs >>ASSERTION
>>#NOTEs >>#NOTE The method of expansion is not clear.
>>#NOTEs When ARTICLE xname event is delivered,
>>#NOTEs then
>>#NOTEs .M serial
>>#NOTEs is set
>>#NOTEs from the serial number reported in the protocol
>>#NOTEs but expanded from the 16-bit least-significant bits
>>#NOTEs to a full 32-bit value.
>>#NOTEm >>ASSERTION
>>#NOTEm When ARTICLE xname event is delivered
>>#NOTEm and the event came from a
>>#NOTEm .S SendEvent
>>#NOTEm protocol request,
>>#NOTEm then
>>#NOTEm .M send_event
>>#NOTEm is set to
>>#NOTEm .S True .
>>#NOTEs >>ASSERTION
>>#NOTEs When ARTICLE xname event is delivered
>>#NOTEs and the event was not generated by a
>>#NOTEs .S SendEvent
>>#NOTEs protocol request,
>>#NOTEs then
>>#NOTEs .M send_event
>>#NOTEs is set to
>>#NOTEs .S False .
>>#NOTEs >>ASSERTION
>>#NOTEs When ARTICLE xname event is delivered,
>>#NOTEs then
>>#NOTEs .M display
>>#NOTEs is set to
>>#NOTEs a pointer to the display on which the event was read.
>>#NOTEs >>ASSERTION
>>#NOTEs When ARTICLE xname event is delivered,
>>#NOTEs then
>>#NOTEs .M parent
>>#NOTEs is set to
>>#NOTEs the parent window of
>>#NOTEs .M window .
>>#NOTEs >>ASSERTION
>>#NOTEs When ARTICLE xname event is delivered,
>>#NOTEs then
>>#NOTEs .M window
>>#NOTEs is set to
>>#NOTEs the child window to be WINDOWTYPE.
>>#NOTEs >>ASSERTION
>>#NOTEs When ARTICLE xname event is delivered
>>#NOTEs and the new position of
>>#NOTEs .M window
>>#NOTEs should be on top of all siblings,
>>#NOTEs then
>>#NOTEs .M place
>>#NOTEs is set to
>>#NOTEs .S PlaceOnTop .
>>#NOTEs >>ASSERTION
>>#NOTEs When ARTICLE xname event is delivered
>>#NOTEs and the new position of
>>#NOTEs .M window
>>#NOTEs should be below all siblings,
>>#NOTEs then
>>#NOTEs .M place
>>#NOTEs is set to
>>#NOTEs .S PlaceOnBottom .
