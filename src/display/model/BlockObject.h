#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <map>

#include "CollisionPoints.h"

#define BLOCKOBJECT_FIELDS_COUNT 2

/**
    Block object class. Used for defining any map cube properties.
*/
class BlockObject {
    public:
    BlockObject( int id ) {
        // static add
        pair < int, BlockObject* > p;
        p.first = id;
        p.second = this;
        _m.insert( p );
        // fields definition
        _id = id;
        _textureID = 0;
    }
    ~BlockObject() {
        map < int, BlockObject* >::iterator found = _m.find( _id );
        if ( found != _m.end() ) {
            _m.erase( found );
        }
    }

    /**
        Gets ID of block.
        @return ID of block.
    */
    int getID() {
        return _id;
    }

    /**
        Gets texture ID binded to block.
        @return texture ID binded to block.
    */
    int getTextureID() {
        return _textureID;
    }

    /**
        Gets block object by ID.
        @param id ID of block.
        @return block mapped by specified ID or NULL if not found.
    */
    static BlockObject* getBlockObject( int id ) {
        map < int, BlockObject* >::iterator found = _m.find( id );
        if ( found != _m.end() ) {
            return found -> second;
        }
        return NULL;
    }

    /**
        Loads block object from specified file.
        @param path Path to file.
        @return block object or NULL if error.
    */
    static BlockObject* load( const char* path ) {
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            BlockObject* object;
            try {
                object = _parse( handle );
            } catch ( int ex ) {
                object = NULL;
            }
            fclose( handle );
            return object;
        } else {
            printf( " > ERROR: file not found: \"%s\"\n", path );
        }
        return NULL;
    }

    private:
    static map < int, BlockObject* > _m;

    static BlockObject* _parse( FILE* f ) {
        bool parray[ BLOCKOBJECT_FIELDS_COUNT ];
        int pparray[ BLOCKOBJECT_FIELDS_COUNT ];
        for ( int i = 0; i < BLOCKOBJECT_FIELDS_COUNT; i++ ) {
            parray[ i ] = false;
        }

        int line = 0;
        while ( !feof( f ) ) {
            char buffer[ 4096 ];
            if ( fgets( buffer, 4096, f ) ) {
                line++;
                char pword[ 4096 ];
                int arg;
                if ( sscanf( buffer, " %s = %d ", pword, &arg ) == 2 ) {
                    for ( int i = 0; i < BLOCKOBJECT_FIELDS_COUNT; i++ ) {
                        if ( strcasecmp( pword, ___Field[ i ] ) == 0 ) {
                            pparray[ i ] = arg;
                            parray[ i ] = true;
                        }
                    }
                } else {
                    throw - line;
                }
            }
        }

        for ( int i = 0; i < BLOCKOBJECT_FIELDS_COUNT; i++ ) {
            if ( !parray[ i ] ) {
                throw i;
            }
        }

        BlockObject* object = new BlockObject( pparray[ 0 ] );
        object -> _textureID = pparray[ 1 ];
        return object;
    }

    // block properties
    int _id;
    int _textureID;

    static const char* ___Field[ BLOCKOBJECT_FIELDS_COUNT ];

};

map < int, BlockObject* > BlockObject::_m;
const char* BlockObject::___Field[ BLOCKOBJECT_FIELDS_COUNT ] = {
    "id",
    "texture_id"
};
