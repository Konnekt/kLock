#include "includes.h"
using namespace kLock;

namespace kLock
{
	int IStart()
	{
		sUIActionInfo nfo;

		//subclassujemy akcjê historii
		{
			nfo = sUIActionInfo(IMIG_MAIN_CNT, IMIA_MAIN_HISTORY);
			nfo.mask = UIAIM_ALL; 
			nfo.txt = new char[100];
			nfo.txtSize = 99;
			UIActionGet(nfo); 
			kLock::cnt_history_owner = ICMessage(IMI_ACTION_GETOWNER, (int)&nfo.act);
			if(!kLock::history_owner)
			{
				kLock::history_owner = ICMessage(IMC_PLUG_ID, 0);
			}
			ICMessage(IMI_ACTION_REMOVE, (int)&nfo.act);
			ICMessage(IMI_ACTION, (int)&nfo);
			delete[] nfo.txt;
		}

		//subclassujemy akcjê historii w menu kontaktu
		{
			nfo = sUIActionInfo(IMIG_CNT, IMIA_CNT_HISTORY);
			nfo.mask = UIAIM_ALL; 
			nfo.txt = new char[100];
			nfo.txtSize = 99;
			UIActionGet(nfo); 
			kLock::cnt_history_owner = ICMessage(IMI_ACTION_GETOWNER, (int)&nfo.act);
			if(!kLock::cnt_history_owner)
			{
				kLock::cnt_history_owner = ICMessage(IMC_PLUG_ID, 0);
			}
			ICMessage(IMI_ACTION_REMOVE, (int)&nfo.act);
			ICMessage(IMI_ACTION, (int)&nfo);
			delete[] nfo.txt;
		}

		//subclassujemy akcjê historii w oknie kontaktu
		{
			nfo = sUIActionInfo(IMIG_MSGTB, IMIA_MSG_HISTORY);
			nfo.mask = UIAIM_ALL;
			nfo.txt = new char[100];
			nfo.txtSize = 99;
			UIActionGet(nfo);
			kLock::msgwnd_history_owner = ICMessage(IMI_ACTION_GETOWNER, (int)&nfo.act);
			if(!kLock::msgwnd_history_owner)
			{
				kLock::msgwnd_history_owner = ICMessage(IMC_PLUG_ID, 0);
			}
			ICMessage(IMI_ACTION_REMOVE, (int)&nfo.act);
			ICMessage(IMI_ACTION, (int)&nfo);
			delete[] nfo.txt;
		}

		//subclassujemy g³ówne okno programu - nie pozwolimy mu siê póŸniej pokazaæ
		old_mainwnd_proc = (WNDPROC)SetWindowLongPtr((HWND)UIGroupHandle(sUIAction(0, IMIG_MAINWND)), GWLP_WNDPROC, (LONG_PTR)MainWindowProc);

		//sprawdzamy, czy has³o nie jest puste
		if((std::string)GETSTRA(kLock::config::PASSWORD) == "")
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
				if((std::string)sde.pass != "")
				{
					break;
				}
				ICMessage(IMI_ERROR,(int)"Has³o nie mo¿e byæ puste!");
			}
			SETSTR(kLock::config::PASSWORD, sde.pass);
			ICMessage(IMI_INFORM, (int)"Has³o zmienione.");
		}

		//jeœli poprzednim razem by³o zablokowane blokujemy
		if(GETINT(kLock::config::STATE))
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
			Ctrl->SetColumn(DTCFG, kLock::config::STATE, DT_CT_INT|DT_CF_SECRET, 0, "kLock::/LockState");
			Ctrl->SetColumn(DTCFG, kLock::config::PASSWORD, DT_CT_STR|DT_CF_SECRET, "", "kLock::/LockPassword");
			Ctrl->SetColumn(DTCFG, kLock::config::SYNCHRONIZEWITHAWAY, DT_CT_INT, 0, "kLock::/SynchronizeWithAutoAway");
			Ctrl->SetColumn(DTCFG, kLock::config::LOCKLISTANDTOOLBARS, DT_CT_INT, 1, "kLock::/LockListAndToolbars");
			Ctrl->SetColumn(DTCFG, kLock::config::LOCKTALKWINDOWS, DT_CT_INT, 1, "kLock::/LockTalkWindows");
			Ctrl->SetColumn(DTCFG, kLock::config::LOCKSOUND, DT_CT_INT, 0, "kLock::/LockSound");
			Ctrl->SetColumn(DTCFG, kLock::config::LOCKTRAY, DT_CT_INT, 0, "kLock::/LockTray");
			Ctrl->SetColumn(DTCFG, kLock::config::LOCKPROCESS, DT_CT_INT, 0, "kLock::/LockProcess");
			Ctrl->SetColumn(DTCFG, kLock::config::LOCKKNOTIFY, DT_CT_INT, 0, "kLock::/LockKNotify");
			Ctrl->SetColumn(DTCFG, kLock::config::ASKFORPASSWORDONHISTORY, DT_CT_INT, 0, "kLock::/AskForPasswordOnHistoryAccess");
			Ctrl->SetColumn(DTCFG, kLock::config::SYNCHRONIZEWITHKAWAY, DT_CT_INT, 0, "kLock::/SynchronizeWithkAway2");
		}
		return 1;
	}

	int IPrepare()
	{
		IMLOG("[IPrepare]:");

		//przycisk
		if(GETINT(kLock::config::BUTTONPLACE) == 0)
		{
			UIActionAdd(Ctrl->IMessage(IMI_GETPLUGINSGROUP, 0, 0), kLock::acts::LOCK, ACTR_INIT, "kLock", 35);
		}
		else if(GETINT(kLock::config::BUTTONPLACE) == 1)
		{
			UIActionAdd(IMIG_MAINTB, kLock::acts::LOCK, ACTR_INIT, "kLock", 35);
		}
		else if(GETINT(kLock::config::BUTTONPLACE) == 2)
		{
			UIActionInsert(IMIG_TRAY, kLock::acts::LOCK, 3, ACTR_INIT, "kLock", 35);
		}

		//konfiguracja
		UIGroupAdd(IMIG_CFG_PLUGS, kLock::config::GROUP, 0, "kLock", 35);
		{
			UIActionCfgAddPluginInfoBox2(kLock::config::GROUP, "Wtyczka ma za zadanie blokowaæ Konnekta, aby nikt niepowo³any nie dosta³ siê do ¿adnych przechowywanych w nim informacji.", "Podstawa: <b>Kamil \"Olórin\" Figiela</b><br />Dalszy rozwój: <b>Micha³ \"Dulek\" Dulko</b><br />Skompilowano: <b>"__TIME__"@"__DATE__"</b>");
			UIActionCfgAdd(kLock::config::GROUP, 0, ACTT_GROUP, "Wybierz co blokowaæ");
			{
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Blokuj okno g³ówne programu", kLock::config::LOCKLISTANDTOOLBARS);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Blokuj okienka rozmowy i historiê", kLock::config::LOCKTALKWINDOWS);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Wyciszaj dŸwiêki", kLock::config::LOCKSOUND);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Ukrywaj ikonkê w tray'u", kLock::config::LOCKTRAY);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Blokuj powiadomienia kNotify", kLock::config::LOCKKNOTIFY);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Ukrywaj Konnekta na liœcie procesów" AP_TIP "U¿ywaæ na w³asn¹ odpowiedzialnoœæ, dzia³a jedynie w Windows 9x", kLock::config::LOCKPROCESS);
			}
			UIActionCfgAdd(kLock::config::GROUP, 0, ACTT_GROUPEND);
			UIActionCfgAdd(kLock::config::GROUP, 0, ACTT_GROUP, "Opcje");
			{
				UIActionCfgAdd(kLock::config::GROUP, 0, ACTT_COMMENT|ACTSC_INLINE, "Has³o:");
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_PASSWORD|ACTS_DISABLED, 0, kLock::config::PASSWORD);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Synchronizuj z trybem auto-away", kLock::config::SYNCHRONIZEWITHAWAY);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Synchronizuj z kAway2" AP_TIP "Wymaga kAway2 w wersji 1.0.10.42", kLock::config::SYNCHRONIZEWITHKAWAY);
				UIActionCfgAdd(kLock::config::GROUP, IMIB_CFG, ACTT_CHECK|ACTS_DISABLED, "Pytaj o has³o przy ka¿dej próbie dostêpu do historii", kLock::config::ASKFORPASSWORDONHISTORY);
				UIActionCfgAdd(kLock::config::GROUP, 0, ACTT_SEP, "Po³o¿enie przycisku");
				UIActionCfgAdd(kLock::config::GROUP, kLock::config::BUTTONPLACE1, ACTT_RADIO|ACTSRADIO_BYPOS|ACTSC_NEEDRESTART|ACTS_DISABLED, "Na toolbarze wtyczek", kLock::config::BUTTONPLACE);
				UIActionCfgAdd(kLock::config::GROUP, kLock::config::BUTTONPLACE2, ACTT_RADIO|ACTSRADIO_BYPOS|ACTSC_NEEDRESTART|ACTS_DISABLED, "Na g³ównym toolbarze", kLock::config::BUTTONPLACE);
				UIActionCfgAdd(kLock::config::GROUP, kLock::config::BUTTONPLACE3, ACTT_RADIO|ACTSRADIO_BYPOS|ACTSRADIO_LAST|ACTSC_NEEDRESTART|ACTS_DISABLED, "W tray'u", kLock::config::BUTTONPLACE);
				UIActionCfgAdd(kLock::config::GROUP, kLock::config::ENABLEACTS, ACTT_BUTTON|ACTR_SHOW, "Aktywuj" AP_ICO "41");
			}
			UIActionCfgAdd(kLock::config::GROUP, 0, ACTT_GROUPEND);
		}

		acts_enabled = 0;
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
					if(GETINT(kLock::config::STATE))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::config::ASKFORPASSWORDONHISTORY))
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
					if(GETINT(kLock::config::STATE))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::config::ASKFORPASSWORDONHISTORY))
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
					if(GETINT(kLock::config::STATE))
					{
						Unlock();
						return 0;
					}
					else if(GETINT(kLock::config::ASKFORPASSWORDONHISTORY))
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
			case kLock::config::ENABLEACTS:
			{
				if(anBase->code == ACTN_ACTION)
				{
					if(!acts_enabled)
					{
						if(AskForPassword("kLock", "Zmiana ustawieñ jest zablokowana.\r\nPodaj has³o dostêpu:", (HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND))))
						{
							EnableActs();
						}
					}
					else if(acts_enabled)
					{
						DisableActs();
					}
				}
				else if(anBase->code == ACTN_HIDE)
				{
					DisableActs();
				}
				break;
			}
			case kLock::acts::LOCK:
			{
				if(anBase->code == ACTN_ACTION)
				{
					if(GETINT(kLock::config::STATE))
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
					UIActionSetText(anBase->act, GETINT(kLock::config::STATE)?"Odblokuj":"Zablokuj");
				}
				break;
			}
			case kLock::config::LOCKTRAY|IMIB_CFG:
			{
				ACTIONONLY(anBase);
				if(*UIActionCfgGetValue(anBase->act, 0, 0) != '0')
				{
					ICMessage(IMI_WARNING, (int)"Przed w³¹czeniem tej opcji ustaw odblokowuj¹cy skrót k.Lawy - inaczej nie odblokujesz Konnekta.");
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
		case IM_PLUG_NET: return kLock::NET;
		case IM_PLUG_TYPE: return IMT_UI|IMT_CONFIG;
		case IM_PLUG_VERSION: return 0;
		case IM_PLUG_SDKVERSION: return KONNEKT_SDK_V;
		case IM_PLUG_SIG: return (int)kLock::SIG;
		case IM_PLUG_NAME: return (int)kLock::NAME;
		case IM_PLUG_NETNAME: return 0;
		case IM_PLUG_INIT: Plug_Init(msg->p1, msg->p2); return 1;
		case IM_PLUG_DEINIT: Plug_Deinit(msg->p1, msg->p2); return 1;
		case kLock::api::UNLOCK:
		{
			return Unlock();
		}
		case kLock::api::ISUNLOCKED:
		{
			if(GETINT(kLock::config::STATE) == 1 && GETINT(kLock::config::LOCKKNOTIFY))
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		case kLock::api::LOCK:
		{
			Lock();
			return 0;
		}
		case IM_ALLPLUGINSINITIALIZED:
		{
			if(ICMessage(IMC_FINDPLUG_BYSIG, (int)"TABLETKA", 0) == 0)
			{
				int ID = ICMessage(IMC_FINDPLUG, kLock::NET, IMT_ALL);
				Ctrl->IMessage(&sIMessage_plugOut(ID, "Wtyczka kLock nie bêdzie dzia³aæ bez wtyczki tabletKa!", sIMessage_plugOut::erYes, sIMessage_plugOut::euNowAndOnNextStart));
				return -1;
			}
			return 0;
		}
		case IM_AWAY:
		{
			if(GETINT(kLock::config::SYNCHRONIZEWITHAWAY))
			{
				Lock();
			}
			return 1;
		}
		case kAway2::api::isAway:
		{
			if(GETINT(kLock::config::SYNCHRONIZEWITHKAWAY))
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