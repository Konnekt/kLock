namespace kLock
{
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam); //funkcja obs³uguj¹ca g³ówne okno, blokuj¹c je
	LRESULT CALLBACK TalkWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam); //funkcja obs³uguj¹ca okienko rozmowy, blokuj¹c je
	int AskForPassword(std::string title, std::string text, HWND parent = 0); //funkcja wyœwietlaj¹ca monit o has³o, jeœli siê zgadza zwraca 1, jeœli nie 0
	void Lock(); //funkcja blokuj¹ca Konnekta
	int Unlock(); //funkcja odblokowuj¹ca Konnekta
	void EnableActs(); //funkcja aktywuj¹ca kontrolki w konfiguracji
	void DisableActs(); //funkcja deaktywuj¹ca kontrolki w konfiguracji
}