#include "includes.h"

using namespace kLock;

namespace kLock
{
	int IStart()
	{
		IMLOG("[IStart]");

		//subclassujemy akcjê historii
		kLock::history_owner = SubclassAction(IMIG_MAIN_CNT, IMIA_MAIN_HISTORY);

		//subclassujemy akcjê historii w menu kontaktu
		kLock::cnt_history_owner = SubclassAction(IMIG_CNT, IMIA_CNT_HISTORY);

		//subclassujemy akcjê historii w oknie kontaktu
		kLock::msgwnd_history_owner = SubclassAction(IMIG_MSGTB, IMIA_MSG_HISTORY);

		if(!PluginExists(Tabs::net))
		{
			IMLOG("Nie ma TabletKi, subclassujê akcjê kIEview");

			//subclassujemy akcjê kontrolki kIEview - pozwoli nam wykrywaæ otwarcie okienka rozmowy
			kLock::kIEview_owner = SubclassAction(IMIG_MSGWND, Konnekt::UI::ACT::msg_ctrlview);
		}

		//subclassujemy g³ówne okno programu - nie pozwolimy mu siê póŸniej pokazaæ
		old_mainwnd_proc = (WNDPROC)SetWindowLongPtr((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), GWLP_WNDPROC, (LONG_PTR)MainWindowProc);

		//jeœli poprzednim razem by³o zablokowane blokujemy
		if(GETINT(kLock::Config::State) || GETINT(kLock::Config::BlockOnStart))
		{
			Lock(1);
		}

		return 1;
	}

	int IEnd()
	{
		return 1;
	}

	int ISetCols()
	{
		//config
		{
			Ctrl->SetColumn(DTCFG, kLock::Config::State, DT_CT_INT|DT_CF_SECRET, 0, "kLock/LockState");
			Ctrl->SetColumn(DTCFG, kLock::Config::Password, DT_CT_STR|DT_CF_SECRET, "", "kLock/LockPassword");
			Ctrl->SetColumn(DTCFG, kLock::Config::SynchronizeWithAway, DT_CT_INT, 0, "kLock/SynchronizeWithAutoAway");
			Ctrl->SetColumn(DTCFG, kLock::Config::LockMainWindow, DT_CT_INT, 1, "kLock/LockListAndToolbars");
			Ctrl->SetColumn(DTCFG, kLock::Config::LockTalkWindows, DT_CT_INT, 1, "kLock/LockTalkWindows");
			Ctrl->SetColumn(DTCFG, kLock::Config::LockSound, DT_CT_INT, 0, "kLock/LockSound");
			Ctrl->SetColumn(DTCFG, kLock::Config::LockTray, DT_CT_INT, 0, "kLock/LockTray");
			Ctrl->SetColumn(DTCFG, kLock::Config::LockProcess, DT_CT_INT, 0, "kLock/LockProcess");
			Ctrl->SetColumn(DTCFG, kLock::Config::LockKNotify, DT_CT_INT, 0, "kLock/LockKNotify");
			Ctrl->SetColumn(DTCFG, kLock::Config::LockkMigacz, DT_CT_INT, 0, "kLock/LockkMigacz");
			Ctrl->SetColumn(DTCFG, kLock::Config::AskForPasswordOnHistory, DT_CT_INT, 0, "kLock/AskForPasswordOnHistoryAccess");
			Ctrl->SetColumn(DTCFG, kLock::Config::SynchronizeWithkAway, DT_CT_INT, 0, "kLock/SynchronizeWithkAway2");
			Ctrl->SetColumn(DTCFG, kLock::Config::ButtonOnToolbar, DT_CT_INT, 0, "kLock/ButtonOnToolbar");
			Ctrl->SetColumn(DTCFG, kLock::Config::ButtonInTray, DT_CT_INT, 1, "kLock/ButtonInTray");
			Ctrl->SetColumn(DTCFG, kLock::Config::ButtonOnMainToolbar, DT_CT_INT, 0, "kLock/ButtonOnMainToolbar");
			Ctrl->SetColumn(DTCFG, kLock::Config::TurnOffkAwayOnUnlocking, DT_CT_INT, 0, "kLock/TurnOffkAwayOnUnlocking");
			Ctrl->SetColumn(DTCFG, kLock::Config::BlockOnStart, DT_CT_INT, 0, "kLock/BlockOnStart");
		}
		return 1;
	}

