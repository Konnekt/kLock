#pragma once

#ifndef __IMESSAGECTRL_H__
#define __IMESSAGECTRL_H__

using namespace Stamina;
using namespace boost;

namespace Konnekt {
  template<typename TC, typename TR>
  TR (TC::* resolve_cast0(TR (TC::* pFunc)(void)))(void) {
    return pFunc;
  }

  template<typename TP, typename TC, typename TR>
  TR (TC::* resolve_cast1(TR (TC::* pFunc)(TP)))(TP) {
    return pFunc;
  }

  class IMController : public SharedObject<iSharedObject>, signals::trackable {
  public:
    /* Class version */
	  STAMINA_OBJECT_CLASS_VERSION(IMController, iSharedObject, Version(0,1,0,0));

  public:
    typedef void tIMCallback;

  public:
    typedef function<tIMCallback(IMController*)> fOnIMessage;
    typedef signal<tIMCallback(IMController*)> sigOnIMessage;

  public:
    typedef std::map<String, signals::connection> tConnections;
    typedef std::vector<sIMessage_setColumn*> tCfgCols;

    struct sObserver {
      tConnections connections;
      sigOnIMessage signal;
    };

    typedef std::map<int, sObserver*> tObservers;
    typedef std::map<int, int> tStaticValues;

  protected:
    inline IMController() : returnCodeSet(false), returnCode(0) { 
      // automagical registration of configuration columns (set via setColumn())
      this->registerObserver(IM_SETCOLS, bind(resolve_cast0(&IMController::_setColumns), this));
      // setting/unsetting Ctrl global pointer
      this->registerObserver(IM_PLUG_INIT, bind(resolve_cast0(&IMController::_plugInit), this));
      this->registerObserver(IM_PLUG_DEINIT, bind(resolve_cast0(&IMController::_plugDeInit), this));

      // default values for common messages, may be overriden
      this->addStaticValue(IM_PLUG_NETNAME, 0);
      this->addStaticValue(IM_PLUG_PRIORITY, 0);
      this->addStaticValue(IM_PLUG_SDKVERSION, KONNEKT_SDK_V);
      this->addStaticValue(IM_PLUG_VERSION, 0);
      this->addStaticValue(IM_PLUG_CORE_V, (int) "W98");
      this->addStaticValue(IM_PLUG_UI_V, 0);
    }

    inline IMController(IMController const&) { }
    inline IMController& operator=(IMController const&) { 
      return *this; 
    }

    inline virtual ~IMController() { 
      for (tObservers::iterator it = this->observers.begin(); it != this->observers.end(); it++) {
        delete it->second;
      }
      for (tObservers::iterator it = this->actionObservers.begin(); it != this->actionObservers.end(); it++) {
        delete it->second;
      }
      for (tCfgCols::iterator it = this->cfgCols.begin(); it != this->cfgCols.end(); it++) {
        delete *it;
      }
    }

  public:
    /*
     * Process incoming IMessage
     * \see registerObserver
     */
    inline int process(sIMessage_base* msgBase) {
      sIMessage_2params* msg = static_cast<sIMessage_2params*>(msgBase);

      // log IMessage and clear residues
      this->clear();

      // first, we're looking for static values
      if (this->staticValues.find(msg->id) != this->staticValues.end()) {
        this->setReturnCode(this->staticValues[msg->id]);
      }
      // then, we notify global observers
      this->notifyObservers(msg);

      // in the end we're notifying action observers if it's action message
      if (msg->id == IM_UIACTION) {
        this->notifyActionObservers(msg);
      }

      if (this->isReturnCodeSet()) {
        IMLOG("[IMController<%i>::process()]: id = %i, returnCode = %i", this, msg->id, 
          this->getReturnCode());
      } else {
        if (Ctrl) {
          Ctrl->setError(IMERROR_NORESULT);
        }
        IMLOG("[IMController<%i>::process()]: id = %i", this, msg->id);
      }
      return this->getReturnCode();
    }

    /*
     * Attach observer to specific IMessage
     * \param id IMessage id
     * \param f function which should be fired on \a id IMessage
     * \param priority callback priority (group)
     * \param pos callback position in group (signals::at_back, signals::at_front)
     * \param name callback name
     * \param overwrite overrides callback with the same \a name if any exists
     * \return logical true if observer was attached
     */
    inline bool registerObserver(int id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return this->_registerObserver(id, f, priority, pos, name, overwrite, this->observers);
    }

    inline bool registerActionObserver(int id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return this->_registerObserver(id, f, priority, pos, name, overwrite, this->actionObservers);
    }

    inline void notifyObservers(sIMessage_2params* msg) {
      if (!this->isObserved(msg->id)) return;

      this->setIM(msg);
      this->observers[msg->id]->signal(this);
    }

    inline void notifyActionObservers(sIMessage_2params* msg) {
      int id = this->setIM(msg)->getAN()->act.id;

      if (!this->isActionObserved(id)) return;
      this->actionObservers[id]->signal(this);
    }

