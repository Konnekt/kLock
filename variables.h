namespace kLock
{
	extern int cnt_history_owner; //poprzedni w�a�ciciel akcji histori w menu kontaktu 
	extern int history_owner; //poprzedni w�a�ciciel akcji histori
	extern int msgwnd_history_owner; //poprzedni w�a�ciciel akcji histori w oknie kontaktu
	extern std::vector<LockedWindow> locked_windows; //okna przypi�te przy zak�adaniu blokady
	extern bool main_visible; //czy g��wne okienko by�o widoczne
	extern bool msg_visible; //czy okienko rozmowy by�o widoczne
	extern bool muted; //czy kSound by� wyciszony
	extern WNDPROC old_mainwnd_proc; //stary proc g��wnego okna
	extern bool unlocking; //czy program jest w trakcie odblokowywania
	extern bool acts_enabled; //czy s� aktywne kontrolki konfiguracji 
	extern bool migacz_status; //czy by�o w��czone miganie przy zmianie statusu
	extern bool migacz_msg; //czy by�o w��czone miganie przy wiadomo�ci
}