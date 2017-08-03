#pragma once

#include "Timer.h"

class TimerFactory {
    public:
    TimerFactory() {
    }
    virtual ~TimerFactory() {
    }
    virtual Timer* buildTimer() = 0;
};


