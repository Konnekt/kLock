/**
  *  IMessage Controller class
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

#ifndef __IMESSAGECTRL_H__
#define __IMESSAGECTRL_H__

#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace Stamina;
using namespace boost;

namespace Konnekt {
  typedef void tIMCallback;

  template<typename TC, typename TR>
  TR (TC::* resolve_cast0(TR (TC::* pFunc)(void)))(void) {
    return pFunc;
  }

  template<typename TP, typename TC, typename TR>
  TR (TC::* resolve_cast1(TR (TC::* pFunc)(TP)))(TP) {
    return pFunc;
  }

  class IMController : public SharedObject<iSharedObject>, public signals::trackable {
  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(IMController, iSharedObject, Version(0,2,1,0));

  public:
    typedef function<tIMCallback(IMController*)> fOnIMessage;
    typedef signal<tIMCallback(IMController*)> sigOnIMessage;

  public:
    typedef std::map<String, signals::connection> tConnections;

    struct sObserver {
      tConnections connections;
      sigOnIMessage signal;
    };

    struct sSubclassedAction {
      static const int notFound = -1;

      bool autoForward;
      int prevOwner;

      int parent;
      int id;

      sSubclassedAction(): id(0), parent(0), prevOwner(notFound), autoForward(false) { }

      sSubclassedAction(int _id, int _parent, bool _autoForward = false):
        id(_id), parent(_parent), prevOwner(notFound), autoForward(_autoForward) { }

      sSubclassedAction(int _id, int _parent, bool _autoForward, int _prevOwner):
        id(_id), parent(_parent), prevOwner(_prevOwner), autoForward(_autoForward) { }
    };

    typedef std::deque<sSubclassedAction> tSubclassedActions;
    typedef std::map<int, sObserver*> tObservers;
    typedef std::map<int, int> tStaticValues;

  public:
    inline IMController(): returnCodeSet(false), returnCode(0) { 
      // setting/unsetting Ctrl global pointer
      registerObserver(IM_PLUG_INIT, bind(resolve_cast0(&IMController::_plugInit), this));
      registerObserver(IM_PLUG_DEINIT, bind(resolve_cast0(&IMController::_plugDeInit), this));
      // actions subclassing
      registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&IMController::_subclass), this));

      // default values for common messages, may be overriden
      setStaticValue(IM_PLUG_NETNAME, 0);
      setStaticValue(IM_PLUG_PRIORITY, 0);
      setStaticValue(IM_PLUG_SDKVERSION, KONNEKT_SDK_V);
      setStaticValue(IM_PLUG_VERSION, 0);
      setStaticValue(IM_PLUG_CORE_V, (int) "W98");
      setStaticValue(IM_PLUG_UI_V, 0);
    }

    inline virtual ~IMController() { 
      for (tObservers::iterator it = observers.begin(); it != observers.end(); it++) {
        delete it->second;
      }
      for (tObservers::iterator it = actionObservers.begin(); it != actionObservers.end(); it++) {
        delete it->second;
      }
    }

  public:
    /*
     * Process incoming IMessage
     * @see registerObserver
     */
    inline int process(sIMessage_base* msgBase) {
      sIMessage_2params* msg = static_cast<sIMessage_2params*>(msgBase);

      // clear residues
      clear();

      // set im
      setIM(msg);

      // looking for static values
      if (staticValues.find(msg->id) != staticValues.end()) {
        setReturnCode(staticValues[msg->id]);
      }
      // notify global observers
      notifyObservers(msg);

      // ui action message
      if (isAction()) {
        // notify action observers
        notifyActionObservers(msg);

        // auto-forward action
        if (isForwardable()) {
          forwardAction();
        }
      }

      // log IMessage
      if (isReturnCodeSet()) {
        IMLOG("[IMController<%i>::process()]: id = %i, returnCode = %i", this, msg->id, 
          getReturnCode());
      } else {
        // set error if no result
        if (Ctrl) {
          Ctrl->setError(IMERROR_NORESULT);
        }
        IMLOG("[IMController<%i>::process()]: id = %i", this, msg->id);
      }
      return getReturnCode();
    }

    /*
     * Attach observer to specific IMessage
     * @param id IMessage id
     * @param f function which should be fired on @a id IMessage
     * @param priority callback priority (group)
     * @param pos callback position in group (signals::at_back, signals::at_front)
     * @param name callback name
     * @param overwrite overrides callback with the same @a name if any exists
     * @return logical true if observer was attached
     */
    inline bool registerObserver(tIMid id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return _registerObserver(id, f, priority, pos, name, overwrite, observers);
    }

    inline bool registerActionObserver(const sSubclassedAction& an, fOnIMessage f, int priority = 0, 
      signals::connect_position pos = signals::at_back, const StringRef& name = "", bool overwrite = true) 
    {
      if (_registerObserver(an.id, f, priority, pos, name, overwrite, actionObservers)) {
        subclassAction(an);
        return true;
      }
      return false;
    }

    inline bool registerActionObserver(int id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return _registerObserver(id, f, priority, pos, name, overwrite, actionObservers);
    }

    inline void notifyObservers(sIMessage_2params* msg) {
      return _notifyObservers(im->id, observers);
    }

    inline void notifyActionObservers(sIMessage_2params* msg) {
      return _notifyObservers(getAN()->act.id, actionObservers);
    }

    /* Subclassing */
    inline bool isSublassed(int id, int parent) {
      for (tSubclassedActions::iterator it = subclassedActions.begin(); it != subclassedActions.end(); it++) {
        if (it->id == id && it->parent == parent) return true;
      }
      return false;
    }

    inline bool isSublassed() {
      if (!isAction()) return false;
      return isSublassed(getAN()->act.id, getAN()->act.parent);
    }

    inline bool isForwardable(int id, int parent) {
      if (isSublassed(id, parent)) {
        return _getSubclassedAction(id, parent).autoForward;
      }
      return false;
    }

    inline bool isForwardable() {
      if (!isAction()) return false;
      return isForwardable(getAN()->act.id, getAN()->act.parent);
    }

    inline int getPrevOwner(int id, int parent) {
      if (isSublassed(id, parent)) {
        return _getSubclassedAction(id, parent).prevOwner;
      }
      return sSubclassedAction::notFound;
    }

    inline int getPrevOwner() {
      if (!isAction()) return sSubclassedAction::notFound;
      return getPrevOwner(getAN()->act.id, getAN()->act.parent);
    }

    inline void setForwardable(int id, int parent, bool set) {
      if (isSublassed(id, parent)) {
        _getSubclassedAction(id, parent).autoForward = set;
      }
    }

    inline void setForwardable(bool set) {
      if (!isAction()) return;
      return setForwardable(getAN()->act.id, getAN()->act.parent, set);
    }

    inline void subclassAction(const sSubclassedAction& an) {
      if (!isSublassed(an.id, an.parent)) {
        subclassedActions.push_back(an);
      }
    }

    inline void subclassAction(int id, int parent, bool autoForward = false) {
      return subclassAction(sSubclassedAction(id, parent, autoForward));
    }

    inline IMController* forwardAction(bool _setReturnCode = true) {
      if (isAction() && isSublassed()) {
        int retValue = Ctrl->IMessageDirect(IM_UIACTION, getPrevOwner(), (int) getAN());
        if (_setReturnCode) setReturnCode(retValue);
      }
      return this;
    }

    // Cleanin' variables
    inline void clear() {
      returnCodeSet = false;
      returnCode = NULL;
      im = NULL;
    }

    inline int getReturnCode() {
      return returnCode;
    }

    inline void setReturnCode(int code) {
      returnCodeSet = true;
      returnCode = code;
    }

    /*
     * Set string as return value
     * @warning { this method uses internal buffer which can be overridden by
     * commonly used functions like @c GETSTR in all flavors. }
     */
    inline void setReturnValue(const StringRef& value) {
      // creating temp buffer
      char* buff = (char*) Ctrl->GetTempBuffer(value.size() + 1);

      // cleaning memory
      memset(buff, 0, value.size() + 1);

      // copying data
      memcpy(buff, value.a_str(), value.size());

      // setting return code to pointer to internal, temp buffer
      setReturnCode((int) buff);
    }

    inline void setSuccess() {
      return setReturnCode(1);
    }

    inline void setFailure() {
      return setReturnCode(-1);
    }

    inline bool isReturnCodeSet() {
      return returnCodeSet;
    }

    inline sIMessage_2params* getIM() {
      return im;
    }

    inline IMController* setIM(sIMessage_2params* im) { 
      this->im = im;
      return this;
    }

    inline bool isAction() {
      return getIM()->id == IM_UIACTION;
    }

    inline sUIActionNotify_2params* getAN() {
      if (!isAction()) return NULL;
      return static_cast<sUIActionNotify_2params*>((sUIActionNotify_base*) getIM()->p1);
    }

    inline void setStaticValue(int id, int value) {
      staticValues[id] = value;
    }

    inline bool isObserved(int id) {
      return _isObserved(id, observers);
    }

    inline bool isActionObserved(int id) {
      return _isObserved(id, actionObservers);
    }

  protected:
    /* Actions subclassing */
    inline sSubclassedAction& _getSubclassedAction(int id, int parent) {
      for (tSubclassedActions::iterator it = subclassedActions.begin(); it != subclassedActions.end(); it++) {
        if (id == it->id && parent == it->parent) return *it;
      }
      return sSubclassedAction();
    }

    /* Observers related methods */
    inline bool _isObserved(int id, tObservers& list) {
      if (list.find(id) != list.end()) {
        return !list[id]->signal.empty();
      }
      return false;
    }

    /* inline void _dbgObservers() {
      for (tObservers::iterator it = observers.begin(); it != observers.end(); it++) {
        for (tConnections::iterator it2 = it->second->connections.begin(); it2 != it->second->connections.end(); it2++) {
          IMLOG("Observer[%i].connection: %s", it->first, it2->first.c_str());
        }
      }
      for (tObservers::iterator it = actionObservers.begin(); it != actionObservers.end(); it++) {
        for (tConnections::iterator it2 = it->second->connections.begin(); it2 != it->second->connections.end(); it2++) {
          IMLOG("ActionObserver[%i].connection: %s", it->first, it2->first.c_str());
        }
      }
    } */

    inline void _notifyObservers(int id, tObservers& list) {
      if (!_isObserved(id, list)) {
        return;
      }
      list[id]->signal(this);
    }

    inline bool _registerObserver(int id, fOnIMessage f, int priority, signals::connect_position pos, 
      StringRef name, bool overwrite, tObservers& list) 
    {
      if (f.empty()) {
        return false;
      }
      if (list.find(id) == list.end()) {
        list[id] = new sObserver;
      }
      if (!name.length()) {
        name = "unnamed." + inttostr(list[id]->connections.size());
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

    /* Callbacks */
    inline tIMCallback _plugInit() {
      Plug_Init(getIM()->p1, getIM()->p2);
      return setSuccess();
    }

    inline tIMCallback _plugDeInit() {
      Plug_Deinit(getIM()->p1, getIM()->p2);
      return setSuccess();
    }

    inline tIMCallback _subclass() {
      for (tSubclassedActions::iterator it = subclassedActions.begin(); it != subclassedActions.end(); it++) {
        sUIActionInfo nfo(it->parent, it->id);
        nfo.mask = UIAIM_ALL;
        nfo.txt = new char[100];
        nfo.txtSize = 99;

        UIActionGet(nfo);
        if (!(it->prevOwner = Ctrl->ICMessage(IMI_ACTION_GETOWNER, (int)&nfo.act))) {
          it->prevOwner = Ctrl->ICMessage(IMC_PLUG_ID, 0);
        }

        IMLOG("[IMController<%i>::subclass()]: name = %s, id = %i, parent = %i, prevOwner = %i", this, nfo.txt, 
          it->id, it->parent, it->prevOwner);

        Ctrl->ICMessage(IMI_ACTION_REMOVE, (int)&nfo.act);
        Ctrl->ICMessage(IMI_ACTION, (int)&nfo);
        delete [] nfo.txt;
      }
    }

  protected:
    tSubclassedActions subclassedActions;
    tStaticValues staticValues;
    tObservers actionObservers;
    tObservers observers;

    sIMessage_2params* im;
    bool returnCodeSet;
    int returnCode;
  };
}

#endif // __IMESSAGECTRL_H__