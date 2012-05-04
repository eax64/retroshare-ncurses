/*
** retroshare-ncurses by Kevin Soules (inceax64@gmail.com)  `date "+©%Y"`
** For license, see COPYING
*/

#ifndef _MENU_H_
#define _MENU_H_

#include "ncurses-wrapper.h"
#include <menu.h>
#include <string>
#include "Cli.h"

class RSCli;

class RSMenu
{

	public:
		RSMenu(std:: string title, int h, int w, int y, int x);
		~RSMenu();
		void addItem(std::string name, void *ptr = NULL); // Add an item to the menu
		void setItem(ITEM **items); // Change items menu
		void setTitle(std::string title); // Change menu name
		void create();

		void down(); // select the item one row above the current
		void up(); // select the item one row below the current
		void selected(RSCli *cli);

		WINDOW* getWin();
		MENU* getMenu();
		ITEM** getItems();

		void printTitle();
		static void print_in_middle(WINDOW *win, int starty, int startx, int width, std::string string, chtype color);

	private:
		int _w; // Weight
		int _h; // Height
		int _x;
		int _y;
		int _n; // Number of items
		std::string _title;
		ITEM **_items;
		ITEM **_subitems;
		WINDOW *_win;
		WINDOW *_subwin;
		MENU *_menu;

};

#endif