	int IPrepare()
	{
		IMLOG("[IPrepare]");

		//przycisk na toolbarze wtyczek
		if(GETINT(kLock::Config::ButtonOnToolbar))
		{
			IMLOG("Tworzê przycisk w grupie wtyczek");
			UIActionAdd(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::Acts::Lock, ACTR_INIT, GETINT(kLock::Config::State) ? "Odblokuj" : "Zablokuj", 35);
		}

		//przycisk w tray'u
		if(GETINT(kLock::Config::ButtonInTray))
		{
			IMLOG("Tworzê przycisk w menu w tray'u");
			UIActionInsert(IMIG_TRAY, kLock::Acts::Lock, Ctrl->IMessage(IMI_GROUP_ACTIONSCOUNT, 0, 0, (int)&sUIAction(IMIG_MAINWND, IMIG_TRAY)) - 1, ACTR_INIT, GETINT(kLock::Config::State) ? "Odblokuj" : "Zablokuj", 35);
		}

		//przycisk na g³ównym toolbarze
		if(GETINT(kLock::Config::ButtonOnMainToolbar))
		{
			if(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0) != IMIG_MAINTB)
			{
				IMLOG("Tworzê przycisk na g³ównym toolbarze");
				UIActionAdd(IMIG_MAINTB, kLock::Acts::Lock, ACTR_INIT, GETINT(kLock::Config::State) ? "Odblokuj" : "Zablokuj", 35);
			}
		}

