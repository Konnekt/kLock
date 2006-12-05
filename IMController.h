/**
  *  IMessage Controller class
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision: 85 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-04 23:09:33 +0100 (Pn, 04 gru 2006) $
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

  class IMController : public SharedObject<iSharedObject>, signals::trackable {
  public:
    /* Class version */
	  STAMINA_OBJECT_CLASS_VERSION(IMController, iSharedObject, Version(0,2,0,0));

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

      bool forwardable;
      int prevOwner;
      int mask;

      int parent;
      int id;

      sSubclassedAction(): id(0), parent(0), prevOwner(0), forwardable(false), mask(UIAIM_ALL) { }

      sSubclassedAction(int _id, int _parent, bool _forwardable = false, int _mask = UIAIM_ALL):
        id(_id), parent(_parent), prevOwner(0), forwardable(_forwardable), mask(_mask) { }

      sSubclassedAction(int _id, int _parent, bool _forwardable, int _prevOwner, int _mask):
        id(_id), parent(_parent), prevOwner(_prevOwner), forwardable(_forwardable), mask(_mask) { }
    };

    typedef std::deque<sSubclassedAction> tSubclassedActions;
    typedef std::map<int, sObserver*> tObservers;
    typedef std::map<int, int> tStaticValues;

  public:
    inline IMController() : returnCodeSet(false), returnCode(0) { 
      // setting/unsetting Ctrl global pointer
      this->registerObserver(IM_PLUG_INIT, bind(resolve_cast0(&IMController::_plugInit), this));
      this->registerObserver(IM_PLUG_DEINIT, bind(resolve_cast0(&IMController::_plugDeInit), this));
      this->registerObserver(IM_UI_PREPARE, bind(resolve_cast0(&IMController::_subclass), this));

      // default values for common messages, may be overriden
      this->addStaticValue(IM_PLUG_NETNAME, 0);
      this->addStaticValue(IM_PLUG_PRIORITY, 0);
      this->addStaticValue(IM_PLUG_SDKVERSION, KONNEKT_SDK_V);
      this->addStaticValue(IM_PLUG_VERSION, 0);
      this->addStaticValue(IM_PLUG_CORE_V, (int) "W98");
      this->addStaticValue(IM_PLUG_UI_V, 0);
    }

    inline virtual ~IMController() { 
      for (tObservers::iterator it = this->observers.begin(); it != this->observers.end(); it++) {
        delete it->second;
      }
      for (tObservers::iterator it = this->actionObservers.begin(); it != this->actionObservers.end(); it++) {
        delete it->second;
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

        if (this->isSublassed() && this->isForwardable()) {
          this->forwardAction();
        }
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
    inline bool registerObserver(tIMid id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return this->_registerObserver(id, f, priority, pos, name, overwrite, this->observers);
    }

    inline bool registerActionObserver(const sSubclassedAction& an, fOnIMessage f, int priority = 0, 
      signals::connect_position pos = signals::at_back, const StringRef& name = "", bool overwrite = true) 
    {
      if (this->_registerObserver(an.id, f, priority, pos, name, overwrite, this->actionObservers)) {
        this->subclassAction(an.id, an.parent, an.forwardable, an.mask);
        return true;
      }
      return false;
    }

    inline bool registerActionObserver(int id, fOnIMessage f, int priority = 0, signals::connect_position pos = signals::at_back, 
      const StringRef& name = "", bool overwrite = true) 
    {
      return this->_registerObserver(id, f, priority, pos, name, overwrite, this->actionObservers);
    }

    inline void notifyObservers(sIMessage_2params* msg) {
      return this->_notifyObservers(this->setIM(msg)->im->id, this->observers);
    }

    inline void notifyActionObservers(sIMessage_2params* msg) {
      return this->_notifyObservers(this->setIM(msg)->getAN()->act.id, this->actionObservers);
    }

    inline void forwardAction() {
      if (!isSublassed()) return;
      return this->setReturnCode(Ctrl->IMessageDirect(IM_UIACTION, this->getPrevOwner(), (int) this->getAN()));
    }

    inline bool isSublassed(int id, int parent) {
      for (tSubclassedActions::iterator it = subclassedActions.begin(); it != subclassedActions.end(); it++) {
        if (it->id == id && it->parent == parent) return true;
      }
      return false;
    }

    inline bool isSublassed() {
      if (!getAN()) return false;
      return this->isSublassed(getAN()->act.id, getAN()->act.parent);
    }

    inline bool isForwardable(int id, int parent) {
      for (tSubclassedActions::iterator it = subclassedActions.begin(); it != subclassedActions.end(); it++) {
        if (it->id == id && it->parent == parent) return it->forwardable;
      }
      return false;
    }

    inline bool isForwardable() {
      if (!getAN()) return false;
      return this->isForwardable(getAN()->act.id, getAN()->act.parent);
    }

    inline void subclassAction(int id, int parent, bool forwardable = false, int mask = UIAIM_ALL) {
      if (this->isSublassed(id, parent)) {
        return;
      }
      this->subclassedActions.push_back(sSubclassedAction(id, parent, forwardable, 0, mask));
    }

    inline int getPrevOwner() {
      for (tSubclassedActions::iterator it = subclassedActions.begin(); it != subclassedActions.end(); it++) {
        if (getAN()->act.id == it->id && getAN()->act.parent == it->parent) return it->prevOwner;
      }
      return sSubclassedAction::notFound;
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
      return this->_isObserved(id, this->observers);
    }

    inline bool isActionObserved(int id) {
      return this->_isObserved(id, this->actionObservers);
    }

  protected:
    /* inline void dbgObservers() {
      for (tObservers::iterator it = this->observers.begin(); it != this->observers.end(); it++) {
        for (tConnections::iterator it2 = it->second->connections.begin(); it2 != it->second->connections.end(); it2++) {
          IMLOG("Observer[%i].connection: %s", it->first, it2->first.c_str());
        }
      }
    } */

    inline bool _isObserved(int id, tObservers& list) {
      if (list.find(id) != list.end()) {
        return !list[id]->signal.empty();
      }
      return false;
    }

    inline void _notifyObservers(int id, tObservers& list) {
      if (!this->_isObserved(id, list)) {
        return;
      }
      list[id]->signal(this);
    }

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

    inline tIMCallback _plugInit() {
      Plug_Init(this->getIM()->p1, this->getIM()->p2);
      return this->setSuccess();
    }

    inline tIMCallback _plugDeInit() {
      Plug_Deinit(this->getIM()->p1, this->getIM()->p2);
      return this->setSuccess();
    }

    inline tIMCallback _subclass() {
      for (tSubclassedActions::iterator it = subclassedActions.begin(); it != subclassedActions.end(); it++) {
        sUIActionInfo nfo(it->parent, it->id);
        nfo.mask = it->mask;
        nfo.txt = new char[100];
        nfo.txtSize = 99;

        UIActionGet(nfo);
        if (!(it->prevOwner = Ctrl->ICMessage(IMI_ACTION_GETOWNER, (int)&nfo.act))) {
          it->prevOwner = Ctrl->ICMessage(IMC_PLUG_ID, 0);
        }

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