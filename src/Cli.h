/*
** retroshare-ncurses by Kevin Soules (inceax64@gmail.com)  `date "+©%Y"`
** For license, see COPYING
*/

#ifndef _CLI_H_
#define _CLI_H_

#include "ncurses-wrapper.h"
#include "Menu.h"
#include "FriendsWindow.h"
#include <string>

/*
** Maybe there is a better way to know what is the current window
** than use un enum
*/
enum ActiveWin{ ENUM_RSFriendsWindow_list,
	ENUM_RSFriendsWindow_chat, ENUM_RSF_NULL };

class RSMenu;
class RSFriendsWindow;

class RSCli
{
	public:
		RSCli();
		~RSCli();
			
		void handleKey(int c);
		void refresh();

		RSMenu* getMainMenu();
		WINDOW* getContentWin();
		WINDOW* getContentWinWrapper();

		RSFriendsWindow *friendsWindow;

		ActiveWin activeWin;

	private:
		RSMenu *_mainMenu;
		RSMenu *_subMenu;
		WINDOW *_contentWinWrapper;
		WINDOW *_contentWin;
		std::string _inputBuffer;
		int _lastChar;
		std::string _lastWord;
		int nbrTimeAutoComplet;
};

#endif
