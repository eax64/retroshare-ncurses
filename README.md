retroshare-ncurses
==================

A retroshare client using ncurses

Just some features are avalable, come back later to if you want more.

You have to compile retroshare-ncurses like others retroshare client : http://retroshare.sourceforge.net/wiki/index.php/UnixCompile
To do that, you have to place retroshare-ncurses directory in the retroshare development directory.
You can get it with this command :

svn co https://retroshare.svn.sourceforge.net/svnroot/retroshare retroshare

Then you can go to the retroshare directory and compile the whole :

cd libbitdht/src
qmake
make
cd ../../libretroshare/src
qmake
make
cd ../../retroshare-ncurses/src
qmake
make

Finaly you can run retroshare-ncurses with:

./retroshare-ncurses
Or
./retroshare-ncurses -U yourRetroshareUser
