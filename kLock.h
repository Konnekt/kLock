namespace kLock
{
	const int Net = 307; //warto�� Net
	const int Sig = (int)"KLOCK"; //sygnatura
	const int Name = (int)"kLock"; //nazwa

	namespace Acts
	{
		const unsigned Lock = Net * 1000 + 1; //blokada okien rozmowy
	}

	namespace Config
	{
		const unsigned Group = Net * 1000 + 100; //grupa w configu
		const unsigned State = Net * 1000 + 101; //status zablokowania
		const unsigned Password = Net * 1000 + 102; //has�o
		const unsigned EnableActs = Net * 1000 + 110; //uruchamia kontrolki w konfiguracji
		const unsigned SynchronizeWithAway = Net * 1000 + 103; //synchronizacja z autoawayem
		const unsigned LockMainWindow = Net * 1000 + 105; //blokuj list� i toolbary
		const unsigned LockTalkWindows = Net * 1000 + 106; //blokuj okienko rozmowy i histori�
		const unsigned LockSound = Net * 1000 + 109; //wyciszaj d�wi�k
		const unsigned LockTray = Net * 1000 + 104; //ukrywaj ikonk� w tray'u
		const unsigned AskForPasswordOnHistory = Net * 1000 + 108; //pytaj o has�o przy pr�bie dost�pu do historii
		const unsigned ButtonInTray = Net * 1000 + 107; //przycisk w menu tray'a
		const unsigned ButtonOnToolbar = Net * 1000 + 111; //przycisk na toolbarze wtyczek
		const unsigned ButtonOnMainToolbar = Net * 1000 + 112; //przycisk na g��wnym toolbarze
		const unsigned LockProcess = Net * 1000 + 114; //ukrywaj na li�cie proces�w
		const unsigned SynchronizeWithkAway = Net * 1000 + 115; //synchronizuj z kAway2
		const unsigned LockKNotify = Net * 1000 + 116; //blokuj powiadomienia kNotify
		const unsigned LockkMigacz = Net * 1000 + 117; //blokuj kMigacza
		const unsigned TurnOffkAwayOnUnlocking = Net * 1000 + 118; //przy odblokowywaniu wy��czaj kAway
		const unsigned BlockOnStart = Net * 1000 + 119; //blokuj zawsze na starcie
	}

	namespace Api
	{
		const unsigned Unlock = Net * 1000 + 1; //po wys�aniu zostanie wy�wietlony monit o has�o i, je�li b�dzie poprawne, odblokowany K, zwraca true je�li po wywo�aniu K jest odblokowane, false je�li nie
		const unsigned IsUnlocked = Net * 1000 + 4; //zwraca true je�li odblokowane, false je�li zablokowane
		const unsigned IsUnlockedForKNotify = Net * 1000 + 2; //zale�ne od opcji, czy blokowa� kNotify - u�ywa� normalnego
		const unsigned IsUnlockedForkMigacz = Net * 1000 + 5; //zale�ne od opcji, czy blokowa� kMigacza - u�ywa� normalnego
		const unsigned Lock = Net * 1000 + 3; //blokuje Konnekta, nic nie zwraca
	}
}