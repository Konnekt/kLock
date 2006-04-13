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
}