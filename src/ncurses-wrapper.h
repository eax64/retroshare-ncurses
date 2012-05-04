#ifndef _NCURSES_WRAPPER_H_
#define _NCURSES_WRAPPER_H_

/*
** this file fix the conflict between some ncurses function
** and some Qt member function
*/

#include <ncurses.h>
inline int ncurses_scroll(WINDOW *w) { return scroll(w); }
inline int ncurses_border(chtype ls, chtype rs, chtype ts, chtype bs, 
		chtype tl, chtype tr, chtype bl, chtype br) { return border(ls, 
			rs, ts, bs, tl, tr, bl, br); }

#ifdef scroll
	#undef scroll
#endif
#ifdef border
	#undef border
#endif

#endif
