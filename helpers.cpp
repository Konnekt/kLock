/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#include "includes.h"

namespace kLock
{
	//funkcja wywo³uj¹ca akcjê
	void CallAction(sUIAction act)
	{
		sUIActionNotify_2params akcja =	sUIActionNotify_2params();											
		akcja.act = act;
		akcja.code = ACTN_ACTION;
		ICMessage(IMI_ACTION_CALL,	(int)&akcja, 0);
	}

	//funkcja wywo³uj¹ca akcjê
	void CallAction(int parent, int id, int cnt)
	{
		CallAction(sUIAction(parent, id, cnt));
	}

	int SubclassAction(int group, int id, int mask)
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

	int PluginExists(int net, int type)
	{
		return(Ctrl->ICMessage(IMC_FINDPLUG, net, type));
	}
}