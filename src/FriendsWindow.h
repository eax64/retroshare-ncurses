/*
** retroshare-ncurses by Kevin Soules (inceax64@gmail.com)  `date "+©%Y"`
** For license, see COPYING
*/

#ifndef _FRIENDWINDOW_H_
#define _FRIENDWINDOW_H_

#include "Cli.h"
#include "Menu.h"
#include "ncurses-wrapper.h"
#include <vector>
#include <string>

class RSCli;
class RSMenu;

class RSFriendsWindow
{
	public:
		RSFriendsWindow();

		static void displayMenu(RSCli *cli);
		static void displayMainMenu(RSCli *cli);
		static void displayList(RSCli *cli);
		static void displayPublicChat(RSCli *cli);
		void addChatLine(RSCli *cli, std::string msg);
		WINDOW* getInputWin();
		void setInputWin(WINDOW *inputWin);
		void updateRemoteStatusTyping();
		std::string autoCompletePseudo(std::string lastWord, int nbrTimeAutoComplet);

		std::vector<std::string>* getChatMsg();

	private:
		RSMenu *_menu;
		WINDOW *_inputWin;
		time_t _lastStatusStringSended;

		std::vector<std::string> _chatMsg;

};

#endif
