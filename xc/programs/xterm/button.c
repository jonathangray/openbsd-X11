/* $TOG: button.c /main/76 1997/07/30 16:56:19 kaleb $ */
/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* $XFree86: xc/programs/xterm/button.c,v 3.27 1998/11/01 12:57:07 dawes Exp $ */

/*
button.c	Handles button events in the terminal emulator.
		does cut/paste operations, change modes via menu,
		passes button events through to some applications.
				J. Gettys.
*/

#include <xterm.h>

#include <X11/Xatom.h>

#include <stdio.h>

#ifdef MINIX
#include <X11/Xos.h>
#endif

#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/StdSel.h>

#include <data.h>
#include <error.h>
#include <menu.h>
#include <xcharmouse.h>

#define KeyState(x) (((x) & (ShiftMask|ControlMask)) + (((x) & Mod1Mask) ? 2 : 0))
    /* adds together the bits:
        shift key -> 1
        meta key  -> 2
        control key -> 4 */

#define TEXTMODES 4
#define NBUTS 3
#define DIRS 2
#define UP 1
#define DOWN 0
#define SHIFTS 8		/* three keys, so eight combinations */
#define	Coordinate(r,c)		((r) * (term->screen.max_col+1) + (c))


/* Selection/extension variables */

/* Raw char position where the selection started */
static int rawRow, rawCol;

/* Selected area before CHAR, WORD, LINE selectUnit processing */
static int startRRow, startRCol, endRRow, endRCol = 0;

/* Selected area after CHAR, WORD, LINE selectUnit processing */
static int startSRow, startSCol, endSRow, endSCol = 0;

/* Valid rows for selection clipping */
static int firstValidRow, lastValidRow;

/* Start, end of extension */
static int startERow, startECol, endERow, endECol;

/* Saved values of raw selection for extend to restore to */
static int saveStartRRow, saveStartRCol, saveEndRRow, saveEndRCol;

/* Saved value of WORD selection for LINE processing to restore to */
static int saveStartWRow, saveStartWCol;

/* Multi-click handling */
static int numberOfClicks = 0;
static Time lastButtonUpTime = 0;
typedef int SelectUnit;
#define SELECTCHAR 0
#define SELECTWORD 1
#define SELECTLINE 2
#define NSELECTUNITS 3
static SelectUnit selectUnit;

/* Send emacs escape code when done selecting or extending? */
static int replyToEmacs;

static char *SaveText (TScreen *screen, int row, int scol, int ecol, char *lp, int *eol);
static int Length (TScreen *screen, int row, int scol, int ecol);
static void ComputeSelect (int startRow, int startCol, int endRow, int endCol, Bool extend);
static void EditorButton (XButtonEvent *event);
static void EndExtend (Widget w, XEvent *event, String *params, Cardinal num_params, Bool use_cursor_loc);
static void ExtendExtend (int row, int col);
static void PointToRowCol (int y, int x, int *r, int *c);
static void ReHiliteText (int frow, int fcol, int trow, int tcol);
static void SaltTextAway (int crow, int ccol, int row, int col, String *params, Cardinal num_params);
static void SelectSet (Widget w, XEvent *event, String *params, Cardinal num_params);
static void SelectionReceived PROTO_XT_SEL_CB_ARGS;
static void StartSelect (int startrow, int startcol);
static void TrackDown (XButtonEvent *event);
static void _OwnSelection (XtermWidget termw, String *selections, Cardinal count);

Boolean SendMousePosition(Widget w, XEvent* event)
{
    TScreen *screen;

    if (!IsXtermWidget(w))
    	return False;

    screen = &((XtermWidget)w)->screen;

    /* If send_mouse_pos mode isn't on, we shouldn't be here */
    if (screen->send_mouse_pos == MOUSE_OFF)
        return False;

    /* Make sure the event is an appropriate type */
    if ((screen->send_mouse_pos != BTN_EVENT_MOUSE)
     && (screen->send_mouse_pos != ANY_EVENT_MOUSE)
     && event->type != ButtonPress
     && event->type != ButtonRelease)
	return False;

#define KeyModifiers \
    (event->xbutton.state & (ShiftMask | LockMask | ControlMask | Mod1Mask | \
			     Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask ))

#define ButtonModifiers \
    (event->xbutton.state & (ShiftMask | LockMask | ControlMask | Mod1Mask | \
			     Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask ))

    switch (screen->send_mouse_pos) {
      case X10_MOUSE: /* X10 compatibility sequences */

	if (KeyModifiers == 0) {
	    if (event->type == ButtonPress)
		EditorButton((XButtonEvent *)event);
	    return True;
	}
	return False;

      case VT200_MOUSE: /* DEC vt200 compatible */

	if (KeyModifiers == 0 || KeyModifiers == ControlMask) {
	    EditorButton((XButtonEvent *)event);
	    return True;
	}
	return False;

      case VT200_HIGHLIGHT_MOUSE: /* DEC vt200 hilite tracking */
	if (  event->type == ButtonPress &&
	      KeyModifiers == 0 &&
	      event->xbutton.button == Button1 ) {
	    TrackDown((XButtonEvent *)event);
	    return True;
	}
	if (KeyModifiers == 0 || KeyModifiers == ControlMask) {
	    EditorButton((XButtonEvent *)event);
	    return True;
	}
	return False;

      /* xterm extension for motion reporting. June 1998 */
      /* EditorButton() will distinguish between the modes */
      case BTN_EVENT_MOUSE:
      case ANY_EVENT_MOUSE:
	if (KeyModifiers == 0 || KeyModifiers == ControlMask) {
	    EditorButton((XButtonEvent *)event);
	    return True;
	}
	return False;

      default:
	return False;
    }
#undef KeyModifiers
}

void
DiredButton(
	Widget w GCC_UNUSED,
	XEvent *event,			/* must be XButtonEvent */
	String *params GCC_UNUSED,	/* selections */
	Cardinal *num_params GCC_UNUSED)
{	/* ^XM-G<line+' '><col+' '> */
	register TScreen *screen = &term->screen;
	int pty = screen->respond;
	char Line[ 6 ];
	register unsigned line, col;

    if (event->type != ButtonPress && event->type != ButtonRelease)
    	return;
    strcpy( Line, "\030\033G  " );

	line = ( event->xbutton.y - screen->border ) / FontHeight( screen );
	col  = ( event->xbutton.x - OriginX(screen)) / FontWidth( screen );
	Line[3] = ' ' + col;
	Line[4] = ' ' + line;
	v_write(pty, Line, 5 );
}

