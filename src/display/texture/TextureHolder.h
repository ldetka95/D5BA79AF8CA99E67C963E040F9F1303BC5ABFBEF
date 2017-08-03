#pragma once

#include "Texture.h"

class TextureHolder {
    protected:
    TextureHolder() {
    }
    public:
    ~TextureHolder() {
        map < int, Texture* >::iterator it = _txmap.begin();
        while ( it != _txmap.end() ) {
            delete ( it -> second );
            it++;
        }
    }

    static TextureHolder* Load( string path ) {
        return Load( path.c_str() );
    }

    static TextureHolder* Load( const char* path ) {
        StringLinker* slf = StringLinker::FromFile( path );
        if ( slf ) {
            TextureHolder* holder = new TextureHolder();
            for ( int i = 0; i < slf -> entryCount(); i++ ) {
                int id = slf -> getEntryID( i );
                string str = slf -> getEntryString( i );
                Texture* texture = Texture::Load( str.c_str() );
                holder -> add( id, texture );
            }
            return holder;
        }
        return NULL;
    }

    static TextureHolder* Create() {
        return new TextureHolder();
    }

    void add( int id, Texture* texture ) {
        if ( texture ) {
            _txmap[ id ] = texture;
        }
    }

    Texture* get( int id ) {
        map < int, Texture* >::iterator found = _txmap.find( id );
        if ( found != _txmap.end() ) {
            return found -> second;
        }
        return NULL;
    }

    bool setTexture( int id ) {
        Texture* texture = get( id );
        if ( texture ) {
            texture -> glUseImage();
            return true;
        }
        return false;
    }

    static Texture* Joined( TextureHolder* holder, int x, int y, int w, int h ) {
        Texture* texture = Texture::Create( x * w, y * h );
        if ( texture ) {
            map < int, Texture* >::iterator it = ( holder -> _txmap ).begin();
            while ( it != ( holder -> _txmap ).end() ) {
                Texture* src = it -> second;
                int i = it -> first;
                src -> copyPixels32( texture, 0, 0, ( i % x ) * w, ( i / x ) * h, w, h );
                it++;
            }
            texture -> glCreateImage();
        }
        return texture;
    }

    static TextureHolder* getGlobalHolder() {
        if ( !_globalInst ) {
            _globalInst = new TextureHolder();
        }
        return _globalInst;
    }

    static void InitGlobalHolder( const char* path ) {
        if ( _globalInst ) {
            delete _globalInst;
        }
        _globalInst = Load( path );
    }

    private:

    static TextureHolder* _globalInst;
    map < int, Texture* > _txmap;

};

TextureHolder* TextureHolder::_globalInst = NULL;

