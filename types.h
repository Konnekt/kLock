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

//wska�nik na funkcj� RegisterServiceProcess
typedef DWORD (__stdcall *MYPROC)(DWORD, DWORD);