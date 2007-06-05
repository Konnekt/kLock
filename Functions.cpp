/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#include "StdAfx.h"
#include "Includes.h"

namespace kLock
{
	//funkcja obs³uguj¹ca wiadomoœci g³ównego okna Konnekta
	LRESULT CALLBACK konnektWindowProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		static Controller* IMCtrl;
		if(!IMCtrl)
			IMCtrl = Controller::getInstance();

		if(GETINT(kLock::CFG::lockState) && GETINT(kLock::CFG::lockMainWindow))
		{
			switch(iMsg)
			{
				case WM_WINDOWPOSCHANGED:
				{
					IMLOG("[konnektWindowProc:] iMsg = WM_WINDOWPOSCHANGED");

					//chowamy okienko
					ShowWindow(hWnd, SW_HIDE);
					return 0;
				}
				case WM_USER+1086:
				{
					//sprawdzamy jaki to notify i decydujemy, czy dopuœciæ dalej, czy nie
					if(lParam == WM_LBUTTONUP || lParam == WM_RBUTTONUP)
					{
						IMLOG("[konnektWindowProc:] iMsg = WM_USER+1086, lParam = WM_LBUTTONUP|WM_RBUTTONUP");

						IMCtrl->lock(L_Unlock);
						return 0;
					}
					else if(lParam == WM_LBUTTONDOWN || lParam == WM_RBUTTONDOWN || lParam == WM_MOUSEMOVE || lParam == WM_LBUTTONDBLCLK)
					{
						IMLOG("[konnektWindowProc:] iMsg = WM_USER+1086, lParam = WM_LBUTTONDOWN|WM_RBUTTONDOWN|WM_MOUSEMOVE|WM_LBUTTONDBLCLK");
						return 0;
					}
					break;
				}
			}
		}
		return CallWindowProc(IMCtrl->konnektWindowOldProc, hWnd, iMsg, wParam, lParam);
	}
}