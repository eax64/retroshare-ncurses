/*
** retroshare-ncurses by Kevin Soules (inceax64@gmail.com)  `date "+©%Y"`
** For license, see COPYING
*/

#include "Cli.h"
#include "Menu.h"
#include "FriendsWindow.h"
#include "ncurses-wrapper.h"
#include <iostream>
#include <retroshare/rsmsgs.h>

RSCli::RSCli()
{
	RSMenu *mainMenu = NULL;
	
	int termWidth = 0;
	int termHeight = 0;
	int menuWidth = 0;
	int menuHeight = 0;
	int contentWidth = 0;
	int contentHeight = 0;

	WINDOW *contentWinWrapper = NULL;
	WINDOW *contentWin = NULL;

	termWidth = getmaxx(stdscr);
	termHeight = getmaxy(stdscr);

	menuWidth = termWidth / 4;
	menuHeight = termHeight - 4;

	contentWidth = termWidth - menuWidth - 3;
	contentHeight = menuHeight;

	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);

	termWidth = getmaxx(stdscr);
	termHeight = getmaxy(stdscr);

	menuWidth = 25;
	menuHeight = termHeight - 4;

	contentWidth = termWidth - menuWidth - 3;
	contentHeight = menuHeight;

	/* For now, the callback has to be set here, in the second arg of RSCLI::addItem() */

	mainMenu = new RSMenu("Menu", menuHeight, menuWidth, 1, 2);
	mainMenu->addItem("Network");
	mainMenu->addItem("Friends", (void*)RSFriendsWindow::displayMenu);
	mainMenu->addItem("Search");
	mainMenu->addItem("Transfers");
	mainMenu->addItem("Files");
	mainMenu->addItem("Message");
	mainMenu->addItem("Channels");
	mainMenu->addItem("Forums");
	mainMenu->addItem("Options");
	
	mainMenu->create();
	keypad(mainMenu->getWin(), TRUE);


	contentWinWrapper = newwin(contentHeight, contentWidth, 1, menuWidth + 2);
	contentWin = derwin(contentWinWrapper, getmaxy(contentWinWrapper) - 4, getmaxx(contentWinWrapper) - 2, 3, 1);

	box(contentWinWrapper, 0, 0);
	wrefresh(contentWinWrapper);

	_mainMenu = mainMenu;
	_subMenu = NULL;
	_contentWinWrapper = contentWinWrapper;
	_contentWin = contentWin;
	activeWin = ENUM_RSF_NULL;
	_inputBuffer = "";
	_lastChar = 0;

	/* For now, all the windows calls must have theire pointer member of this class */

	friendsWindow = new RSFriendsWindow();

}

RSCli::~RSCli()
{
	delete _mainMenu;
	endwin();
}

void RSCli::handleKey(int c)
{

	/* TODO: This method has to be an highter level method that call lower level method */
	
	if (activeWin != ENUM_RSFriendsWindow_chat)
		{
			switch(c)
				{
					case KEY_DOWN:
						_mainMenu->down();
						break;
					case KEY_UP:
						_mainMenu->up();
						break;
					case '\n':
						_mainMenu->selected(this);
						break;
				}
			}
		else
			{
				WINDOW *inputWin = friendsWindow->getInputWin();
				if (c == '\n') /* Enter */
					{
						std::wstring ws;
						ws.assign(_inputBuffer.begin(), _inputBuffer.end());

						rsMsgs->sendPublicChat(ws);
						_inputBuffer = "";
						wdeleteln(inputWin);
						wmove(inputWin, 0, 0);
					}
				else if (c == 127) /* backspace */
				{
					int x, y;
					getyx(inputWin, y, x);
					_inputBuffer.erase(_inputBuffer.end() - 1, _inputBuffer.end());
					mvwdelch(inputWin, y, x - 1);
				}
				// we must do something to avoid repetition here
				else if (c == 23) /* CTRL + W */
				{
					std::string lastWord;
					int x, y;
					size_t lastWordPos;

					if ((lastWordPos = _inputBuffer.find_last_of(" ")) == std::string::npos)
						lastWord = _inputBuffer;
					else
						lastWord = _inputBuffer.substr(lastWordPos + 1);

					_inputBuffer.erase(_inputBuffer.end() - lastWord.length(), _inputBuffer.end());
					getyx(inputWin, y, x);
					for (size_t i = 0 ; i < lastWord.length() ; i++)
						mvwdelch(inputWin, y, x - i - 1);
				}
				// we must do something to avoid repetition here
				else if (c == 21) /* CTRL + U */
				{
					int x, y;

					_inputBuffer.erase(_inputBuffer.begin(), _inputBuffer.end());
					getyx(inputWin, y, x);
					for (int i = x ; i >= 0 ; i--)
						mvwdelch(inputWin, y, i);
				}
				else if (c == '\t')
					{
						std::string lastWord;
						size_t lastWordPos;
						if ((lastWordPos = _inputBuffer.find_last_of(" ")) == std::string::npos)
							lastWord = _inputBuffer;
						else
							lastWord = _inputBuffer.substr(lastWordPos + 1);

						nbrTimeAutoComplet = (_lastChar == '\t') ? nbrTimeAutoComplet + 1 : 0;
						std::string pseudo = friendsWindow->autoCompletePseudo(lastWord, nbrTimeAutoComplet);
						if (pseudo != "")
							{
								_inputBuffer.erase(_inputBuffer.end() - lastWord.length(), _inputBuffer.end());
								_inputBuffer += pseudo;
								int x, y;
								getyx(inputWin, y, x);
								for (size_t i = 0 ; i < lastWord.length() ; i++)
									mvwdelch(inputWin, y, x - i - 1);
								wprintw(inputWin, "%s", pseudo.c_str());
							}
						_lastWord == lastWord;
					}
				else
					{
						friendsWindow->updateRemoteStatusTyping();
						_inputBuffer += char(c);
						wprintw(inputWin, "%c", c);
					}
				_lastChar = c;
				wrefresh(inputWin);
				wrefresh(_contentWin);
			}
}

void RSCli::refresh()
{
	wrefresh(_mainMenu->getWin());
	wrefresh(_contentWin);
	wrefresh(_contentWinWrapper);
	wrefresh(stdscr);

	/*
	** wrefresh(stdscr) is same as refresh() function from ncurses. But if
	** we call refresh() it's the member function of this class 
	** (this current method) that will be called. 
	*/
}

RSMenu* RSCli::getMainMenu()
{
	return _mainMenu;
}

WINDOW* RSCli::getContentWin()
{
	return _contentWin;
}

WINDOW* RSCli::getContentWinWrapper()
{
	return _contentWinWrapper;
}
