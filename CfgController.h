/**
  *  Configuration Controller
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision: 77 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-20 17:42:35 +0100 (Pn, 20 lis 2006) $
  *  @license      http://creativecommons.org/licenses/LGPL/2.1/
  */

#pragma once

#ifndef __CFGCTRL_H__
#define __CFGCTRL_H__

#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "IMController.h"
// #include "CfgActions.h"

using namespace Konnekt;
using namespace Stamina;
using namespace boost;

namespace Konnekt {
  class CfgController : public SharedObject<iSharedObject> {
  public:
    /* Class version */
	  STAMINA_OBJECT_CLASS_VERSION(CfgController, iSharedObject, Version(0,1,0,0));

  public:
    typedef std::vector<sIMessage_setColumn*> tCfgCols;

  public:
    inline CfgController(IMController* _IMCtrl) : IMCtrl(_IMCtrl) {
      // automagical registration of configuration columns (set via setColumn())
      IMCtrl->registerObserver(IM_SETCOLS, bind(resolve_cast0(&CfgController::_setColumns), this));
      // drawing configuration UI
      // IMCtrl->registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&CfgController::_drawUI), this));
    }

    inline virtual ~CfgController() { 
      for (tCfgCols::iterator it = this->_cols.begin(); it != this->_cols.end(); it++) {
        delete *it;
      }
    }

  public:
    inline void setColumn(tTable table, Tables::tColId id, int type, const char* def, const char* name) {
      this->_cols.push_back(new sIMessage_setColumn(table, id, type, def, name));
    }
    inline void setColumn(tTable table, Tables::tColId id, int type, int def, const char* name) {
      this->_cols.push_back(new sIMessage_setColumn(table, id, type, def, name));
    }

    inline void resetColumns(tTable table = Tables::tableNotFound) {
      if (!this->_cols.size()) return;

      bool resetCnt = table == Tables::tableContacts;
      bool resetCfg = table == Tables::tableConfig;

      if (table == Tables::tableNotFound) {
        resetCfg = resetCnt = true;
      }

      if (!resetCnt && !resetCfg) {
        return;
      }

      tCfgCols dtCnt;
      for (tCfgCols::iterator it = this->_cols.begin(); it != this->_cols.end(); it++) {
        if ((*it)->_table == Tables::tableConfig && resetCfg) {
          this->_resetColumn(*it);
        }
        if ((*it)->_table == Tables::tableContacts && resetCnt) {
          dtCnt.push_back(*it);
        }
      }

      if (dtCnt.size()) {
        int count = Ctrl->IMessage(IMC_CNT_COUNT);
        for (int i = 1; i < count; i++) {
          for (tCfgCols::iterator it = dtCnt.begin(); it != dtCnt.end(); it++) {
            this->_resetColumn(*it, i);
          }
        }
      }
    }

    inline void resetColumn(int id, int cnt = 0) {
      if (!this->_cols.size())  return;

      tTable table = !cnt ? Tables::tableConfig : Tables::tableContacts;
      for (tCfgCols::iterator it = this->_cols.begin(); it != this->_cols.end(); it++) {
        if ((*it)->_id == id && (*it)->_table == table) {
          this->_resetColumn(*it, cnt); break;
        }
      }
    }

    /*
    inline void addPage(const CfgPage& page) {
      this->_pages.push_back(page);
    }

    inline void operator += (CfgPage& page) {
      return addPage(page);
    }
    */

  protected:
    inline void _resetColumn(sIMessage_setColumn* it, int cnt = 0) {
      bool isCnt = it->_table == Tables::tableContacts && cnt;
      bool isConfig = it->_table == Tables::tableConfig;

      if (!isCnt && !isConfig) {
        return;
      }

      switch (it->_type) {
        case Tables::ctypeInt: {
          if (isConfig) {
            SETINT(it->_id, it->_def);
          }
          if (isCnt) {
            SETCNTI(cnt, it->_id, it->_def);
          }
          break;
        }
        case Tables::ctypeInt64: {
          if (isConfig) {
            // SETINT(it->_id, *it->_def_p64);
          }
          if (isCnt) {
            SETCNTI64(cnt, it->_id, *it->_def_p64);
          }
          break;
        }
        case Tables::ctypeString: {
          if (isConfig) {
            SETSTR(it->_id, it->_def_ch);
          }
          if (isCnt) {
            SETCNTC(cnt, it->_id, it->_def_ch);
          }
          break;
        }
      }
    }

    /*
    inline tIMCallback _drawUI() {
      for (tCfgPages::iterator it = this->_pages.begin(); it != this->_pages.end(); it++) {
        it->draw();
      }
    }
    */

    inline tIMCallback _setColumns() {
      for (tCfgCols::iterator it = this->_cols.begin(); it != this->_cols.end(); it++) {
        Ctrl->IMessage(*it);
      }
      return IMCtrl->setSuccess();
    }

  protected:
    IMController* IMCtrl;
    // tCfgPages _pages;
    tCfgCols _cols;
  };

  typedef SharedPtr<CfgController> oCfgCtrl;
}

#endif // __CFGCTRL_H__