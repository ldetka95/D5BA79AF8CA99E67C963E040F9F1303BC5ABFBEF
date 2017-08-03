#pragma once

#define INTERACTIVE_CLASS 11939

#include "Map.h"
#include "Physics.h"

#include "object/EntityVarDef.h"
#include "object/EntityInterface.h"
#include "weapon/Weapon.h"
#include "object/Entity.h"

class Interactive : public Restorable {
    protected:

    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        Restorable* build() {
            return new Interactive();
        }
    };

    public:
    Interactive() : Restorable( INTERACTIVE_CLASS ) {
        _bulletContainer = new Container();
        _pickupContainer = new Container();
        _allEntityContainer = new Container();
        _player = NULL;
        _playerID = -1;
    }
    ~Interactive() {
        if ( _allEntityContainer ) {
            _allEntityContainer -> deleteAll();
            delete _allEntityContainer;
        }
        if ( _bulletContainer ) {
            _bulletContainer -> deleteAll();
            delete _bulletContainer;
        }
        if ( _pickupContainer ) {
            _pickupContainer -> deleteAll();
            delete _pickupContainer;
        }
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( ( bool )( _allEntityContainer != NULL ) );
        Restorable::Serialize( _allEntityContainer, stream );
        stream -> add( ( bool )( _bulletContainer != NULL ) );
        Restorable::Serialize( _bulletContainer, stream );
        stream -> add( ( bool )( _pickupContainer != NULL ) );
        Restorable::Serialize( _pickupContainer, stream );
        stream -> add( _playerID );
        return true;
    }
    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        if ( stream -> get < bool > () ) {
            _allEntityContainer = Restorable::TryDeserializeThrow < Container > ( stream );
        }
        if ( stream -> get < bool > () ) {
            _bulletContainer = Restorable::TryDeserializeThrow < Container > ( stream );
        }
        if ( stream -> get < bool > () ) {
            _pickupContainer = Restorable::TryDeserializeThrow < Container > ( stream );
        }
        _playerID = stream -> get < ID > ();
        setPlayer( _playerID );
        return true;
    }

    void tick( Map* mapInstance ) {
        /// @todo PICKUPS
        if ( _allEntityContainer ) {
            for ( Container::Iterator i = _allEntityContainer -> getIterator(); i.hasNext(); i.next() ) {
                Entity* ent = dynamic_cast < Entity* > ( i.get() );
                if ( ent ) {
                    if ( !ent -> isPlayer() ) {
                        Vector3D posToReach = *( _player -> getPosition() );
                        Vector3D entPos = *( ent -> getPosition() );
                        ent -> lookAt( posToReach );
                        if ( entPos.dist3D( posToReach ) > 2.0 ) {
                            ent -> tryReachTarget( mapInstance, getAllEntity(), ent -> getVar( EV_SPEED ) );
                        }
                        /// @todo If can't shoot, then reload.
                    } else {
                        double z = 0.0;
                        double v = ent -> getVar( EVT_IS_ON_WALL );
                        if ( v == 1.0 ) {
                            PolarVector3D ang = PolarVector3D( Vector3D( *( ent -> getAngles() ) ) );
                            PolarVector3D joint = PolarVector3D( *( ent -> getWallJoint() ) );
                            z = sin( radn( 180.0 - degr( ang.i - joint.i ) ) ) * 32.0;
                        }
                        Camera::SetZRotation( z );
                    }
                    ent -> idleTick();
                }
            }
        }
        if ( _bulletContainer ) {
            for ( Container::Iterator i = _bulletContainer -> getIterator(); i.hasNext(); ) {
                Ammo* a = dynamic_cast < Ammo* > ( i.get() );
                if ( a ) {
                    if ( !( a -> move( mapInstance, _allEntityContainer ) ) ) {
                        i.remove();
                        delete a;
                        continue;
                    }
                }
                i.next();
            }
        }
    }

    void display() {
        if ( _allEntityContainer ) {
            /*
            glDisable( GL_TEXTURE_2D );
            for ( Container::Iterator i = _allEntityContainer -> getIterator(); i.hasNext(); i.next() ) {
                Entity* ent = dynamic_cast < Entity* > ( i.get() );
                if ( ent ) {
                    glPushMatrix();
                    Vector3D v = *( ent -> getPosition() );
                    glTranslated( v.x, v.y, v.z );
                    glColor3d( 0.0, 0.7, 0.5 );
                    glutSolidCube( 1.0 );
                    glPopMatrix();
                }
            }
            glEnable( GL_TEXTURE_2D );*/
            for ( Container::Iterator i = _allEntityContainer -> getIterator(); i.hasNext(); i.next() ) {
                Entity* ent = dynamic_cast < Entity* > ( i.get() );
                if ( ent ) {
                    ent -> display();
                }
            }
        }
        if ( _bulletContainer ) {
            glDisable( GL_TEXTURE_2D );
            glLineWidth( 3.0 );
            glBegin( GL_LINES );
                for ( Container::Iterator i = _bulletContainer -> getIterator(); i.hasNext(); i.next() ) {
                    Ammo* a = dynamic_cast < Ammo* > ( i.get() );
                    if ( a ) {
                        a -> display();
                    }
                }
            glEnd();
            glLineWidth( 1.0 );
            glEnable( GL_TEXTURE_2D );
        }
    }

    Entity* getPlayer() {
        return _player;
    }

    void setPlayer( ID id ) {
        if ( _allEntityContainer ) {
            Entity* player = dynamic_cast < Entity* > ( _allEntityContainer -> get( id ) );
            if ( player ) {
                _player = player;
                _playerID = id;
            }
        }
    }

    ID addEntity( Entity* e ) {
        if ( ( _allEntityContainer ) && ( e ) ) {
            return _allEntityContainer -> add( e );
        }
        return -1;
    }

    ID addBullet( Ammo* a ) {
        if ( ( _bulletContainer ) && ( a ) ) {
            return _bulletContainer -> add( a );
        }
        return -1;
    }

    Container* getAllEntity() {
        return _allEntityContainer;
    }

    static void Register() {
        Restorable::Register( INTERACTIVE_CLASS, new Builder() );
        Entity::Register();
    }

    private:

    Container* _allEntityContainer;
    Container* _bulletContainer;
    Container* _pickupContainer;
    Entity* _player;
    ID _playerID;

};