void
ViButton(
	Widget w GCC_UNUSED,
	XEvent *event,			/* must be XButtonEvent */
	String *params GCC_UNUSED,	/* selections */
	Cardinal *num_params GCC_UNUSED)
{	/* ^XM-G<line+' '><col+' '> */
	register TScreen *screen = &term->screen;
	int pty = screen->respond;
	char Line[ 6 ];
	register int line;

    if (event->type != ButtonPress && event->type != ButtonRelease)
    	return;

	line = screen->cur_row -
		(( event->xbutton.y - screen->border ) / FontHeight( screen ));
/* fprintf( stderr, "xtdb line=%d\n", line ); */
	if ( ! line ) return;
	Line[ 1 ] = 0;
	Line[ 0 ] = 27;
	v_write(pty, Line, 1 );

	Line[ 0 ] = 'p' & 0x1f;

	if ( line < 0 )
	{	line = -line;
		Line[ 0 ] = 'n' & 0x1f;
	}
	while ( --line >= 0 ) v_write(pty, Line, 1 );
}


/*
 * This function handles button-motion events
 */
/*ARGSUSED*/
void HandleSelectExtend(
	Widget w,
	XEvent *event,		/* must be XMotionEvent */
	String *params GCC_UNUSED,
	Cardinal *num_params GCC_UNUSED)
{
	register TScreen *screen;
	int row, col;

	if (!IsXtermWidget(w))
		return;

	screen = &((XtermWidget)w)->screen;
	screen->selection_time = event->xmotion.time;
	switch (eventMode) {
		/* If not in one of the DEC mouse-reporting modes */
		case LEFTEXTENSION :
		case RIGHTEXTENSION :
			PointToRowCol (event->xmotion.y, event->xmotion.x,
				       &row, &col);
			ExtendExtend (row, col);
			break;

		/* If in motion reporting mode, send mouse position to
		   character process as a key sequence \E[M... */
		case NORMAL :
			/* will get here if send_mouse_pos != MOUSE_OFF */
			if ( screen->send_mouse_pos == BTN_EVENT_MOUSE
			 ||  screen->send_mouse_pos == ANY_EVENT_MOUSE )
			    SendMousePosition(w,event);
		        break;
	}
}

static void do_select_end (
	Widget w,
	XEvent *event,		/* must be XButtonEvent */
	String *params,		/* selections */
	Cardinal *num_params,
	Bool use_cursor_loc)
{
	if (!IsXtermWidget(w))
		return;

	((XtermWidget)w)->screen.selection_time = event->xbutton.time;
	switch (eventMode) {
		case NORMAL :
		    (void) SendMousePosition(w, event);
		    break;
		case LEFTEXTENSION :
		case RIGHTEXTENSION :
		    EndExtend(w, event, params, *num_params, use_cursor_loc);
		    break;
	}
}


void HandleSelectEnd(
	Widget w,
	XEvent *event,		/* must be XButtonEvent */
	String *params,		/* selections */
	Cardinal *num_params)
{
	do_select_end (w, event, params, num_params, False);
}


void HandleKeyboardSelectEnd(
	Widget w,
	XEvent *event,		/* must be XButtonEvent */
	String *params,		/* selections */
	Cardinal *num_params)
{
	do_select_end (w, event, params, num_params, True);
}




struct _SelectionList {
    String *params;
    Cardinal count;
    Time time;
};


static void _GetSelection(
	Widget w,
	Time ev_time,
	String *params,			/* selections in precedence order */
	Cardinal num_params)
{
    Atom selection;
    int cutbuffer;

    XmuInternStrings(XtDisplay(w), params, (Cardinal)1, &selection);
    switch (selection) {
      case XA_CUT_BUFFER0: cutbuffer = 0; break;
      case XA_CUT_BUFFER1: cutbuffer = 1; break;
      case XA_CUT_BUFFER2: cutbuffer = 2; break;
      case XA_CUT_BUFFER3: cutbuffer = 3; break;
      case XA_CUT_BUFFER4: cutbuffer = 4; break;
      case XA_CUT_BUFFER5: cutbuffer = 5; break;
      case XA_CUT_BUFFER6: cutbuffer = 6; break;
      case XA_CUT_BUFFER7: cutbuffer = 7; break;
      default:	       cutbuffer = -1;
    }
    if (cutbuffer >= 0) {
	int inbytes;
	unsigned long nbytes;
	int fmt8 = 8;
	Atom type = XA_STRING;
	char *line = XFetchBuffer(XtDisplay(w), &inbytes, cutbuffer);
	nbytes = (unsigned long) inbytes;
	if (nbytes > 0)
	    SelectionReceived(w, NULL, &selection, &type, (XtPointer)line,
			      &nbytes, &fmt8);
	else if (num_params > 1)
	    _GetSelection(w, ev_time, params+1, num_params-1);
    } else {
	struct _SelectionList* list;
	if (--num_params) {
	    list = XtNew(struct _SelectionList);
	    list->params = params + 1;
	    list->count = num_params; /* decremented above */
	    list->time = ev_time;
	} else list = NULL;
	XtGetSelectionValue(w, selection, XA_STRING, SelectionReceived,
			    (XtPointer)list, ev_time);
    }
}

/* SelectionReceived: stuff received selection text into pty */

/* ARGSUSED */
static void SelectionReceived(
	Widget w,
	XtPointer client_data,
	Atom *selection GCC_UNUSED,
	Atom *type,
	XtPointer value,
	unsigned long *length,
	int *format GCC_UNUSED)
{
    int pty;
    register char *lag, *cp, *end;
    char *line = (char*)value;

    if (!IsXtermWidget(w))
	return;

    pty = ((XtermWidget)w)->screen.respond;	/* file descriptor of pty */
    if (*type == 0 /*XT_CONVERT_FAIL*/ || *length == 0 || value == NULL) {
	/* could not get this selection, so see if there are more to try */
	struct _SelectionList* list = (struct _SelectionList*)client_data;
	if (list != NULL) {
	    _GetSelection(w, list->time, list->params, list->count);
	    XtFree((char *)client_data);
	}
	return;
    }

    /* Write data to pty a line at a time. */
    /* Doing this one line at a time may no longer be necessary
       because v_write has been re-written. */

    end = &line[*length];
    lag = line;
    for (cp = line; cp != end; cp++)
	{
	    if (*cp != '\n') continue;
	    *cp = '\r';
	    v_write(pty, lag, cp - lag + 1);
	    lag = cp + 1;
	}
    if (lag != end)
	v_write(pty, lag, end - lag);

    XtFree((char *)client_data);
    XtFree((char *)value);
}


void
HandleInsertSelection(
	Widget w,
	XEvent *event,			/* assumed to be XButtonEvent* */
	String *params,			/* selections in precedence order */
	Cardinal *num_params)
{
    if (SendMousePosition(w, event)) return;
    _GetSelection(w, event->xbutton.time, params, *num_params);
}


