#include "includes.h"

using namespace kLock;

namespace kLock
{
	int IStart()
	{
    //sprawdzamy czy jest separator w menu
		if(GETINT(kLock::Config::ButtonInTray))
		{
      int n = 0, sepCount = 0, btnsCount = Ctrl->IMessage(IMI_GROUP_ACTIONSCOUNT, 0, 0, (int)&sUIAction(IMIG_MAINWND, IMIG_TRAY));
      for (int idx = (btnsCount - 1); idx > 0; idx--) {
        if (!Ctrl->IMessage(IMI_ACTION_GETID, 0, 0, IMIG_TRAY, idx)) sepCount++;
      }
      //jeœli nie, to dodajemy, ¿eby "odci¹æ" siê akcji wtyczek sieciowych
      if (sepCount < 2) {
		    UIActionSetStatus(sUIAction(IMIG_TRAY, ACTT_SEP), 0, ACTS_HIDDEN);
      }
    }

		sUIActionInfo nfo;

		//subclassujemy akcjê historii
		kLock::history_owner = SubclassAction(IMIG_MAIN_CNT, IMIA_MAIN_HISTORY);

		//subclassujemy akcjê historii w menu kontaktu
		kLock::cnt_history_owner = SubclassAction(IMIG_CNT, IMIA_CNT_HISTORY);

		//subclassujemy akcjê historii w oknie kontaktu
		kLock::msgwnd_history_owner = SubclassAction(IMIG_MSGTB, IMIA_MSG_HISTORY);

		//subclassujemy g³ówne okno programu - nie pozwolimy mu siê póŸniej pokazaæ
		old_mainwnd_proc = (WNDPROC)SetWindowLongPtr((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), GWLP_WNDPROC, (LONG_PTR)MainWindowProc);

		//sprawdzamy, czy has³o nie jest puste
		if(!strlen(GETSTRA(kLock::Config::Password)))
		{
			ICMessage(IMI_INFORM, (int)"Przed pierwszym u¿yciem ustaw has³o blokowania Konnekta!");
			sDIALOG_access sde;

			sde.pass = "";
			sde.info = "Podaj nowe has³o";
			sde.handle = (HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND));
			sde.title = "kLock";
			for(;;)
			{
				if(!ICMessage(IMI_DLGSETPASS, (int)&sde))
				{
					return 0;
				}
				if(strlen(sde.pass))
				{
					break;
				}
				ICMessage(IMI_ERROR,(int)"Has³o nie mo¿e byæ puste!");
			}
			SETSTR(kLock::Config::Password, sde.pass);
			ICMessage(IMI_INFORM, (int)"Has³o zmienione.");
		}

