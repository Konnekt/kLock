#include "includes.h"

namespace kLock
{
	//funkcja wywo�uj�ca akcj�
	void CallAction(sUIAction act)
	{
		sUIActionNotify_2params akcja =	sUIActionNotify_2params();											
		akcja.act = act;
		akcja.code = ACTN_ACTION;
		ICMessage(IMI_ACTION_CALL,	(int)&akcja, 0);
	}

	//funkcja wywo�uj�ca akcj�
	void CallAction(int parent, int id, int cnt)
	{
		CallAction(sUIAction(parent, id, cnt));
	}
}