/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#include "includes.h"

namespace kLock
{
	//funkcja obs�uguj�ca g��wne okno, blokuj�c je
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		if(GETINT(kLock::Config::State) && GETINT(kLock::Config::LockMainWindow))
		{
			switch(iMsg)
			{
				case WM_WINDOWPOSCHANGING:
				case WM_WINDOWPOSCHANGED:
				case WM_SHOWWINDOW:
				{
					ShowWindow(hwnd, 0);
					return 0;
				}
				case WM_USER+1086:
				{
					if(lParam == 514 || lParam == 517)
					{
						Unlock();
						return 0;
					}
					else if(lParam == 513 || lParam == 516 || lParam == 512)
					{
						return 0;
					}
					break;
				}
			}
		}
		return CallWindowProc(old_mainwnd_proc, hwnd, iMsg, wParam, lParam);
	}

	//funkcja wy�wietlaj�ca monit o has�o, je�li si� zgadza zwraca 1, je�li nie 0
	int AskForPassword(std::string title, std::string text, std::string text2, HWND parent)
	{
		IMLOG("[AskForPassword]");

		if(!unlocking)
		{
			unlocking = 1;

			if(strlen(GETSTRA(kLock::Config::Password)))
			{
				IMLOG("Jest has�o");
				sDIALOG_access sde;

				//wy�wietlamy pytanie o has�o
				{
					sde.info = text.c_str();
					sde.title = title.c_str();
					if(!parent)
					{
						sde.handle = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));
					}
					else
					{
						sde.handle = parent;
					}
					if(!ICMessage(IMI_DLGPASS, (int)&sde))
					{
						unlocking = 0;
						IMLOG("Okienko zamkni�to");
						return 0;
					}
				}

				//je�li has�o si� zgadza zwracamy 1, je�li nie 0
				if((std::string)sde.pass == (std::string)GETSTRA(kLock::Config::Password))
				{
					unlocking = 0;
					IMLOG("Poprawne has�o");
					return 1;
				}
				else
				{
					ICMessage(IMI_ERROR, (int)"Z�e has�o!");
					unlocking = 0;
					IMLOG("Niepoprawne has�o");
					return 0;
				}
				unlocking = 0;
			}
			else
			{
				IMLOG("Nie ma has�a");
				if(ICMessage(IMI_CONFIRM, (int)text2.c_str()))
				{
					unlocking = 0;
					IMLOG("Tak");
					return 1;
				}
				else
				{
					unlocking = 0;
					IMLOG("Nie");
					return 0;
				}
			}
		}
		else
		{
			IMLOG("Ju� uruchomione okienko dialogowe");
			return 0;
		}
	}

	//funkcja blokuj�ca Konnekta
	void Lock(bool force)
	{
		IMLOG("[Lock]");

		if(!GETINT(kLock::Config::State) || force)
		{
			//ustawiamy w configu, �e jest blokowane
			SETINT(kLock::Config::State, 1);

			//je�li jest zaznaczona odpowiednia opcja blokujemy okienka rozmowy
			if(GETINT(kLock::Config::LockTalkWindows))
			{
				if(PluginExists(Tabs::net))
				{
					IMLOG("Blokuj� okienka rozmowy, TabletKa w��czona");

					int count = ICMessage(IMC_CNT_COUNT);
					for(int i = 0; i < count; i++)
					{
						if(UIGroupHandle(sUIAction(0, IMIG_MSGWND, Ctrl->DTgetID(DTCNT, i))) && GetProp((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, Ctrl->DTgetID(DTCNT, i))), "TABBED") == 0)
						{
							LockedWindow w;
							w.cnt = Ctrl->DTgetID(DTCNT, i);
							w.placement.length = sizeof(WINDOWPLACEMENT);
							GetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, w.cnt)), &w.placement);
							GetWindowRect((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, w.cnt)), &w.r);
							CallAction(IMIG_CNT, Tabs::Acts::Detach, w.cnt);
							locked_windows.push_back(w);
						}
						msg_visible = IsWindowVisible((HWND)IMessage(Tabs::IM::GetTabWindow, Tabs::net, IMT_ALL));
					}
					ShowWindow((HWND)IMessage(Tabs::IM::GetTabWindow, Tabs::net, IMT_ALL), SW_HIDE);
				}
				else
				{
					IMLOG("Blokuj� okienka rozmowy, TabletKa wy��czona");

					int count = ICMessage(IMC_CNT_COUNT);
					for(int i = 0; i < count; i++)
					{
						if(UIGroupHandle(sUIAction(0, IMIG_MSGWND, Ctrl->DTgetID(DTCNT, i))))
						{
							if(IsWindowVisible((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, Ctrl->DTgetID(DTCNT, i)))))
							{
								LockedWindow w;
								w.cnt = Ctrl->DTgetID(DTCNT, i);
								w.placement.length = sizeof(WINDOWPLACEMENT);
								GetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, w.cnt)), &w.placement);
								GetWindowRect((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, w.cnt)), &w.r);
								locked_windows.push_back(w);
							}
							DestroyWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, Ctrl->DTgetID(DTCNT, i))));
						}
					}
				}
			}

			//je�li jest zaznaczona opcja ukrywamy g��wne okno - jego nowy proc nie pozwoli mu si� pokaza�
			if(GETINT(kLock::Config::LockMainWindow) && IsWindowVisible((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND))))
			{
				IMLOG("Ukrywam g��wne okno");

				main_visible = 1;
				ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_HIDE);
			}
			else
			{
				main_visible = 0;
			}

			//je�li jest zaznaczona opcja i d�wi�ki nie s� ju� wyciszone, wyciszamy je
			if(GETINT(kLock::Config::LockSound) && !GETINT(kSound::Cfg::mute))
			{
				IMLOG("Wyciszam d�wi�ki");

				CallAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);
				muted = 0;
			}
			else
			{
				muted = 1;
			}

			//je�li jest zaznaczona opcja usuwamy ikonk� z tray'a
			if(GETINT(kLock::Config::LockTray))
			{
				IMLOG("Usuwam ikonk� z tray'a");

				NOTIFYICONDATA tray;
				ZeroMemory(&tray, sizeof(NOTIFYICONDATA));
				tray.cbSize = sizeof(NOTIFYICONDATA);
				tray.uID = 1000;
				tray.uFlags = 0;
				tray.hWnd = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));
				tray.uCallbackMessage = WM_USER+1086;
				Shell_NotifyIcon(NIM_DELETE, &tray);
			}

			//je�li jest zaznaczona opcja usuwamy z listy proces�w
			if(GETINT(kLock::Config::LockProcess))
			{
				IMLOG("Usuwam Konnekta z listy proces�w");
				HMODULE kernel;
				MYPROC RegisterServiceProcess;
				kernel = LoadLibrary("KERNEL32.DLL");
				if(kernel)
				{
					RegisterServiceProcess = (MYPROC)GetProcAddress(kernel, "RegisterServiceProcess");
					if(RegisterServiceProcess)
					{
						RegisterServiceProcess(GetCurrentProcessId(), 1);
					}
					FreeLibrary(kernel);
				}
			}

			//zmieniamy napis na przycisku
			if(GETINT(kLock::Config::ButtonOnToolbar))
			{
				UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::Acts::Lock, "Odblokuj");
			}
			if(GETINT(kLock::Config::ButtonInTray))
			{
				UIActionSetText(IMIG_TRAY, kLock::Acts::Lock, "Odblokuj");
			}
			if(GETINT(kLock::Config::ButtonOnMainToolbar))
			{
				UIActionSetText(IMIG_MAINTB, kLock::Acts::Lock, "Odblokuj");
			}
		}
	}

	//funkcja odblokowuj�ca Konnekta
	int Unlock()
	{
		//je�eli jest zablokowane
		if(GETINT(kLock::Config::State))
		{
			IMLOG("[Unlock]");

			//je�li has�o si� zgadza odblokowujemy
			if(AskForPassword("kLock", "Konnekt zosta� zablokowany.\r\nPodaj has�o dost�pu:", "Odblokowa�?"))
			{
				IMLOG("Odblokowuj�");

				//ustawiamy w configu, �e jest odblokowane
				SETINT(kLock::Config::State, 0);

				//odblokowujemy okienka rozmowy
				{
					if(PluginExists(Tabs::net))
					{
						for(std::list<kLock::LockedWindow>::iterator i = kLock::locked_windows.begin(); i != kLock::locked_windows.end(); i++)
						{
							CallAction(IMIG_CNT, Tabs::Acts::Detach, i->cnt);			
							SetWindowPos((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), 0, i->r.left, i->r.top, i->r.right - i->r.left, i->r.bottom - i->r.top, 0);
							SetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), &i->placement);
						}
						kLock::locked_windows.clear();

						if(msg_visible)
						{
							ShowWindow((HWND)IMessage(Tabs::IM::GetTabWindow, Tabs::net,IMT_ALL), SW_SHOW);
						}
					}
					else
					{
						for(std::list<kLock::LockedWindow>::iterator i = kLock::locked_windows.begin(); i != kLock::locked_windows.end(); i++)
						{
							CallAction(IMIG_CNT, IMIA_CNT_MSGOPEN, i->cnt);
							SetWindowPos((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), 0, i->r.left, i->r.top, i->r.right - i->r.left, i->r.bottom - i->r.top, 0);
							SetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), &i->placement);
							CallAction(0xBE870, 0xBE8A5, i->cnt);
						}
						kLock::locked_windows.clear();
					}
				}

				//pokazujemy g��wne okno - nowy proc powinien ju� na to pozwoli�
				if(main_visible)
				{
					ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_SHOW);
				}

				//wy�aczamy wyciszenie d�wi�k�w
				if(!muted && GETINT(kSound::Cfg::mute))
				{
					CallAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);
				}

				//przywracamy ikonk� do tray'a
				{
					NOTIFYICONDATA tray;
					ZeroMemory(&tray, sizeof(NOTIFYICONDATA));
					tray.cbSize = sizeof(NOTIFYICONDATA);
					tray.uID = 1000;
					tray.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
					tray.hIcon = (HICON)ICMessage(IMI_ICONGET, ICON_KONNEKT, IML_16);
					tray.hWnd = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));
					tray.uCallbackMessage = WM_USER+1086;
					Shell_NotifyIcon(NIM_ADD, &tray);
					DestroyIcon(tray.hIcon);
				}

				//przywracamy na list� proces�w
				{
					HMODULE kernel;
					MYPROC RegisterServiceProcess;
					kernel = LoadLibrary("KERNEL32.DLL");
					if(kernel)
					{
						RegisterServiceProcess = (MYPROC)GetProcAddress(kernel, "RegisterServiceProcess");
						if(RegisterServiceProcess)
						{
							RegisterServiceProcess(GetCurrentProcessId(), 0);
						}
						FreeLibrary(kernel);
					}
				}

				//wy��czamy kAway2
				if(GETINT(kLock::Config::TurnOffkAwayOnUnlocking))
				{
					Ctrl->IMessage(kAway2::api::disable, kAway2::net, IMT_ALL, (int)"");
				}

				//zmieniamy napis na przycisku
				if(GETINT(kLock::Config::ButtonOnToolbar))
				{
					UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::Acts::Lock, "Zablokuj");
				}
				if(GETINT(kLock::Config::ButtonInTray))
				{
					UIActionSetText(IMIG_TRAY, kLock::Acts::Lock, "Zablokuj");
				}
				if(GETINT(kLock::Config::ButtonOnMainToolbar))
				{
					UIActionSetText(IMIG_MAINTB, kLock::Acts::Lock, "Zablokuj");
				}
				return 1;
			}
			else
			{
				IMLOG("Nie odblokowuj�");
				return 0;
			}
		}
		return 1;
	}

	//funkcja aktywuj�ca kontrolki w konfiguracji
	void EnableActs()
	{
		IMLOG("[EnableActs]");

		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::Password), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::AskForPasswordOnHistory), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockMainWindow), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockSound), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockTalkWindows), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::SynchronizeWithAway), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockTray), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockProcess), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::ButtonInTray), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::ButtonOnToolbar), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::ButtonOnMainToolbar), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::SynchronizeWithkAway), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockKNotify), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockkMigacz), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::TurnOffkAwayOnUnlocking), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::BlockOnStart), 0, ACTS_DISABLED);
		UIActionSetText(kLock::Config::Group, kLock::Config::EnableActs, "Deaktywuj opcje" AP_ICO "41");
		acts_enabled = 1;
	}

	//funkcja deaktywuj�ca kontrolki w konfiguracji
	void DisableActs()
	{
		IMLOG("[DisableActs]");

		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::Password), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::AskForPasswordOnHistory), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockMainWindow), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockSound), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockTalkWindows), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::SynchronizeWithAway), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockTray), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockProcess), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::ButtonInTray), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::ButtonOnToolbar), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::ButtonOnMainToolbar), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::SynchronizeWithkAway), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockKNotify), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::LockkMigacz), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::TurnOffkAwayOnUnlocking), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::Config::Group, kLock::Config::BlockOnStart), -1, ACTS_DISABLED);
		UIActionSetText(kLock::Config::Group, kLock::Config::EnableActs, "Aktywuj opcje" AP_ICO "40");
		acts_enabled = 0;
	}
}