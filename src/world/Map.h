#pragma once

#include "drawing/CubeDrawer.h"

#include "Chunk.h"
#include "TextureSet.h"
#include <math.h>

#define MAP_CLASS 11936
#define MAP_HEADER_CLASS 11940

#define CHUNK_SIZE_Y 8

class Map : public Restorable, public AbstractDisplay {
    protected:

    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        virtual ~Builder() {}
        Restorable* build() {
            return new Map();
        }
    };

    class Header : public Restorable {
        public:
        Header() : Restorable( MAP_HEADER_CLASS ) {
            sizeX = 0;
            sizeZ = 0;
            chunkSizeX = 0;
            chunkSizeY = 0;
            chunkSizeZ = 0;
            startX = 0.0;
            startZ = 0.0;
        }
        Header( int sX, int sZ, int chunkSX, int chunkSY, int chunkSZ ) : Restorable( MAP_HEADER_CLASS ) {
            sizeX = sX;
            sizeZ = sZ;
            chunkSizeX = chunkSX;
            chunkSizeY = chunkSY;
            chunkSizeZ = chunkSZ;
            startX = 0.0;
            startZ = 0.0;
        }
        virtual ~Header() {
        }
        bool onSerialize( AbstractSerializationOutputStream* stream ) {
            stream -> add( sizeX );
            stream -> add( sizeZ );
            stream -> add( chunkSizeX );
            stream -> add( chunkSizeY );
            stream -> add( chunkSizeZ );
            stream -> add( startX );
            stream -> add( startZ );
            return true;
        }
        bool onDeserialize( AbstractSerializationInputStream* stream ) {
            sizeX = stream -> get < int > ();
            sizeZ = stream -> get < int > ();
            chunkSizeX = stream -> get < int > ();
            chunkSizeY = stream -> get < int > ();
            chunkSizeZ = stream -> get < int > ();
            startX = stream -> get < double > ();
            startZ = stream -> get < double > ();
            return true;
        }
        int sizeX;
        int sizeZ;
        int chunkSizeX;
        int chunkSizeY;
        int chunkSizeZ;
        double startX;
        double startZ;
    };

    class HeaderBuilder : public RestorableBuilder {
        public:
        HeaderBuilder() : RestorableBuilder() {}
        virtual ~HeaderBuilder() {}
        Restorable* build() {
            return new Header();
        }
    };

    class ChunkCheckOperation : public AbstractRestorableOperation {
        public:
        ChunkCheckOperation() : AbstractRestorableOperation() {}
        virtual ~ChunkCheckOperation() {}
        void perform( Restorable* restorable ) {
            if ( restorable ) {
                if ( dynamic_cast < Chunk* > ( restorable ) == NULL ) {
                    char msg[ 4096 ];
                    sprintf( msg, "Instance [%p] is not a map chunk: inside map container (detected class %d)", restorable, restorable -> getClass() );
                    throw SaginaSerializationException( msg );
                }
            }
        }
    };

    public:
    Map() : Restorable( MAP_CLASS ), AbstractDisplay() {
        _header = NULL;
        _chunkContainer = NULL;
        _set = NULL;
        _holder = NULL;
    }
    virtual ~Map() {
        if ( _header ) {
            delete _header;
        }
        if ( _chunkContainer ) {
            _chunkContainer -> deleteAll();
            delete _chunkContainer;
        }
        clearTextureInfo();
    }

    static void Register() {
        Restorable::Register( MAP_CLASS, new Builder() );
        Restorable::Register( MAP_HEADER_CLASS, new HeaderBuilder() );
        TextureSet::Register();
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        Restorable::Serialize( _header, stream );
        Restorable::Serialize( _chunkContainer, stream );
        Restorable::Serialize( _set, stream );
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        _header = Restorable::TryDeserialize < Header > ( stream );
        _chunkContainer = Restorable::TryDeserialize < Container > ( stream );
        ChunkCheckOperation checkOperation;
        _chunkContainer -> forEach( &checkOperation );
        TextureSet* set = Restorable::TryDeserializeThrow < TextureSet > ( stream );
        setTextures( set );
        Vector3D pos = Vector3D( _header -> startX, 2.0, _header -> startZ );
        Camera::SetPosition( pos );
        //printf( "StartAt %lf %lf\n", pos.x, pos.z );
        return true;
    }

    Chunk* getChunk( int x, int z ) {
        if ( _chunkContainer ) {
            return ( Chunk* )( _chunkContainer -> get( _getID( x, z ) ) );
        }
        return NULL;
    }

    int getField( int x, int y, int z ) {
        if ( ( y < 0 ) || ( y >= _header -> chunkSizeY ) ) {
            return -1;
        }
        int cx = x / _header -> chunkSizeX;
        int cz = z / _header -> chunkSizeZ;
        int mx = x % _header -> chunkSizeX;
        int mz = z % _header -> chunkSizeZ;
        Chunk* c = getChunk( cx, cz );
        if ( c ) {
            return c -> getField( mx, y, mz );
        }
        return -1;
    }

    void setField( int x, int y, int z, int value ) {
        if ( ( y < 0 ) || ( y >= _header -> chunkSizeY ) ) {
            return;
        }
        int cx = x / _header -> chunkSizeX;
        int cz = z / _header -> chunkSizeZ;
        int mx = x % _header -> chunkSizeX;
        int mz = z % _header -> chunkSizeZ;
        Chunk* c = getChunk( cx, cz );
        if ( c ) {
            c -> setField( mx, y, mz, value );
        }
    }

    static Map* CreateMap( int mapSizeX, int mapSizeZ, int chunkSizeX, int chunkSizeZ ) {
        Map* m = new Map();
        int chunkX = ( int ) ceil( double( mapSizeX ) / double( chunkSizeX ) );
        int chunkZ = ( int ) ceil( double( mapSizeZ ) / double( chunkSizeZ ) );
        if ( !( m -> _chunkContainer ) ) {
            m -> _chunkContainer = new Container();
        }
        m -> _header = new Header( mapSizeX, mapSizeZ, chunkSizeX, CHUNK_SIZE_Y, chunkSizeZ );
        for ( int i = 0; i < chunkX; i++ ) {
            for ( int j = 0; j < chunkZ; j++ ) {
                m -> _setChunk( i, j, new Chunk( chunkSizeX, CHUNK_SIZE_Y, chunkSizeZ, i * chunkSizeX, j * chunkSizeZ ) );
            }
        }
        return m;
    }

    void startAt( double x, double z ) {
        //printf( "StartAt %lf %lf\n", x, z );
        _header -> startX = x;
        _header -> startZ = z;
    }

    void onDisplay() {
        map < ID, Restorable* >* m = _chunkContainer -> getMap();
        if ( m ) {
            map < ID, Restorable* >::iterator it = m -> begin();
            while ( it != m -> end() ) {
                Chunk* c = ( Chunk* ) it -> second;
                c -> onDisplay( _mapTexture );
                it++;
            }
        }
    }

    void setTextures( TextureSet* set ) {
        clearTextureInfo();
        _set = set;
        //printf( "[SET] = %p\n", set );
        if ( set ) {
            _holder = set -> buildHolder();
            _mapTexture = TextureHolder::Joined( _holder, CubeDrawer::textureCountX, CubeDrawer::textureCountY, 32, 32 );
        }
    }

    void useMapTexture() {
        if ( _mapTexture ) {
            _mapTexture -> glUseImage();
        }
    }

    void clearTextureInfo() {
        if ( _set ) {
            delete _set;
            _set = NULL;
        }
        if ( _holder ) {
            delete _holder;
            _holder = NULL;
        }
    }

    Vector3D checkDerivativeCollision( Vector3D point, Vector3D movement ) { /// PROBLEMS MAY SHOW UP AT MAP CORNERS
        Vector3D comb;
        Vector3D chck;
        Vector3D finalMovement = movement;
        Vector3D collsV = Vector3D( 1.0, 1.0, 1.0 );
        double* colls = collsV.getFields();
        static Vector3D axesCombination[ 7 ] = {
            Vector3D( 1.0, 0.0, 0.0 ),
            Vector3D( 0.0, 1.0, 0.0 ),
            Vector3D( 0.0, 0.0, 1.0 ),
            Vector3D( 1.0, 1.0, 0.0 ),
            Vector3D( 1.0, 0.0, 1.0 ),
            Vector3D( 0.0, 1.0, 1.0 ),
            Vector3D( 1.0, 1.0, 1.0 )
        };
        for ( int i = 0; i < 7; i++ ) {
            comb = axesCombination[ i ].multiplyElements( collsV );
            chck = point + ( movement.multiplyElements( comb ) );
            if ( pointCollides( chck ) ) {
                for ( int j = 0; j < 3; j++ ) {
                    colls[ j ] = ( 1.0 - comb.getFields()[ j ] ) * colls[ j ];
                }
            }
        }
        return finalMovement.multiplyElements( collsV );
    }

    bool pointCollides( Vector3D vec ) {
        return ( getField( vec.x, vec.y, vec.z ) > 0 );
    }

    /// @todo REMOVE

    /*void testDisplay() {
        map < ID, Restorable* >* m = _chunkContainer -> getMap();
        if ( m ) {
            map < ID, Restorable* >::iterator it = m -> begin();
            while ( it != m -> end() ) {
                glPushMatrix();
                Chunk* c = ( Chunk* ) it -> second;
                c -> testDisplay();
                glPopMatrix();
                it++;
            }
        }
    }*/
    private:

    Header* _header;
    Container* _chunkContainer;

    TextureHolder* _holder;
    TextureSet* _set;
    Texture* _mapTexture;

    void _setChunk( int x, int z, Chunk* chunk ) {
        if ( ( chunk ) && ( _chunkContainer ) ) {
            ID id = _getID( x, z );
            if ( !_chunkContainer -> get( id ) ) {
                _chunkContainer -> add( id, chunk );
            }
        }
    }

    ID _getID( int x, int z ) {
        return ( ( ( ID ) x ) << 32 ) + ( ( ID ) z );
    }

};

