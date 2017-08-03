#pragma once

#include "Perspective.h"

//Vector3D POS = Vector3D( -5.0, -15.0, -5.0 );
//PolarVector3D ANGLES = PolarVector3D( 1.0, radn( 135.0 ), radn( 45.0 ) );

class Level : public Perspective {
    public:
    Level( const char* levelsPath ) : Perspective( PERSPECTIVE_LEVEL_ID, new GlutTimer() ) { //
        _slf = StringLinker::FromFile( levelsPath );
        _currentWorld = NULL;
        _levelID = -1;
    }
    ~Level() {
        if ( _slf ) {
            delete _slf;
        }
    }

    virtual void onCreate() {
        glutSetCursor( GLUT_CURSOR_NONE );
        glutWarpPointer( glutGet( GLUT_WINDOW_WIDTH ) >> 1, glutGet( GLUT_WINDOW_HEIGHT ) >> 1 );
    }

    virtual void onDestroy() {
        glutSetCursor( GLUT_CURSOR_INHERIT );
        glutWarpPointer( glutGet( GLUT_WINDOW_WIDTH ) >> 1, glutGet( GLUT_WINDOW_HEIGHT ) >> 1 );
    }

    virtual void onTick() {
        glColor3d( 1.0, 1.0, 1.0 );
        if ( _currentWorld ) {
            Entity* player = _currentWorld -> getInteractive() -> getPlayer();
            if ( player ) {
                double lr = 0.0;
                double fw = 0.0;
                double P_SPEED = player -> getVar( EV_SPEED );
                if ( Keys::isStrucked( 27 ) ) {
                    Perspective::ChangeTo( PERSPECTIVE_MENU_ID );
                }
                if ( Keys::isPressed( 'x' ) ) {
                    P_SPEED = player -> getVar( EV_SPEED_SPRINT );
                }
                if ( Keys::isPressed( 'w' ) ) {
                    fw += P_SPEED;
                }
                if ( Keys::isPressed( 's' ) ) {
                    fw -= P_SPEED;
                }
                if ( Keys::isPressed( 'd' ) ) {
                    lr += P_SPEED;
                }
                if ( Keys::isPressed( 'a' ) ) {
                    lr -= P_SPEED;
                }
                if ( Keys::isStrucked( ' ' ) ) {
                    player -> jump( _currentWorld -> getMap(), player -> getVar( EV_JUMP ) );
                }
                if ( Keys::isStrucked( 'r' ) ) {
                    player -> reloadWeapon();
                }
                if ( Keys::isMousePressed( GLUT_LEFT_BUTTON ) ) {
                    //_currentWorld -> getInteractive() -> addBullet( new Ammo( AmmoType::getAmmoType( 1 ), player, player -> getHotSpot() + Vector3D( 0.0, -0.2, 0.0 ), Vector3D( *( player -> getAngles() ) ) + Vector3D( random( 0.2 ) - 0.1, random( 0.2 ) - 0.1, random( 0.2 ) - 0.1 ) ) );
                    //printf( "Shoot attempt.\n" );
                    _currentWorld -> getInteractive() -> addBullet( player -> shoot() );
                }
                if ( ( fw * lr ) != 0.0 ) {
                    fw *= M_SQRT1_2;
                    lr *= M_SQRT1_2;
                }
                player -> move( _currentWorld -> getMap(), fw, lr, _currentWorld -> getInteractive() -> getAllEntity() );// = Physics::Gravity( POS );
            }

            _currentWorld -> tick();
            _currentWorld -> display();

        } else {
            Perspective::Exception( "ERR NO WORLD\n" );
        }
    }

    bool changeLevel( string root, int levelID ) {
        if ( _slf ) {
            string path = root + _slf -> getString( levelID );
            if ( path != "" ) {
                // load level
                /// @todo
                if ( _currentWorld ) {
                    delete _currentWorld;
                    _currentWorld = NULL;
                }
                World* world = World::loadWorld( path.c_str() );
                if ( world ) {
                    _currentWorld = world;
                    _levelID = levelID;
                    Entity* player = _currentWorld -> getInteractive() -> getPlayer();
                    if ( player ) {
                        player -> cameraView();
                    }
                    return true;
                }
            } else {
                // throw
                Perspective::Exception( "FAILURE BAD PATH\n" );
            }
        } else {
            // throw
            Perspective::Exception( "FAILURE NO SLF\n" );
        }
        return false;
    }

    bool saveCurrentGame( string path ) {
        if ( ( _currentWorld ) && ( _levelID >= 0 ) ) {
            FileSerializationOutputStream* out = FileSerializationOutputStream::create( path.c_str() );
            if ( out ) {
                out -> add( _levelID );
                _currentWorld -> saveState( out );
                delete out;
                return true;
            }
        }
        return false;
    }

    bool loadCurrentGame( string path, string root ) {
        FileSerializationInputStream* in = FileSerializationInputStream::create( path.c_str() );
        if ( in ) {
            bool loaded = false;
            int id = in -> get < int > ();
            if ( changeLevel( root, id ) ) {
                if ( _currentWorld -> loadState( in ) ) {
                    loaded = true;
                    Entity* player = _currentWorld -> getInteractive() -> getPlayer();
                    if ( player ) {
                        player -> cameraView();
                    }
                }
            }
            delete in;
            return loaded;
        }
        return false;
    }

    void debugInitWeapon() {
        Entity* player = _currentWorld -> getInteractive() -> getPlayer();
        player -> getInventory() -> addWeapon( 12, new Weapon( WeaponType::getWeaponType( 1 ) ) );
        player -> getInventory() -> selectWeapon( 12 );
    }

    static void Register() {
        World::Register();
        Map::Register();
        Chunk::Register();
    }

    private:

    StringLinker* _slf;
    World* _currentWorld;
    int _levelID;

};