static SelectUnit
EvalSelectUnit(Time buttonDownTime, SelectUnit defaultUnit)
{
    int delta;

    if (lastButtonUpTime == (Time) 0) /* first time and once in a blue moon */
	delta = term->screen.multiClickTime + 1;
    else if (buttonDownTime > lastButtonUpTime) /* most of the time */
	delta = buttonDownTime - lastButtonUpTime;
    else /* time has rolled over since lastButtonUpTime */
	delta = (((Time) ~0) - lastButtonUpTime) + buttonDownTime;

    if (delta > term->screen.multiClickTime) {
	numberOfClicks = 1;
	return defaultUnit;
    } else {
	++numberOfClicks;
	return ((selectUnit + 1) % NSELECTUNITS);
    }
}

static void do_select_start (
	Widget w,
	XEvent *event,			/* must be XButtonEvent* */
	int startrow,
	int startcol)
{
	if (SendMousePosition(w, event)) return;
	selectUnit = EvalSelectUnit(event->xbutton.time, SELECTCHAR);
	replyToEmacs = FALSE;
	StartSelect(startrow, startcol);
}

/* ARGSUSED */
void
HandleSelectStart(
	Widget w,
	XEvent *event,			/* must be XButtonEvent* */
	String *params GCC_UNUSED,
	Cardinal *num_params GCC_UNUSED)
{
	register TScreen *screen;
	int startrow, startcol;

	if (!IsXtermWidget(w))
		return;

	screen = &((XtermWidget)w)->screen;
	firstValidRow = 0;
	lastValidRow  = screen->max_row;
	PointToRowCol(event->xbutton.y, event->xbutton.x, &startrow, &startcol);
	do_select_start (w, event, startrow, startcol);
}


/* ARGSUSED */
void
HandleKeyboardSelectStart(
	Widget w,
	XEvent *event,			/* must be XButtonEvent* */
	String *params GCC_UNUSED,
	Cardinal *num_params GCC_UNUSED)
{
	register TScreen *screen;

	if (!IsXtermWidget(w))
		return;

	screen = &((XtermWidget)w)->screen;
	do_select_start (w, event, screen->cursor_row, screen->cursor_col);
}


static void
TrackDown(register XButtonEvent *event)
{
	int startrow, startcol;

	selectUnit = EvalSelectUnit(event->time, SELECTCHAR);
	if (numberOfClicks > 1 ) {
		PointToRowCol(event->y, event->x, &startrow, &startcol);
		replyToEmacs = TRUE;
		StartSelect(startrow, startcol);
	} else {
		waitingForTrackInfo = 1;
		EditorButton((XButtonEvent *)event);
	}
}


#define boundsCheck(x)	if (x < 0) \
			    x = 0; \
			else if (x >= screen->max_row) \
			    x = screen->max_row;

void
TrackMouse(int func, int startrow, int startcol, int firstrow, int lastrow)
{
	TScreen *screen = &term->screen;

	if (!waitingForTrackInfo) {	/* Timed out, so ignore */
		return;
	}
	waitingForTrackInfo = 0;
	if (func == 0) return;
	boundsCheck (startrow)
	boundsCheck (firstrow)
	boundsCheck (lastrow)
	firstValidRow = firstrow;
	lastValidRow  = lastrow;
	replyToEmacs = TRUE;
	StartSelect(startrow, startcol);
}

static void
StartSelect(int startrow, int startcol)
{
	TScreen *screen = &term->screen;

	TRACE(("StartSelect row=%d, col=%d\n", startrow, startcol))
	if (screen->cursor_state)
	    HideCursor ();
	if (numberOfClicks == 1) {
		/* set start of selection */
		rawRow = startrow;
		rawCol = startcol;

	} /* else use old values in rawRow, Col */

	saveStartRRow = startERow = rawRow;
	saveStartRCol = startECol = rawCol;
	saveEndRRow   = endERow   = rawRow;
	saveEndRCol   = endECol   = rawCol;
	if (Coordinate(startrow, startcol) < Coordinate(rawRow, rawCol)) {
		eventMode = LEFTEXTENSION;
		startERow = startrow;
		startECol = startcol;
	} else {
		eventMode = RIGHTEXTENSION;
		endERow = startrow;
		endECol = startcol;
	}
	ComputeSelect(startERow, startECol, endERow, endECol, False);

}

static void
EndExtend(
	Widget w,
	XEvent *event,			/* must be XButtonEvent */
	String *params,			/* selections */
	Cardinal num_params,
	Bool use_cursor_loc)
{
	int	row, col, count;
	TScreen *screen = &term->screen;
	Char line[9];

	if (use_cursor_loc) {
	    row = screen->cursor_row;
	    col = screen->cursor_col;
	} else {
	    PointToRowCol(event->xbutton.y, event->xbutton.x, &row, &col);
	}
	ExtendExtend (row, col);
	lastButtonUpTime = event->xbutton.time;
	if (startSRow != endSRow || startSCol != endSCol) {
		if (replyToEmacs) {
			count = 0;
			if (screen->control_eight_bits) {
				line[count++] = CSI;
			} else {
				line[count++] = ESC;
				line[count++] = '[';
			}
			if (rawRow == startSRow && rawCol == startSCol
			    && row == endSRow && col == endSCol) {
			 	/* Use short-form emacs select */
				line[count++] = 't';
				line[count++] = ' ' + endSCol + 1;
				line[count++] = ' ' + endSRow + 1;
			} else {
				/* long-form, specify everything */
				line[count++] = 'T';
				line[count++] = ' ' + startSCol + 1;
				line[count++] = ' ' + startSRow + 1;
				line[count++] = ' ' + endSCol + 1;
				line[count++] = ' ' + endSRow + 1;
				line[count++] = ' ' + col + 1;
				line[count++] = ' ' + row + 1;
			}
			v_write(screen->respond, (char *)line, count);
			TrackText(0, 0, 0, 0);
		}
	}
	SelectSet(w, event, params, num_params);
	eventMode = NORMAL;
}

void
HandleSelectSet(
	Widget w,
	XEvent *event,
	String *params,
	Cardinal *num_params)
{
	SelectSet (w, event, params, *num_params);
}

/* ARGSUSED */
static void
SelectSet (
	Widget	w GCC_UNUSED,
	XEvent	*event GCC_UNUSED,
	String	*params,
	Cardinal    num_params)
{
	/* Only do select stuff if non-null select */
	if (startSRow != endSRow || startSCol != endSCol) {
		SaltTextAway(startSRow, startSCol, endSRow, endSCol,
			     params, num_params);
	} else
		DisownSelection(term);
}

#define Abs(x)		((x) < 0 ? -(x) : (x))

