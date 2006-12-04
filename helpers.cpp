/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#include "StdAfx.h"
#include "Includes.h"

namespace kLock
{
	//funkcja wywo�uj�ca akcj�
	void callAction(sUIAction act)
	{
		sUIActionNotify_2params akcja =	sUIActionNotify_2params();											
		akcja.act = act;
		akcja.code = ACTN_ACTION;
		ICMessage(IMI_ACTION_CALL, (int)&akcja, 0);
	}

	//funkcja wywo�uj�ca akcj�
	void callAction(int parent, int id, int cnt)
	{
		callAction(sUIAction(parent, id, cnt));
	}

	int subclassAction(int group, int id, int mask)
	{
		sUIActionInfo nfo(group, id);
		int prevOwner;

		nfo.mask = mask;
		nfo.txt = new char[100];
		nfo.txtSize = 99;

		UIActionGet(nfo);
		if(!(prevOwner = Ctrl->ICMessage(IMI_ACTION_GETOWNER, (int)&nfo.act)))
		{
			prevOwner = Ctrl->ICMessage(IMC_PLUG_ID, 0);
		}

		Ctrl->ICMessage(IMI_ACTION_REMOVE, (int)&nfo.act);
		Ctrl->ICMessage(IMI_ACTION, (int)&nfo);
		delete[] nfo.txt;

		return prevOwner;
	}

	int pluginExists(int net, int type)
	{
		return Ctrl->ICMessage(IMC_FINDPLUG, net, type);
	}

	int pluginExists(std::string sig, int type)
	{
		return Ctrl->ICMessage(IMC_FINDPLUG_BYSIG, (int)sig.c_str(), type);
	}
}