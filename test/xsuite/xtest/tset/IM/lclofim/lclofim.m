/* $XConsortium: lclofim.m,v 1.2 94/04/17 21:13:58 rws Exp $ */
/*

Copyright (c) 1993  X Consortium

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
 * Copyright 1993 by Sun Microsystems, Inc. Mountain View, CA.
 *
 *                   All Rights Reserved
 *
 * Permission  to  use,  copy,  modify,  and  distribute   this
 * software  and  its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright no-
 * tice  appear  in all copies and that both that copyright no-
 * tice and this permission notice appear in  supporting  docu-
 * mentation,  and  that the name of Sun not be used in
 * advertising or publicity pertaining to distribution  of  the
 * software  without specific prior written permission. Sun 
 * makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without any
 * express or implied warranty.
 *
 * SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
 * NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
 * ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
 * PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
>>EXTERN
#include <locale.h>
#include <ximtest.h>

>>TITLE XLocaleOfIM	IM	
char *

XIM im = NULL;
>>SET startup localestartup
>>SET cleanup localecleanup
>>ASSERTION Good A
A call to xname returns the locale of the input method.  
>>STRATEGY
For all locales, create an resource database and open an input method 
compare the locale returned from XLocaleOfIM to the locale which was 
current at the time of the call to XOpenIM. 
>>CODE
char *plocale,*im_locale;
XrmDatabase db = NULL;

	XrmInitialize();

	resetlocale();
	while(nextlocale(&plocale))
	{

		if (locale_set(plocale))
			CHECK;
		else
		{
			report("Couldn't set locale.");
			FAIL;
			continue;
		}

		cleanup_locale(NULL,NULL,im,db);

		db = rm_db_open();
		if(db != NULL)
			CHECK;
		else
		{
			report("Couldn't open database.");
			FAIL;
			continue;
		}

		im = im_open(db);
		if(im != NULL)
			CHECK;
		else
		{
			report("Couldn't open imput method.");
			FAIL;
			continue;
		}

		im_locale = XCALL;
		if(strcmp(im_locale,plocale) != 0)
		{
			report("Locale %s, returned by %s, does not match current locale, %s",
					im_locale,TestName,plocale);
			FAIL;
		}
		else
			CHECK;

	}	/* nextlocale */
	cleanup_locale(NULL,NULL,im,db);

	CHECKPASS(4*nlocales());
