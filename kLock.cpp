/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#include "StdAfx.h"
#include "Includes.h"

int __stdcall IMessageProc(sIMessage_base* msgBase)
{
	return kLock::Controller::getInstance()->process(msgBase);
}