/* ARGSUSED */
static void do_start_extend (
	Widget w,
	XEvent *event,			/* must be XButtonEvent* */
	String *params GCC_UNUSED,
	Cardinal *num_params GCC_UNUSED,
	Bool use_cursor_loc)
{
	TScreen *screen;
	int row, col, coord;

	if (!IsXtermWidget(w))
		return;

	screen = &((XtermWidget)w)->screen;
	if (SendMousePosition(w, event)) return;
	firstValidRow = 0;
	lastValidRow  = screen->max_row;
	selectUnit = EvalSelectUnit(event->xbutton.time, selectUnit);
	replyToEmacs = FALSE;

	if (numberOfClicks == 1) {
		/* Save existing selection so we can reestablish it if the guy
		   extends past the other end of the selection */
		saveStartRRow = startERow = startRRow;
		saveStartRCol = startECol = startRCol;
		saveEndRRow   = endERow   = endRRow;
		saveEndRCol   = endECol   = endRCol;
	} else {
		/* He just needed the selection mode changed, use old values. */
		startERow = startRRow = saveStartRRow;
		startECol = startRCol = saveStartRCol;
		endERow   = endRRow   = saveEndRRow;
		endECol   = endRCol   = saveEndRCol;

	}
	if (use_cursor_loc) {
	    row = screen->cursor_row;
	    col = screen->cursor_col;
	} else {
	    PointToRowCol(event->xbutton.y, event->xbutton.x, &row, &col);
	}
	coord = Coordinate(row, col);

	if (Abs(coord - Coordinate(startSRow, startSCol))
	     < Abs(coord - Coordinate(endSRow, endSCol))
	    || coord < Coordinate(startSRow, startSCol)) {
	 	/* point is close to left side of selection */
		eventMode = LEFTEXTENSION;
		startERow = row;
		startECol = col;
	} else {
	 	/* point is close to left side of selection */
		eventMode = RIGHTEXTENSION;
		endERow = row;
		endECol = col;
	}
	ComputeSelect(startERow, startECol, endERow, endECol, True);
}

static void
ExtendExtend (int row, int col)
{
	int coord = Coordinate(row, col);

	TRACE(("ExtendExtend row=%d, col=%d\n", row, col))
	if (eventMode == LEFTEXTENSION
	 && (coord + (selectUnit!=SELECTCHAR)) > Coordinate(endSRow, endSCol)) {
		/* Whoops, he's changed his mind.  Do RIGHTEXTENSION */
		eventMode = RIGHTEXTENSION;
		startERow = saveStartRRow;
		startECol = saveStartRCol;
	} else if (eventMode == RIGHTEXTENSION
	 && coord < Coordinate(startSRow, startSCol)) {
	 	/* Whoops, he's changed his mind.  Do LEFTEXTENSION */
		eventMode = LEFTEXTENSION;
		endERow   = saveEndRRow;
		endECol   = saveEndRCol;
	}
	if (eventMode == LEFTEXTENSION) {
		startERow = row;
		startECol = col;
	} else {
		endERow = row;
		endECol = col;
	}
	ComputeSelect(startERow, startECol, endERow, endECol, False);
}


void HandleStartExtend(
	Widget w,
	XEvent *event,			/* must be XButtonEvent* */
	String *params,			/* unused */
	Cardinal *num_params)		/* unused */
{
    do_start_extend (w, event, params, num_params, False);
}

void HandleKeyboardStartExtend(
	Widget w,
	XEvent *event,			/* must be XButtonEvent* */
	String *params,			/* unused */
	Cardinal *num_params)		/* unused */
{
    do_start_extend (w, event, params, num_params, True);
}

void
ScrollSelection(register TScreen* screen, register int amount)
{
    register int minrow = -screen->savedlines - screen->topline;
    register int maxrow = screen->max_row - screen->topline;
    register int maxcol = screen->max_col;

#define scroll_update_one(row, col) \
    	row += amount; \
	if (row < minrow) { \
	    row = minrow; \
	    col = 0; \
	} \
	if (row > maxrow) { \
	    row = maxrow; \
	    col = maxcol; \
	}

    scroll_update_one(startRRow, startRCol);
    scroll_update_one(endRRow, endRCol);
    scroll_update_one(startSRow, startSCol);
    scroll_update_one(endSRow, endSCol);

    scroll_update_one(rawRow, rawCol);

    scroll_update_one(screen->startHRow, screen->startHCol);
    scroll_update_one(screen->endHRow, screen->endHCol);

    screen->startHCoord = Coordinate (screen->startHRow, screen->startHCol);
    screen->endHCoord = Coordinate (screen->endHRow, screen->endHCol);
}


/*ARGSUSED*/
void
ResizeSelection (TScreen *screen GCC_UNUSED, int rows, int cols)
{
    rows--;				/* decr to get 0-max */
    cols--;

    if (startRRow > rows) startRRow = rows;
    if (startSRow > rows) startSRow = rows;
    if (endRRow > rows) endRRow = rows;
    if (endSRow > rows) endSRow = rows;
    if (rawRow > rows) rawRow = rows;

    if (startRCol > cols) startRCol = cols;
    if (startSCol > cols) startSCol = cols;
    if (endRCol > cols) endRCol = cols;
    if (endSCol > cols) endSCol = cols;
    if (rawCol > cols) rawCol = cols;
}

static void
PointToRowCol(
    register int y,
    register int x,
    int *r,
    int *c)
/* Convert pixel coordinates to character coordinates.
   Rows are clipped between firstValidRow and lastValidRow.
   Columns are clipped between to be 0 or greater, but are not clipped to some
       maximum value. */
{
	register TScreen *screen = &term->screen;
	register int row, col;

	row = (y - screen->border) / FontHeight(screen);
	if(row < firstValidRow)
		row = firstValidRow;
	else if(row > lastValidRow)
		row = lastValidRow;
	col = (x - OriginX(screen)) / FontWidth(screen);
	if(col < 0)
		col = 0;
	else if(col > screen->max_col+1) {
		col = screen->max_col+1;
	}
	*r = row;
	*c = col;
}

static int
LastTextCol(register int row)
{
	register TScreen *screen =  &term->screen;
	register int i;
	register Char *ch;

	for ( i = screen->max_col,
	        ch = SCRN_BUF_ATTRS(screen, (row + screen->topline)) + i ;
	      i >= 0 && !(*ch & CHARDRAWN) ;
	      ch--, i--)
	    ;
	return(i);
}

