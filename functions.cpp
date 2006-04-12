#include "includes.h"

namespace kLock
{
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		if(GETINT(kLock::config::STATE) && GETINT(kLock::config::LOCKLISTANDTOOLBARS))
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

	int AskForPassword(std::string title, std::string text, HWND parent)
	{
		if(!unlocking)
		{
			unlocking = 1;

			sDIALOG_access sde;

			//wyœwietlamy pytanie o has³o
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
					return 0;
				}
			}

			//jeœli has³o siê zgadza zwracamy 1, jeœli nie 0
			if((std::string)sde.pass == (std::string)GETSTRA(kLock::config::PASSWORD))
			{
				unlocking = 0;
				return 1;
			}
			else
			{
				ICMessage(IMI_ERROR, (int)"Z³e has³o!");
				unlocking = 0;
				return 0;
			}
			unlocking = 0;
		}
		else
		{
			return 0;
		}
	}

	void Lock()
	{
		//ustawiamy w configu, ¿e jest blokowane
		SETINT(kLock::config::STATE, 1);

		//jeœli jest zaznaczona odpowiednia opcja blokujemy okienka rozmowy
		if(GETINT(kLock::config::LOCKTALKWINDOWS))
		{
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

		//jeœli jest zaznaczona opcja ukrywamy g³ówne okno - jego nowy proc nie pozwoli mu siê pokazaæ
		if(GETINT(kLock::config::LOCKLISTANDTOOLBARS))
		{
			main_visible = IsWindowVisible((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)));
			ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_HIDE);
		}

		//jeœli jest zaznaczona opcja i dŸwiêki nie s¹ ju¿ wyciszone, wyciszamy je
		if(GETINT(kLock::config::LOCKSOUND) && !GETINT(kSound::Cfg::mute))
		{
			muted = 0;
			CallAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);
		}
		else
		{
			muted = 1;
		}

		//jeœli jest zaznaczona opcja usuwamy ikonkê z tray'a
		if(GETINT(kLock::config::LOCKTRAY))
		{
			NOTIFYICONDATA tray;
			ZeroMemory(&tray, sizeof(NOTIFYICONDATA));
			tray.cbSize = sizeof(NOTIFYICONDATA);
			tray.uID = 1000;
			tray.uFlags = 0;
			tray.hWnd = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));
			tray.uCallbackMessage = WM_USER+1086;
			Shell_NotifyIcon(NIM_DELETE, &tray);
		}

		//usuwamy z listy procesów
		if(GETINT(kLock::config::LOCKPROCESS))
		{
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
		if(GETINT(kLock::config::BUTTONPLACE) == 0)
		{
			UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::acts::LOCK, "Zablokuj");
		}
		else if(GETINT(kLock::config::BUTTONPLACE) == 1)
		{
			UIActionSetText(IMIG_MAINTB, kLock::acts::LOCK, "Zablokuj");
		}
		else if(GETINT(kLock::config::BUTTONPLACE) == 2)
		{
			UIActionSetText(IMIG_TRAY, kLock::acts::LOCK, "Zablokuj");
		}
	}

	int Unlock()
	{
		//je¿eli jest zablokowane i nie trwa ju¿ odblokowywanie
		if(GETINT(kLock::config::STATE))
		{
			//jeœli has³o siê zgadza odblokowujemy
			if(AskForPassword("kLock", "Konnekt zosta³ zablokowany.\r\nPodaj has³o dostêpu:"))
			{
				//ustawiamy w configu, ¿e jest odblokowane
				SETINT(kLock::config::STATE, 0);

				//odblokowujemy okienka rozmowy
				{
					std::vector<kLock::LockedWindow>::iterator iter;
					for(iter = kLock::locked_windows.begin(); iter != kLock::locked_windows.end(); iter++)
					{
						CallAction(IMIG_CNT, Tabs::Acts::Detach, iter->cnt);			
						SetWindowPos((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, iter->cnt)),0, iter->r.left, iter->r.top, iter->r.right - iter->r.left, iter->r.bottom - iter->r.top, 0);
						SetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, iter->cnt)), &iter->placement);
					}
					kLock::locked_windows.clear();

					if(msg_visible)
					{
						ShowWindow((HWND)IMessage(Tabs::IM::GetTabWindow,Tabs::net,IMT_ALL), SW_SHOW);
					}
				}

				//pokazujemy g³ówne okno - nowy proc powinien na to pozwoliæ
				{
					if(main_visible)
					{
						ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_SHOW);
					}
				}

				//wy³aczamy wyciszenie dŸwiêków
				if(GETINT(kLock::config::LOCKSOUND))
				{
					if(!muted)
					{
						CallAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);
					}
				}

				//przywracamy ikonkê do tray'a
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

				//przywracamy na listê procesów
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

				//zmieniamy napis na przycisku
				if(GETINT(kLock::config::BUTTONPLACE) == 0)
				{
					UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::acts::LOCK, "Odblokuj");
				}
				else if(GETINT(kLock::config::BUTTONPLACE) == 1)
				{
					UIActionSetText(IMIG_MAINTB, kLock::acts::LOCK, "Odblokuj");
				}
				else if(GETINT(kLock::config::BUTTONPLACE) == 2)
				{
					UIActionSetText(IMIG_TRAY, kLock::acts::LOCK, "Odblokuj");
				}
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 1;
		}
	}

	void EnableActs()
	{
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::PASSWORD), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::ASKFORPASSWORDONHISTORY), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKLISTANDTOOLBARS), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKSOUND), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKTALKWINDOWS), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::SYNCHRONIZEWITHAWAY), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKTRAY), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKPROCESS), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, kLock::config::BUTTONPLACE1), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, kLock::config::BUTTONPLACE2), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, kLock::config::BUTTONPLACE3), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::SYNCHRONIZEWITHKAWAY), 0, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKKNOTIFY), 0, ACTS_DISABLED);
		UIActionSetText(kLock::config::GROUP, kLock::config::ENABLEACTS, "Deaktywuj" AP_ICO "41");
		acts_enabled = 1;
	}

	void DisableActs()
	{
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::PASSWORD), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::ASKFORPASSWORDONHISTORY), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKLISTANDTOOLBARS), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKSOUND), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKTALKWINDOWS), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::SYNCHRONIZEWITHAWAY), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKTRAY), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKPROCESS), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, kLock::config::BUTTONPLACE1), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, kLock::config::BUTTONPLACE2), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, kLock::config::BUTTONPLACE3), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::SYNCHRONIZEWITHKAWAY), -1, ACTS_DISABLED);
		UIActionSetStatus(sUIAction(kLock::config::GROUP, IMIB_CFG|kLock::config::LOCKKNOTIFY), -1, ACTS_DISABLED);
		UIActionSetText(kLock::config::GROUP, kLock::config::ENABLEACTS, "Aktywuj" AP_ICO "40");
		acts_enabled = 0;
	}
}