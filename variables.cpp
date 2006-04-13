#include "includes.h"

namespace kLock
{
	int cnt_history_owner; //poprzedni w³aœciciel akcji histori w menu kontaktu 
	int history_owner; //poprzedni w³aœciciel akcji histori
	int msgwnd_history_owner; //poprzedni w³aœciciel akcji histori w oknie kontaktu
	std::vector<LockedWindow> locked_windows; //okna przypiête przy zak³adaniu blokady
	bool main_visible; //czy g³ówne okienko by³o widoczne
	bool msg_visible; //czy okienko rozmowy by³o widoczne
	bool muted; //czy kSound by³ wyciszony
	WNDPROC old_mainwnd_proc; //stary proc g³ównego okna
	bool unlocking; //czy program jest w trakcie odblokowywania
	bool acts_enabled; //czy s¹ aktywne kontrolki konfiguracji 
}