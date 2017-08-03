#pragma once

#include "drawing/CubeDrawer.h"

#define CHUNK_CLASS 11937

class Chunk : public Restorable {
    protected:

    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        Restorable* build() {
            return new Chunk();
        }
    };

    public:
    Chunk() : Restorable( CHUNK_CLASS ) {
        _xShift = 0;
        _zShift = 0;
        _x = 0;
        _y = 0;
        _z = 0;
        _xy = 0;
        _size = 0;
        _field = NULL;
    }

    Chunk( int x, int y, int z, int xShift, int zShift ) : Restorable( CHUNK_CLASS ) {
        _xShift = xShift;
        _zShift = zShift;
        _x = 0;
        _y = 0;
        _z = 0;
        _xy = 0;
        _size = 0;
        _field = NULL;
        resize( x, y, z );
    }

    ~Chunk() {
        if ( _field ) {
            free( _field );
        }
    }

    static void Register() {
        Restorable::Register( CHUNK_CLASS, new Builder() );
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( _x );
        stream -> add( _y );
        stream -> add( _z );
        stream -> add( _xShift );
        stream -> add( _zShift );
        stream -> addMemory( _field, _size );
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        _x = stream -> get < int > ();
        _y = stream -> get < int > ();
        _z = stream -> get < int > ();
        _xShift = stream -> get < int > ();
        _zShift = stream -> get < int > ();
        resize( _x, _y, _z );
        char* buffer = ( char* ) stream -> getMemory( _size );
        memcpy( _field, buffer, _size );
        return true;
    }

    void resize( int x, int y, int z ) {
        _x = x;
        _y = y;
        _z = z;
        _xy = x * y;
        int size = x * y * z;
        _field = ( char* ) realloc( _field, size );
        memset( _field, 0, size );
        _size = size;
    }

    int getField( int x, int y, int z ) {
        if ( _field ) {
            if ( ( x >= 0 ) && ( x < _x ) && ( y >= 0 ) && ( y < _y ) && ( z >= 0 ) && ( z < _z ) ) {
                return _field[ _index( x, y, z ) ];
            }
        }
        return -1;
    }

    void setField( int x, int y, int z, int value ) {
        if ( _field ) {
            if ( ( x >= 0 ) && ( x < _x ) && ( y >= 0 ) && ( y < _y ) && ( z >= 0 ) && ( z < _z ) ) {
                _field[ _index( x, y, z ) ] = ( value & 0xFF );
            }
        }
    }

    void onDisplay( Texture* tex ) {
        glPushMatrix();
        glTranslated( _xShift, 0, _zShift );
        glBegin( GL_QUADS );
        for ( int i = 0; i < _x; i++ ) {
            for ( int j = 0; j < _y; j++ ) {
                for ( int k = 0; k < _z; k++ ) {
                    int tid = getField( i, j, k );
                    if ( tid > 0 ) {
                        for ( int l = 0; l < 6; l++ ) {
                            int nextTo = getField(
                                i + CubeDrawer::sideOffset[ l ].x,
                                j + CubeDrawer::sideOffset[ l ].y,
                                k + CubeDrawer::sideOffset[ l ].z
                            );
                            if ( nextTo <= 0 ) {
                                CubeDrawer::DrawSide( i, j, k, tid, l );
                            }
                        }
                    }
                }
            }
        }
        glEnd();
        glPopMatrix();
    }

    private:

    inline int _index( int x, int y, int z ) {
        return ( z * _xy + y * _x + x );
    }

    int _x;
    int _y;
    int _z;
    int _xy;
    int _size;
    int _xShift;
    int _zShift;
    char* _field;

};


