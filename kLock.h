/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */
 
 #pragma once

namespace kLock
{
	const int net = 307; //warto�� net
	const int sig = (int)"KLOCK"; //sygnatura
	const int name = (int)"kLock"; //nazwa

	namespace CFG
	{
		const unsigned group = net * 1000 + 100; //grupa w configu
		const unsigned lockState = net * 1000 + 101; //status zablokowania
		const unsigned password = net * 1000 + 102; //has�o
		const unsigned synchronizeWithAway = net * 1000 + 103; //synchronizacja z autoawayem
		const unsigned lockMainWindow = net * 1000 + 105; //blokuj list� i toolbary
		const unsigned lockTalkWindows = net * 1000 + 106; //blokuj okienko rozmowy i histori�
		const unsigned lockSound = net * 1000 + 109; //wyciszaj d�wi�k
		const unsigned lockTray = net * 1000 + 104; //ukrywaj ikonk� w tray'u
		const unsigned askForPasswordOnHistory = net * 1000 + 108; //pytaj o has�o przy pr�bie dost�pu do historii
		const unsigned buttonInTray = net * 1000 + 107; //przycisk w menu tray'a
		const unsigned buttonOnToolbar = net * 1000 + 111; //przycisk na toolbarze wtyczek
		const unsigned buttonOnMainToolbar = net * 1000 + 112; //przycisk na g��wnym toolbarze
		const unsigned lockProcess = net * 1000 + 114; //ukrywaj na li�cie proces�w
		const unsigned synchronizeWithkAway = net * 1000 + 115; //synchronizuj z kAway2
		const unsigned lockKNotify = net * 1000 + 116; //blokuj powiadomienia kNotify
		const unsigned lockkMigacz = net * 1000 + 117; //blokuj kMigacza
		const unsigned disablekAwayWhenUnlocking = net * 1000 + 118; //przy odblokowywaniu wy��czaj kAway
		const unsigned blockOnStart = net * 1000 + 119; //blokuj zawsze na starcie
		const unsigned synchronizeWithExtendedAway = net * 1000 + 120; //synchronizacja z extended autoawayem
	}

	namespace ACT
	{
		const unsigned lock = net * 1000 + 1; //blokada okien rozmowy
		const unsigned enableActs = net * 1000 + 2; //uruchamia kontrolki w konfiguracji
	}

	namespace API
	{
		const unsigned unlock = net * 1000 + 1; //po wys�aniu zostanie wy�wietlony monit o has�o i, je�li b�dzie poprawne, odblokowany K. Zwraca true je�li po wywo�aniu K jest odblokowany, false je�li nie.
		const unsigned lock = net * 1000 + 3; //blokuje Konnekta, nic nie zwraca
		const unsigned isUnlocked = net * 1000 + 4; //zwraca true je�li odblokowane, false je�li zablokowane
		const unsigned isUnlockedForKNotify = net * 1000 + 2; //zale�ne od opcji, czy blokowa� kNotify - u�ywa� normalnego
		const unsigned isUnlockedForkMigacz = net * 1000 + 5; //zale�ne od opcji, czy blokowa� kMigacza - u�ywa� normalnego
	}
}