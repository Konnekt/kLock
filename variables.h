namespace kLock
{
	extern int cnt_history_owner; //poprzedni w³aœciciel akcji histori w menu kontaktu 
	extern int history_owner; //poprzedni w³aœciciel akcji histori
	extern int msgwnd_history_owner; //poprzedni w³aœciciel akcji histori w oknie kontaktu
	extern std::vector<LockedWindow> locked_windows; //okna przypiête przy zak³adaniu blokady
	extern bool main_visible; //czy g³ówne okienko by³o widoczne
	extern bool msg_visible; //czy okienko rozmowy by³o widoczne
	extern bool muted; //czy kSound by³ wyciszony
	extern WNDPROC old_mainwnd_proc; //stary proc g³ównego okna
	extern bool unlocking; //czy program jest w trakcie odblokowywania
	extern bool acts_enabled; //czy s¹ aktywne kontrolki konfiguracji 
	extern bool migacz_status; //czy by³o w³¹czone miganie przy zmianie statusu
	extern bool migacz_msg; //czy by³o w³¹czone miganie przy wiadomoœci
}