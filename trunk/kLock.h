namespace kLock
{
	const int NET = 307; //wartoœæ NET
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
		const unsigned PASSWORD = NET * 1000 + 102; //has³o
		const unsigned ENABLEACTS = NET * 1000 + 110; //uruchamia kontrolki w konfiguracji
		const unsigned SYNCHRONIZEWITHAWAY = NET * 1000 + 103; //synchronizacja z autoawayem
		const unsigned LOCKLISTANDTOOLBARS = NET * 1000 + 105; //blokuj listê i toolbary
		const unsigned LOCKTALKWINDOWS = NET * 1000 + 106; //blokuj okienko rozmowy i historiê
		const unsigned LOCKSOUND = NET * 1000 + 109; //wyciszaj dŸwiêk
		const unsigned LOCKTRAY = NET * 1000 + 104; //ukrywaj ikonkê w tray'u
		const unsigned ASKFORPASSWORDONHISTORY = NET * 1000 + 108; //pytaj o has³o przy próbie dostêpu do historii
		const unsigned BUTTONPLACE = NET * 1000 + 107; //gdzie wyœwietliæ przycisk
		const unsigned BUTTONPLACE1 = NET * 1000 + 111; //na toolbarze wtyczek
		const unsigned BUTTONPLACE2 = NET * 1000 + 112; //na g³ównym toolbarze
		const unsigned BUTTONPLACE3 = NET * 1000 + 113; //w tray'u
		const unsigned LOCKPROCESS = NET * 1000 + 114; //ukrywaj na liœcie procesów
		const unsigned SYNCHRONIZEWITHKAWAY = NET * 1000 + 115; //synchronizuj z kAway2
		const unsigned LOCKKNOTIFY = NET * 1000 + 116; //blokuj powiadomienia kNotify
	}

	namespace api
	{
		const unsigned UNLOCK = NET * 1000 + 1; //po wys³aniu zostanie wyœwietlony monit o has³o i, jeœli bêdzie poprawne, odblokowany K, zwraca true jeœli po wywo³aniu K jest odblokowane, false jeœli nie
		const unsigned ISUNLOCKED = NET * 1000 + 2; //otrzymamy true jeœli jest odblokowane, false jeœli zablokowane
		const unsigned LOCK = NET * 1000 + 3; //blokuje Konnekta, nic nie zwraca
	}
}