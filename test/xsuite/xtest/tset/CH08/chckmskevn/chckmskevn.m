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
 * $XConsortium: chckmskevn.m,v 1.9 94/04/17 21:07:13 rws Exp $
 */
>>TITLE XCheckMaskEvent CH08
Bool
XCheckMaskEvent(display, event_mask, event_return)
Display *display = Dsp;
long event_mask;
XEvent	*event_return = &_event;
>>EXTERN
/*
 * Can not use "xcall" because it empties the event queue.
 */
#define	_xcall_(rvalue)	\
		_startcall(display);\
		rvalue = XCheckMaskEvent(display, event_mask, event_return);\
		_endcall(display)
static XEvent _event;
>>ASSERTION Good A
A call to xname
returns in
.A event_return
the first event in the event queue matching
.A event_mask .
>>STRATEGY
Discard all events on the event queue.
Call XPutBackEvent to put events on the event queue.
Call XCheckMaskEvent.
Verify that XCheckMaskEvent returned True.
Verify the correct event-type was returned.
Verify the first matching event in event queue was returned.
>>CODE
XEvent	event;
XAnyEvent *ep;
Bool	return_value;

/* Discard all events on the event queue. */
	XSync(display, True);
/* Call XPutBackEvent to put events on the event queue. */
	ep = (XAnyEvent *) &event;
	ep->type = KeyPress;
	ep->send_event = False;
	XPutBackEvent(display, &event);
	ep->type = ButtonPress;
	ep->send_event = False;
	XPutBackEvent(display, &event);
	ep->type = ButtonPress;
	ep->send_event = True;	/* first occurrence has send_event True */
	XPutBackEvent(display, &event);
	ep->type = KeyPress;
	ep->send_event = False;
	XPutBackEvent(display, &event);
/* Call XCheckMaskEvent. */
	event_mask = ButtonPressMask;
	_xcall_(return_value);
/* Verify that XCheckMaskEvent returned True. */
	if (return_value != True) {	
		report("Did not return True: returned %d", return_value);
		FAIL;
	}
	else
		CHECK;
/* Verify the correct event-type was returned. */
	ep = (XAnyEvent *) event_return;
	if (ep->type != ButtonPress) {
		report("Got %s, expected %s", eventname(ep->type), eventname(ButtonPress));
		FAIL;
	}
	else
		CHECK;
/* Verify the first matching event in event queue was returned. */
	if (ep->send_event != True) {
		report("First event in event queue was not returned.");
		FAIL;
	}
	else
		CHECK;
	CHECKPASS(3);
>>ASSERTION def
When a call to xname finds a matching event,
then
xname
returns
.S True .
>>ASSERTION Good A
A call to xname removes the returned event from the event queue.
>>STRATEGY
Discard all events on the event queue.
Call XPutBackEvent to put events on the event queue.
Call XPending to get the current event queue size.
Call XCheckMaskEvent.
Call XPending to get the current event queue size.
Verify that size of event queue has decreased by one.
>>CODE
XEvent	event;
int	oldqsize;
int	newqsize;
Bool	return_value;

/* Discard all events on the event queue. */
	XSync(display, True);
/* Call XPutBackEvent to put events on the event queue. */
	event.type = ButtonPress;
	XPutBackEvent(display, &event);
/* Call XPending to get the current event queue size. */
	oldqsize = XPending(display);
/* Call XCheckMaskEvent. */
	event_mask = ButtonPressMask;
	_xcall_(return_value);
/* Call XPending to get the current event queue size. */
	newqsize = XPending(display);
/* Verify that size of event queue has decreased by one. */
	if (newqsize != (oldqsize-1)) {
		report("Event queue size %d, expected %d", newqsize, oldqsize-1);
		FAIL;
	}
	else
		CHECK;
	CHECKPASS(1);
>>ASSERTION def
When a matching event is not in the event queue,
then a call to xname
returns in
.A event_return
the first matching event available on the X server connection.
>>ASSERTION Good A
When a matching event is not in the event queue and
is not available on the X server connection,
then a call to xname
returns
.S False .
>>STRATEGY
Discard all events on the event queue.
Call XCheckMaskEvent.
Verify that XCheckMaskEvent returned False.
>>CODE
Bool	return_value;

/* Discard all events on the event queue. */
	XSync(display, True);
/* Call XCheckMaskEvent. */
	event_mask = ButtonPressMask;
	_xcall_(return_value);
/* Verify that XCheckMaskEvent returned False. */
	if (return_value != False) {	
		report("Did not return False: returned %d", return_value);
		FAIL;
	}
	else
		CHECK;
	CHECKPASS(1);
>>ASSERTION Good A
When a matching event is not in the event queue and
is not available on the X server connection,
then a call to xname
flushes the output buffer.
>>STRATEGY
Create client2.
Discard all events on the event queue.
Create pixmap.
Call XCheckMaskEvent.
Verify that XCheckMaskEvent returned False.
Empty the buffer.
Ensure the server has dealt with anything flushed to it: do XSync()
Verify that the output buffer was flushed by effect on server.
>>CODE
Bool	return_value;
Pixmap	pm;
Display *client2;

/* Create client2. */
	client2 = opendisplay();
	if (client2 == (Display *) NULL) {
		delete("Can not open display");
		return;
	}
	else
		CHECK;
/* Discard all events on the event queue. */
	XSync(display, True);
/* Create pixmap. */
	/* avoid using makepixm() */
	pm = XCreatePixmap(display, DRW(display), 10, 10, 1);
/* Call XCheckMaskEvent. */
	event_mask = ButtonPressMask;
	_xcall_(return_value);
/* Verify that XCheckMaskEvent returned False. */
	if (return_value != False) {	
		report("Did not return False: returned %d", return_value);
		FAIL;
	}
	else
		CHECK;
/* Empty the buffer. */
	(void)XTestDiscard(display);
/* Ensure the server has dealt with anything flushed to it: do XSync() */
	XSync(display, False);
/* Verify that the output buffer was flushed by effect on server. */
	_startcall(client2);
	XFreePixmap(client2, pm);
	XSync(client2, True);
	_endcall(client2);
	if (geterr() != Success) {
		report("The output buffer was not flushed.");
		XFreePixmap(display, pm);
		FAIL;
	}
	else
		CHECK;
	CHECKPASS(3);