/*
** double click table for cut and paste in 8 bits
**
** This table is divided in four parts :
**
**	- control characters	[0,0x1f] U [0x80,0x9f]
**	- separators		[0x20,0x3f] U [0xa0,0xb9]
**	- binding characters	[0x40,0x7f] U [0xc0,0xff]
**  	- execeptions
*/
static int charClass[256] = {
/* NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL */
    32,   1,   1,   1,   1,   1,   1,   1,
/*  BS   HT   NL   VT   NP   CR   SO   SI */
     1,  32,   1,   1,   1,   1,   1,   1,
/* DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB */
     1,   1,   1,   1,   1,   1,   1,   1,
/* CAN   EM  SUB  ESC   FS   GS   RS   US */
     1,   1,   1,   1,   1,   1,   1,   1,
/*  SP    !    "    #    $    %    &    ' */
    32,  33,  34,  35,  36,  37,  38,  39,
/*   (    )    *    +    ,    -    .    / */
    40,  41,  42,  43,  44,  45,  46,  47,
/*   0    1    2    3    4    5    6    7 */
    48,  48,  48,  48,  48,  48,  48,  48,
/*   8    9    :    ;    <    =    >    ? */
    48,  48,  58,  59,  60,  61,  62,  63,
/*   @    A    B    C    D    E    F    G */
    64,  48,  48,  48,  48,  48,  48,  48,
/*   H    I    J    K    L    M    N    O */
    48,  48,  48,  48,  48,  48,  48,  48,
/*   P    Q    R    S    T    U    V    W */
    48,  48,  48,  48,  48,  48,  48,  48,
/*   X    Y    Z    [    \    ]    ^    _ */
    48,  48,  48,  91,  92,  93,  94,  48,
/*   `    a    b    c    d    e    f    g */
    96,  48,  48,  48,  48,  48,  48,  48,
/*   h    i    j    k    l    m    n    o */
    48,  48,  48,  48,  48,  48,  48,  48,
/*   p    q    r    s    t    u    v    w */
    48,  48,  48,  48,  48,  48,  48,  48,
/*   x    y    z    {    |    }    ~  DEL */
    48,  48,  48, 123, 124, 125, 126,   1,
/* x80  x81  x82  x83  IND  NEL  SSA  ESA */
     1,   1,   1,   1,   1,   1,   1,   1,
/* HTS  HTJ  VTS  PLD  PLU   RI  SS2  SS3 */
     1,   1,   1,   1,   1,   1,   1,   1,
/* DCS  PU1  PU2  STS  CCH   MW  SPA  EPA */
     1,   1,   1,   1,   1,   1,   1,   1,
/* x98  x99  x9A  CSI   ST  OSC   PM  APC */
     1,   1,   1,   1,   1,   1,   1,   1,
/*   -    i   c/    L   ox   Y-    |   So */
   160, 161, 162, 163, 164, 165, 166, 167,
/*  ..   c0   ip   <<    _        R0    - */
   168, 169, 170, 171, 172, 173, 174, 175,
/*   o   +-    2    3    '    u   q|    . */
   176, 177, 178, 179, 180, 181, 182, 183,
/*   ,    1    2   >>  1/4  1/2  3/4    ? */
   184, 185, 186, 187, 188, 189, 190, 191,
/*  A`   A'   A^   A~   A:   Ao   AE   C, */
    48,  48,  48,  48,  48,  48,  48,  48,
/*  E`   E'   E^   E:   I`   I'   I^   I: */
    48,  48,  48,  48,  48,  48,  48,  48,
/*  D-   N~   O`   O'   O^   O~   O:    X */
    48,  48,  48,  48,  48,  48,  48, 216,
/*  O/   U`   U'   U^   U:   Y'    P    B */
    48,  48,  48,  48,  48,  48,  48,  48,
/*  a`   a'   a^   a~   a:   ao   ae   c, */
    48,  48,  48,  48,  48,  48,  48,  48,
/*  e`   e'   e^   e:    i`  i'   i^   i: */
    48,  48,  48,  48,  48,  48,  48,  48,
/*   d   n~   o`   o'   o^   o~   o:   -: */
    48,  48,  48,  48,  48,  48,  48,  248,
/*  o/   u`   u'   u^   u:   y'    P   y: */
    48,  48,  48,  48,  48,  48,  48,  48};

int SetCharacterClassRange (
    register int low,		 	/* in range of [0..255] */
    register int high,
    register int value)			/* arbitrary */
{

    if (low < 0 || high > 255 || high < low) return (-1);

    for (; low <= high; low++) charClass[low] = value;

    return (0);
}

/*
 * sets startSRow startSCol endSRow endSCol
 * ensuring that they have legal values
 */

static void
ComputeSelect(
	int startRow,
	int startCol,
	int endRow,
	int endCol,
	Bool extend)
{
	register TScreen *screen = &term->screen;
	register Char *ptr;
	register int length;
	register int cclass;

	if (Coordinate(startRow, startCol) <= Coordinate(endRow, endCol)) {
		startSRow = startRRow = startRow;
		startSCol = startRCol = startCol;
		endSRow   = endRRow   = endRow;
		endSCol   = endRCol   = endCol;
	} else {	/* Swap them */
		startSRow = startRRow = endRow;
		startSCol = startRCol = endCol;
		endSRow   = endRRow   = startRow;
		endSCol   = endRCol   = startCol;
	}

	switch (selectUnit) {
		case SELECTCHAR :
			if (startSCol > (LastTextCol(startSRow) + 1)) {
				startSCol = 0;
				startSRow++;
			}
			if (endSCol > (LastTextCol(endSRow) + 1)) {
				endSCol = 0;
				endSRow++;
			}
			break;
		case SELECTWORD :
			if (startSCol > (LastTextCol(startSRow) + 1)) {
				startSCol = 0;
				startSRow++;
			} else {
				ptr = SCRN_BUF_CHARS(screen, startSRow+screen->topline)
				 + startSCol;
				cclass = charClass[*ptr];
				do {
					--startSCol;
					--ptr;
					if (startSCol <= 0
					    && ScrnTstWrapped(screen, startSRow - 1)) {
						--startSRow;
						startSCol = LastTextCol(startSRow);
						ptr = SCRN_BUF_CHARS(screen, startSRow+screen->topline)
						 + startSCol;
					}
				} while (startSCol >= 0
				 && charClass[*ptr] == cclass);
				++startSCol;
			}
			if (endSCol > (LastTextCol(endSRow) + 1)) {
				endSCol = 0;
				endSRow++;
			} else {
				length = LastTextCol(endSRow);
				ptr = SCRN_BUF_CHARS(screen, endSRow+screen->topline)
				 + endSCol;
				cclass = charClass[*ptr];
				do {
					++endSCol;
					++ptr;
					if (endSCol > length
					    && ScrnTstWrapped(screen, endSRow)) {
						endSCol = 0;
						++endSRow;
						length = LastTextCol(endSRow);
						ptr = SCRN_BUF_CHARS(screen, endSRow+screen->topline)
						 + endSCol;
					}
				} while (endSCol <= length
				 && charClass[*ptr] == cclass);
				/* Word select selects if pointing to any char
				   in "word", especially in that it includes
				   the last character in a word.  So no --endSCol
				   and do special eol handling */
				if (endSCol > length+1) {
					endSCol = 0;
					++endSRow;
				}
			}
			saveStartWRow = startSRow;
			saveStartWCol = startSCol;
			break;
		case SELECTLINE :
			while (ScrnTstWrapped(screen, endSRow)) {
				++endSRow;
			}
			if (term->screen.cutToBeginningOfLine
			    || startSRow < saveStartWRow) {
			    startSCol = 0;
			    while (ScrnTstWrapped(screen, startSRow - 1)) {
				--startSRow;
			    }
			} else if (!extend) {
			    if ((startRow < saveStartWRow)
				|| (startRow == saveStartWRow
				    && startCol < saveStartWCol)) {
				startSCol = 0;
				while (ScrnTstWrapped(screen, startSRow - 1)) {
				    --startSRow;
				}
			    } else {
				startSRow = saveStartWRow;
				startSCol = saveStartWCol;
			    }
			}
			if (term->screen.cutNewline) {
			    endSCol = 0;
			    ++endSRow;
			} else {
			    endSCol = LastTextCol(endSRow) + 1;
			}
			break;
	}

	TrackText(startSRow, startSCol, endSRow, endSCol);
	return;
}

