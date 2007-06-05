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
		//instancja klasy do obs³ugi konfiguracji
		this->config = new CfgController(this);

		//wartoœci statyczne
		this->setStaticValue(IM_PLUG_TYPE, IMT_UI | IMT_CONFIG);
		this->setStaticValue(IM_PLUG_NAME, kLock::name);
		this->setStaticValue(IM_PLUG_SIG, kLock::sig);
		this->setStaticValue(IM_PLUG_NET, kLock::net);

		//zdarzenia - obserwatorzy
		this->registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&Controller::onPrepare), this));
		this->registerObserver(IM_START, bind(resolve_cast0(&Controller::onStart), this));
		this->registerObserver(IM_UIACTION, bind(resolve_cast0(&Controller::onAction), this));
		this->registerObserver(IM_AWAY, bind(resolve_cast0(&Controller::onAway), this));
		this->registerObserver(kAway2::im::extendedAutoAway, bind(resolve_cast0(&Controller::onExtendedAway), this));
		this->registerObserver(kAway2::im::away, bind(resolve_cast0(&Controller::onkAway), this));

		//api
		this->registerObserver(kLock::API::lock, bind(resolve_cast0(&Controller::apiLock), this));
		this->registerObserver(kLock::API::unlock, bind(resolve_cast0(&Controller::apiUnlock), this));
		this->registerObserver(kLock::API::isUnlocked, bind(resolve_cast0(&Controller::apiIsUnlocked), this));
		this->registerObserver(kLock::API::isUnlockedForKNotify, bind(resolve_cast0(&Controller::apiIsUnlockedForKNotify), this));
		this->registerObserver(kLock::API::isUnlockedForkMigacz, bind(resolve_cast0(&Controller::apiIsUnlockedForkMigacz), this));

		//subclassujemy akcje
		this->subclassAction(IMIA_MAIN_HISTORY, IMIG_MAIN_CNT);
		this->subclassAction(IMIA_CNT_HISTORY, IMIG_CNT);
		this->subclassAction(IMIA_MSG_HISTORY, IMIG_MSGTB);

		//ustawiamy kolumny
		config->setColumn(DTCFG, kLock::CFG::lockState, DT_CT_INT | DT_CF_SECRET, 0, "kLock/lockState");
		config->setColumn(DTCFG, kLock::CFG::password, DT_CT_STR | DT_CF_SECRET, "", "kLock/lockPassword");
		config->setColumn(DTCFG, kLock::CFG::lockMainWindow, DT_CT_INT, 1, "kLock/lockListAndToolbars");
		config->setColumn(DTCFG, kLock::CFG::lockTalkWindows, DT_CT_INT, 1, "kLock/lockTalkWindows");
		config->setColumn(DTCFG, kLock::CFG::lockSound, DT_CT_INT, 0, "kLock/lockSound");
		config->setColumn(DTCFG, kLock::CFG::lockTray, DT_CT_INT, 0, "kLock/lockTray");
		config->setColumn(DTCFG, kLock::CFG::lockProcess, DT_CT_INT, 0, "kLock/lockProcess");
		config->setColumn(DTCFG, kLock::CFG::lockKNotify, DT_CT_INT, 0, "kLock/lockKNotify");
		config->setColumn(DTCFG, kLock::CFG::lockkMigacz, DT_CT_INT, 0, "kLock/lockkMigacz");
		config->setColumn(DTCFG, kLock::CFG::askForPasswordOnHistory, DT_CT_INT, 0, "kLock/askForPasswordOnHistoryAccess");
		config->setColumn(DTCFG, kLock::CFG::synchronizeWithAway, DT_CT_INT, 0, "kLock/synchronizeWithAutoAway");
		config->setColumn(DTCFG, kLock::CFG::synchronizeWithExtendedAway, DT_CT_INT, 0, "kLock/synchronizeWithExtendedAway");
		config->setColumn(DTCFG, kLock::CFG::synchronizeWithkAway, DT_CT_INT, 0, "kLock/synchronizeWithkAway2");
		config->setColumn(DTCFG, kLock::CFG::buttonOnToolbar, DT_CT_INT, 0, "kLock/buttonOnToolbar");
		config->setColumn(DTCFG, kLock::CFG::buttonInTray, DT_CT_INT, 1, "kLock/buttonInTray");
		config->setColumn(DTCFG, kLock::CFG::buttonOnMainToolbar, DT_CT_INT, 0, "kLock/buttonOnMainToolbar");
		config->setColumn(DTCFG, kLock::CFG::disablekAwayWhenUnlocking, DT_CT_INT, 0, "kLock/disablekAwayWhenUnlocking");
		config->setColumn(DTCFG, kLock::CFG::blockOnStart, DT_CT_INT, 0, "kLock/blockOnStart");
	}

	//IM_UI_PREPARE - przygotowanie interfejsu
	void Controller::onPrepare()
	{
		//przycisk na toolbarze wtyczek
		if(GETINT(kLock::CFG::buttonOnToolbar))
			UIActionAdd(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::ACT::lock, ACTR_INIT, GETINT(kLock::CFG::lockState) ? "Odblokuj" : "Zablokuj", 35);

		//przycisk w tray'u
		if(GETINT(kLock::CFG::buttonInTray))
			UIActionInsert(IMIG_TRAY, kLock::ACT::lock, Ctrl->IMessage(IMI_GROUP_ACTIONSCOUNT, 0, 0, (int)&sUIAction(IMIG_MAINWND, IMIG_TRAY)) - 1, ACTR_INIT, GETINT(kLock::CFG::lockState) ? "Odblokuj" : "Zablokuj", 35);

		//przycisk na g³ównym toolbarze
		if(GETINT(kLock::CFG::buttonOnMainToolbar) && Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0) != IMIG_MAINTB)
			UIActionAdd(IMIG_MAINTB, kLock::ACT::lock, ACTR_INIT, GETINT(kLock::CFG::lockState) ? "Odblokuj" : "Zablokuj", 35);

		//konfiguracja
		UIGroupAdd(IMIG_CFG_PLUGS, kLock::CFG::group, 0, "kLock", 35);
		{
			UIActionCfgAddPluginInfoBox2(kLock::CFG::group, 
				"Wtyczka ma za zadanie blokowaæ Konnekta, aby nikt niepowo³any nie dosta³ siê do ¿adnych przechowywanych w nim informacji.", 
				"<span class='note'>Skompilowano: <b>"__DATE__"</b> [<b>"__TIME__"</b>]</span><br/><br/>"
				"Copyright © 2006 <b>Micha³ \"Dulek\" Dulko</b><br/>"
				"Copyright © 2006 <b>Sijawusz Pur Rahnama</b><br/>"
				"Copyright © 2006 <b>Kamil \"Olórin\" Figiela</b>", "reg://IML16/35.ico"
			);
		
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUP, "Blokada opcji");
			{
				UIActionCfgAdd(kLock::CFG::group, kLock::ACT::enableActs, ACTT_BUTTON|ACTR_SHOW|ACTR_INIT, "Aktywuj opcje" AP_ICO "41");
			}
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUPEND);

			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUP, "Wybierz co blokowaæ");
			{
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockMainWindow, ACTT_CHECK|ACTR_STATUS, "Blokuj okno g³ówne programu", kLock::CFG::lockMainWindow);
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockTalkWindows, ACTT_CHECK|ACTR_STATUS, "Blokuj okienka rozmowy i historiê", kLock::CFG::lockTalkWindows);
				if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelAdvanced))
				{
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockTray, ACTT_CHECK, "Ukrywaj ikonkê w tray'u", kLock::CFG::lockTray);
					if(!Ctrl->ICMessage(IMC_ISWINXP))
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockProcess, ACTT_CHECK, "Ukrywaj Konnekta na liœcie procesów" AP_TIP "U¿ywaæ na w³asn¹ odpowiedzialnoœæ!", kLock::CFG::lockProcess);
				}
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockSound, ACTT_CHECK, "Wyciszaj dŸwiêki", kLock::CFG::lockSound);
				if(pluginExists(KNotify::net))
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockKNotify, ACTT_CHECK, "Blokuj powiadomienia kNotify" AP_TIPRICH "Wymaga zmodyfikowanego <b>kNotify</b>!", kLock::CFG::lockKNotify);
				if(pluginExists(plugsNET::kmigacz))
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::lockkMigacz, ACTT_CHECK, "Blokuj powiadomienia kMigacza", kLock::CFG::lockkMigacz);
			}
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUPEND);

			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUP, "Opcje");
			{
				UIActionCfgAdd(kLock::CFG::group, 0, ACTT_COMMENT|ACTSC_INLINE, "Has³o:");
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::password, ACTT_PASSWORD, 0, kLock::CFG::password);
				UIActionCfgAdd(kLock::CFG::group, kLock::CFG::synchronizeWithAway, ACTT_CHECK, "Synchronizuj z trybem auto-away", kLock::CFG::synchronizeWithAway);
				if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelIntermediate))
				{
					if(pluginExists(kAway2::net))
					{
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::synchronizeWithExtendedAway, ACTT_CHECK, "Synchronizuj z trybem extended auto-away", kLock::CFG::synchronizeWithExtendedAway);
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::synchronizeWithkAway, ACTT_CHECK, "Synchronizuj z kAway2", kLock::CFG::synchronizeWithkAway);
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::disablekAwayWhenUnlocking, ACTT_CHECK, "Wy³¹czaj kAway2 przy odblokowywaniu", kLock::CFG::disablekAwayWhenUnlocking);
					}
					if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelNormal))
					{
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::askForPasswordOnHistory, ACTT_CHECK, "Pytaj o has³o przy ka¿dej próbie dostêpu do historii", kLock::CFG::askForPasswordOnHistory);
						UIActionCfgAdd(kLock::CFG::group, kLock::CFG::blockOnStart, ACTT_CHECK, "Blokuj przy ka¿dym starcie programu", kLock::CFG::blockOnStart);
					}
					UIActionCfgAdd(kLock::CFG::group, 0, ACTT_SEP, "Po³o¿enie przycisku");
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::buttonOnToolbar, ACTT_CHECK|ACTSC_NEEDRESTART, "W menu wtyczek", kLock::CFG::buttonOnToolbar);
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::buttonInTray, ACTT_CHECK|ACTSC_NEEDRESTART, "W menu tray'a", kLock::CFG::buttonInTray);
					UIActionCfgAdd(kLock::CFG::group, kLock::CFG::buttonOnMainToolbar, ACTT_CHECK|ACTSC_NEEDRESTART, "Na g³ównym toolbarze", kLock::CFG::buttonOnMainToolbar);
				}
			}
			UIActionCfgAdd(kLock::CFG::group, 0, ACTT_GROUPEND);
		}

		//wy³¹czamy kontrolki konfiguracji
		this->disableActs();
	}

	//IM_START - wystartowanie wtyczki (po przygotowaniu interfejsu)
	void Controller::onStart()
	{
		//je¿eli nie ma TabletKi tosubclassujemy akcjê kontrolki kIEview
		if(!pluginExists("TABLETKA"))
		{
			IMLOG("Nie ma TabletKi, subclassujê akcjê kIEview");
			this->subclassAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlview);
		}

		//subclassujemy g³ówne okno programu
		this->konnektWindowOldProc = (WNDPROC)SetWindowLongPtr((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), GWLP_WNDPROC, (LONG_PTR)konnektWindowProc);

		//jeœli poprzednim razem by³o zablokowane blokujemy
		if(GETINT(kLock::CFG::lockState) || GETINT(kLock::CFG::blockOnStart))
			this->lock(L_Lock, 1);
	}

	//IM_UIACTION - handler akcji
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
					IMLOG("[Controller::onAction]: an->act.id = IMIA_MSG_HISTORY|IMIA_CNT_HISTORY|IMIA_MSG_HISTORY, an->code = ACTN_ACTION");

					//Nie pozwalamy pokazaæ siê okienku historii, jeœli jest zablokowane lub mamy pytaæ o has³o
					if(GETINT(kLock::CFG::lockState) && GETINT(kLock::CFG::lockTalkWindows))
					{
						if(!this->lock(L_Unlock));
							return this->setReturnCode(0);
					}
					else if(GETINT(kLock::CFG::askForPasswordOnHistory))
					{
						if(!this->askForPassword("kLock", "Dostêp do historii jest zablokowany.\r\nPodaj has³o dostêpu:", "Otworzyæ historiê?"))
							return this->setReturnCode(0);
					}
				}
				this->forwardAction();
				return;
			}
			case Konnekt::UI::ACT::msg_ctrlview:
			{
				if(an->code == ACTN_CREATEWINDOW)
				{
					IMLOG("[ActionProc]: an->act.id = UI::ACT::msg_ctrlview, an->code = ACTN_CREATEWINDOW");

					//jeœli jest zablokowane niszczymy tworzone okienko
					if(GETINT(kLock::CFG::lockState) && GETINT(kLock::CFG::lockTalkWindows))
					{
						sUIActionNotify_createWindow* anCW = static_cast<sUIActionNotify_createWindow*>(static_cast<sUIActionNotify_base*>(an));
						DestroyWindow(anCW->hwndParent);
						DestroyWindow(anCW->hwnd);
						return this->setReturnCode(0);
					}
				}
				this->forwardAction();
				return;
			}
			case kLock::ACT::enableActs:
			{
				//w³¹czamy lub wy³¹czamy kontrolki w konfiguracji
				if(an->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: an->act.id = kLock::ACT::EnableActs, an->code = ACTN_ACTION");

					this->disableActs(L_Detect);
				}
				else if(an->code == ACTN_HIDE || an->code == ACTN_DESTROY)
				{
					IMLOG("[ActionProc]: an->act.id = kLock::ACT::EnableActs, an->code = ACTN_HIDE|ACTN_DESTROY");

					this->disableActs(L_Lock);
				}
				break;
			}
			case kLock::ACT::lock:
			{
				if(an->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: an->act.id = kLock::CFG::Lock, an->code = ACTN_ACTION");

					//blokujemy lub wywo³ujemy procedurê odblokowywania
					this->lock();
				}
				break;
			}
			case kLock::CFG::lockTray:
			{
				if(an->code == ACTN_ACTION)
					//wyœwietlamy ostrze¿enie na temat opcji usuwania ikonki z traya
					if(*UIActionCfgGetValue(an->act, 0, 0) != '0')
						ICMessage(IMI_WARNING, (int)"Przed w³¹czeniem tej opcji ustaw odblokowuj¹cy skrót k.Lawy\n - inaczej nie odblokujesz Konnekta.");
				break;
			}
			case kLock::CFG::lockMainWindow:
			case kLock::CFG::lockTalkWindows:
			{
				if(an->code == ACTN_ACTION)
					//wymuszamy odœwie¿enie zablokowañ kontrolek - ¿eby zale¿noœci mog³y zagraæ
					this->disableActs(L_Unlock, 1);
				break;
			}
		}
	}

	//IM_AWAY - w³¹czenie trybu away
	void Controller::onAway()
	{
		IMLOG("[Controller::onAway]:");
		if(GETINT(kLock::CFG::synchronizeWithAway))
			this->lock(L_Lock);
	}

	//kAway2::im::extendedAutoAway - w³¹czenie trybu extended away (kAway2)
	void Controller::onExtendedAway()
	{
		IMLOG("[Controller::onExtendedAway]:");
		if(GETINT(kLock::CFG::synchronizeWithExtendedAway))
			this->lock(L_Lock);
	}

	//kAway2::im::away - w³¹czenie kAway2
	void Controller::onkAway()
	{
		IMLOG("[Controller::onkAway]:");
		if(GETINT(kLock::CFG::synchronizeWithkAway))
			this->lock(L_Lock);
	}

	//kLock::API::lock - api, blokowanie
	void Controller::apiLock()
	{
		this->setReturnCode(this->lock(L_Lock));
	}

	//kLock::API::unlock - api, odblokowywanie
	void Controller::apiUnlock()
	{
		this->setReturnCode(this->lock(L_Unlock));
	}

	//kLock::API::isUnlocked - api, czy jest odblokowane
	void Controller::apiIsUnlocked()
	{
		this->setReturnCode(!GETINT(kLock::CFG::lockState));
	}

	//kLock::API::isUnlockedForKNotify - api, czy jest odblokowane zale¿ne od opcji kLocka
	void Controller::apiIsUnlockedForKNotify()
	{
		this->setReturnCode(!(GETINT(kLock::CFG::lockState) == 1 && GETINT(kLock::CFG::lockKNotify)));
	}

	//kLock::API::apiIsUnlockedForkMigacz - api, czy jest odblokowane zale¿ne od opcji kLocka
	void Controller::apiIsUnlockedForkMigacz()
	{
		this->setReturnCode(!(GETINT(kLock::CFG::lockState) == 1 && GETINT(kLock::CFG::lockkMigacz)));
	}

	//funkcja blokuj¹ca i odblokowuj¹ca Konnekta Konnekta
	bool Controller::lock(kLock::Lock lock, bool force)
	{
		IMLOG("[Controller::lock:] lock = %i, force = %i", lock, force);

		//zmienne
		static tLockedWindows locked_windows; //okna przypiête przy zak³adaniu blokady
		static bool konnektWndVisible; //czy g³ówne okienko by³o widoczne
		static bool msgWndVisible; //czy okienko rozmowy by³o widoczne
		static bool muted; //czy kSound by³ wyciszony

		//jeœli wszystko siê zgadza blokujemy lub odblokowujemy
		if((lock == L_Lock && (force || !GETINT(kLock::CFG::lockState))) || (lock == L_Detect && !GETINT(kLock::CFG::lockState)))
		{
			//ustawiamy w configu, ¿e jest blokowane
			SETINT(kLock::CFG::lockState, 1);

			//jeœli jest zaznaczona odpowiednia opcja blokujemy okienka rozmowy
			if(GETINT(kLock::CFG::lockTalkWindows))
			{
				//z TabletK¹ lub bez TabletKi
				if(pluginExists("TABLETKA"))
				{
					IMLOG("Blokujê okienka rozmowy, TabletKa w³¹czona");

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
					IMLOG("Blokujê okienka rozmowy, TabletKa wy³¹czona");

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

			//jeœli jest zaznaczona opcja ukrywamy g³ówne okno - jego nowy proc nie pozwoli mu siê pokazaæ
			if(GETINT(kLock::CFG::lockMainWindow))
			{
				IMLOG("Ukrywam g³ówne okno");

				if(IsWindowVisible((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND))))
				{
					konnektWndVisible = 1;
					ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_HIDE);
				}
				else
					konnektWndVisible = 0;
			}

			//jeœli jest zaznaczona opcja i dŸwiêki nie s¹ ju¿ wyciszone, wyciszamy je
			if(GETINT(kLock::CFG::lockSound))
			{
				IMLOG("Wyciszam dŸwiêki");

				if(!GETINT(kSound::Cfg::mute))
				{
					muted = 0;
					callAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);
				}
				else
					muted = 1;
			}

			//jeœli jest zaznaczona opcja usuwamy ikonkê z tray'a
			if(GETINT(kLock::CFG::lockTray) && GETINT(kLock::CFG::lockMainWindow) && GETINT(kLock::CFG::lockTalkWindows))
			{
				IMLOG("Usuwam ikonkê z tray'a");

				NOTIFYICONDATA tray;
				ZeroMemory(&tray, sizeof(NOTIFYICONDATA));
				tray.cbSize = sizeof(NOTIFYICONDATA);
				tray.uID = 1000;
				tray.uFlags = 0;
				tray.hWnd = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));
				tray.uCallbackMessage = WM_USER+1086;
				Shell_NotifyIcon(NIM_DELETE, &tray);
			}

			//jeœli jest zaznaczona opcja usuwamy z listy procesów (przynajmniej próbujemy)
			if(GETINT(kLock::CFG::lockProcess))
			{
				IMLOG("Usuwam Konnekta z listy procesów");
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
			{
				UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::ACT::lock, "Odblokuj");
				UIActionSetText(IMIG_TRAY, kLock::ACT::lock, "Odblokuj");
				UIActionSetText(IMIG_MAINTB, kLock::ACT::lock, "Odblokuj");
			}
		}
		else if(lock == L_Unlock && (force || GETINT(kLock::CFG::lockState)) || (lock == L_Detect && GETINT(kLock::CFG::lockState)))
		{
			//jeœli has³o siê zgadza odblokowujemy
			if(this->askForPassword("kLock", "Konnekt zosta³ zablokowany.\r\nPodaj has³o dostêpu:", "Odblokowaæ?"))
			{
				IMLOG("Odblokowujê");

				//ustawiamy w configu, ¿e jest odblokowane
				SETINT(kLock::CFG::lockState, 0);

				//odblokowujemy okienka rozmowy
				{
					//z TabletK¹ lub bez TabletKi
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

				//pokazujemy g³ówne okno - nowy proc powinien ju¿ na to pozwoliæ
				if(konnektWndVisible)
					ShowWindow((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), SW_SHOW);

				//wy³aczamy wyciszenie dŸwiêków
				if(!muted && GETINT(kSound::Cfg::mute))
					callAction(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kSound::action::mute);

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

				//przywracamy na listê procesów (przynajmniej próbujemy)
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

				//wy³¹czamy kAway2
				if(GETINT(kLock::CFG::disablekAwayWhenUnlocking))
					Ctrl->IMessage(kAway2::api::disable, kAway2::net, IMT_ALL, (int)"");

				//zmieniamy napis na przycisku
				{
					UIActionSetText(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::ACT::lock, "Zablokuj");
					UIActionSetText(IMIG_TRAY, kLock::ACT::lock, "Zablokuj");
					UIActionSetText(IMIG_MAINTB, kLock::ACT::lock, "Zablokuj");
				}
			}
			else
			{
				IMLOG("Nie odblokowujê");
			}
		}
		return !GETINT(kLock::CFG::lockState);
	}

	//funkcja deaktywuj¹ca kontrolki w konfiguracji
	bool Controller::disableActs(kLock::Lock lock, bool force)
	{
		IMLOG("[Controller::disableActs]");

		//zmienne
		static bool actsLocked;

		//jeœli wszystko siê zgadza disablujemy lub enablujemy kontrolki
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
		}
		else if(lock == L_Unlock && (force || (actsLocked && this->askForPassword("kLock", "Zmiana ustawieñ jest zablokowana.\r\nPodaj has³o dostêpu:", "Odblokowaæ ustawienia?", (HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND))))) || (lock == L_Detect && actsLocked && this->askForPassword("kLock", "Zmiana ustawieñ jest zablokowana.\r\nPodaj has³o dostêpu:", "Odblokowaæ ustawienia?", (HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND)))))
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
		}
		return !actsLocked;
	}

	//funkcja wyœwietlaj¹ca monit o has³o, jeœli siê zgadza zwraca 1, jeœli nie 0
	int Controller::askForPassword(std::string title, std::string text, std::string text2, HWND parent)
	{
		IMLOG("[Controller::askForPassword]");

		//zmienne
		static bool unlocking;

		//je¿eli nie trwa ju¿ odblokowywanie
		if(!unlocking)
		{
			unlocking = 1;
			
			if(!parent)
				parent = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));

			//je¿eli jest has³o lub nie ma has³a
			if(strlen(GETSTRA(kLock::CFG::password)))
			{
				sDIALOG_access sde;

				//wyœwietlamy pytanie o has³o
				{
					sde.info = text.c_str();
					sde.title = title.c_str();
					sde.handle = parent;
					//jeœli siê nie uda³o zwracamy 0
					if(!ICMessage(IMI_DLGPASS, (int)&sde))
					{
						unlocking = 0;
						return 0;
					}
				}

				//jeœli has³o siê zgadza zwracamy 1, jeœli nie 0
				if((std::string)sde.pass == (std::string)GETSTRA(kLock::CFG::password))
				{
					unlocking = 0;
					return 1;
				}
				else
				{
					MessageBox(parent, "Z³e has³o", title.c_str(), MB_ICONERROR);
					unlocking = 0;
					return 0;
				}
			}
			else
			{
				IMLOG("Nie ma has³a");

				//jeœli potwierdzono lub nie
				if(MessageBox(parent, text2.c_str(), title.c_str(), MB_ICONQUESTION|MB_YESNO) == IDYES)
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
		return 0;
	}
}