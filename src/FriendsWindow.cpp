/*
** retroshare-ncurses by Kevin Soules (inceax64@gmail.com)  `date "+©%Y"`
** For license, see COPYING
*/

#include "FriendsWindow.h"
#include "ncurses-wrapper.h"
#include "Menu.h"
#include "Cli.h"
#include <retroshare/rspeers.h>
#include <retroshare/rsmsgs.h>
#include <iostream>
#include <sstream>
#include <ctime>

RSFriendsWindow::RSFriendsWindow()
{
	_menu = NULL;
	_inputWin = NULL;
	_lastStatusStringSended = 0;
}


void RSFriendsWindow::displayMainMenu(RSCli *cli)
{
	char *menuTitle = (char*)item_name(current_item(cli->getMainMenu()->getMenu()));
	RSMenu *m = cli->getMainMenu();
	m->setItem(m->getItems());
	cli->getMainMenu()->setTitle(menuTitle);
}

void RSFriendsWindow::displayMenu(RSCli *cli)
{
	char *menuTitle = (char*)item_name(current_item(cli->getMainMenu()->getMenu()));

	 /* FIXME: /!\ Warning memory leak here */
	RSMenu *menu = new RSMenu("", 42, 42, 42, 42);

	menu->addItem("Main menu", (void*)displayMainMenu);
	menu->addItem("List", (void*)displayList);
	menu->addItem("Public chat", (void*)displayPublicChat);
	menu->addItem("");

	cli->getMainMenu()->setItem(menu->getItems());
	cli->getMainMenu()->setTitle(menuTitle);
	
}

void RSFriendsWindow::displayList(RSCli *cli)
{
	WINDOW *contentWin = cli->getContentWin();

	cli->activeWin = ENUM_RSFriendsWindow_list;
	wclear(contentWin);


	wclear(contentWin);
	std::list<std::string> ids;
	std::list<std::string>::iterator it;

	rsPeers->getFriendList(ids);

	std::ostringstream out;
	for(it = ids.begin(); it != ids.end(); it++)
		{
			RsPeerDetails detail;
			rsPeers->getPeerDetails(*it, detail);
			wprintw(contentWin, "%s (%s)\n", detail.name.c_str(), ((detail.state & RS_PEER_STATE_CONNECTED) ? "Connected" : "Not connected"));
		}

	wrefresh(contentWin);
}

void RSFriendsWindow::displayPublicChat(RSCli *cli)
{
	WINDOW *contentWin = cli->getContentWin();
	std::vector<std::string> *msg = cli->friendsWindow->getChatMsg();
	cli->activeWin = ENUM_RSFriendsWindow_chat;
	wclear(contentWin);
	for (size_t i = 0 ; i < msg->size() ; i++)
		wprintw(contentWin, ((*msg)[i] + "\n").c_str());

	if (cli->friendsWindow->getInputWin() == NULL)
		cli->friendsWindow->setInputWin(derwin(contentWin, 1, getmaxx(contentWin), getmaxy(contentWin) - 1, 0));
	
	wmove(cli->getContentWin(), 0, 0);
	wmove(cli->friendsWindow->getInputWin(), 0, 0);
	wrefresh(contentWin);
		
}

void RSFriendsWindow::addChatLine(RSCli *cli, std::string msg)
{
	_chatMsg.push_back(msg);
	if (cli->activeWin == ENUM_RSFriendsWindow_chat)
		{

			if (_chatMsg.size() > (unsigned)getmaxy(cli->getContentWin()) - 2) /* if we have to scroll */
				{
					wprintw(cli->getContentWin(), (msg).c_str());
					scrollok(cli->getContentWin(), TRUE);
					wscrl(cli->getContentWin(), 1);
					wmove(cli->getContentWin(), getmaxy(cli->getContentWin()) - 2, 0);
				}
			else
				wprintw(cli->getContentWin(), (msg + "\n").c_str());
		}
	wrefresh(cli->getContentWin());
}

std::vector<std::string>* RSFriendsWindow::getChatMsg()
{
	return &_chatMsg;
}

WINDOW* RSFriendsWindow::getInputWin()
{
	return _inputWin;
}

void RSFriendsWindow::setInputWin(WINDOW *inputWin)
{
	_inputWin = inputWin;
}

void RSFriendsWindow::updateRemoteStatusTyping()
{
	if(time(NULL) - _lastStatusStringSended > 5)  // limit 'peer is typing' packets to at most every 10 sec
		{   
			rsMsgs->sendGroupChatStatusString("is typing... on his ncurses client !");
			_lastStatusStringSended = time(NULL) ;
		}   
}

std::string RSFriendsWindow::autoCompletePseudo(std::string lastWord, int nbrTimeAutoComplet)
{
	std::list<std::string> peers;
	std::list<std::string>::iterator it;
	size_t i = 0;

	if (!rsPeers)
		return "";
	
	rsPeers->getOnlineList(peers);

	for(it = peers.begin(); it != peers.end(); it++)
		{
			RsPeerDetails details;
			if (!rsPeers->getPeerDetails(*it, details))
				{
					continue; /* BAD */
					/* It was a Retroshare's dev comment, I thinks this line is a bad one... */
				}
			if (details.name.find(lastWord) == 0 && (nbrTimeAutoComplet % peers.size()) <= i)
				{
					return details.name;
					i++;
				}
		}
	return "";

}