void
TrackText(
	register int frow,
	register int fcol,
	register int trow,
	register int tcol)
    /* Guaranteed (frow, fcol) <= (trow, tcol) */
{
	register int from, to;
	register TScreen *screen = &term->screen;
	int old_startrow, old_startcol, old_endrow, old_endcol;

	old_startrow = screen->startHRow;
	old_startcol = screen->startHCol;
	old_endrow = screen->endHRow;
	old_endcol = screen->endHCol;
	if (frow == old_startrow && fcol == old_startcol &&
	    trow == old_endrow   && tcol == old_endcol) return;
	screen->startHRow = frow;
	screen->startHCol = fcol;
	screen->endHRow   = trow;
	screen->endHCol   = tcol;
	from = Coordinate(frow, fcol);
	to = Coordinate(trow, tcol);
	if (to <= screen->startHCoord || from > screen->endHCoord) {
	    /* No overlap whatsoever between old and new hilite */
	    ReHiliteText(old_startrow, old_startcol, old_endrow, old_endcol);
	    ReHiliteText(frow, fcol, trow, tcol);
	} else {
	    if (from < screen->startHCoord) {
		    /* Extend left end */
		    ReHiliteText(frow, fcol, old_startrow, old_startcol);
	    } else if (from > screen->startHCoord) {
		    /* Shorten left end */
		    ReHiliteText(old_startrow, old_startcol, frow, fcol);
	    }
	    if (to > screen->endHCoord) {
		    /* Extend right end */
		    ReHiliteText(old_endrow, old_endcol, trow, tcol);
	    } else if (to < screen->endHCoord) {
		    /* Shorten right end */
		    ReHiliteText(trow, tcol, old_endrow, old_endcol);
	    }
	}
	screen->startHCoord = from;
	screen->endHCoord = to;
}

static void
ReHiliteText(
    register int frow,
    register int fcol,
    register int trow,
    register int tcol)
    /* Guaranteed that (frow, fcol) <= (trow, tcol) */
{
	register TScreen *screen = &term->screen;
	register int i;

	if (frow < 0)
	    frow = fcol = 0;
	else if (frow > screen->max_row)
	    return;		/* nothing to do, since trow >= frow */

	if (trow < 0)
	    return;		/* nothing to do, since frow <= trow */
	else if (trow > screen->max_row) {
	    trow = screen->max_row;
	    tcol = screen->max_col+1;
	}
	if (frow == trow && fcol == tcol)
		return;

	if(frow != trow) {	/* do multiple rows */
		if((i = screen->max_col - fcol + 1) > 0) {     /* first row */
		    ScrnRefresh(screen, frow, fcol, 1, i, True);
		}
		if((i = trow - frow - 1) > 0) {		       /* middle rows*/
		    ScrnRefresh(screen, frow+1, 0,i, screen->max_col+1, True);
		}
		if(tcol > 0 && trow <= screen->max_row) {      /* last row */
		    ScrnRefresh(screen, trow, 0, 1, tcol, True);
		}
	} else {		/* do single row */
		ScrnRefresh(screen, frow, fcol, 1, tcol - fcol, True);
	}
}

static void
SaltTextAway(
    int crow, int ccol, int row, int col,
    String *params,			/* selections */
    Cardinal num_params)
    /* Guaranteed that (crow, ccol) <= (row, col), and that both points are valid
       (may have row = screen->max_row+1, col = 0) */
{
	register TScreen *screen = &term->screen;
	register int i, j = 0;
	int eol;
	char *line, *lp;

	if (crow == row && ccol > col) {
	    int tmp = ccol;
	    ccol = col;
	    col = tmp;
	}

	--col;
	/* first we need to know how long the string is before we can save it*/

	if ( row == crow ) j = Length(screen, crow, ccol, col);
	else {	/* two cases, cut is on same line, cut spans multiple lines */
		j += Length(screen, crow, ccol, screen->max_col) + 1;
		for(i = crow + 1; i < row; i++)
			j += Length(screen, i, 0, screen->max_col) + 1;
		if (col >= 0)
			j += Length(screen, row, 0, col);
	}

	/* now get some memory to save it in */

	if (screen->selection_size <= j) {
	    if((line = (char *)malloc((unsigned) j + 1)) == 0)
		SysError(ERROR_BMALLOC2);
	    XtFree(screen->selection);
	    screen->selection = line;
	    screen->selection_size = j + 1;
	} else line = screen->selection;
	if (!line || j < 0) return;

	line[j] = '\0';		/* make sure it is null terminated */
	lp = line;		/* lp points to where to save the text */
	if ( row == crow ) lp = SaveText(screen, row, ccol, col, lp, &eol);
	else {
		lp = SaveText(screen, crow, ccol, screen->max_col, lp, &eol);
		if (eol)
			*lp ++ = '\n';	/* put in newline at end of line */
		for(i = crow +1; i < row; i++) {
			lp = SaveText(screen, i, 0, screen->max_col, lp, &eol);
			if (eol)
				*lp ++ = '\n';
			}
		if (col >= 0)
			lp = SaveText(screen, row, 0, col, lp, &eol);
	}
	*lp = '\0';		/* make sure we have end marked */

	TRACE(("Salted TEXT:%.*s\n", lp - line, line))
	screen->selection_length = (lp - line);
	_OwnSelection(term, params, num_params);
}

