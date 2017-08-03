#pragma once

#include "ByteBuffer.h"

class MemorySerializationInputStream : public AbstractSerializationInputStream, public ByteBuffer {
    public:
    MemorySerializationInputStream() : AbstractSerializationInputStream(), ByteBuffer() {
    }
    MemorySerializationInputStream( int bufferSize ) : AbstractSerializationInputStream(), ByteBuffer( bufferSize ) {
    }
    MemorySerializationInputStream( const void* initial, int bufferSize ) : AbstractSerializationInputStream(), ByteBuffer( initial, bufferSize ) {
    }
    virtual ~MemorySerializationInputStream() {}

    void* getMemory( int bytes ) {
        int positionAfter = getCursor() + bytes;
        if ( positionAfter > getSize() ) {
            char msg[ 4096 ];
            sprintf( msg, "Deserialization exception: expected [%d] bytes, got [%d] bytes (lacking %d)", bytes, getSize() - getCursor(), positionAfter - getSize() );
            return NULL;
            throw SaginaSerializationException( msg );
        }
        int currentPoint = getCursor();
        setCursor( positionAfter );
        return ( void* )( int( getRawData() ) + currentPoint );
    }

    void seek( int position ) {
        setCursor( position );
    }

    private:

};
