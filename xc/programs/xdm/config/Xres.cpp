! $XConsortium: Xresources /main/8 1996/11/11 09:24:46 swick $
#define BS \ /* cpp can be trickier than m4 */
#define NLBS \n\ /* don't remove these comments */
xlogin*login.translations: #override BS
	Ctrl<Key>R: abort-display()NLBS
	<Key>F1: set-session-argument(failsafe) finish-field()NLBS
	Ctrl<Key>Return: set-session-argument(failsafe) finish-field()NLBS
	<Key>Return: set-session-argument() finish-field()
xlogin*borderWidth: 3
#ifndef XPM
xlogin*greeting: CLIENTHOST
xlogin*namePrompt: login:\040
#else
xlogin*greeting: Welcome to CLIENTHOST
xlogin*namePrompt: \040\040\040\040\040\040\040Login:
#endif /* XPM */
xlogin*fail: Login incorrect
#ifdef XPM
xlogin*greetFont: -adobe-helvetica-bold-o-normal--24-240-75-75-p-138-iso8859-1
xlogin*font: -adobe-helvetica-medium-r-normal--18-180-75-75-p-103-iso8859-1
xlogin*promptFont: -adobe-helvetica-bold-r-normal--18-180-75-75-p-103-iso8859-1
xlogin*failFont: -adobe-helvetica-bold-r-normal--18-180-75-75-p-103-iso8859-1
#endif /* XPM */
/**/#ifdef COLOR
#ifndef XPM
xlogin*greetColor: CadetBlue
#else
xlogin*borderWidth: 1
xlogin*frameWidth: 5
xlogin*innerFramesWidth: 2
xlogin*shdColor: grey30
xlogin*hiColor: grey90
xlogin*background: grey
!xlogin*foreground: darkgreen
xlogin*greetColor: Blue3
#endif /* XPM */
xlogin*failColor: red
*Foreground: black
*Background: #fffff0
/**/#else
#ifdef XPM
xlogin*borderWidth: 3
xlogin*frameWidth: 0
xlogin*innerFramesWidth: 0
xlogin*shdColor: black
xlogin*hiColor: white
#endif /* XPM */
xlogin*Foreground: black
xlogin*Background: white
/**/#endif
#ifdef XPM
xlogin*logoFileName: BITMAPDIR/**//OpenBSD.xpm
xlogin*useShape: true
xlogin*logoPadding: 10

#endif /* XPM */

XConsole.text.geometry:	480x130
XConsole.verbose:	true
XConsole*iconic:	true
#ifdef XPM
XConsole*background:	black
XConsole*foreground:	white
XConsole*borderWidth:	2
XConsole*borderColor:   grey
#endif /* XPM */
XConsole*font:		fixed

Chooser*geometry:		700x500+300+200
Chooser*allowShellResize:	false
Chooser*viewport.forceBars:	true
Chooser*label.font:		*-new century schoolbook-bold-i-normal-*-240-*
Chooser*label.label:		XDMCP Host Menu  from CLIENTHOST
Chooser*list.font:		-*-*-medium-r-normal-*-*-230-*-*-c-*-iso8859-1
Chooser*Command.font:		*-new century schoolbook-bold-r-normal-*-180-*
