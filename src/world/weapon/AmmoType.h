#pragma once

#include <map>

#define AMMOTYPE_CLASS 12562

class AmmoType : public Restorable {
    private:
    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        AmmoType* build() {
            return new AmmoType();
        }
    };

    protected:
    AmmoType() : Restorable( AMMOTYPE_CLASS ) {
        _id = 0;
        _color = Vector3D( 1.0, 1.0, 0.0 );
        ___avc = 0;
        ___var = NULL;
        resize( AMMO_VAR_COUNT );
    }

    public:
    virtual ~AmmoType() {
        resize( 0 );
    }

    static void Register() {
        Restorable::Register( AMMOTYPE_CLASS, new Builder() );
        ___varConverter -> appendMapping( AV_RANGE, "RANGE" );
        ___varConverter -> appendMapping( AV_DAMAGE_MIN, "DAMAGE_MIN" );
        ___varConverter -> appendMapping( AV_DAMAGE_MAX, "DAMAGE_MAX" );
        ___varConverter -> appendMapping( AV_SPEED, "SPEED" );
        ___varConverter -> appendMapping( AV_CAPACITY, "CAPACITY" );
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( _id );
        stream -> add( _color );
        stream -> add( ___avc );
        for ( int i = 0; i < ___avc; i++ ) {
            stream -> add( ___var[ i ] );
        }
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        _id = stream -> get < ID > ();
        _color = stream -> get < Vector3D > ();
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
                AmmoType* type = ___ReadFrom( pth.c_str() );
                if ( type ) {
                    int id = slf -> getEntryID( i );
                    type -> _id = id;
                    addAmmoType( id, type );
                }
                //type -> addAmmo( 3 );
            }
            delete slf;
            return true;
        }
        return false;
    }

    static AmmoType* getAmmoType( ID id ) {
        map < ID, AmmoType* >::iterator found = ___AMMO.find( id );
        if ( found != ___AMMO.end() ) {
            return found -> second;
        }
        return NULL;
    }

    Vector3D getBulletColor() {
        return _color;
    }

    double getAmmoVar( int index ) {
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

    bool addAmmo( int amount ) {
        if ( amount > 0 ) {
            if ( ___var[ AV_QUANTITY ] < ___var[ AV_CAPACITY ] ) {
                ___var[ AV_QUANTITY ] += amount;
                if ( ___var[ AV_QUANTITY ] > ___var[ AV_CAPACITY ] ) {
                    ___var[ AV_QUANTITY ] = ___var[ AV_CAPACITY ];
                }
                return true;
            }
            return false;
        } else if ( amount < 0 ) {
            if ( ___var[ AV_QUANTITY ] >= ( - amount ) ) {
                ___var[ AV_QUANTITY ] += amount;
                return true;
            }
            return false;
        }
        return true;
    }

    protected:

    static bool addAmmoType( ID id, AmmoType* type ) {
        map < ID, AmmoType* >::iterator found = ___AMMO.find( id );
        if ( found != ___AMMO.end() ) {
            return false;
        }
        if ( type ) {
            ___AMMO[ id ] = type;
        }
        return true;
    }

    private:

    static AmmoType* ___ReadFrom( const char* path ) {
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            AmmoType* ammo = new AmmoType();
            try {
                while ( !feof( handle ) ) {
                    char buffer[ 4096 ];
                    double value;
                    if ( fscanf( handle, "%s = %lf", buffer, &value ) == 2 ) {
                        /*
                            if ( strcmp( buffer, "ID" ) == 0 ) {
                                ammo -> _id = value;
                            }
                        */
                        if ( strcmp( buffer, "COLOR.R" ) == 0 ) {
                            ammo -> _color.x = value;
                        } else if ( strcmp( buffer, "COLOR.G" ) == 0 ) {
                            ammo -> _color.y = value;
                        } else if ( strcmp( buffer, "COLOR.B" ) == 0 ) {
                            ammo -> _color.z = value;
                        } else {
                            string word = string( buffer );
                            int i = ___varConverter -> getID( word );
                            if ( ( i >= 0 ) && ( i < ammo -> ___avc ) ) {
                                ammo -> ___var[ i ] = value;
                            }
                        }
                    }
                }
            } catch ( int ex ) {
                delete ammo;
                ammo = NULL;
            }
            fclose( handle );
            return ammo;
        }
        return NULL;
    }

    static map < ID, AmmoType* > ___AMMO;
    static StringLinker* ___varConverter;

    ID _id;
    Vector3D _color;
    double* ___var;
    int ___avc;

};

map < ID, AmmoType* > AmmoType::___AMMO;
StringLinker* AmmoType::___varConverter = new StringLinker();

