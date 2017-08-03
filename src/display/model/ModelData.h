#pragma once

//#include "../../desolated/3D/Camera.h"

#include "BlockObject.h"

#include <list>
#include <map>

/**
    Contains block model info.
    Each model has structure of tree.
*/
class ModelData {
    protected:
    /**
        Single block of model. Can be cube or cuboid. Only one root block is allowed in model's tree structure, marking center of whole model.
    */
    class Block {
        public:
        /**
            Creates new instance of block. Newly created block does not append itself anywhere.
            @param prel Vector specifying relative position of this block.
            @param joint Vector specifying alignment point of this block, to parent block (if any).
            @param rot Rotation holder, keeps RPY angles coordinates specified for block.
            @param dim Scale of block.
            @param s Base size of block. Final size will appear as Vector( dim.x * s, dim.y * s, dim.z * s ).
            @param tid Texture ID to display.
        */
        Block( Vector3D prel, Vector3D joint, Vector3D rot, Vector3D dim, float s, int tid ) {
            _positionRelative = prel;
            _jointPoint = joint;
            _rotation = rot;
            _dimension = dim;
            _size = s;
            _textureID = tid;
        }
        ~Block() {
            list < Block* >::iterator it = _children.begin();
            while ( it != _children.end() ) {
                Block* td = ( *it );
                if ( td ) {
                    delete td;
                }
                it++;
            }
        }

        /**
            Displays this block on screen.
            @param textureID Specified texture ID, or -1 to use default (one specified in constructor.
        */
        void display( int textureID ) {
            glPushMatrix();
            glTranslatef( _positionRelative.x, _positionRelative.y, _positionRelative.z );
            glRotatef( _rotation.z, 0.0f, 0.0f, 1.0f );
            glRotatef( _rotation.x, 0.0f, 1.0f, 0.0f );
            glRotatef( _rotation.y, 1.0f, 0.0f, 0.0f );
            glTranslatef( -_jointPoint.x, -_jointPoint.y, -_jointPoint.z );
            //Texture* tex = SDS::TextureModel::getInstance() -> getTexture( _textureID );
            int blockObj = ( textureID < 0 ) ? _textureID : textureID;
            //BlockObject* obj = BlockObject::getBlockObject( blockObj );
            //if ( obj ) {
                Texture* tex = TextureHolder::getGlobalHolder() -> get( blockObj ); //obj -> getTextureID() );
                if ( tex ) {
                    //printf( "CUBE %d\n", _textureID );
                    if ( tex -> glUseImage() ) {
                        //printf( "CUBE %d\n", _textureID );
                        ___modelCube();
                    } else {
                        //printf( "ERR CAN'T USE %d\n", _textureID );
                    }
                } else {
                    //printf( "ERR NO TEXTURE %d\n", _textureID );
                }
            //}
            list < Block* >::iterator it = _children.begin();
            while ( it != _children.end() ) {
                Block* td = ( *it );
                if ( td ) {
                    td -> display( textureID );
                }
                it++;
            }
            glPopMatrix();
        }

        /**
            Appends block as child of this block.
            @param block Block to append as child.
        */
        void appendSubBlock( Block* block ) {
            _children.push_back( block );
        }

        /**
            Appends each block on list as child of this block.
            @param l List to merge. Acts like pointer to constant.
        */
        void appendSubBlock( list < Block* >* l ) {
            _children.merge( *l );
        }

        void translateFirstRelative() {
            glTranslatef( - _positionRelative.x, - _positionRelative.y, - _positionRelative.z );
        }

