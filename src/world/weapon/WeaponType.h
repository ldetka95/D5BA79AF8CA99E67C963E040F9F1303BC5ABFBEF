#pragma once

#include "AmmoType.h"

#define WEAPONTYPE_CLASS 12563

class WeaponType : public Restorable {
    private:
    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        WeaponType* build() {
            return new WeaponType();
        }
    };

    protected:
    WeaponType() : Restorable( WEAPONTYPE_CLASS ) {
        ___ammoType = NULL;
        _id = -1;
        ___avc = 0;
        ___var = NULL;
        resize( WEAPON_VAR_COUNT );
    }

    public:
    virtual ~WeaponType() {
        resize( 0 );
    }

    static void Register() {
        Restorable::Register( WEAPONTYPE_CLASS, new Builder() );
        ___varConverter -> appendMapping( WV_DELAY, "DELAY" );
        ___varConverter -> appendMapping( WV_MAX_RECOIL, "MAX_RECOIL" );
        ___varConverter -> appendMapping( WV_CLIP, "CLIP" );
        ___varConverter -> appendMapping( WV_RELOAD_TIME, "RELOAD_TIME" );
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( ___aid );
        stream -> add( _id );
        stream -> add( ___avc );
        for ( int i = 0; i < ___avc; i++ ) {
            stream -> add( ___var[ i ] );
        }
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        ___aid = stream -> get < ID > ();
        _id = stream -> get < ID > ();
        ___avc = stream -> get < int > ();
        resize( ___avc );
        for ( int i = 0; i < ___avc; i++ ) {
            ___var[ i ] = stream -> get < double > ();
        }
        return true;
    }

    static bool ReadRaw( const char* slfPath ) {
        StringLinker* slf = StringLinker::FromFile( slfPath );
        if ( slf ) {
            for ( int i = 0; i < slf -> entryCount(); i++ ) {
                string pth = slf -> getEntryString( i );
                WeaponType* type = ___ReadFrom( pth.c_str() );
                if ( type ) {
                    int id = slf -> getEntryID( i );
                    type -> _id = id;
                    addWeaponType( id, type );
                }
            }
            delete slf;
            return true;
        }
        return false;
    }

    static WeaponType* getWeaponType( ID id ) {
        map < ID, WeaponType* >::iterator found = ___WPN.find( id );
        if ( found != ___WPN.end() ) {
            return found -> second;
        }
        return NULL;
    }

    AmmoType* getAmmoType() {
        if ( !___ammoType ) {
            ___ammoType = AmmoType::getAmmoType( ___aid );
        }
        return ___ammoType;
    }

    double getWeaponVar( int index ) {
        if ( ( index >= 0 ) && ( index < ___avc ) ) {
            return ___var[ index ];
        }
        return NAN;
    }

    void resize( int n ) {
        if ( n < 0 ) {
            return;
        }
        ___avc = n;
        ___var = ( double* ) realloc( ___var, n * sizeof( double ) );
        for ( int i = 0; i < n; i++ ) {
            ___var[ i ] = 0.0;
        }
    }

    int getVarCount() {
        return ___avc;
    }

    ID getID() {
        return _id;
    }

    protected:

    static bool addWeaponType( ID id, WeaponType* type ) {
        map < ID, WeaponType* >::iterator found = ___WPN.find( id );
        if ( found != ___WPN.end() ) {
            return false;
        }
        if ( type ) {
            ___WPN[ id ] = type;
        }
        return true;
    }

    private:

    static WeaponType* ___ReadFrom( const char* path ) {
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            WeaponType* wpn = new WeaponType();
            try {
                while ( !feof( handle ) ) {
                    char buffer[ 4096 ];
                    double value;
                    if ( fscanf( handle, "%s = %lf", buffer, &value ) == 2 ) {
                        /*if ( strcmp( buffer, "ID" ) == 0 ) {
                            wpn -> _id = value;
                        }*/
                        if ( strcmp( buffer, "AMMO_ID" ) == 0 ) {
                            wpn -> ___aid = value;
                        } else {
                            string word = string( buffer );
                            int i = ___varConverter -> getID( word );
                            if ( i >= 0 ) {
                                wpn -> ___var[ i ] = value;
                            }
                        }
                    }
                }
            } catch ( int ex ) {
                delete wpn;
                wpn = NULL;
            }
            fclose( handle );
            return wpn;
        }
        return NULL;
    }

    static map < ID, WeaponType* > ___WPN;
    static StringLinker* ___varConverter;

    AmmoType* ___ammoType;
    ID ___aid;
    ID _id;
    double* ___var;
    int ___avc;

};

map < ID, WeaponType* > WeaponType::___WPN;
StringLinker* WeaponType::___varConverter = new StringLinker();

