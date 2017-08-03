#pragma once

class AbstractComponent {
    public:
    AbstractComponent( double x, double y, double w, double h ) {
        _x = x;
        _y = y;
        _w = w;
        _h = h;
        _title = string( "" );
        _active = false;
        _tActive = 0;
        _componentOver = NULL;
        _componentPressed = NULL;
    }
    AbstractComponent( double x, double y, double w, double h, string title ) {
        _x = x;
        _y = y;
        _w = w;
        _h = h;
        _title = title;
        _active = false;
        _tActive = 0;
        _componentOver = NULL;
    }
    virtual ~AbstractComponent() {
        for ( int i = 0; i < ( int ) _childList.size(); i++ ) {
            delete _childList[ i ];
        }
    }

    virtual void onDisplay() = 0;
    virtual void onMousePressed( int button, int x, int y ) = 0;
    virtual void onMouseReleased( int button, int x, int y ) = 0;
    virtual void onMouseReleasedNoClick( int button, int x, int y ) = 0;
    virtual void onMouseIn( int x, int y ) = 0;
    virtual void onMouseOut( int x, int y ) = 0;
    virtual void onReset() = 0;

    static void DisplayGuiTexture( Texture* tex, double x, double y, double w, double h ) {
        if ( !tex ) {
            return;
        }
        glEnable( GL_TEXTURE_2D );
        glDisable( GL_DEPTH_TEST );
        tex -> glUseImage();
        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();
        double xd = x * 2.0 - 1.0;
        double yd = y * 2.0 - 1.0;
        double wd = xd + w * 2.0;
        double hd = yd + h * 2.0;
        glBegin( GL_QUADS );
            glTexCoord2d( 0.0, 0.0 );
            glVertex3d( xd, - yd, -1.0 );
            glTexCoord2d( 1.0, 0.0 );
            glVertex3d( wd, - yd, -1.0 );
            glTexCoord2d( 1.0, 1.0 );
            glVertex3d( wd, - hd, -1.0 );
            glTexCoord2d( 0.0, 1.0 );
            glVertex3d( xd, - hd, -1.0 );
        glEnd();
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
        glPopMatrix();
        glEnable( GL_DEPTH_TEST );
    }

    void display() {
        onDisplay();
        for ( int i = 0; i < ( int ) _childList.size(); i++ ) {
            _childList[ i ] -> display();
        }
    }

    void reset() {
        onReset();
        for ( int i = 0; i < ( int ) _childList.size(); i++ ) {
            _childList[ i ] -> reset();
        }
    }

    AbstractComponent* getComponentOver( double mx, double my ) {
        for ( int i = 0; i < ( int ) _childList.size(); i++ ) {
            if ( _childList[ i ] -> getComponentOver( mx, my ) ) {
                return _childList[ i ];
            }
        }
        if ( mouseOver( mx, my ) ) {
            return this;
        }
        return NULL;
    }

    AbstractComponent* mousePressed( int button, int x, int y ) {
        double mx = double( x ) / double( glutGet( GLUT_WINDOW_WIDTH ) );
        double my = double( y ) / double( glutGet( GLUT_WINDOW_HEIGHT ) );
        //printf( "mx = %g, my = %g\n", mx, my );
        _componentPressed = getComponentOver( mx, my );
        if ( _componentPressed ) {
            _componentPressed -> onMousePressed( button, x, y );
        }
        return _componentPressed;
    }

    void mouseReleased( int button, int x, int y ) {
        double mx = double( x ) / double( glutGet( GLUT_WINDOW_WIDTH ) );
        double my = double( y ) / double( glutGet( GLUT_WINDOW_HEIGHT ) );
        AbstractComponent* componentOver = getComponentOver( mx, my );
        if ( ( componentOver == _componentPressed ) && ( _componentPressed != NULL ) ) {
            componentOver -> onMouseReleased( button, x, y );
            componentOver -> onMouseIn( x, y );
        } else {
            if ( _componentPressed ) {
                _componentPressed -> onMouseReleasedNoClick( button, x, y );
            }
        }
        _componentPressed = NULL;
    }

    void mouseOverTree( int x, int y ) {
        double mx = double( x ) / double( glutGet( GLUT_WINDOW_WIDTH ) );
        double my = double( y ) / double( glutGet( GLUT_WINDOW_HEIGHT ) );
        AbstractComponent* componentOver = getComponentOver( mx, my );
        if ( componentOver != _componentOver ) {
            if ( _componentOver ) {
                _componentOver -> onMouseOut( x, y );
            }
            if ( componentOver ) {
                componentOver -> onMouseIn( x, y );
            }
            _componentOver = componentOver;
        }
    }

    void addComponent( AbstractComponent* a ) {
        if ( a ) {
            _childList.push_back( a );
        }
    }

    virtual bool mouseOver( double x, double y ) {
        if ( ( x >= _x ) && ( y >= _y ) && ( x < _x + _w ) && ( y < _y + _h ) ) {
            return true;
        }
        return false;
    }

    double getComponentPosX() {
        return _x;
    }

    double getComponentPosY() {
        return _y;
    }

    double getComponentWidth() {
        return _w;
    }

    double getComponentHeight() {
        return _h;
    }

    void setComponentTranslation( double x, double y ) {
        _x = x;
        _y = y;
    }

    string getTitle() {
        return _title;
    }

    void setTitle( string title ) {
        _title = title;
    }

    void activate() {
        _active = true;
        _tActive = SDL_GetTicks();
    }

    void deactivate() {
        _active = false;
    }

    bool isActive() {
        return _active;
    }

    Uint32 getTimeActivated() {
        return _tActive;
    }

    private:

    //SDL_Surface* _surf;
    double _x;
    double _y;
    double _w;
    double _h;
    string _title;

    bool _active;
    Uint32 _tActive;

    vector < AbstractComponent* > _childList;
    AbstractComponent* _componentPressed;
    AbstractComponent* _componentOver;
};