        private:
        void ___modelCube() {
            double hsize = double( _size ) / 2.0;
            Vector3D cubePoints[ 8 ] = {
                Vector3D( - hsize * _dimension.x, - hsize * _dimension.y, - hsize * _dimension.z ),
                Vector3D( - hsize * _dimension.x, - hsize * _dimension.y, hsize * _dimension.z ),
                Vector3D( - hsize * _dimension.x, hsize * _dimension.y, - hsize * _dimension.z ),
                Vector3D( - hsize * _dimension.x, hsize * _dimension.y, hsize * _dimension.z ),
                Vector3D( hsize * _dimension.x, - hsize * _dimension.y, - hsize * _dimension.z ),
                Vector3D( hsize * _dimension.x, - hsize * _dimension.y, hsize * _dimension.z ),
                Vector3D( hsize * _dimension.x, hsize * _dimension.y, - hsize * _dimension.z ),
                Vector3D( hsize * _dimension.x, hsize * _dimension.y, hsize * _dimension.z )
            };
            int sequences[ 6 ][ 4 ] = {
                { 6, 2, 0, 4 },
                { 3, 7, 5, 1 },
                { 7, 6, 4, 5 },
                { 2, 3, 1, 0 },
                { 2, 6, 7, 3 },
                { 4, 0, 1, 5 }
            };
            float texCoord[ 4 ][ 2 ] = {
                { 0.0, 0.0 },
                { 0.0, 1.0 },
                { 1.0, 1.0 },
                { 1.0, 0.0 }
            };
            glBegin( GL_QUADS );
                for ( int is = 0; is < 6; is++ ) {
                    for ( int i = 0; i < 4; i++ ) {
                        glTexCoord2f( texCoord[ i ][ 0 ], texCoord[ i ][ 1 ] );
                        Vector3D point = cubePoints[ sequences[ is ][ i ] ];
                        glVertex3f( point.x, point.y, point.z );
                    }
                }
            glEnd();
        }

