namespace kLock
{
	//funkcja obs³uguj¹ca wiadomoœci g³ównego okna
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	//funkcja obs³uguj¹ca wiadomoœci okienek rozmowy
	LRESULT CALLBACK TalkWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	//funkcja wyœwietlaj¹ca monit o has³o, jeœli siê zgadza zwraca 1, jeœli nie 0
	int AskForPassword(std::string title, std::string text, HWND parent = 0);

	//funkcja blokuj¹ca Konnekta
	void Lock();

	//funkcja pytaj¹ca o has³o i odblokowuj¹ca Konnekta
	int Unlock();

	//funkcja aktywuj¹ca kontrolki w konfiguracji
	void EnableActs();

	//funkcja deaktywuj¹ca kontrolki w konfiguracji
	void DisableActs();
}