/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

namespace kLock
{
	//struktura dla danych przymusowo przypinanych okienek
	struct LockedWindow
	{
		int cnt;
		WINDOWPLACEMENT placement;
		RECT r;
	};

	typedef std::list<kLock::LockedWindow> tLockedWindows;

	//enum dla funkcji Lock
	enum Lock
	{
		L_Detect, //wykryj
		L_Lock, //zablokuj
		L_Unlock //odblokuj
	};
}

//wskaŸnik na funkcjê RegisterServiceProcess
typedef DWORD (__stdcall *MYPROC)(DWORD, DWORD);