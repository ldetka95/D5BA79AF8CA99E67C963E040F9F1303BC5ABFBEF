#pragma once

#include "Ammo.h"
#include "WeaponType.h"

#define WEAPON_CLASS 12561

class Weapon : public Restorable {
    private:
    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        Weapon* build() {
            return new Weapon();
        }
    };

    Weapon() : Restorable( WEAPON_CLASS ) {
        ___weaponType = NULL;
        _id = 0;
        ___var = NULL;
        ___avc = 0;
        resize( ___avc );
    }

    public:

    static const bool AUTORELOAD = true;

    Weapon( WeaponType* type ) : Restorable( WEAPON_CLASS ) {
        ___weaponType = type;
        _id = 0;
        if ( type ) {
            _id = type -> getID();
        }
        ___var = NULL;
        ___avc = 0;
        if ( type ) {
            resize( type -> getVarCount() );
            for ( int i = 0; i < type -> getVarCount(); i++ ) {
                ___var[ i ] = type -> getWeaponVar( i );
            }
        }
        ___var[ WV_CLIP_STATE ] = ___var[ WV_CLIP ];
    }

    virtual ~Weapon() {
        resize( 0 );
    }

    Ammo* shoot( EntityInterface* shooting ) {
        if ( ( shooting ) && ( getWeaponType() ) ) {
            if ( canShoot() ) {
                Vector3D pos = shooting -> getHotSpot() - Vector3D( 0.0, 0.2, 0.0 );
                Vector3D dir = Vector3D( *( shooting -> getAngles() ) );
                double len = dir.dist3D();
                double r = ___var[ WV_MAX_RECOIL ];
                PolarVector3D v = PolarVector3D( random( r ), random( 2.0 * M_PI ), random( M_PI ) - M_PI / 2.0 );
                dir = ( dir + Vector3D( v ) ).normalize() * len;
                return new Ammo( getWeaponType() -> getAmmoType(), shooting, pos, dir );
            }
        }
        return NULL;
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( _id );
        stream -> add( ___avc );
        for ( int i = 0; i < ___avc; i++ ) {
            stream -> add( ___var[ i ] );
        }
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        _id = stream -> get < ID > ();
        ___avc = stream -> get < int > ();
        resize( ___avc );
        for ( int i = 0; i < ___avc; i++ ) {
            ___var[ i ] = stream -> get < double > ();
        }
        return true;
    }

    static void Register() {
        Restorable::Register( WEAPON_CLASS, new Builder() );
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

    static void Init( const char* wPath, const char* aPath ) {
        AmmoType::ReadRaw( aPath );
        WeaponType::ReadRaw( wPath );
    }

    bool canShoot() {
        double t = double( glutGet( GLUT_ELAPSED_TIME ) ) / 1000.0;
        double dCnt = ___var[ WV_DELAY_COUNTER ];
        int clipState = ___var[ WV_CLIP_STATE ];
        if ( ( t > dCnt ) && ( clipState > 0 ) ) {
            double delay = ___var[ WV_DELAY ];
            ___var[ WV_DELAY_COUNTER ] = t + delay;
            ___var[ WV_CLIP_STATE ]--;
            return true;
        } else if ( ( clipState == 0 ) && ( AUTORELOAD ) ) {
            reload();
        }
        return false;
    }

    bool reload() {
        if ( ( ___var[ WV_RELOAD_STATE ] < 0.5 ) && ( ___var[ WV_CLIP_STATE ] < ___var[ WV_CLIP ] ) && ( getAmmoCapacity() > 0 ) ) {
            ___var[ WV_RELOAD_STATE ] = 1.0;
            ___var[ WV_RELOAD_TIME_END ] = double( glutGet( GLUT_ELAPSED_TIME ) ) + double( ___var[ WV_RELOAD_TIME ] ) * 1000.0;
            return true;
        }
        return false;
    }

    void reloadTick() {
        if ( ( ___var[ WV_RELOAD_STATE ] > 0.5 ) && ( ___var[ WV_RELOAD_TIME_END ] <= glutGet( GLUT_ELAPSED_TIME ) ) ) {
            ___var[ WV_RELOAD_STATE ] = 0.0;
            double amount = min( ___var[ WV_CLIP ], getAmmoCapacity() );
            ___var[ WV_CLIP_STATE ] = amount;
            getWeaponType() -> getAmmoType() -> addAmmo( - amount );
        }
    }

    double getClipCount() {
        return ___var[ WV_CLIP_STATE ];
    }

    double getAmmoCapacity() {
        return getWeaponType() -> getAmmoType() -> getAmmoVar( AV_QUANTITY );
    }

    double getReloadingProgress() {
        if ( ___var[ WV_RELOAD_STATE ] > 0.5 ) {
            return 1.0 - ( ___var[ WV_RELOAD_TIME_END ] - double( glutGet( GLUT_ELAPSED_TIME ) ) ) / ___var[ WV_RELOAD_TIME ];
        } else {
            return 1.0;
        }
    }

    WeaponType* getWeaponType() {
        if ( !___weaponType ) {
            ___weaponType = WeaponType::getWeaponType( _id );
        }
        return ___weaponType;
    }

    private:

    WeaponType* ___weaponType;
    ID _id;
    double* ___var;
    int ___avc;

};



