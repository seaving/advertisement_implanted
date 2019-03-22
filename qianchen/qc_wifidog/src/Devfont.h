/*
 * Copyright (c) 2000, 2002 Greg Haerr <greg@censoft.com>
 *
 * Device-independent font and text drawing routines
 *
 * These routines do the necessary range checking, clipping, and cursor
 * overwriting checks, and then call the lower level device dependent
 * routines to actually do the drawing.  The lower level routines are
 * only called when it is known that all the pixels to be drawn are
 * within the device area and are visible.
 */

#ifndef DEVFONT_H
#define DEVFONT_H

typedef unsigned short	wchar;
typedef unsigned char	mchar;
int MultiByteToWideChar(wchar *wstr,int wstrsize,mchar *mstr,int mstrlen);
int WideCharToMultiByte(wchar *wstr,int wstrsize,mchar *mstr,int mstrlen);

#endif