		//jeœli poprzednim razem by³o zablokowane blokujemy
		if(GETINT(kLock::Config::State))
		{
			Lock();
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
		}
		return 1;
	}

	int IPrepare()
	{
		//przycisk
		if(GETINT(kLock::Config::ButtonOnToolbar))
		{
			UIActionAdd(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::Acts::Lock, ACTR_INIT, "kLock", 35);
		}
		if(GETINT(kLock::Config::ButtonInTray))
		{
      UIActionInsert(IMIG_TRAY, ACTT_SEP, 0, ACTT_SEP|ACTS_HIDDEN);
			UIActionInsert(IMIG_TRAY, kLock::Acts::Lock, 0, ACTR_INIT, "kLock", 35);
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
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockMainWindow, ACTT_CHECK, "Blokuj okno g³ówne programu", 
          kLock::Config::LockMainWindow);
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockTalkWindows, ACTT_CHECK, "Blokuj okienka rozmowy i historiê", 
          kLock::Config::LockTalkWindows);
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockTray, ACTT_CHECK, "Ukrywaj ikonkê w tray'u", kLock::Config::LockTray);
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockSound, ACTT_CHECK, "Wyciszaj dŸwiêki", kLock::Config::LockSound);
				if(PluginExists(KNotify::net))
				{
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockKNotify, ACTT_CHECK, "Blokuj powiadomienia kNotify" 
            AP_TIPRICH "Wymaga zmodyfikowanego <b>kNotify</b>!", kLock::Config::LockKNotify);
				}
				if(PluginExists(NET))
				{
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockkMigacz, ACTT_CHECK, "Blokuj powiadomienia kMigacza", 
            kLock::Config::LockkMigacz);
				}
        if(!Ctrl->ICMessage(IMC_ISWINXP))
				{
				  UIActionCfgAdd(kLock::Config::Group, kLock::Config::LockProcess, ACTT_CHECK, "Ukrywaj Konnekta na liœcie procesów" 
            AP_TIP "U¿ywaæ na w³asn¹ odpowiedzialnoœæ!", kLock::Config::LockProcess);
				}
			}
			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUPEND);

			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUP, "Opcje");
			{
				UIActionCfgAdd(kLock::Config::Group, 0, ACTT_COMMENT|ACTSC_INLINE, "Has³o:");
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::Password, ACTT_PASSWORD, 0, kLock::Config::Password);
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::SynchronizeWithAway, ACTT_CHECK, "Synchronizuj z trybem auto-away", 
          kLock::Config::SynchronizeWithAway);
				if(PluginExists(kAway2::net))
				{
					UIActionCfgAdd(kLock::Config::Group, kLock::Config::SynchronizeWithkAway, ACTT_CHECK, "Synchronizuj z kAway2", 
            kLock::Config::SynchronizeWithkAway);
				}
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::AskForPasswordOnHistory, ACTT_CHECK, 
          "Pytaj o has³o przy ka¿dej próbie dostêpu do historii", kLock::Config::AskForPasswordOnHistory);
				UIActionCfgAdd(kLock::Config::Group, 0, ACTT_SEP, "Po³o¿enie przycisku");
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::ButtonOnToolbar, ACTT_CHECK|ACTSC_NEEDRESTART, 
          "W menu wtyczek", kLock::Config::ButtonOnToolbar);
				UIActionCfgAdd(kLock::Config::Group, kLock::Config::ButtonInTray, ACTT_CHECK|ACTSC_NEEDRESTART, 
          "W menu tray'a", kLock::Config::ButtonInTray);
			}
			UIActionCfgAdd(kLock::Config::Group, 0, ACTT_GROUPEND);
		}

    DisableActs();
		return 1;
	}

	ActionProc(sUIActionNotify_base* anBase)
	{
		switch (anBase->act.id)
		{
			case IMIA_MAIN_HISTORY:
			{
				if(anBase->code == ACTN_ACTION)
				{
					if(GETINT(kLock::Config::State))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::Config::AskForPasswordOnHistory))
					{
						if(!AskForPassword("kLock", "Dostêp do historii jest zablokowany.\r\nPodaj has³o dostêpu:"))
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
					if(GETINT(kLock::Config::State))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::Config::AskForPasswordOnHistory))
					{
						if(!AskForPassword("kLock", "Dostêp do historii jest zablokowany.\r\nPodaj has³o dostêpu:"))
						{
							return 0;
						}
					}
				}
				return IMessageDirect(IM_UIACTION, kLock::cnt_history_owner, (int)anBase);
				break;
			}
			case IMIA_MSG_HISTORY:
			{
				if(anBase->code == ACTN_ACTION)
				{
					if(GETINT(kLock::Config::State))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::Config::AskForPasswordOnHistory))
					{
						if(!AskForPassword("kLock", "Dostêp do historii jest zablokowany.\r\nPodaj has³o dostêpu:"))
						{
							return 0;
						}
					}
				}
				return IMessageDirect(IM_UIACTION, kLock::msgwnd_history_owner, (int)anBase);
				break;
			}
			case kLock::Config::EnableActs:
			{
				if(anBase->code == ACTN_ACTION)
				{
					if(!acts_enabled)
					{
						if(AskForPassword("kLock", "Zmiana ustawieñ jest zablokowana.\r\nPodaj has³o dostêpu:", 
              (HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND))))
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
					DisableActs();
				}
				break;
			}
			case kLock::Acts::Lock:
			{
				if(anBase->code == ACTN_ACTION)
				{
					if(GETINT(kLock::Config::State))
					{
						//odblokowujemy Konnekta
						Unlock();
					}
					else
					{
						//blokujemy Konnekta
						Lock();
					}						
				}
				else if(anBase->code == ACTN_CREATE)
				{
					UIActionSetText(anBase->act, GETINT(kLock::Config::State) ? "Odblokuj" : "Zablokuj");
				}
				break;
			}
			case kLock::Config::LockTray:
			{
				ACTIONONLY(anBase);
				if(*UIActionCfgGetValue(anBase->act, 0, 0) != '0')
				{
					ICMessage(IMI_WARNING, (int)"Przed w³¹czeniem tej opcji ustaw odblokowuj¹cy skrót k.Lawy\n"
            "- inaczej nie odblokujesz Konnekta.");
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
		case kLock::Api::Unlock:
		{
			return Unlock();
		}
		case kLock::Api::IsUnlockedForKNotify:
		{
			if(GETINT(kLock::Config::State) == 1 && GETINT(kLock::Config::LockKNotify))
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		case kLock::Api::IsUnlocked:
		{
			return (GETINT(kLock::Config::State) == 0);
		}
		case kLock::Api::Lock:
		{
			Lock();
			return 0;
		}
		case IM_ALLPLUGINSINITIALIZED:
		{
			if(!PluginExists(Tabs::net))
			{
        Ctrl->IMessage(&sIMessage_plugOut(Ctrl->ID(), "Wtyczka kLock nie bêdzie dzia³aæ bez wtyczki tabletKa!", 
          sIMessage_plugOut::erYes, sIMessage_plugOut::euNowAndOnNextStart));
				return -1;
			}
			return 0;
		}
		case IM_AWAY:
		{
			if(GETINT(kLock::Config::SynchronizeWithAway))
			{
				Lock();
			}
			return 1;
		}
		case kAway2::api::isAway:
		{
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
	if (Ctrl) Ctrl->setError(IMERROR_NORESULT);
	return 0;
}