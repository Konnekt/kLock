/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

#pragma once

//definicje
#define _WIN32_WINNT 0x0500

//standardowe
#include <string>
#include <list>
#include <deque>

//windowsowe
#include <windows.h>
#include <commctrl.h>

//konnektowe
#include <konnekt/plug_export.h>
#include <konnekt/ui.h>
#include <konnekt/plug_func.h>
#include <konnekt/ui_message_controls.h>
#include <konnekt/tabletKa.h>
#include <konnekt/kSound.h>
#include <konnekt/lib.h>
#include <konnekt/kAway2.h>
#include <konnekt/KNotify.h>
//#include <konnekt/kMigacz.h>
#include <konnekt/plugsNET.h>

//Stamina.lib
#ifdef _DEBUG
  #pragma comment(lib, "stamina_d.lib")
#else
  #pragma comment(lib, "stamina.lib")
#endif
#include <stamina/object.h>
#include <stamina/objectimpl.h>
#include <stamina/helpers.h>

//boost
#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>