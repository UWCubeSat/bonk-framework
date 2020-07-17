#ifndef STATE_MANAGER_H_
#define STATE_MANAGER_H_

#include "Arduino.h"

class StateManager
{
  public:
    StateManager();
    void* getState() const;
    bool setState(void* new_state_st);
    bool flushToSD() const;
  private:
    uint32_t _writes;
    void* _state_st;
    bool _last_successful;    
};  // StateManager class

#endif  // STATE_MANAGER_H_