static Boolean
ConvertSelection(
	Widget w,
	Atom *selection,
	Atom *target,
	Atom *type,
	XtPointer *value,
	unsigned long *length,
	int *format)
{
    Display* d = XtDisplay(w);
    TScreen *screen;

    if (!IsXtermWidget(w))
	return False;

    screen = &((XtermWidget)w)->screen;
    if (screen->selection == NULL) return False; /* can this happen? */

    if (*target == XA_TARGETS(d)) {
	Atom* targetP;
	Atom* std_targets;
	unsigned long std_length;
	XmuConvertStandardSelection(
		    w, screen->selection_time, selection,
		    target, type, (XPointer *)&std_targets, &std_length, format
		   );
	*length = std_length + 5;
	targetP = (Atom*)XtMalloc(sizeof(Atom)*(*length));
	*value = (XtPointer) targetP;
	*targetP++ = XA_STRING;
	*targetP++ = XA_TEXT(d);
	*targetP++ = XA_COMPOUND_TEXT(d);
	*targetP++ = XA_LENGTH(d);
	*targetP++ = XA_LIST_LENGTH(d);
	memcpy ( (char*)targetP, (char*)std_targets, sizeof(Atom)*std_length);
	XtFree((char*)std_targets);
	*type = XA_ATOM;
	*format = 32;
	return True;
    }

    if (*target == XA_STRING ||
	*target == XA_TEXT(d) ||
	*target == XA_COMPOUND_TEXT(d)) {
	if (*target == XA_COMPOUND_TEXT(d)) {
	    XTextProperty textprop;

	    *value = (XtPointer) screen->selection;
	    if (XmbTextListToTextProperty (d, (char**)value, 1,
					   XCompoundTextStyle, &textprop)
			< Success) return False;
	    *value = (XtPointer) textprop.value;
	    *length = textprop.nitems;
	    *type = *target;
	} else {
	    *type = XA_STRING;
	    *value = screen->selection;
	    *length = screen->selection_length;
	}
	*format = 8;
	return True;
    }
    if (*target == XA_LIST_LENGTH(d)) {
	*value = XtMalloc(4);
	if (sizeof(long) == 4)
	    *(long*)*value = 1;
	else {
	    long temp = 1;
	    memcpy ( (char*)*value, ((char*)&temp)+sizeof(long)-4, 4);
	}
	*type = XA_INTEGER;
	*length = 1;
	*format = 32;
	return True;
    }
    if (*target == XA_LENGTH(d)) {
	*value = XtMalloc(4);
	if (sizeof(long) == 4)
	    *(long*)*value = screen->selection_length;
	else {
	    long temp = screen->selection_length;
	    memcpy ( (char*)*value, ((char*)&temp)+sizeof(long)-4, 4);
	}
	*type = XA_INTEGER;
	*length = 1;
	*format = 32;
	return True;
    }
    if (XmuConvertStandardSelection(w, screen->selection_time, selection,
				    target, type,
				    (XPointer *)value, length, format))
	return True;

    /* else */
    return False;

}


static void
LoseSelection(Widget w, Atom *selection)
{
    register TScreen* screen;
    register Atom* atomP;
    Cardinal i;

    if (!IsXtermWidget(w))
	return;

    screen = &((XtermWidget)w)->screen;
    for (i = 0, atomP = screen->selection_atoms;
	 i < screen->selection_count; i++, atomP++)
    {
	if (*selection == *atomP) *atomP = (Atom)0;
	switch (*atomP) {
	  case XA_CUT_BUFFER0:
	  case XA_CUT_BUFFER1:
	  case XA_CUT_BUFFER2:
	  case XA_CUT_BUFFER3:
	  case XA_CUT_BUFFER4:
	  case XA_CUT_BUFFER5:
	  case XA_CUT_BUFFER6:
	  case XA_CUT_BUFFER7:	*atomP = (Atom)0;
	}
    }

    for (i = screen->selection_count; i; i--) {
	if (screen->selection_atoms[i-1] != 0) break;
    }
    screen->selection_count = i;

    for (i = 0, atomP = screen->selection_atoms;
	 i < screen->selection_count; i++, atomP++)
    {
	if (*atomP == (Atom)0) {
	    *atomP = screen->selection_atoms[--screen->selection_count];
	}
    }

    if (screen->selection_count == 0)
	TrackText(0, 0, 0, 0);
}


/* ARGSUSED */
static void SelectionDone(
	Widget w GCC_UNUSED,
	Atom *selection GCC_UNUSED,
	Atom *target GCC_UNUSED)
{
    /* empty proc so Intrinsics know we want to keep storage */
}


static void
_OwnSelection(
	register XtermWidget termw,
	String *selections,
	Cardinal count)
{
    Atom* atoms = termw->screen.selection_atoms;
    Cardinal i;
    Boolean have_selection = False;

    if (termw->screen.selection_length < 0) return;

    if (count > termw->screen.sel_atoms_size) {
	XtFree((char*)atoms);
	atoms = (Atom*)XtMalloc(count*sizeof(Atom));
	termw->screen.selection_atoms = atoms;
	termw->screen.sel_atoms_size = count;
    }
    XmuInternStrings( XtDisplay((Widget)termw), selections, count, atoms );
    for (i = 0; i < count; i++) {
	int cutbuffer;
	switch (atoms[i]) {
	  case XA_CUT_BUFFER0: cutbuffer = 0; break;
	  case XA_CUT_BUFFER1: cutbuffer = 1; break;
	  case XA_CUT_BUFFER2: cutbuffer = 2; break;
	  case XA_CUT_BUFFER3: cutbuffer = 3; break;
	  case XA_CUT_BUFFER4: cutbuffer = 4; break;
	  case XA_CUT_BUFFER5: cutbuffer = 5; break;
	  case XA_CUT_BUFFER6: cutbuffer = 6; break;
	  case XA_CUT_BUFFER7: cutbuffer = 7; break;
	  default:	       cutbuffer = -1;
	}
	if (cutbuffer >= 0)
	    if ( termw->screen.selection_length >
		 4*XMaxRequestSize(XtDisplay((Widget)termw))-32)
		fprintf(stderr,
			"%s: selection too big (%d bytes), not storing in CUT_BUFFER%d\n",
			xterm_name, termw->screen.selection_length, cutbuffer);
	    else
		XStoreBuffer( XtDisplay((Widget)termw), termw->screen.selection,
			      termw->screen.selection_length, cutbuffer );
	else if (!replyToEmacs) {
	    have_selection |=
		XtOwnSelection( (Widget)termw, atoms[i],
			    termw->screen.selection_time,
			    ConvertSelection, LoseSelection, SelectionDone );
	}
    }
    if (!replyToEmacs)
	termw->screen.selection_count = count;
    if (!have_selection)
	TrackText(0, 0, 0, 0);
}

