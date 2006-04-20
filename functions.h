namespace kLock
{
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam); //funkcja obs�uguj�ca g��wne okno, blokuj�c je
	LRESULT CALLBACK TalkWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam); //funkcja obs�uguj�ca okienko rozmowy, blokuj�c je
	int AskForPassword(std::string title, std::string text, HWND parent = 0); //funkcja wy�wietlaj�ca monit o has�o, je�li si� zgadza zwraca 1, je�li nie 0
	void Lock(); //funkcja blokuj�ca Konnekta
	int Unlock(); //funkcja odblokowuj�ca Konnekta
	void EnableActs(); //funkcja aktywuj�ca kontrolki w konfiguracji
	void DisableActs(); //funkcja deaktywuj�ca kontrolki w konfiguracji
}