		//konfiguracja
		UIGroupAdd(IMIG_CFG_PLUGS, kLock::Config::Group, 0, "kLock", 35);
		{
			UIActionCfgAddPluginInfoBox2(kLock::Config::Group, 
				"Wtyczka ma za zadanie blokowaæ Konnekta, aby nikt niepowo³any nie dosta³ siê do ¿adnych przechowywanych w nim informacji.", 
				"<span class='note'>Skompilowano: <b>"__DATE__"</b> [<b>"__TIME__"</b>]</span><br/><br/>"
				"Copyright © 2006 <b>Micha³ \"Dulek\" Dulko</b><br/>"
				"Copyright © 2006 <b>Sijawusz Pur Rahnama</b><br/>"
				"Copyright © 2006 <b>Kamil \"Olórin\" Figiela</b>", "reg://IML16/35.ico"
			);
		
			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUP, "Blokada opcji");
			{
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::EnableActs, ACTT_BUTTON|ACTR_SHOW|ACTR_INIT, "Aktywuj opcje" AP_ICO "41");
			}
			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUPEND);

			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUP, "Wybierz co blokowaæ");
			{
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockMainWindow, ACTT_CHECK, "Blokuj okno g³ówne programu", kLock::Config::LockMainWindow);
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockTalkWindows, ACTT_CHECK, "Blokuj okienka rozmowy i historiê", kLock::Config::LockTalkWindows);
				if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelAdvanced))
				{
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockTray, ACTT_CHECK, "Ukrywaj ikonkê w tray'u", kLock::Config::LockTray);
					if(!Ctrl->ICMessage(IMC_ISWINXP))
					{
						IMLOG("Win9x, wyœwietlam opcjê");
						UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockProcess, ACTT_CHECK, "Ukrywaj Konnekta na liœcie procesów" AP_TIP "U¿ywaæ na w³asn¹ odpowiedzialnoœæ!", kLock::Config::LockProcess);
					}
				}
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockSound, ACTT_CHECK, "Wyciszaj dŸwiêki", kLock::Config::LockSound);
				if(PluginExists(KNotify::net))
				{
					IMLOG("Jest kNotify, wyœwietlam opcjê");
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockKNotify, ACTT_CHECK, "Blokuj powiadomienia kNotify" AP_TIPRICH "Wymaga zmodyfikowanego <b>kNotify</b>!", kLock::Config::LockKNotify);
				}
				if(PluginExists(kMigacz::Net))
				{
					IMLOG("Jest kMigacz, wyœwietlam opcjê");
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockkMigacz, ACTT_CHECK, "Blokuj powiadomienia kMigacza", kLock::Config::LockkMigacz);
				}
			}
			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUPEND);

			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUP, "Opcje");
			{
				UIActionCfgAdd(kLock::Config::Group, 0, ACTT_COMMENT|ACTSC_INLINE, "Has³o:");
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::Password, ACTT_PASSWORD, 0, kLock::Config::Password);
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::SynchronizeWithAway, ACTT_CHECK, "Synchronizuj z trybem auto-away", kLock::Config::SynchronizeWithAway);
				if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelIntermediate))
				{
					if(PluginExists(kAway2::net))
					{
						IMLOG("Jest kAway2, wyœwietlam opcje");
						UIActionCfgAdd(kLock::Config::Group, kLock::Config::SynchronizeWithkAway, ACTT_CHECK, "Synchronizuj z kAway2", kLock::Config::SynchronizeWithkAway);
						UIActionCfgAdd(kLock::Config::Group, kLock::Config::TurnOffkAwayOnUnlocking, ACTT_CHECK, "Wy³¹czaj kAway2 przy odblokowywaniu", kLock::Config::TurnOffkAwayOnUnlocking);
					}
					if(Konnekt::ShowBits::checkLevel(Konnekt::ShowBits::levelNormal))
					{
						UIActionCfgAdd(kLock::Config::Group, kLock::Config::AskForPasswordOnHistory, ACTT_CHECK, "Pytaj o has³o przy ka¿dej próbie dostêpu do historii", kLock::Config::AskForPasswordOnHistory);
						UIActionCfgAdd(kLock::Config::Group, kLock::Config::BlockOnStart, ACTT_CHECK, "Blokuj przy ka¿dym starcie programu", kLock::Config::BlockOnStart);
					}
					UIActionCfgAdd(kLock::Config::Group, 0, ACTT_SEP, "Po³o¿enie przycisku");
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::ButtonOnToolbar, ACTT_CHECK|ACTSC_NEEDRESTART, "W menu wtyczek", kLock::Config::ButtonOnToolbar);
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::ButtonInTray, ACTT_CHECK|ACTSC_NEEDRESTART, "W menu tray'a", kLock::Config::ButtonInTray);
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::ButtonOnMainToolbar, ACTT_CHECK|ACTSC_NEEDRESTART, "Na g³ównym toolbarze", kLock::Config::ButtonOnMainToolbar);
				}
			}
			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUPEND);
		}

		DisableActs();
		return 1;
	}

	ActionProc(sUIActionNotify_base* anBase)
	{
		switch(anBase->act.id)
		{
			case IMIA_MAIN_HISTORY:
			{
				if(anBase->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: anBase->act.id = IMIA_MAIN_HISTORY, anBase->code = ACTN_ACTION");

					if(GETINT(kLock::Config::State) && GETINT(kLock::Config::LockTalkWindows))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::Config::AskForPasswordOnHistory))
					{
						if(!AskForPassword("kLock", "Dostêp do historii jest zablokowany.\r\nPodaj has³o dostêpu:", "Otworzyæ historiê?"))
						{
							return 0;
						}
					}
				}
				return IMessageDirect(IM_UIACTION, kLock::history_owner, (int)anBase);
				break;
			}
			case IMIA_CNT_HISTORY:
			{
				if(anBase->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: anBase->act.id = IMIA_CNT_HISTORY, anBase->code = ACTN_ACTION");

					if(GETINT(kLock::Config::State) && GETINT(kLock::Config::LockTalkWindows))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::Config::AskForPasswordOnHistory))
					{
						if(!AskForPassword("kLock", "Dostêp do historii jest zablokowany.\r\nPodaj has³o dostêpu:", "Otworzyæ historiê?"))
						{
							return 0;
						}
					}
				}
				return IMessageDirect(IM_UIACTION, kLock::history_owner, (int)anBase);
				break;
			}
			case IMIA_MSG_HISTORY:
			{
				if(anBase->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: anBase->act.id = IMIA_MSG_HISTORY, anBase->code = ACTN_ACTION");

					if(GETINT(kLock::Config::State) && GETINT(kLock::Config::LockTalkWindows))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::Config::AskForPasswordOnHistory))
					{
						if(!AskForPassword("kLock", "Dostêp do historii jest zablokowany.\r\nPodaj has³o dostêpu:", "Otworzyæ historiê?"))
						{
							return 0;
						}
					}
				}
				return IMessageDirect(IM_UIACTION, kLock::history_owner, (int)anBase);
				break;
			}
			case Konnekt::UI::ACT::msg_ctrlview:
			{
				if(anBase->code == ACTN_CREATEWINDOW)
				{
					IMLOG("[ActionProc]: anBase->act.id = Konnekt::UI::ACT::msg_ctrlview, anBase->code = ACTN_CREATEWINDOW");

					if(GETINT(kLock::Config::State) && GETINT(kLock::Config::LockTalkWindows))
					{
						sUIActionNotify_createWindow* an = static_cast<sUIActionNotify_createWindow*>(anBase);
						DestroyWindow(an->hwndParent);
						return 0;
					}
				}
				return IMessageDirect(IM_UIACTION, kLock::kIEview_owner, (int)anBase);
			}
			case kLock::Config::EnableActs:
			{
				if(anBase->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: anBase->act.id = kLock::Config::EnableActs, anBase->code = ACTN_ACTION");

					if(!acts_enabled)
					{
						if(AskForPassword("kLock", "Zmiana ustawieñ jest zablokowana.\r\nPodaj has³o dostêpu:", "Odblokowaæ ustawienia?", (HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND))))
						{
							EnableActs();
						}
					}
					else
					{
						DisableActs();
					}
				}
				else if(anBase->code == ACTN_HIDE || anBase->code == ACTN_DESTROY)
				{
					IMLOG("[ActionProc]: anBase->act.id = kLock::Config::EnableActs, anBase->code = ACTN_HIDE|ACTN_DESTROY");

					DisableActs();
				}
				break;
			}
			case kLock::Acts::Lock:
			{
				if(anBase->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: anBase->act.id = kLock::Config::Lock, anBase->code = ACTN_ACTION");

					if(GETINT(kLock::Config::State))
					{
						Unlock();
					}
					else
					{
						Lock();
					}						
				}
				break;
			}
			case kLock::Config::LockTray:
			{
				if(anBase->code == ACTN_ACTION)
				{
					IMLOG("[ActionProc]: anBase->act.id = kLock::Config::LockTray, anBase->code = ACTN_ACTION");

					if(*UIActionCfgGetValue(anBase->act, 0, 0) != '0')
					{
						ICMessage(IMI_WARNING, (int)"Przed w³¹czeniem tej opcji ustaw odblokowuj¹cy skrót k.Lawy\n - inaczej nie odblokujesz Konnekta.");
					}
				}
			}
		}
		return 0;
	}
}

