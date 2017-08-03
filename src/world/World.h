#pragma once

#include "Map.h"
#include "Interactive.h"

#define WORLD_CLASS 11938

class World : public Restorable {
    protected:

    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        Restorable* build() {
            return new World();
        }
    };

    class DeleteOperation : public AbstractRestorableOperation {
        public:
        DeleteOperation() : AbstractRestorableOperation() {}
        ~DeleteOperation() {}
        void perform( Restorable* restorable ) {
            Container* cont = dynamic_cast < Container* > ( restorable );
            if ( cont ) {
                cont -> deleteAll();
            }
        }
    };

    class Sky {
        public:
        Sky( World* world, int imageID ) {
            _id = imageID;
            _skyTex = TextureHolder::getGlobalHolder() -> get( _id );
            _w = world;
        }
        ~Sky() {
        }
        static const int MODE_FULLSPHERE = 0x01;
        static const int MODE_FLAT = 0x02;
        void display( int mode ) {
            //if ( _skyTex ) {
                glPushMatrix();
                Entity* p = _w -> getInteractive() -> getPlayer();
                Vector3D* vec = p -> getPosition();
                _skyTex -> glUseImage();
                glEnable( GL_TEXTURE_2D );
                glPushAttrib( GL_ALL_ATTRIB_BITS );
                glDisable( GL_DEPTH_TEST );
                glTranslated( vec -> x, vec -> y, vec -> z );
                switch ( mode ) {
                    case MODE_FULLSPHERE: {
                        glRotated( 180.0, 0.0, 1.0, 0.0 );
                        glRotated( 180.0, 1.0, 0.0, 0.0 );
                        GLUquadric* quad = gluNewQuadric();
                        gluQuadricTexture( quad, GL_TRUE );
                        gluSphere( quad, 3.0, 128, 128 );
                        gluDeleteQuadric( quad );
                        break;
                    }
                    case MODE_FLAT: {
                        double h = 20.0;
                        double w = h * 4.0;
                        glBegin( GL_QUADS );
                            glTexCoord2d( 0.0, 0.0 );
                            glVertex3d( - w, h, - w );
                            glTexCoord2d( 1.0, 0.0 );
                            glVertex3d( w, h, - w );
                            glTexCoord2d( 1.0, 1.0 );
                            glVertex3d( w, h, w );
                            glTexCoord2d( 0.0, 1.0 );
                            glVertex3d( - w, h, w );
                        glEnd();
                        break;
                    }
                }
                glPopAttrib();
                glPopMatrix();
            //}
        }
        private:
        Texture* _skyTex;
        int _id;
        World* _w;
    };


    public:
    World() : Restorable( WORLD_CLASS ) {
        _map = NULL;
        _interactive = NULL;
        _sky = new Sky( this, 510 );
    }
    ~World() {
        if ( _map ) {
            delete _map;
        }
        if ( _interactive ) {
            delete _interactive;
        }
        if ( _sky ) {
            delete _sky;
        }
    }

    static void Register() {
        Restorable::Register( WORLD_CLASS, new Builder() );
        Map::Register();
        Interactive::Register();
        AmmoType::Register();
        WeaponType::Register();
        Ammo::Register();
        Weapon::Register();
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        Restorable::Serialize( _map, stream );
        Restorable::Serialize( _interactive, stream );
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        _map = Restorable::TryDeserializeThrow < Map > ( stream );
        _interactive = Restorable::TryDeserializeThrow < Interactive > ( stream );
        return true;
    }

    Map* getMap() {
        return _map;
    }

    Interactive* getInteractive() {
        return _interactive;
    }

    void tick() {
        if ( ( !_map ) || ( !_interactive ) ) {
            return;
        }
        _interactive -> tick( _map );
    }

    void display() {
        if ( ( !_map ) || ( !_interactive ) ) {
            return;
        }
        Camera::SetViewByMouse( Keys::getMouseX(), Keys::getMouseY(), Camera::NO_BOBBING );
        _sky -> display( Sky::MODE_FLAT );
        Camera::SetViewByMouse( Keys::getMouseX(), Keys::getMouseY(), 0 );
        _map -> useMapTexture();
        _map -> display();
        _interactive -> display();
        Camera::NextTick();
    }

    bool saveState( AbstractSerializationOutputStream* stream ) {
        return Restorable::Serialize( _interactive, stream );
    }

    bool loadState( AbstractSerializationInputStream* stream ) {
        Interactive* interactive = Restorable::TryDeserialize < Interactive > ( stream );
        if ( interactive ) {
            if ( _interactive ) {
                delete _interactive;
            }
            _interactive = interactive;
            return true;
        }
        return false;
    }

    static World* loadWorld( const char* path ) {
        FileSerializationInputStream* stream = FileSerializationInputStream::create( path );
        if ( stream ) {
            Map* m = Restorable::TryDeserialize < Map > ( stream );
            World* w = new World();
            w -> _map = m;
            w -> loadState( stream );
            delete stream;
            return w;
        }
        return NULL;
    }

    bool saveWorld( const char* path ) {
        FileSerializationOutputStream* stream = FileSerializationOutputStream::create( path );
        if ( stream ) {
            Restorable::Serialize( _map, stream );
            saveState( stream );
            delete stream;
            return true;
        }
        return false;
    }

    static World* CreateFromMap( Map* m, Interactive* ia ) {
        World* world = new World();
        world -> _map = m;
        world -> _interactive = ia;
        return world;
    }

    private:

    Map* _map;
    Interactive* _interactive;
    Sky* _sky;

};

#include "compatible/BioCoraMapLoader.h"