        list < Block* > _children;
        Vector3D _positionRelative;
        Vector3D _jointPoint;
        Vector3D _rotation;
        Vector3D _dimension;
        float _size;
        int _textureID;
    };
    /**
        Used when something is wrong with parsed file.
    */
    class ModelParseException {
        public:
        /**
            Created instance of ModelParseException.
            @param handle In which file created exception occurred.
            @param msg Additional message to print. @see print()
        */
        ModelParseException( FILE* handle, const char* msg ) {
            _msg = new string( msg );
            _handle = handle;
        }
        ~ModelParseException() {
            delete _msg;
        }
        /**
            Displays description of error.
        */
        void print() {
            printf( "ModelParseException [%s] with handle [%p]\n", _msg -> c_str(), _handle );
        }
        private:
        string* _msg;
        FILE* _handle;
    };
    /**
        Constructs model.
        @param root Root block in tree structure. Must contain all needed children until this point.
        @param points Collision points specified for model.
    */
    ModelData( Block* root, CollisionPoints* points ) {
        _root = root;
        _collision = points;
        _collision -> addPoint( Vector3D( 0.0, 0.0, 0.0 ) );
        //_imageView = NULL;
        //_createdImage = false;
        _imageView = NULL;
        //_createImageView( 256, 256 );
        _createdImage = true;
    }
    public:
    /**
        Loads models from path. Models must be defined using special language. This method fails over if any exception occurs during loading process.
        @see BlockModel::loadModels()
        @param path File which contains model data.
        @return new instance of map, containing all models from file, or NULL on error or when file has been not found.
    */
    static map < int, ModelData* >* getModelsFrom( const char* path ) {
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            map < int, ModelData* >* m = new map < int, ModelData* >();
            // parse
            try {
                bool parse = true;
                while ( parse ) {
                    char buffer[ 4096 ];
                    char errorbuffer[ 4096 ];
                    if ( fscanf( handle, "%s", buffer ) <= 0 ) {
                        parse = false;
                    }
                    if ( parse ) {
                        if ( strcmp( buffer, "MODEL" ) == 0 ) {
                            int key;
                            ___doParseInteger( handle, &key );
                            CollisionPoints* points = ___doParseCollisionPoints( handle );
                            pair < int, ModelData* > p;
                            p.first = key;
                            p.second = new ModelData( ___doParseModel( handle ), points );
                            m -> insert( p );
                        } else {
                            sprintf( errorbuffer, "error: unknown keyword: \"%s\".", buffer );
                            throw new ModelParseException( handle, errorbuffer );
                        }
                    }
                }
            } catch ( ModelParseException* e ) {
                e -> print();
                delete e;
                map < int, ModelData* >::iterator it = m -> begin();
                while ( it != m -> end() ) {
                    delete it -> second;
                    it++;
                }
                delete m;
                m = NULL;
            }
            // parse end
            fclose( handle );
            //printf( "Model %p\n", m );
            return m;
        }
        return NULL;
    }
    ~ModelData() {
        if ( _root ) {
            delete _root;
        }
        if ( _collision ) {
            delete _collision;
        }
        if ( _imageView ) {
            delete _imageView;
        }
    }
    /**
        Displays model on screen. Calls recursive for blocks in tree structure, starting from tree's root.
        @param textureID [optional] Special ID of texture, or -1 to use current block's texture.
    */
    void display( int textureID = -1 ) {
        if ( _root ) {
            /// @todo enable later
            //glEnable( GL_TEXTURE_2D );
            glColor3d( 1.0, 1.0, 1.0 );
            _root -> display( textureID );
            //glDisable( GL_TEXTURE_2D );
        }
    }

    /**
        Gets collision point array of model.
        @return collision point array of model.
    */
    CollisionPoints* getCollisionPoints() {
        return _collision;
    }

    /**
        Should get rendered image of the texture.
        @return rendered icon-like image.
    */
    Texture* getImageView() {
        /*if ( !_createdImage ) {
            _createImageView( 128, 128 );
            _createdImage = true;
        }
        printf( "ImageView = [%p]\n", _imageView );*/
        return _imageView;
    }
    private:
    /*void _createImageView( int w, int h ) {
        // prepare
        glEnable( GL_TEXTURE_2D );

        GLuint frame = 0;
        glGenFramebuffers( 1, &frame );
        glBindFramebuffer( GL_FRAMEBUFFER, frame );

        GLuint imggl = 0;
        glGenTextures( 1, &imggl );

        glBindTexture( GL_TEXTURE_2D, imggl );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        _imageView = Texture::createFromGlTexture( imggl, w, h );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );

        GLuint depthStorage = 0;
        glGenRenderbuffers( 1, &depthStorage );
        glBindRenderbuffer( GL_RENDERBUFFER, depthStorage );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStorage );

        glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, imggl, 0 );

        GLenum DrawBuffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers( 1, DrawBuffers );

        if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
            printf( "[DEBUG] ERROR OCCURED\n" );
        }

        int viewPort[ 4 ];
        glGetIntegerv( GL_VIEWPORT, viewPort );

        glViewport( 0, 0, w, h );

        // render

        glMatrixMode( GL_PROJECTION );

            glPushMatrix();

            glLoadIdentity();
            gluPerspective( 60.0, 1.0, 0.1, 2000.0 );

            glMatrixMode( GL_MODELVIEW );

            glPushMatrix();

                glClearColor( 0.3, 0.3, 0.3, 0.0 );
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                glLoadIdentity();
                glTranslated( 0.0, 0.0, -1.2 );
                glRotated( 150.0, 0.0, 1.0, 0.0 );
                _root -> translateFirstRelative();

                display();

            glPopMatrix();

            glMatrixMode( GL_PROJECTION );

        glPopMatrix();

        glMatrixMode( GL_MODELVIEW );

        // out

        glReadBuffer( frame );
        _imageView -> refreshPixelsFromScreen();
        _imageView -> makeRoundBorders( 0.2 );

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport( viewPort[ 0 ], viewPort[ 1 ], viewPort[ 2 ], viewPort[ 3 ] );

        glDeleteFramebuffers( 1, &frame );

    }*/
    static Block* ___doParseModel( FILE* handle ) {
        float s;
        int tid;
        Vector3D prel;
        Vector3D joint;
        Vector3D rot;
        Vector3D dim;
        int childCount;
        char buffer[ 4096 ];
        if ( fscanf( handle, "%s", buffer ) == 0 ) {
            throw new ModelParseException( handle, "parseModel: incorrect fscanf" );
        }
        if ( strcmp( buffer, "BLOCK" ) == 0 ) {
            ___doParseFloating( handle, &s );
            ___doParseInteger( handle, &tid );
            ___doParseVector( handle, &dim );
            ___doParseVector( handle, &prel );
            ___doParseVector( handle, &joint );
            ___doParseVector( handle, &rot );
            ___doParseInteger( handle, &childCount );
            Block* root = new Block( prel, joint, rot, dim, s, tid );
            try {
                for ( int i = 0; i < childCount; i++ ) {
                    Block* subBlock = ___doParseModel( handle );
                    root -> appendSubBlock( subBlock );
                }
            } catch ( ModelParseException* e ) {
                delete root;
                throw e;
            }
            return root;
        } else {
            char errorbuffer[ 4096 ];
            sprintf( errorbuffer, "parseModel: unknown keyword \"%s\".", buffer );
            throw new ModelParseException( handle, errorbuffer );
        }
    }
    static CollisionPoints* ___doParseCollisionPoints( FILE* handle ) {
        char buffer[ 4096 ];
        int fOffset = ftell( handle );
        if ( fscanf( handle, "%s", buffer ) == 0 ) {
            //throw new ModelParseException( handle, "parseModel: incorrect fscanf" );
            return new CollisionPoints( Vector3D(), Vector3D() );
        }
        if ( strcmp( buffer, "COLLISION_VERTEX_LIST" ) == 0 ) {
            Vector3D vecDimPoints;
            Vector3D vecDimCenter;
            ___doParseVector( handle, &vecDimPoints );
            ___doParseVector( handle, &vecDimCenter );
            CollisionPoints* collision = new CollisionPoints( vecDimPoints, vecDimCenter );
            //double bottom = - vecDimPoints.y / 2.0;
            //double top = vecDimPoints.y / 2.0;
            //printf( "Bottom = %lf, Top = %lf\n", bottom, top );
            Vector3D vi = Vector3D(
                vecDimPoints.x + vecDimCenter.x,
                vecDimCenter.y,
                vecDimPoints.z + vecDimCenter.z
            );
            /*for ( double iy = bottom; iy < top; iy += 0.5 ) {
                vi.y += iy;
                vi.x = - vi.x;
                collision -> addPoint( vi );
                vi.z = - vi.z;
                collision -> addPoint( vi );
                vi.x = - vi.x;
                collision -> addPoint( vi );
                vi.z = - vi.z;
                collision -> addPoint( vi );
                //printf( "[ADDING] iy = %lf\n", vi.y );
            }*/
            vi.y = 0.0;//+= top;
            collision -> addPoint( vi );
            vi.z = - vi.z;
            collision -> addPoint( vi );
            vi.x = - vi.x;
            collision -> addPoint( vi );
            vi.z = - vi.z;
            collision -> addPoint( vi );
            /*for ( int i = 0; i < vertexCount; i++ ) {
                Vector3D v;
                ___doParseVector( handle, &v );
                //printf( "COLLISION_VERTEX_LIST ADD: %g %g %g\n", v.x, v.y, v.z );
                collision -> addPoint( v );
            }*/
            return collision;
        }
        fseek( handle, fOffset, SEEK_SET );
        return new CollisionPoints( Vector3D(), Vector3D() );
    }
    static void ___doParseVector( FILE* handle, Vector3D* vec ) {
        int n;
        char bracket[ 2 ];
        if ( ( n = fscanf( handle, " %c %lg %lg %lg %c ", &bracket[ 0 ], &( vec -> x ), &( vec -> y ), &( vec -> z ), &bracket[ 1 ] ) ) == 5 ) {
            if ( ( bracket[ 0 ] != '(' ) || ( bracket[ 1 ] != ')' ) ) {
                printf( "ERROR PARSING: brackets does not match\n" );
                throw new ModelParseException( handle, "parseVector: incorrect fscanf" );
            }
            return;
        }
        printf( "ERROR PARSING: %d parsed count.\n", n );
        throw new ModelParseException( handle, "parseVector: incorrect fscanf" );
    }
    static void ___doParseInteger( FILE* handle, int* val ) {
        int n;
        if ( ( n = fscanf( handle, "%d", val ) ) == 1 ) {
            return;
        }
        printf( "ERROR PARSING: %d parsed count.\n", n );
        throw new ModelParseException( handle, "parseInteger: incorrect fscanf" );
    }
    static void ___doParseFloating( FILE* handle, float* val ) {
        int n;
        if ( ( n = fscanf( handle, "%f", val ) ) == 1 ) {
            return;
        }
        printf( "ERROR PARSING: %d parsed count.\n", n );
        throw new ModelParseException( handle, "parseFloating: incorrect fscanf" );
    }
    Block* _root;
    CollisionPoints* _collision;
    Texture* _imageView;
    bool _createdImage;
};