int __stdcall IMessageProc(sIMessage_base * msgBase)
{
	sIMessage_2params * msg = (msgBase->s_size>=sizeof(sIMessage_2params))?static_cast<sIMessage_2params*>(msgBase):0;
	switch (msgBase->id)
	{
		case IM_PLUG_NET: return kLock::Net;
		case IM_PLUG_TYPE: return IMT_UI|IMT_CONFIG;
		case IM_PLUG_VERSION: return 0;
		case IM_PLUG_SDKVERSION: return KONNEKT_SDK_V;
		case IM_PLUG_SIG: return (int)kLock::Sig;
		case IM_PLUG_NAME: return (int)kLock::Name;
		case IM_PLUG_NETNAME: return 0;
		case IM_PLUG_INIT: Plug_Init(msg->p1, msg->p2); return 1;
		case IM_PLUG_DEINIT: Plug_Deinit(msg->p1, msg->p2); return 1;
		case kLock::Api::Lock:
		{
			Lock();

			return 0;
		}
		case kLock::Api::Unlock:
		{
			return Unlock();
		}
		case kLock::Api::IsUnlockedForKNotify:
		{
			return !(GETINT(kLock::Config::State) == 1 && GETINT(kLock::Config::LockKNotify));
		}
		case kLock::Api::IsUnlockedForkMigacz:
		{
			return !(GETINT(kLock::Config::State) == 1 && GETINT(kLock::Config::LockkMigacz));
		}
		case kLock::Api::IsUnlocked:
		{
			IMLOG("[kLock::Api::IsUnlocked]");

			return !GETINT(kLock::Config::State);
		}
		case IM_AWAY:
		{
			IMLOG("[IMessageProc]: msgBase->id = IM_AWAY");

			if(GETINT(kLock::Config::SynchronizeWithAway))
			{
				Lock();
			}
			return 1;
		}
		case kAway2::api::isAway:
		{
			IMLOG("[IMessageProc]: msgBase->id = kAway2::api::isAway");

			if(GETINT(kLock::Config::SynchronizeWithkAway))
			{
				Lock();
			}
			return 1;
		}
		case IM_SETCOLS: return ISetCols();
		case IM_UI_PREPARE: return IPrepare();
		case IM_START: return IStart();
		case IM_END: return IEnd();
		case IM_UIACTION: return ActionProc((sUIActionNotify_base*)msg->p1);
	}
	if(Ctrl)
	{
		Ctrl->setError(IMERROR_NORESULT);
	}
	return 0;
}