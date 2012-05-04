/*
** retroshare-ncurses by Kevin Soules (inceax64@gmail.com)  `date "+©%Y"`
** For license, see COPYING
*/

#include "Menu.h"
#include "ncurses-wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>

RSMenu::RSMenu(std::string title, int h, int w, int y, int x)
{
	_w = w;
	_h = h;
	_y = y;
	_x = x;
	_title = title.c_str();
	_n = 0;
	_items = NULL;
	_subitems = NULL;
	_menu = NULL;
}

RSMenu::~RSMenu()
{
	int i;

	unpost_menu(_menu);
	free_menu(_menu);
	for(i = 0; i < _n ; ++i)
		free_item(_items[i]);
}

/* I dont use c++ vectors here because ncurses need items to be a C array. */
void RSMenu::addItem(std::string name, void *ptr)
{

	/* Double the size instead of incrementing it could be faster,
	** but we won't have more than max 30-40 add, so I don't thinks it
	** is worth to do.
	*/

	_items = (ITEM**) realloc(_items, (_n + 1) * sizeof (*_items));
	_items[_n] = new_item(name.c_str(), "");
	set_item_userptr(_items[_n], ptr);
	_n++;
}


void RSMenu::setItem(ITEM **items)
{
	unpost_menu(_menu);
	set_menu_items(_menu, items);
	post_menu(_menu);
}

void RSMenu::setTitle(std::string title)
{
	_title = title.c_str();
	printTitle();
}

void RSMenu::create()
{
	addItem(NULL);
	_menu = new_menu(_items);
	_win = newwin(_h, _w, _y, _x);
	_subwin =  derwin(_win, getmaxy(_win) - 4, getmaxx(_win) - 2, 3, 1);

	set_menu_win(_menu, _win);
	set_menu_sub(_menu, _subwin);
	
	box(_win, 0, 0);
	print_in_middle(_win, 1, 0, _w, _title, COLOR_PAIR(1));
	mvwaddch(_win, 2, 0, ACS_LTEE);
	mvwhline(_win, 2, 1, ACS_HLINE, _w - 2);
	mvwaddch(_win, 2, _w - 1 , ACS_RTEE);
	set_menu_mark(_menu, "* ");
	//mvprintw(0, 40, "%d\n", post_menu(m->menu));
	post_menu(_menu);
}

/*
** The following code (print_in_middle) as bean adapted from :
** http://www.ibiblio.org/pub/linux/docs/howto/other-formats/html_single/NCURSES-Programming-HOWTO.html#MMEWI
*/

void RSMenu::printTitle()
{
	print_in_middle(_win, 1, 0, _w, _title, COLOR_PAIR(1));
}

void RSMenu::print_in_middle(WINDOW *win, int starty, int startx, int width, std::string string, chtype color)
{	

	int length, x, y;
	float temp;
	const char *cstr = string.c_str();

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(cstr);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	mvwprintw(win, y, 1, "%*s", width - 2, ""); // erase the title
	wattron(win, color);
	mvwprintw(win, y, x, "%s", cstr);
	wattroff(win, color);
	refresh();
}

void RSMenu::down()
{
	menu_driver(_menu, REQ_DOWN_ITEM);
	pos_menu_cursor(_menu);
}

void RSMenu::up()
{
	menu_driver(_menu, REQ_UP_ITEM);
	pos_menu_cursor(_menu);
}

void RSMenu::selected(RSCli *cli)
{
	void *p = NULL;
	typedef void (*fptr)(RSCli *cli);
	fptr fp = NULL;

	p = (void *)item_userptr(current_item(_menu));
	
	/*
	** HAAAARG ! UGLY NON-PORTABLE CODE !
	** http://stackoverflow.com/questions/1096341/function-pointers-casting-in-c
	*/

	fp = reinterpret_cast<fptr>(reinterpret_cast<long>(p));
	if (fp != NULL)
		fp(cli);
}

WINDOW* RSMenu::getWin()
{
	return _win;
}

MENU* RSMenu::getMenu()
{
	return _menu;
}

ITEM** RSMenu::getItems()
{
	return _items;
}
