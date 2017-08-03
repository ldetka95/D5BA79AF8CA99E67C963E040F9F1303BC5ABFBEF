#pragma once

#define TEXTURESET_CLASS 11962

class TextureSet : public Restorable {
    private:

    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        virtual ~Builder() {}
        Restorable* build() {
            return new TextureSet();
        }
    };

    public:

    struct Info {
        public:
        int id;
        string path;
        int destroyID;
        int walkable;
        int shootable;
        int transparent;
        int maxHp;
    };

    TextureSet() : Restorable( TEXTURESET_CLASS ) {
    }
    virtual ~TextureSet() {
    }

    static void Register() {
        Restorable::Register( TEXTURESET_CLASS, new Builder() );
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( ( int )( _m.size() ) );
        map < int, Info >::iterator it = _m.begin();
        while ( it != _m.end() ) {
            _serializeInfo( stream, it -> second );
            it++;
        }
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        int s = stream -> get < int > ();
        for ( int i = 0; i < s; i++ ) {
            Info info;
            _deserializeInfo( stream, &info );
            _m[ info.id ] = info;
        }
        return true;
    }

    void add( int id, Info info ) {
        _m[ id ] = info;
    }

    bool get( int id, Info* info ) {
        map < int, Info >::iterator found = _m.find( id );
        if ( found != _m.end() ) {
            if ( info ) {
                ( *info ) = found -> second;
            }
            return true;
        }
        return false;
    }

    TextureHolder* buildHolder() {
        TextureHolder* holder = TextureHolder::Create();
        map < int, Info >::iterator it = _m.begin();
        while ( it != _m.end() ) {
            const char* ch = ( it -> second ).path.c_str();
            printf( "[CH] = %s\n", ch );
            holder -> add( it -> first, Texture::Load( ch ) );
            it++;
        }
        return holder;
    }

    TextureSet* clone() {
        TextureSet* set = new TextureSet();
        map < int, Info >::iterator it = _m.begin();
        while ( it != _m.end() ) {
            set -> add( it -> first, it -> second );
            it++;
        }
        return set;
    }

    private:

    void _serializeInfo( AbstractSerializationOutputStream* stream, Info info ) {
        stream -> add( info.id );
        stream -> addString( info.path );
        stream -> add( info.destroyID );
        stream -> add( info.walkable );
        stream -> add( info.shootable );
        stream -> add( info.transparent );
        stream -> add( info.maxHp );
    }

    void _deserializeInfo( AbstractSerializationInputStream* stream, Info* info ) {
        info -> id = stream -> get < int > ();
        info -> path = stream -> getString();
        info -> destroyID = stream -> get < int > ();
        info -> walkable = stream -> get < int > ();
        info -> shootable = stream -> get < int > ();
        info -> transparent = stream -> get < int > ();
        info -> maxHp = stream -> get < int > ();
    }

    map < int, Info > _m;
};

