#include "includes.h"

namespace kLock
{
	int cnt_history_owner; //poprzedni w�a�ciciel akcji histori w menu kontaktu 
	int history_owner; //poprzedni w�a�ciciel akcji histori
	int msgwnd_history_owner; //poprzedni w�a�ciciel akcji histori w oknie kontaktu
	std::vector<LockedWindow> locked_windows; //okna przypi�te przy zak�adaniu blokady
	bool main_visible; //czy g��wne okienko by�o widoczne
	bool msg_visible; //czy okienko rozmowy by�o widoczne
	bool muted; //czy kSound by� wyciszony
	WNDPROC old_mainwnd_proc; //stary proc g��wnego okna
	bool unlocking; //czy program jest w trakcie odblokowywania
	bool acts_enabled; //czy s� aktywne kontrolki konfiguracji 
}