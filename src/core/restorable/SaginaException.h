#pragma once

#include <string>
#include <stdio.h>

class SaginaException {
    public:
    SaginaException( string s ) {
        _s = s;
    }

    SaginaException( const char* s ) {
        _s = string( s );
    }

    virtual ~SaginaException() {
    }

    string cause() {
        return _s;
    }

    void log() {
        printf( "[SaginaException] Exception occurred: %s\n", _s.c_str() );
    }

    private:
    string _s;

};