    // Cleanin' variables
    inline void clear() {
      this->returnCodeSet = false;
      this->returnCode = NULL;
      this->im = NULL;
    }

    inline int getReturnCode() {
      return this->returnCode;
    }

    inline void setReturnCode(int code) {
      this->returnCodeSet = true;
      this->returnCode = code;
    }

    /*
     * Set string as return value
     * \warning { this method uses internal buffer which can be overridden by
     * commonly used functions like \c GETSTR in all flavors. }
     */
    inline void setReturnValue(const StringRef& value) {
      // tworzymy tymczasowy bufor
      char* buff = (char*) Ctrl->GetTempBuffer(value.size() + 1);

      // czyszczenie pamiêci
      memset(buff, 0, value.size() + 1);

      // kopiujemy dane
      memcpy(buff, value.a_str(), value.size());

      this->setReturnCode((int) buff);
    }

    inline void setSuccess() {
      return this->setReturnCode(1);
    }

    inline void setFailure() {
      return this->setReturnCode(-1);
    }

    inline bool isReturnCodeSet() {
      return this->returnCodeSet;
    }

    inline sIMessage_2params* getIM() {
      return this->im;
    }

    inline sUIActionNotify_2params* getAN() {
      return static_cast<sUIActionNotify_2params*>((sUIActionNotify_base*) this->getIM()->p1);
    }

    inline IMController* setIM(sIMessage_2params* im) { 
      this->im = im;

      return this;
    }

    inline void addStaticValue(int id, int value) {
      this->staticValues[id] = value;
    }

    inline bool isObserved(int id) {
      if (this->observers.find(id) != this->observers.end()) {
        return !this->observers[id]->signal.empty();
      }
      return false;
    }

    inline bool isActionObserved(int id) {
      if (this->actionObservers.find(id) != this->actionObservers.end()) {
        return !this->actionObservers[id]->signal.empty();
      }
      return false;
    }

    inline void setColumn(tTable table, int id, int type, const char* def, const char* name) {
      this->cfgCols.push_back(new sIMessage_setColumn(table, id, type, def, name));
    }
    inline void setColumn(tTable table, int id, int type, int def, const char* name) {
      this->cfgCols.push_back(new sIMessage_setColumn(table, id, type, def, name));
    }

    inline void resetColumns(tTable table = Tables::tableNotFound) {
      if (!this->cfgCols.size()) return;

      bool resetCnt = table == Tables::tableContacts;
      bool resetCfg = table == Tables::tableConfig;

      if (table == Tables::tableNotFound) {
        resetCfg = resetCnt = true;
      }

      if (!resetCnt && !resetCfg) {
        return;
      }

      tCfgCols dtCnt;
      for (tCfgCols::iterator it = this->cfgCols.begin(); it != this->cfgCols.end(); it++) {
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
      if (!this->cfgCols.size()) return;

      tTable table = !cnt ? Tables::tableConfig : Tables::tableContacts;
      for (tCfgCols::iterator it = this->cfgCols.begin(); it != this->cfgCols.end(); it++) {
        if ((*it)->_id == id && (*it)->_table == table) {
          this->_resetColumn(*it, cnt); break;
        }
      }
    }


  protected:
    /* inline void dbgObservers() {
      for (tObservers::iterator it = this->observers.begin(); it != this->observers.end(); it++) {
        for (tConnections::iterator it2 = it->second->connections.begin(); it2 != it->second->connections.end(); it2++) {
          IMLOG("Observer[%i].connection: %s", it->first, it2->first.c_str());
        }
      }
    } */

    inline bool _registerObserver(
      int id, fOnIMessage f, int priority, signals::connect_position pos, 
      StringRef name, bool overwrite, tObservers& list) 
    {
      if (f.empty()) {
        return false;
      }
      if (list.find(id) == list.end()) {
        list[id] = new sObserver;
      }
      if (!name.length()) {
        name = "unnamed." + list[id]->connections.size();
      }
      if (list[id]->connections.find(name) != list[id]->connections.end()) {
        if (overwrite) {
          list[id]->connections[name].disconnect();
        } else {
          return false;
        }
      }
      return (list[id]->connections[name] = list[id]->signal.connect(priority, f, pos)).connected();
    }

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

    tIMCallback inline _setColumns() {
      for (tCfgCols::iterator it = this->cfgCols.begin(); it != this->cfgCols.end(); it++) {
        Ctrl->IMessage(*it);
      }
      return this->setSuccess();
    }

    tIMCallback inline _plugInit() {
      Plug_Init(this->getIM()->p1, this->getIM()->p2);
      return this->setSuccess();
    }

    tIMCallback inline _plugDeInit() {
      Plug_Deinit(this->getIM()->p1, this->getIM()->p2);
      return this->setSuccess();
    }

  protected:
    tStaticValues staticValues;
    tObservers actionObservers;
    tObservers observers;
    tCfgCols cfgCols;

    sIMessage_2params* im;
    bool returnCodeSet;
    int returnCode;
  };
}

#endif // __IMESSAGECTRL_H__