void
DisownSelection(register XtermWidget termw)
{
    Atom* atoms = termw->screen.selection_atoms;
    Cardinal count = termw->screen.selection_count;
    Cardinal i;

    for (i = 0; i < count; i++) {
	int cutbuffer;
	switch (atoms[i]) {
	  case XA_CUT_BUFFER0: cutbuffer = 0; break;
	  case XA_CUT_BUFFER1: cutbuffer = 1; break;
	  case XA_CUT_BUFFER2: cutbuffer = 2; break;
	  case XA_CUT_BUFFER3: cutbuffer = 3; break;
	  case XA_CUT_BUFFER4: cutbuffer = 4; break;
	  case XA_CUT_BUFFER5: cutbuffer = 5; break;
	  case XA_CUT_BUFFER6: cutbuffer = 6; break;
	  case XA_CUT_BUFFER7: cutbuffer = 7; break;
	  default:	       cutbuffer = -1;
	}
	if (cutbuffer < 0)
	    XtDisownSelection( (Widget)termw, atoms[i],
			       termw->screen.selection_time );
    }
    termw->screen.selection_count = 0;
    termw->screen.startHRow = termw->screen.startHCol = 0;
    termw->screen.endHRow = termw->screen.endHCol = 0;
}


/* returns number of chars in line from scol to ecol out */
/* ARGSUSED */
static int
Length(
    register TScreen *screen GCC_UNUSED,
    register int row,
    register int scol,
    register int ecol)
{
        register int lastcol = LastTextCol(row);

	if (ecol > lastcol)
	    ecol = lastcol;
	return (ecol - scol + 1);
}

/* copies text into line, preallocated */
static char *
SaveText(
    TScreen *screen,
    int row,
    int scol,
    int ecol,
    register char *lp,		/* pointer to where to put the text */
    int *eol)
{
	register int i = 0;
	register Char *ch = SCRN_BUF_CHARS(screen, row + screen->topline);
	register int c;

	i = Length(screen, row, scol, ecol);
	ecol = scol + i;
#if OPT_DEC_CHRSET
	if (CSET_DOUBLE(SCRN_BUF_CSETS(screen, row + screen->topline)[0])) {
		scol = (scol + 0) / 2;
		ecol = (ecol + 1) / 2;
	}
#endif
	*eol = !ScrnTstWrapped(screen, row);
	for (i = scol; i < ecol; i++) {
	        c = ch[i];
		if (c == 0)
			c = ' ';
		else if(c < ' ') {
			if(c == '\036')
				c = '#'; /* char on screen is pound sterling */
			else
				c += 0x5f; /* char is from DEC drawing set */
		} else if(c == 0x7f)
			c = 0x5f;
		*lp++ = c;
	}
	return(lp);
}

static int
BtnCode(XButtonEvent *event, int button)
{
	if (button < 0 || button > 3)
		button = 3;
	return ' ' + (KeyState(event->state) << 2) + button;
}

#define MOUSE_LIMIT (255 - 32)

static void
EditorButton(register XButtonEvent *event)
{
	TScreen *screen = &term->screen;
	int pty = screen->respond;
	Char line[6];
	int row, col;
	int button, count = 0;

	/* If button event, get button # adjusted for DEC compatibility */
	button = event->button - 1;

	/* Compute character position of mouse pointer */
	row = (event->y - screen->border) / FontHeight(screen);
	col = (event->x - OriginX(screen)) / FontWidth(screen);

	/* Limit to screen dimensions */
	if (row < 0)
		row = 0;
	else if (row > screen->max_row)
		row = screen->max_row;
	else if (row > MOUSE_LIMIT)
		row = MOUSE_LIMIT;

	if (col < 0)
		col = 0;
	else if (col > screen->max_col)
		col = screen->max_col;
	else if (col > MOUSE_LIMIT)
		col = MOUSE_LIMIT;

	/* Build key sequence starting with \E[M */
	if (screen->control_eight_bits) {
		line[count++] = CSI;
	} else {
		line[count++] = ESC;
		line[count++] = '[';
	}
	line[count++] = 'M';

	/* Add event code to key sequence */
	if (screen->send_mouse_pos == X10_MOUSE) {
		line[count++] = ' ' + button;
	}
	else
	{
	    /* Button-Motion events */
	    switch(event->type)
	    {
	    case ButtonPress:
		line[count++] = BtnCode(event, screen->mouse_button = button);
		break;
	    case ButtonRelease:
		line[count++] = BtnCode(event, screen->mouse_button = -1);
		break;
	    case MotionNotify:
		/* BTN_EVENT_MOUSE and ANY_EVENT_MOUSE modes send motion
		 * events only if character cell has changed.
		 */
		if ((row == screen->mouse_row)
		 && (col == screen->mouse_col))
			return;
		line[count++] = BtnCode(event, screen->mouse_button) + 32;
		break;
	    default:
		return;
	    }
	}

	screen->mouse_row = row;
	screen->mouse_col = col;

	/* Add pointer position to key sequence */
	line[count++] = ' ' + col + 1;
	line[count++] = ' ' + row + 1;

	/* Transmit key sequence to process running under xterm */
	v_write(pty, (char *)line, count);
}


/*ARGSUSED*/
#if OPT_TEK4014
void HandleGINInput (
    Widget w GCC_UNUSED,
    XEvent *event GCC_UNUSED,
    String *param_list,
    Cardinal *nparamsp)
{
    if (term->screen.TekGIN && *nparamsp == 1) {
	int c = param_list[0][0];
	switch (c) {
	  case 'l': case 'm': case 'r':
	  case 'L': case 'M': case 'R':
	    break;
	  default:
	    Bell (XkbBI_MinorError,0);	/* let them know they goofed */
	    c = 'l';				/* provide a default */
	}
	TekEnqMouse (c | 0x80);
	TekGINoff();
    } else {
	Bell (XkbBI_MinorError,0);
    }
}
#endif /* OPT_TEK4014 */


/* ARGSUSED */
void HandleSecure(
    Widget w GCC_UNUSED,
    XEvent *event,			/* unused */
    String *params GCC_UNUSED,		/* [0] = volume */
    Cardinal *param_count GCC_UNUSED)	/* 0 or 1 */
{
    Time ev_time = CurrentTime;

    if ((event->xany.type == KeyPress) ||
	(event->xany.type == KeyRelease))
	ev_time = event->xkey.time;
    else if ((event->xany.type == ButtonPress) ||
	     (event->xany.type == ButtonRelease))
        ev_time = event->xbutton.time;
    DoSecureKeyboard (ev_time);
}
