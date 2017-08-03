#pragma once

#include "ByteBuffer.h"

class MemorySerializationOutputStream : public AbstractSerializationOutputStream {
    public:
    MemorySerializationOutputStream() : AbstractSerializationOutputStream() {
        ___total = 0;
    }
    ~MemorySerializationOutputStream() {
        clear();
    }

    void addMemory( const void* src, int srcLength ) {
        ___bufferList.push_back( new ByteBuffer( src, srcLength ) );
        ___total += srcLength;
    }

    void addBuffer( ByteBuffer* buffer ) {
        if ( buffer ) {
            ___bufferList.push_back( buffer );
            ___total += buffer -> getSize();
        }
    }

    bool removeHead( ByteBuffer** out ) {
        if ( ___bufferList.empty() ) {
            return false;
        }
        ByteBuffer* frontal = ___bufferList.front();
        if ( out ) {
            *out = frontal;
        }
        ___bufferList.pop_front();
        ___total -= frontal -> getSize();
        return true;
    }

    int getBuffersCount() {
        return ___bufferList.size();
    }

    int getTotalByteSize() {
        return ___total;
    }

    MemorySerializationInputStream* toInputStream() {
        MemorySerializationInputStream* buffer = new MemorySerializationInputStream( ___total );
        char* data = buffer -> getData < char > ();
        list < ByteBuffer* >::iterator it = ___bufferList.begin();
        while ( it != ___bufferList.end() ) {
            ByteBuffer* part = *it;
            memcpy( data, part -> getRawData(), part -> getSize() );
            data += part -> getSize();
            it++;
        }
        return buffer;
    }

    void clear() {
        while ( !___bufferList.empty() ) {
            delete ___bufferList.front();
            ___bufferList.pop_front();
        }
        ___total = 0;
    }

    private:

    list < ByteBuffer* > ___bufferList;
    int ___total;

};



