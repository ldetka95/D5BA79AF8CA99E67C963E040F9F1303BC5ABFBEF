#pragma once

#include <stdlib.h>
#include <string.h>

class ByteBuffer {
    public:
    ByteBuffer() {
        ___data = NULL;
        ___size = 0;
    }
    ByteBuffer( int bufferSize ) {
        ___data = malloc( bufferSize );
        ___size = bufferSize;
    }
    ByteBuffer( const void* initial, int bufferSize ) {
        ___data = NULL;
        ___size = 0;
        fastResize( bufferSize );
        memcpy( ___data, initial, bufferSize );
    }
    virtual ~ByteBuffer() {
        if ( ___data ) {
            free( ___data );
        }
    }

    inline void fastResize( int newSize ) {
        ___data = realloc( ___data, newSize );
        ___size = newSize;
    }

    void resize( int newSize ) {
        int oldSize = ___size;
        fastResize( newSize );
        for ( int i = oldSize; i < newSize; i++ ) {
            ( ( char* ) ___data )[ i ] = 0;
        }
    }

    inline const void* getRawData() {
        return ___data;
    }

    inline int getSize() {
        return ___size;
    }

    template < class T > T* getData() {
        return ( T* ) ___data;
    }

    ByteBuffer* clone() {
        return new ByteBuffer( ___data, ___size );
    }

    private:

    void* ___data;
    int ___size;

};


