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

	//funkcja sprawdzaj�ca, czy plugin o danym necie istnieje
	int pluginExists(int net, int type)
	{
		return Ctrl->ICMessage(IMC_FINDPLUG, net, type);
	}

	//funkcja sprawdzaj�ca, czy plugin o danym sigu istnieje
	int pluginExists(std::string sig, int type)
	{
		return Ctrl->ICMessage(IMC_FINDPLUG_BYSIG, (int)sig.c_str(), type);
	}
}