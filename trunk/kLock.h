namespace kLock
{
	const int Net = 307; //wartoœæ Net
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
		const unsigned Password = Net * 1000 + 102; //has³o
		const unsigned EnableActs = Net * 1000 + 110; //uruchamia kontrolki w konfiguracji
		const unsigned SynchronizeWithAway = Net * 1000 + 103; //synchronizacja z autoawayem
		const unsigned LockMainWindow = Net * 1000 + 105; //blokuj listê i toolbary
		const unsigned LockTalkWindows = Net * 1000 + 106; //blokuj okienko rozmowy i historiê
		const unsigned LockSound = Net * 1000 + 109; //wyciszaj dŸwiêk
		const unsigned LockTray = Net * 1000 + 104; //ukrywaj ikonkê w tray'u
		const unsigned AskForPasswordOnHistory = Net * 1000 + 108; //pytaj o has³o przy próbie dostêpu do historii
		const unsigned ButtonPlace = Net * 1000 + 107; //gdzie wyœwietliæ przycisk
		const unsigned ButtonPlace1 = Net * 1000 + 111; //na toolbarze wtyczek
		const unsigned ButtonPlace2 = Net * 1000 + 112; //na g³ównym toolbarze
		const unsigned ButtonPlace3 = Net * 1000 + 113; //w tray'u
		const unsigned LockProcess = Net * 1000 + 114; //ukrywaj na liœcie procesów
		const unsigned SynchronizeWithkAway = Net * 1000 + 115; //synchronizuj z kAway2
		const unsigned LockKNotify = Net * 1000 + 116; //blokuj powiadomienia kNotify
	}

	namespace Api
	{
		const unsigned Unlock = Net * 1000 + 1; //po wys³aniu zostanie wyœwietlony monit o has³o i, jeœli bêdzie poprawne, odblokowany K, zwraca true jeœli po wywo³aniu K jest odblokowane, false jeœli nie
		const unsigned IsUnlocked = Net * 1000 + 4; //zwraca true jeœli odblokowane, false jeœli zablokowane
		const unsigned IsUnlockedForKNotify = Net * 1000 + 2; //zale¿ne od opcji, czy blokowaæ kNotify - lepiej u¿ywaæ normalnego
		const unsigned Lock = Net * 1000 + 3; //blokuje Konnekta, nic nie zwraca
	}
}