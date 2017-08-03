#pragma once

#include <list>
#include <map>
using namespace std;

typedef int CLASS;

class SaginaObject {
    public:
    SaginaObject( CLASS classID ) {
        _classID = classID;
    }
    virtual ~SaginaObject() = 0;

    CLASS getClass() {
        return _classID;
    }

    private:

    CLASS _classID;

};

SaginaObject::~SaginaObject() {}

#include "SaginaException.h"
#include "SaginaSerializationException.h"

