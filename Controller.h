/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#pragma once
#include "IMController.h"

namespace kLock
{
	class Controller : public IMController
	{
		public:
			/* Class version */
			STAMINA_OBJECT_CLASS_VERSION(Controller, IMController, Version(1,2,0,0));

		public:
			inline static Controller* getInstance()
			{
				if(!instance.isValid())
					instance = new Controller;
				return instance;
			}
			bool lock(kLock::Lock lock = L_Detect, bool force = 0);
			bool disableActs(kLock::Lock lock = L_Detect, bool force = 0);
			int askForPassword(std::string title, std::string text, std::string text2, HWND parent = 0);

		public:
			int historyOwner; //poprzedni w�a�ciciel akcji histori
			int cntHistoryOwner; //poprzedni w�a�ciciel akcji histori w menu kontaktu 
			int msgWndHistoryOwner; //poprzedni w�a�ciciel akcji histori w oknie kontaktu
			int kIEviewOwner; //poprzedni w�a�ciciel kontrolki kIEview
			WNDPROC konnektWindowOldProc; //stary proc g��wnego okna

		protected:
			Controller();
			void onPrepare();
			void onStart();
			void onEnd();
			void onAction();
			void onAway();
			void onExtendedAway();
			void onkAway();

			void apiLock();
			void apiUnlock();
			void apiIsUnlocked();
			void apiIsUnlockedForKNotify();
			void apiIsUnlockedForkMigacz();

		protected:
			static SharedPtr<Controller> instance;
	};
}