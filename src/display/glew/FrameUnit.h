#pragma once

/**
    Defines wrapping class for FBO.
*/
class FrameUnit {
    public:
    /**
        Creates new frame unit.
        @param w Width of screen.
        @param h Height of screen.
    */
    FrameUnit( int w, int h ) {
        _w = w;
        _h = h;
        glGenFramebuffers( 1, &_frame );
        glBindFramebuffer( GL_FRAMEBUFFER, _frame );

        glGenTextures( 1, &_imggl );
        glBindTexture( GL_TEXTURE_2D, _imggl );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        _screen = Texture::CreateFromGlTexture( _imggl, _w, _h );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, _w, _h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );

        glGenRenderbuffers( 1, &_depthStorage );
        glBindRenderbuffer( GL_RENDERBUFFER, _depthStorage );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _w, _h );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthStorage );

        glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _imggl, 0 );

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );

        glGetFloatv( GL_VIEWPORT, _vp );
    }
    ~FrameUnit() {
        glDeleteFramebuffers( 1, &_frame );
        glDeleteTextures( 1, &_imggl );
        glDeleteRenderbuffers( 1, &_depthStorage );
        if ( _screen ) {
            delete _screen;
        }
    }

    /**
        Starts rendering to the wrapped FBO.
    */
    void openFrame() {
        glGetFloatv( GL_VIEWPORT, _vp );
        glViewport( 0, 0, _w, _h );

        glBindFramebuffer( GL_FRAMEBUFFER, _frame );
        glBindRenderbuffer( GL_RENDERBUFFER, _depthStorage );
        glBindTexture( GL_TEXTURE_2D, _imggl );

        GLenum DrawBuffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers( 1, DrawBuffers );
    }

    /**
        Ends rendering to the wrapped FBO.
    */
    void closeFrame() {
        glReadBuffer( GL_COLOR_ATTACHMENT0 );
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );

        glViewport( _vp[ 0 ], _vp[ 1 ], _vp[ 2 ], _vp[ 3 ] );
    }

    /**
        Renders the wrapped FBO on screen or to another FBO.
    */
    void renderFrame() {
        glClear( GL_COLOR_BUFFER_BIT );
        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
            glLoadIdentity();
            glMatrixMode( GL_MODELVIEW );
                glPushMatrix();
                glLoadIdentity();
                _screen -> glUseImage();
                glPushAttrib( GL_LIGHTING_BIT | GL_FOG_BIT );
                glColor3d( 1.0, 1.0, 1.0 );
                glDisable( GL_LIGHTING );
                glDisable( GL_FOG );
                glDisable( GL_DEPTH_TEST );
                glBegin( GL_QUADS );

                    glTexCoord2d( 0.0, 0.0 );
                    glVertex3d( -1.0, -1.0, -0.25 );

                    glTexCoord2d( 0.0, 1.0 );
                    glVertex3d( -1.0, 1.0, -0.25 );

                    glTexCoord2d( 1.0, 1.0 );
                    glVertex3d( 1.0, 1.0, -0.25 );

                    glTexCoord2d( 1.0, 0.0 );
                    glVertex3d( 1.0, -1.0, -0.25 );

                glEnd();
                glEnable( GL_DEPTH_TEST );
                glPopAttrib();
                glPopMatrix();
            glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
    }

    static FrameUnit* getDefault() {
        if ( !_default ) {
            replaceDefault( _W, _H );
        }
        return _default;
    }

    static void replaceDefault( int w, int h ) {
        if ( _default ) {
            delete _default;
        }
        _default = new FrameUnit( w, h );
    }

    static void disposeDefault() {
        if ( _default ) {
            delete _default;
            _default = NULL;
        }
    }

    Texture* getScreen() {
        _screen -> refreshPixelsFromScreen();
        return _screen;
    }

    static int getScreenWidth() {
        return _W;
    }

    static int getScreenHeight() {
        return _H;
    }

    static void setResolution( int w, int h ) {
        _W = w;
        _H = h;
    }

    static FrameUnit* CreateUnitFromSettings( int settingID ) {
        if ( settingID < 0 ) {
            return NULL;
        }
        disposeDefault();
        if ( settingID == 0 ) {
            setResolution( glutGet( GLUT_WINDOW_WIDTH ), glutGet( GLUT_WINDOW_HEIGHT ) );
        } else {
            int w;
            int h;
            if ( Settings::getSettings() -> getResolution( settingID - 1, &w, &h ) ) {
                setResolution( w, h );
            }
        }
        return getDefault();
    }

    private:

    int _w;
    int _h;
    GLuint _frame;
    GLuint _imggl;
    GLuint _depthStorage;

    Texture* _screen;

    static FrameUnit* _default;
    static int _W;
    static int _H;

    float _vp[ 4 ];

};

FrameUnit* FrameUnit::_default = NULL;
int FrameUnit::_W = 800;
int FrameUnit::_H = 800;


