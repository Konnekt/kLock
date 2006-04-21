namespace kLock
{
	//funkcja obs�uguj�ca wiadomo�ci g��wnego okna
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	//funkcja obs�uguj�ca wiadomo�ci okienek rozmowy
	LRESULT CALLBACK TalkWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	//funkcja wy�wietlaj�ca monit o has�o, je�li si� zgadza zwraca 1, je�li nie 0
	int AskForPassword(std::string title, std::string text, HWND parent = 0);

	//funkcja blokuj�ca Konnekta
	void Lock();

	//funkcja pytaj�ca o has�o i odblokowuj�ca Konnekta
	int Unlock();

	//funkcja aktywuj�ca kontrolki w konfiguracji
	void EnableActs();

	//funkcja deaktywuj�ca kontrolki w konfiguracji
	void DisableActs();
}