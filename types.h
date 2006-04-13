namespace kLock
{
	struct LockedWindow
	{
		int cnt;
		WINDOWPLACEMENT placement;
		RECT r;
	};
}

typedef DWORD (__stdcall *MYPROC)(DWORD, DWORD);