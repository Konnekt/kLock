namespace kLock
{
	//struktura dla danych przymusowo przypinanych okienek
	struct LockedWindow
	{
		int cnt;
		WINDOWPLACEMENT placement;
		RECT r;
	};
}

//wskaŸnik na funkcjê RegisterServiceProcess
typedef DWORD (__stdcall *MYPROC)(DWORD, DWORD);