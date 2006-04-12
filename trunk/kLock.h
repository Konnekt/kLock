namespace kLock
{
	const int NET = 307; //warto�� NET
	const int SIG = (int)"KLOCK"; //sygnatura
	const int NAME = (int)"kLock"; //nazwa

	namespace acts
	{
		const unsigned LOCK = NET * 1000 + 1; //blokada okien rozmowy
	}

	namespace config
	{
		const unsigned GROUP	= NET * 1000 + 100; //grupa w configu
		const unsigned STATE = NET * 1000 + 101; //status zablokowania
		const unsigned PASSWORD = NET * 1000 + 102; //has�o
		const unsigned ENABLEACTS = NET * 1000 + 110; //uruchamia kontrolki w konfiguracji
		const unsigned SYNCHRONIZEWITHAWAY = NET * 1000 + 103; //synchronizacja z autoawayem
		const unsigned LOCKLISTANDTOOLBARS = NET * 1000 + 105; //blokuj list� i toolbary
		const unsigned LOCKTALKWINDOWS = NET * 1000 + 106; //blokuj okienko rozmowy i histori�
		const unsigned LOCKSOUND = NET * 1000 + 109; //wyciszaj d�wi�k
		const unsigned LOCKTRAY = NET * 1000 + 104; //ukrywaj ikonk� w tray'u
		const unsigned ASKFORPASSWORDONHISTORY = NET * 1000 + 108; //pytaj o has�o przy pr�bie dost�pu do historii
		const unsigned BUTTONPLACE = NET * 1000 + 107; //gdzie wy�wietli� przycisk
		const unsigned BUTTONPLACE1 = NET * 1000 + 111; //na toolbarze wtyczek
		const unsigned BUTTONPLACE2 = NET * 1000 + 112; //na g��wnym toolbarze
		const unsigned BUTTONPLACE3 = NET * 1000 + 113; //w tray'u
		const unsigned LOCKPROCESS = NET * 1000 + 114; //ukrywaj na li�cie proces�w
		const unsigned SYNCHRONIZEWITHKAWAY = NET * 1000 + 115; //synchronizuj z kAway2
		const unsigned LOCKKNOTIFY = NET * 1000 + 116; //blokuj powiadomienia kNotify
	}

	namespace api
	{
		const unsigned UNLOCK = NET * 1000 + 1; //po wys�aniu zostanie wy�wietlony monit o has�o i, je�li b�dzie poprawne, odblokowany K, zwraca true je�li po wywo�aniu K jest odblokowane, false je�li nie
		const unsigned ISUNLOCKED = NET * 1000 + 2; //otrzymamy true je�li jest odblokowane, false je�li zablokowane
		const unsigned LOCK = NET * 1000 + 3; //blokuje Konnekta, nic nie zwraca
	}
}