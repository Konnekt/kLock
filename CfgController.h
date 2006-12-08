/**
  *  Configuration Controller
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision: 98 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-08 18:27:59 +0100 (Pt, 08 gru 2006) $
  *  @license      http://creativecommons.org/licenses/LGPL/2.1/
  */

#pragma once

#ifndef __CFGCTRL_H__
#define __CFGCTRL_H__

#include "IMController.h"

using namespace Konnekt::Tables;
using namespace Stamina;
using namespace boost;

namespace Konnekt {
  class CfgController : public SharedObject<iSharedObject>, public signals::trackable {
  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(CfgController, iSharedObject, Version(0,1,1,0));

  public:
    typedef std::vector<sIMessage_setColumn*> tCfgCols;

  public:
    inline CfgController(IMController* IMCtrl) {
      this->attachObservers(IMCtrl);
    }
    inline CfgController() { }

    inline virtual ~CfgController() { 
      for (tCfgCols::iterator it = _cols.begin(); it != _cols.end(); it++) {
        delete *it;
      }
    }

  public:
    /* automagical registration of configuration columns (set via setColumn()) */
    inline void attachObservers(IMController* IMCtrl) {
      IMCtrl->registerObserver(IM_SETCOLS, bind(&CfgController::_setColumns, this, _1));
    }

    inline void setColumn(tTable table, tColId id, int type, const char* def, const char* name) {
      _cols.push_back(new sIMessage_setColumn(table, id, type, def, name));
    }
    inline void setColumn(tTable table, tColId id, int type, int def, const char* name) {
      _cols.push_back(new sIMessage_setColumn(table, id, type, def, name));
    }

    inline void resetColumns(tTable table = tableNotFound) {
      if (!_cols.size()) return;

      bool resetCnt = table == tableContacts;
      bool resetCfg = table == tableConfig;

      if (table == tableNotFound) {
        resetCfg = resetCnt = true;
      }

      if (!resetCnt && !resetCfg) {
        return;
      }

      tCfgCols dtCnt;
      for (tCfgCols::iterator it = _cols.begin(); it != _cols.end(); it++) {
        if ((*it)->_table == tableConfig && resetCfg) {
          _resetColumn(*it);
        }
        if ((*it)->_table == tableContacts && resetCnt) {
          dtCnt.push_back(*it);
        }
      }

      if (dtCnt.size()) {
        int count = Ctrl->IMessage(IMC_CNT_COUNT);
        for (int i = 1; i < count; i++) {
          for (tCfgCols::iterator it = dtCnt.begin(); it != dtCnt.end(); it++) {
            _resetColumn(*it, i);
          }
        }
      }
    }

    inline void resetColumn(int id, tCntId cnt = 0) {
      if (!_cols.size()) return;

      tTable table = !cnt ? tableConfig : tableContacts;
      for (tCfgCols::iterator it = _cols.begin(); it != _cols.end(); it++) {
        if ((*it)->_id == id && (*it)->_table == table) {
          _resetColumn(*it, cnt); break;
        }
      }
    }

    /*
     * @todo find some better way to handle it
     */
    inline int getInheritedIValue(int col, tCntId cnt) {
      return GETCNTI(cnt, col) >= 0 ? GETCNTI(cnt, col) : GETINT(col);
    }

    inline bool getInheritedBValue(int col, tCntId cnt) {
      return (GETINT(col) && (GETCNTI(cnt, col) < 2)) || (!GETINT(col) && (GETCNTI(cnt, col) == 1));
    }

    inline const char* getInheritedCValue(int col, tCntId cnt) {
      return strlen(GETCNTC(cnt, col)) ? GETCNTC(cnt, col) : GETSTRA(col);
    }

  protected:
    inline void _resetColumn(sIMessage_setColumn* it, tCntId cnt = 0) {
      bool isCnt = it->_table == tableContacts && cnt;
      bool isConfig = it->_table == tableConfig;

      if (!isCnt && !isConfig) {
        return;
      }

      switch (it->_type) {
        case ctypeInt: {
          if (isConfig) {
            SETINT(it->_id, it->_def);
          }
          if (isCnt) {
            SETCNTI(cnt, it->_id, it->_def);
          }
          break;
        }
        case ctypeInt64: {
          if (isConfig) {
            // SETINT(it->_id, *it->_def_p64);
          }
          if (isCnt) {
            SETCNTI64(cnt, it->_id, *it->_def_p64);
          }
          break;
        }
        case ctypeString: {
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

    inline tIMCallback _setColumns(IMController* IMCtrl) {
      for (tCfgCols::iterator it = _cols.begin(); it != _cols.end(); it++) {
        Ctrl->IMessage(*it);
      }
      IMCtrl->setSuccess();
    }

  protected:
    tCfgCols _cols;
  };

  typedef SharedPtr<CfgController> oCfgCtrl;
}

#endif // __CFGCTRL_H__