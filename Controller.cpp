/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#include "StdAfx.h"
#include "Includes.h"

namespace kLock
{
	SharedPtr<Controller> Controller::instance = 0;

	Controller::Controller()
	{
		this->addStaticValue(IM_PLUG_TYPE, IMT_UI|IMT_CONFIG);
		this->addStaticValue(IM_PLUG_PRIORITY, PLUGP_STANDARD);
		this->addStaticValue(IM_PLUG_NAME, kLock::name);
		this->addStaticValue(IM_PLUG_SIG, kLock::sig);
		this->addStaticValue(IM_PLUG_NET, kLock::net);

		this->registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&Controller::onPrepare), this));
		this->registerObserver(IM_START, bind(resolve_cast0(&Controller::onStart), this));
		this->registerObserver(IM_UIACTION, bind(resolve_cast0(&Controller::onAction), this));
		this->registerObserver(IM_AWAY, bind(resolve_cast0(&Controller::onAway), this));
		this->registerObserver(kAway2::im::extendedAutoAway, bind(resolve_cast0(&Controller::onExtendedAway), this));
		this->registerObserver(kAway2::im::away, bind(resolve_cast0(&Controller::onkAway), this));

		this->registerObserver(kLock::API::lock, bind(resolve_cast0(&Controller::apiLock), this));
		this->registerObserver(kLock::API::unlock, bind(resolve_cast0(&Controller::apiUnlock), this));
		this->registerObserver(kLock::API::isUnlocked, bind(resolve_cast0(&Controller::apiIsUnlocked), this));
		this->registerObserver(kLock::API::isUnlockedForKNotify, bind(resolve_cast0(&Controller::apiIsUnlockedForKNotify), this));
		this->registerObserver(kLock::API::isUnlockedForkMigacz, bind(resolve_cast0(&Controller::apiIsUnlockedForkMigacz), this));

		this->setColumn(DTCFG, kLock::CFG::lockState, DT_CT_INT|DT_CF_SECRET, 0, "kLock/lockState");
		this->setColumn(DTCFG, kLock::CFG::password, DT_CT_STR|DT_CF_SECRET, "", "kLock/lockPassword");
		this->setColumn(DTCFG, kLock::CFG::lockMainWindow, DT_CT_INT, 1, "kLock/lockListAndToolbars");
		this->setColumn(DTCFG, kLock::CFG::lockTalkWindows, DT_CT_INT, 1, "kLock/lockTalkWindows");
		this->setColumn(DTCFG, kLock::CFG::lockSound, DT_CT_INT, 0, "kLock/lockSound");
		this->setColumn(DTCFG, kLock::CFG::lockTray, DT_CT_INT, 0, "kLock/lockTray");
		this->setColumn(DTCFG, kLock::CFG::lockProcess, DT_CT_INT, 0, "kLock/lockProcess");
		this->setColumn(DTCFG, kLock::CFG::lockKNotify, DT_CT_INT, 0, "kLock/lockKNotify");
		this->setColumn(DTCFG, kLock::CFG::lockkMigacz, DT_CT_INT, 0, "kLock/lockkMigacz");
		this->setColumn(DTCFG, kLock::CFG::askForPasswordOnHistory, DT_CT_INT, 0, "kLock/askForPasswordOnHistoryAccess");
		this->setColumn(DTCFG, kLock::CFG::synchronizeWithAway, DT_CT_INT, 0, "kLock/synchronizeWithAutoAway");
		this->setColumn(DTCFG, kLock::CFG::synchronizeWithExtendedAway, DT_CT_INT, 0, "kLock/synchronizeWithExtendedAway");
		this->setColumn(DTCFG, kLock::CFG::synchronizeWithkAway, DT_CT_INT, 0, "kLock/synchronizeWithkAway2");
		this->setColumn(DTCFG, kLock::CFG::buttonOnToolbar, DT_CT_INT, 0, "kLock/buttonOnToolbar");
		this->setColumn(DTCFG, kLock::CFG::buttonInTray, DT_CT_INT, 1, "kLock/buttonInTray");
		this->setColumn(DTCFG, kLock::CFG::buttonOnMainToolbar, DT_CT_INT, 0, "kLock/buttonOnMainToolbar");
		this->setColumn(DTCFG, kLock::CFG::disablekAwayWhenUnlocking, DT_CT_INT, 0, "kLock/disablekAwayWhenUnlocking");
		this->setColumn(DTCFG, kLock::CFG::blockOnStart, DT_CT_INT, 0, "kLock/blockOnStart");
	}

	void Controller::onPrepare()
	{
		//przycisk na toolbarze wtyczek
		if(GETINT(kLock::CFG::buttonOnToolbar))
			UIActionAdd(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::ACT::lock, ACTR_INIT, GETINT(kLock::CFG::lockState) ? "Odblokuj" : "Zablokuj", 35);

		//przycisk w tray'u
		if(GETINT(kLock::CFG::buttonInTray))
			UIActionInsert(IMIG_TRAY, kLock::ACT::lock, Ctrl->IMessage(IMI_GROUP_ACTIONSCOUNT, 0, 0, (int)&sUIAction(IMIG_MAINWND, IMIG_TRAY)) - 1, ACTR_INIT, GETINT(kLock::CFG::lockState) ? "Odblokuj" : "Zablokuj", 35);

		//przycisk na g��wnym toolbarze
		if(GETINT(kLock::CFG::buttonOnMainToolbar) && Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0) != IMIG_MAINTB)
			UIActionAdd(IMIG_MAINTB, kLock::ACT::lock, ACTR_INIT, GETINT(kLock::CFG::lockState) ? "Odblokuj" : "Zablokuj", 35);

		//konfiguracja
		UIGroupAdd(IMIG_CFG_PLUGS, kLock::CFG::group, 0, "kLock", 35);
		{
			UIActionCfgAddPluginInfoBox2(kLock::CFG::group, 
				"Wtyczka ma za zadanie blokowa� Konnekta, aby nikt niepowo�any nie dosta� si� do �adnych przechowywanych w nim informacji.", 
				"<span class='note'>Skompilowano: <b>"__DATE__"</b> [<b>"__TIME__"</b>]</span><br/><br/>"
				"Copyright � 2006 <b>Micha� \"Dulek\" Dulko</b><br/>"
				"Copyright � 2006 <b>Sijawusz Pur Rahnama</b><br/>"
				"Copyright � 2006 <b>Kamil \"Ol�rin\" Figiela</b>", "reg://IML16/35.ico"
			);
		
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUP, "Blokada opcji");
			{
				UIActionCfgAdd(kLock::CFG::group, kLock::ACT::enableActs, ACTT_BUTTON|ACTR_SHOW|ACTR_INIT, "Aktywuj opcje" AP_ICO "41");
			}
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUPEND);

			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUP, "Wybierz co blokowa�");
			{
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockMainWindow, ACTT_CHECK|ACTR_STATUS, "Blokuj okno g��wne programu", kLock::CFG::lockMainWindow);
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockTalkWindows, ACTT_CHECK|ACTR_STATUS, "Blokuj okienka rozmowy i histori�", kLock::CFG::lockTalkWindows);
				if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelAdvanced))
				{
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockTray, ACTT_CHECK, "Ukrywaj ikonk� w tray'u", kLock::CFG::lockTray);
					if(!Ctrl->ICMessage(IMC_ISWINXP))
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockProcess, ACTT_CHECK, "Ukrywaj Konnekta na li�cie proces�w" AP_TIP "U�ywa� na w�asn� odpowiedzialno��!", kLock::CFG::lockProcess);
				}
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockSound, ACTT_CHECK, "Wyciszaj d�wi�ki", kLock::CFG::lockSound);
				if(pluginExists(KNotify::net))
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockKNotify, ACTT_CHECK, "Blokuj powiadomienia kNotify" AP_TIPRICH "Wymaga zmodyfikowanego <b>kNotify</b>!", kLock::CFG::lockKNotify);
				if(pluginExists(kMigacz::Net))
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockkMigacz, ACTT_CHECK, "Blokuj powiadomienia kMigacza", kLock::CFG::lockkMigacz);
			}
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUPEND);

			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUP, "Opcje");
			{
				UIActionCfgAdd(kLock::CFG::group, 0, ACTT_COMMENT|ACTSC_INLINE, "Has�o:");
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::password, ACTT_PASSWORD, 0, kLock::CFG::password);
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::synchronizeWithAway, ACTT_CHECK, "Synchronizuj z trybem auto-away", kLock::CFG::synchronizeWithAway);
				if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelIntermediate))
				{
					if(pluginExists(kAway2::net))
					{
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::synchronizeWithExtendedAway, ACTT_CHECK, "Synchronizuj z trybem extended auto-away", kLock::CFG::synchronizeWithExtendedAway);
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::synchronizeWithkAway, ACTT_CHECK, "Synchronizuj z kAway2", kLock::CFG::synchronizeWithkAway);
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::disablekAwayWhenUnlocking, ACTT_CHECK, "Wy��czaj kAway2 przy odblokowywaniu", kLock::CFG::disablekAwayWhenUnlocking);
					}
					if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelNormal))
					{
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::askForPasswordOnHistory, ACTT_CHECK, "Pytaj o has�o przy ka�dej pr�bie dost�pu do historii", kLock::CFG::askForPasswordOnHistory);
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::blockOnStart, ACTT_CHECK, "Blokuj przy ka�dym starcie programu", kLock::CFG::blockOnStart);
					}
					UIActionCfgAdd(kLock::CFG::group, 0, ACTT_SEP, "Po�o�enie przycisku");
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::buttonOnToolbar, ACTT_CHECK|ACTSC_NEEDRESTART, "W menu wtyczek", kLock::CFG::buttonOnToolbar);
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::buttonInTray, ACTT_CHECK|ACTSC_NEEDRESTART, "W menu tray'a", kLock::CFG::buttonInTray);
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::buttonOnMainToolbar, ACTT_CHECK|ACTSC_NEEDRESTART, "Na g��wnym toolbarze", kLock::CFG::buttonOnMainToolbar);
				}
			}
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUPEND);
		}

		this->disableActs();
	}

	void Controller::onStart()
	{
		//subclassujemy akcj� historii
		this->historyOwner = subclassAction(IMIG_MAIN_CNT, IMIA_MAIN_HISTORY);

		//subclassujemy akcj� historii w menu kontaktu
		this->cntHistoryOwner = subclassAction(IMIG_CNT, IMIA_CNT_HISTORY);

		//subclassujemy akcj� historii w oknie kontaktu
		this->msgWndHistoryOwner = subclassAction(IMIG_MSGTB, IMIA_MSG_HISTORY);

		if(!pluginExists("TABLETKA"))
		{
			IMLOG("Nie ma TabletKi, subclassuj� akcj� kIEview");

			//subclassujemy akcj� kontrolki kIEview - pozwoli nam wykrywa� otwarcie okienka rozmowy
			this->kIEviewOwner = subclassAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlview);
		}

		//subclassujemy g��wne okno programu - nie pozwolimy mu si� p�niej pokaza�
		this->konnektWindowOldProc = (WNDPROC)SetWindowLongPtr((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), GWLP_WNDPROC, (LONG_PTR)konnektWindowProc);

		//je�li poprzednim razem by�o zablokowane blokujemy
		if(GETINT(kLock::CFG::lockState) || GETINT(kLock::CFG::blockOnStart))
			this->lock(L_Lock, 1);
	}

	void Controller::onAction()
	{
		sUIActionNotify_2params* an = this->getAN();

		switch(an->act.id)
		{
			case IMIA_MAIN_HISTORY:
			case IMIA_CNT_HISTORY:
			case IMIA_MSG_HISTORY:
			{
				if(an->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: an->act.id = IMIA_MSG_HISTORY, an->code = ACTN_ACTION");

					if(GETINT(kLock::CFG::lockState) && GETINT(kLock::CFG::lockTalkWindows))
					{
						this->lock(L_Unlock);
						this->setReturnCode(0);
						return;
					}
					else if(GETINT(kLock::CFG::askForPasswordOnHistory))
					{
						if(!this->askForPassword("kLock", "Dost�p do historii jest zablokowany.\r\nPodaj has�o dost�pu:", "Otworzy� histori�?"))
						{
							this->setReturnCode(0);
							return;
						}
					}
				}
				switch(an->act.id)
				{
					case IMIA_MAIN_HISTORY:
						this->setReturnCode(IMessageDirect(IM_UIACTION, this->historyOwner, (int)an));
					case IMIA_CNT_HISTORY:
						this->setReturnCode(IMessageDirect(IM_UIACTION, this->cntHistoryOwner, (int)an));
					case IMIA_MSG_HISTORY:
						this->setReturnCode(IMessageDirect(IM_UIACTION, this->msgWndHistoryOwner, (int)an));
				}
				break;
			}
			case Konnekt::UI::ACT::msg_ctrlview:
			{
				if(an->code == ACTN_CREATEWINDOW)
				{
					IMLOG("[ActionProc]: an->act.id = Konnekt::UI::ACT::msg_ctrlview, an->code = ACTN_CREATEWINDOW");

					if(GETINT(kLock::CFG::lockState) && GETINT(kLock::CFG::lockTalkWindows))
					{
						sUIActionNotify_createWindow* anCW = static_cast<sUIActionNotify_createWindow*>(static_cast<sUIActionNotify_base*>(an));
						DestroyWindow(anCW->hwndParent);
						this->setReturnCode(0);
						return;
					}
				}
				this->setReturnCode(IMessageDirect(IM_UIACTION, this->kIEviewOwner, (int)an));
			}
			case kLock::ACT::enableActs:
			{
				if(an->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: an->act.id = kLock::CFG::EnableActs, an->code = ACTN_ACTION");

					this->disableActs(L_Detect);
				}
				else if(an->code == ACTN_HIDE || an->code == ACTN_DESTROY)
				{
					IMLOG("[ActionProc]: an->act.id = kLock::CFG::EnableActs, an->code = ACTN_HIDE|ACTN_DESTROY");

					this->disableActs(L_Lock);
				}
				break;
			}
			case kLock::ACT::lock:
			{
				if(an->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: an->act.id = kLock::CFG::Lock, an->code = ACTN_ACTION");

					this->lock();
				}
				break;
			}
			case kLock::CFG::lockTray:
			{
				if(an->code == ACTN_ACTION)
					if(*UIActionCfgGetValue(an->act, 0, 0) != '0')
						ICMessage(IMI_WARNING, (int)"Przed w��czeniem tej opcji ustaw odblokowuj�cy skr�t k.Lawy\n - inaczej nie odblokujesz Konnekta.");
				break;
			}
			case kLock::CFG::lockMainWindow:
			case kLock::CFG::lockTalkWindows:
			{
				if(an->code == ACTN_ACTION)
					this->disableActs(L_Unlock, 1);
				break;
			}
		}
	}

	void Controller::onAway()
	{
		if(GETINT(kLock::CFG::synchronizeWithAway))
			this->lock(L_Lock);
	}

	void Controller::onExtendedAway()
	{
		if(GETINT(kLock::CFG::synchronizeWithExtendedAway))
			this->lock(L_Lock);
	}

	void Controller::onkAway()
	{
		if(GETINT(kLock::CFG::synchronizeWithkAway))
			this->lock(L_Lock);
	}

	void Controller::apiLock()
	{
		this->setReturnCode(this->lock(L_Lock));
	}

	void Controller::apiUnlock()
	{
		IMLOG("bla");
		this->setReturnCode(this->lock(L_Unlock));
	}

	void Controller::apiIsUnlocked()
	{
		this->setReturnCode(!GETINT(kLock::CFG::lockState));
	}

	void Controller::apiIsUnlockedForKNotify()
	{
		this->setReturnCode(!(GETINT(kLock::CFG::lockState) == 1 && GETINT(kLock::CFG::lockKNotify)));
	}

	void Controller::apiIsUnlockedForkMigacz()
	{
		this->setReturnCode(!(GETINT(kLock::CFG::lockState) == 1 && GETINT(kLock::CFG::lockkMigacz)));
	}

	//funkcja blokuj�ca Konnekta
	bool Controller::lock(kLock::Lock lock, bool force)
	{
		IMLOG("[Controller::lock]");

		static tLockedWindows locked_windows; //okna przypi�te przy zak�adaniu blokady
		static bool konnektWndVisible; //czy g��wne okienko by�o widoczne
		static bool msgWndVisible; //czy okienko rozmowy by�o widoczne
		static bool muted; //czy kSound by� wyciszony

		if((lock == L_Lock && (force || !GETINT(kLock::CFG::lockState))) || (lock == L_Detect && !GETINT(kLock::CFG::lockState)))
		{
			//ustawiamy w configu, �e jest blokowane
			SETINT(kLock::CFG::lockState, 1);

			//je�li jest zaznaczona odpowiednia opcja blokujemy okienka rozmowy
			if(GETINT(kLock::CFG::lockTalkWindows))
			{
				if(pluginExists("TABLETKA"))
				{
					IMLOG("Blokuj� okienka rozmowy, TabletKa w��czona");

					int count = ICMessage(IMC_CNT_COUNT);
					for(int i = 0; i < count; i++)
					{
						if(UIGroupHandle(sUIAction(0, IMIG_MSGWND, Ctrl->DTgetID(DTCNT, i))) && GetProp((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, Ctrl->DTgetID(DTCNT, i))), "TABBED") == 0)
						{
							kLock::LockedWindow w;
							w.cnt = Ctrl->DTgetID(DTCNT, i);
							w.placement.length = sizeof(WINDOWPLACEMENT);
							GetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, w.cnt)), &w.placement);
							GetWindowRect((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, w.cnt)), &w.r);
							callAction(IMIG_CNT, Tabs::Acts::Detach, w.cnt);
							locked_windows.push_back(w);
						}
						msgWndVisible = IsWindowVisible((HWND)IMessage(Tabs::IM::GetTabWindow, Tabs::net, IMT_ALL));
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
			if(GETINT(kLock::CFG::lockMainWindow) && IsWindowVisible((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND))))
			{
				IMLOG("Ukrywam g��wne okno");

				konnektWndVisible = 1;
				ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_HIDE);
			}
			else
				konnektWndVisible = 0;

			//je�li jest zaznaczona opcja i d�wi�ki nie s� ju� wyciszone, wyciszamy je
			if(GETINT(kLock::CFG::lockSound) && !GETINT(kSound::Cfg::mute))
			{
				IMLOG("Wyciszam d�wi�ki");

				callAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);
				muted = 0;
			}
			else
				muted = 1;

			//je�li jest zaznaczona opcja usuwamy ikonk� z tray'a
			if(GETINT(kLock::CFG::lockTray) && GETINT(kLock::CFG::lockMainWindow) && GETINT(kLock::CFG::lockTalkWindows))
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
			if(GETINT(kLock::CFG::lockProcess))
			{
				IMLOG("Usuwam Konnekta z listy proces�w");
				HMODULE kernel;
				MYPROC RegisterServiceProcess;
				kernel = LoadLibrary("KERNEL32.DLL");
				if(kernel)
				{
					RegisterServiceProcess = (MYPROC)GetProcAddress(kernel, "RegisterServiceProcess");
					if(RegisterServiceProcess)
						RegisterServiceProcess(GetCurrentProcessId(), 1);
					FreeLibrary(kernel);
				}
			}

			//zmieniamy napis na przycisku
			UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::ACT::lock, "Odblokuj");
			UIActionSetText(IMIG_TRAY, kLock::ACT::lock, "Odblokuj");
			UIActionSetText(IMIG_MAINTB, kLock::ACT::lock, "Odblokuj");

			return 1;
		}
		else if(lock == L_Unlock && (force || GETINT(kLock::CFG::lockState)) || (lock == L_Detect && GETINT(kLock::CFG::lockState)))
		{
			//je�li has�o si� zgadza odblokowujemy
			if(this->askForPassword("kLock", "Konnekt zosta� zablokowany.\r\nPodaj has�o dost�pu:", "Odblokowa�?"))
			{
				IMLOG("Odblokowuj�");

				//ustawiamy w configu, �e jest odblokowane
				SETINT(kLock::CFG::lockState, 0);

				//odblokowujemy okienka rozmowy
				{
					if(pluginExists("TABLETKA"))
					{
						for(kLock::tLockedWindows::iterator i = locked_windows.begin(); i != locked_windows.end(); i++)
						{
							callAction(IMIG_CNT, Tabs::Acts::Detach, i->cnt);			
							SetWindowPos((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), 0, i->r.left, i->r.top, i->r.right - i->r.left, i->r.bottom - i->r.top, 0);
							SetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), &i->placement);
						}
						locked_windows.clear();

						if(msgWndVisible)
							ShowWindow((HWND)IMessage(Tabs::IM::GetTabWindow, Tabs::net,IMT_ALL), SW_SHOW);
					}
					else
					{
						for(kLock::tLockedWindows::iterator i = locked_windows.begin(); i != locked_windows.end(); i++)
						{
							callAction(IMIG_CNT, IMIA_CNT_MSGOPEN, i->cnt);
							SetWindowPos((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), 0, i->r.left, i->r.top, i->r.right - i->r.left, i->r.bottom - i->r.top, 0);
							SetWindowPlacement((HWND)UIGroupHandle(sUIAction(0, IMIG_MSGWND, i->cnt)), &i->placement);
							callAction(0xBE870, 0xBE8A5, i->cnt);
						}
						locked_windows.clear();
					}
				}

				//pokazujemy g��wne okno - nowy proc powinien ju� na to pozwoli�
				if(konnektWndVisible)
					ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_SHOW);

				//wy�aczamy wyciszenie d�wi�k�w
				if(!muted && GETINT(kSound::Cfg::mute))
					callAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);

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
							RegisterServiceProcess(GetCurrentProcessId(), 0);
						FreeLibrary(kernel);
					}
				}

				//wy��czamy kAway2
				if(GETINT(kLock::CFG::disablekAwayWhenUnlocking))
					Ctrl->IMessage(kAway2::api::disable, kAway2::net, IMT_ALL, (int)"");

				//zmieniamy napis na przycisku
				UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::ACT::lock, "Zablokuj");
				UIActionSetText(IMIG_TRAY, kLock::ACT::lock, "Zablokuj");
				UIActionSetText(IMIG_MAINTB, kLock::ACT::lock, "Zablokuj");
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

	//funkcja deaktywuj�ca kontrolki w konfiguracji
	bool Controller::disableActs(kLock::Lock lock, bool force)
	{
		IMLOG("[Controller::disableActs]");

		static bool actsLocked;

		if((lock == L_Lock && (force || !actsLocked)) || (lock == L_Detect && !actsLocked))
		{
			actsLocked = 1;
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::password), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::askForPasswordOnHistory), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockMainWindow), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockSound), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockTalkWindows), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::synchronizeWithAway), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockTray), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockProcess), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::buttonInTray), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::buttonOnToolbar), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::buttonOnMainToolbar), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::synchronizeWithkAway), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockKNotify), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockkMigacz), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::disablekAwayWhenUnlocking), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::blockOnStart), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::synchronizeWithExtendedAway), -1, ACTS_DISABLED);
			UIActionSetText(kLock::CFG::group, kLock::ACT::enableActs, "Aktywuj opcje" AP_ICO "40");
			return 1;
		}
		else if(lock == L_Unlock && (force || (actsLocked && this->askForPassword("kLock", "Zmiana ustawie� jest zablokowana.\r\nPodaj has�o dost�pu:", "Odblokowa� ustawienia?", (HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND))))) || (lock == L_Detect && actsLocked && this->askForPassword("kLock", "Zmiana ustawie� jest zablokowana.\r\nPodaj has�o dost�pu:", "Odblokowa� ustawienia?", (HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND)))))
		{
			actsLocked = 0;
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::password), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::askForPasswordOnHistory), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockMainWindow), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockSound), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockTalkWindows), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::synchronizeWithAway), 0, ACTS_DISABLED);
			if(*UIActionCfgGetValue(sUIAction(kLock::CFG::group, kLock::CFG::lockMainWindow), 0, 0) != '0' && *UIActionCfgGetValue(sUIAction(kLock::CFG::group, kLock::CFG::lockTalkWindows), 0, 0) != '0')
				UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockTray), 0, ACTS_DISABLED);
			else
				UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockTray), -1, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockProcess), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::buttonInTray), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::buttonOnToolbar), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::buttonOnMainToolbar), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::synchronizeWithkAway), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockKNotify), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::lockkMigacz), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::disablekAwayWhenUnlocking), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::blockOnStart), 0, ACTS_DISABLED);
			UIActionSetStatus(sUIAction(kLock::CFG::group, kLock::CFG::synchronizeWithExtendedAway), 0, ACTS_DISABLED);
			UIActionSetText(kLock::CFG::group, kLock::ACT::enableActs, "Deaktywuj opcje" AP_ICO "41");
			return 1;
		}
		return 0;
	}

	//funkcja wy�wietlaj�ca monit o has�o, je�li si� zgadza zwraca 1, je�li nie 0
	int Controller::askForPassword(std::string title, std::string text, std::string text2, HWND parent)
	{
		static bool unlocking;
		IMLOG("[Controller::askForPassword]");

		if(!unlocking)
		{
			unlocking = 1;

			if(strlen(GETSTRA(kLock::CFG::password)))
			{
				sDIALOG_access sde;

				//wy�wietlamy pytanie o has�o
				{
					sde.info = text.c_str();
					sde.title = title.c_str();
					if(!parent)
						sde.handle = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));
					else
						sde.handle = parent;
					if(!ICMessage(IMI_DLGPASS, (int)&sde))
					{
						unlocking = 0;
						return 0;
					}
				}

				//je�li has�o si� zgadza zwracamy 1, je�li nie 0
				if((std::string)sde.pass == (std::string)GETSTRA(kLock::CFG::password))
				{
					unlocking = 0;
					return 1;
				}
				else
				{
					ICMessage(IMI_ERROR, (int)"Z�e has�o!");
					unlocking = 0;
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
					return 1;
				}
				else
				{
					unlocking = 0;
					return 0;
				}
			}
		}
		else
			return 0;
	}
}