/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#include "StdAfx.h"
#include "Includes.h"

namespace kLock
{
	//funkcja obs³uguj¹ca g³ówne okno, blokuj¹c je
	LRESULT CALLBACK konnektWindowProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		static Controller* IMCtrl;
		if(!IMCtrl)
			IMCtrl = Controller::getInstance();

		if(GETINT(kLock::CFG::lockState) && GETINT(kLock::CFG::lockMainWindow))
		{
			switch(iMsg)
			{
				case WM_WINDOWPOSCHANGING:
				case WM_WINDOWPOSCHANGED:
				case WM_SHOWWINDOW:
				{
					ShowWindow(hWnd, 0);
					return 0;
				}
				case WM_USER+1086:
				{
					if(lParam == 514 || lParam == 517)
					{
						IMCtrl->lock(L_Unlock);
						return 0;
					}
					else if(lParam == 513 || lParam == 516 || lParam == 512)
					{
						return 0;
					}
					break;
				}
			}
		}
		return CallWindowProc(IMCtrl->konnektWindowOldProc, hWnd, iMsg, wParam, lParam);
	}
}