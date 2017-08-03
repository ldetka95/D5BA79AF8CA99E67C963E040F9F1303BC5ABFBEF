#pragma once

class AbstractSerializationOutputStream {
    public:
    AbstractSerializationOutputStream() {
    }
    virtual ~AbstractSerializationOutputStream() {
    }

    template < class T > void add( T item ) {
        addMemory( &item, sizeof( item ) );
    }

    void addString( string str ) {
        const char* s = str.c_str();
        int slen = strlen( s );
        addMemory( &slen, sizeof( int ) );
        addMemory( s, slen );
    }

    virtual void addMemory( const void* src, int srcLength ) = 0;
    //virtual void addBuffer( ByteBuffer* buffer ) = 0;

    virtual AbstractSerializationInputStream* toInputStream() = 0;

    private:
};
