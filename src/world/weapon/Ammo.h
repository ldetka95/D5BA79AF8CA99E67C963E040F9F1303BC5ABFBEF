#pragma once

#include "AmmoType.h"

#define AMMO_CLASS 12560

#define AMMO_TRACE_LENGTH 16
#define AMMO_TRACE_MASK 0x0000000F

#define AMMO_STD_DIST 100.0
#define AMMO_SPEED_MULTIPLIER 8

class Ammo : public Restorable {
    private:
    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        Ammo* build() {
            return new Ammo();
        }
    };

    protected:
    Ammo() : Restorable( AMMO_CLASS ) {
        ___type = NULL;
        ___mother = NULL;
        ___traceCurIndex = 0;
    }

    public:

    Ammo( AmmoType* type, EntityInterface* shooting, Vector3D posInit, Vector3D dir ) : Restorable( AMMO_CLASS ) {
        ___type = type;
        ___mother = shooting;
        ___posInit = posInit;
        ___trace[ 0 ] = posInit;
        ___dir = dir;
        ___dir.y = - ___dir.y; // lel
        if ( ___type ) {
            ___dir = ___dir.normalize() * ( ___type -> getAmmoVar( AV_SPEED ) * 1000.0 / 3600.0 / 60.0 / double( AMMO_SPEED_MULTIPLIER ) );
        }
        ___traceCurIndex = 0;
        if ( type ) {
            ___color = type -> getBulletColor();
        } else {
            ___color = Vector3D( 1.0, 1.0, 1.0 );
        }
    }

    ~Ammo() {
    }

    static void Register() {
        Restorable::Register( AMMO_CLASS, new Builder() );
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        return true;
    }

    bool move( Map* m, Container* eachEntityContainer ) {
        double dist = AMMO_STD_DIST;
        if ( ___type ) {
            dist = ___type -> getAmmoVar( AV_RANGE );
        }
        if ( ___posInit.dist3D( ___trace[ ___traceCurIndex ] ) > dist ) {
            return false;
        }
        if ( m ) {
            if ( m -> pointCollides( ___trace[ ___traceCurIndex ] ) ) {
                return false;
            }
        }
        Vector3D nextPos = ___trace[ ___traceCurIndex ];
        if ( eachEntityContainer ) {
            for ( int j = 0; j < AMMO_SPEED_MULTIPLIER; j++ ) {
                for ( Container::Iterator i = eachEntityContainer -> getIterator(); i.hasNext(); i.next() ) {
                    EntityInterface* e = dynamic_cast < EntityInterface* > ( i.get() );
                    if ( ( e != NULL ) && ( e != ___mother ) ) {
                        //if ( ( e -> getHotSpot() ).dist3D( ___trace[ ___traceCurIndex ] ) < e -> getVar( EV_COLLISIONRADIUS ) ) {
                        if ( CollisionPoints::InsideCylinder( nextPos, e -> getHotSpot(), e -> getVar( EV_CYLINDER_RADIUS ), e -> getVar( EV_CYLINDER_HEIGHT ) ) ) {
                            if ( ___type ) {
                                double v = random( ___type -> getAmmoVar( AV_DAMAGE_MIN ), ___type -> getAmmoVar( AV_DAMAGE_MAX ) );
                                double hp = e -> getVar( EV_HP ) - v;
                                e -> setVar( EV_HP, hp );
                                if ( hp <= 0.0 ) {
                                    delete i.remove();
                                }
                            }
                            return false;
                        }
                    }
                }
                nextPos += ___dir;
            }
        }
        ___traceCurIndex = ( ___traceCurIndex + 1 ) & AMMO_TRACE_MASK;
        ___trace[ ___traceCurIndex ] = nextPos;
        return true;
    }

    void display() {
        Vector3D t1 = ___trace[ ___traceCurIndex ];
        int tciLast = ___traceCurIndex - 1;
        if ( tciLast < 0 ) {
            tciLast += AMMO_TRACE_LENGTH;
        }
        Vector3D t2 = ___trace[ tciLast ];
        glColor3d( ___color.x, ___color.y, ___color.z );
        glVertex3d( t1.x, t1.y, t1.z );
        glVertex3d( t2.x, t2.y, t2.z );
    }

    private:

    AmmoType* ___type;
    EntityInterface* ___mother;
    Vector3D ___dir;
    Vector3D ___color;
    Vector3D ___posInit;
    Vector3D ___trace[ AMMO_TRACE_LENGTH ];
    int ___traceCurIndex;

};


