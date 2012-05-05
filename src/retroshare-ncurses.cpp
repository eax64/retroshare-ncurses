/*
** retroshare-ncurses by Kevin Soules (inceax64@gmail.com)  `date '+©%Y'`
** For license, see COPYING
*/

#include <retroshare/rsiface.h>
#include <retroshare/rsinit.h>

#include "notifytxt.h"

#include <QApplication>
#include <iostream>
#include <unistd.h>
#include <ctype.h>
#include <curses.h>
#include <menu.h>
#include <string.h>
#include "Menu.h"
#include "Cli.h"


int main(int argc, char **argv)
{

	RsInit::InitRsConfig();
	int initResult = RsInit::InitRetroShare(argc, argv);
	int c;				
	WINDOW *menuWin = NULL;
	RSCli *cli = NULL;

	//QApplication app(argc, argv);

	if (initResult < 0)
		{
			/* Error occured */
			switch (initResult)
				{
					case RS_INIT_AUTH_FAILED:
						std::cerr << "RsInit::InitRetroShare AuthGPG::InitAuth failed" << std::endl;
						break;
					default:
					/* Unexpected return code */
					std::cerr << "RsInit::InitRetroShare unexpected return code " << initResult << std::endl;
					break;
				}
		return 1;
	}

	cli = new RSCli();
	menuWin = cli->getMainMenu()->getWin();

	NotifyTxt *notify = new NotifyTxt(cli);
	RsIface *iface = createRsIface(*notify);
	RsControl *rsServer = createRsControl(*iface, *notify);
	rsicontrol = rsServer ;

	notify->setRsIface(iface);

	std::string preferredId, gpgId, gpgName, gpgEmail, sslName;
	RsInit::getPreferedAccountId(preferredId);

	if (RsInit::getAccountDetails(preferredId, gpgId, gpgName, gpgEmail, sslName))
	{
		RsInit::SelectGPGAccount(gpgId);
	}

	std::string error_string ;
	int retVal = RsInit::LockAndLoadCertificates(false,error_string);
	switch(retVal)
	{
		case 0:	break;
		case 1:	std::cerr << "Error: another instance of retroshare is already using this profile" << std::endl;
				return 1;
		case 2: std::cerr << "An unexpected error occurred while locking the profile" << std::endl;
				return 1;
		case 3: std::cerr << "An error occurred while login with the profile" << std::endl;
				return 1;
		default: std::cerr << "Main: Unexpected switch value " << retVal << std::endl;
				return 1;
	}



	/* Start-up libretroshare server threads */
	rsServer->StartupRetroShare();

	while((c = wgetch(menuWin)) != 27) /* 27 is ASCII code for ESC */
		{
			cli->handleKey(c); 
		}	

	delete cli;

	return 0;
}
