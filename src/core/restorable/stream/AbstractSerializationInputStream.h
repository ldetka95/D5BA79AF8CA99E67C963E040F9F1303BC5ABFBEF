#pragma once

#include <stdlib.h>
#include <string.h>

class AbstractSerializationInputStream {
    public:
    AbstractSerializationInputStream() {
        ___cursor = 0;
    }
    virtual ~AbstractSerializationInputStream() {
    }

    template < class T > T get() {
        return *( ( T* ) getMemory( sizeof( T ) ) );
    }

    string getString() {
        int len = get < int > ();
        char* buffer = ( char* )( malloc( len + 1 ) );
        void* mem = getMemory( len );
        memcpy( buffer, mem, len );
        buffer[ len ] = 0;
        string str = string( buffer );
        free( buffer );
        return str;
    }

    virtual void* getMemory( int bytes ) = 0;
    virtual void seek( int position ) = 0;

    void reset() {
        ___cursor = 0;
    }

    inline int getCursor() {
        return ___cursor;
    }

    inline void setCursor( int cursor ) {
        ___cursor = cursor;
    }

    private:

    int ___cursor;

};

