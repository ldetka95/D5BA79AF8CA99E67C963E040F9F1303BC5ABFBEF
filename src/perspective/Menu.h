#pragma once

#include "Perspective.h"

#include <stack>

#define FORM_OPTIONS_ID 101

class Menu : public Perspective {
    private:
    static void MENU_BUTTON_START( Button* b, int button ) {
        if ( button == GLUT_LEFT_BUTTON ) {
            Perspective::ChangeTo( PERSPECTIVE_LEVEL_ID );
        }
    }
    static void MENU_BUTTON_LOAD( Button* b, int button ) {
        if ( button == GLUT_LEFT_BUTTON ) {
            Level* level = dynamic_cast < Level* > ( Perspective::GetPerspective( PERSPECTIVE_LEVEL_ID ) );
            if ( level ) {
                if ( level -> loadCurrentGame( "data/saveTest/1.sav", "data/map/" ) ) {
                    Perspective::ChangeTo( PERSPECTIVE_LEVEL_ID );
                }
            }
        }
    }
    static void MENU_BUTTON_OPTIONS( Button* b, int button ) {
        if ( button == GLUT_LEFT_BUTTON ) {
            Menu* m = dynamic_cast < Menu* > ( Perspective::Current() );
            if ( m ) {
                m -> moveIntoForm( FORM_OPTIONS_ID );
            }
        }
    }
    static void MENU_BUTTON_EXIT( Button* b, int button ) {
        if ( button == GLUT_LEFT_BUTTON ) {
            Level* level = dynamic_cast < Level* > ( Perspective::GetPerspective( PERSPECTIVE_LEVEL_ID ) );
            if ( level ) {
                level -> saveCurrentGame( "data/saveTest/1.sav" );
            }
            exit( 0 );
        }
    }
    static void MENU_BUTTON_RETURN( Button* b, int button ) {
        if ( button == GLUT_LEFT_BUTTON ) {
            Menu* m = dynamic_cast < Menu* > ( Perspective::Current() );
            if ( m ) {
                m -> backForm();
            }
        }
    }
    public:
    Menu( int startForm, const char* slfConfig ) : Perspective( PERSPECTIVE_MENU_ID, ( Timer* ) NULL ) {
        //_root = new Form( 0.0, 0.0, 1.0, 1.0, "" );
        //_root -> addComponent( new Button( 0.5, 0.5, 0.5, 0.25, "What the fuck did you just fucking say about me, you little bitch? I’ll have you know I graduated top of my class in the Navy Seals, and I’ve been involved in numerous secret raids on Al-Quaeda, and I have over 300 confirmed kills." ) );
        //_root -> addComponent( new Button( 0.5, 0.75, 0.5, 0.25, "1234567890" ) );
        //_root -> addComponent( new Button( 0.5, 0.25, 0.5, 0.25, "a" ) );
        //double aspect = double( glutGet( GLUT_WINDOW_WIDTH ) ) / double( glutGet( GLUT_WINDOW_HEIGHT ) ) / 0.75;
        //double stdh = 0.09;
        //double stdw = stdh * double( Button::BgWidth() ) / double( Button::BgHeight() ) / aspect;
        //double xStringTranslation = 0.25;
        //double yStringTranslation = 0.0;
        //double stringBorderOffset = 0.33;
        //_root -> addComponent( new Button( 0.1, 0.05, stdw, stdh, "NEW GAME   ", xStringTranslation, yStringTranslation, stringBorderOffset, MENU_START, 1 ) );
        //_root -> addComponent( new Button( 0.1, 0.15, stdw, stdh, "LOAD GAME  ", xStringTranslation, yStringTranslation, stringBorderOffset, MENU_LOAD, 2 ) );
        //_root -> addComponent( new Button( 0.1, 0.25, stdw, stdh, "OPTIONS   ", xStringTranslation, yStringTranslation, stringBorderOffset, MENU_OPTIONS, 3 ) );
        //_root -> addComponent( new Button( 0.05, 0.80, stdw, stdh, "EXIT      ", xStringTranslation, yStringTranslation, stringBorderOffset, MENU_EXIT, 4 ) );
        StringLinker* slf = StringLinker::FromFile( slfConfig );
        if ( slf ) {
            for ( int i = 0; i < slf -> entryCount(); i++ ) {
                int id = slf -> getEntryID( i );
                string s = slf -> getEntryString( i );
                Form* f = _BuildFromFile( s.c_str() );
                if ( f ) {
                    _formMap[ id ] = f;
                }
            }
            delete slf;
        }
        setForm( startForm );
    }
    ~Menu() {
        //delete _root;
        map < int, Form* >::iterator it = _formMap.begin();
        while ( it != _formMap.end() ) {
            delete it -> second;
            it++;
        }
    }

    void onCreate() {
    }

    void onDestroy() {
    }

    void onTick() {
        Form* root = getCurrentForm();
        if ( root ) {
            int x = *Keys::getMouseX();
            int y = *Keys::getMouseY();
            if ( Keys::isMouseClicked( GLUT_LEFT_BUTTON ) ) {
                root -> mousePressed( GLUT_LEFT_BUTTON, x, y );
            }
            if ( Keys::isMouseReleased( GLUT_LEFT_BUTTON ) ) {
                root -> mouseReleased( GLUT_LEFT_BUTTON, x, y );
            }
            root -> mouseOverTree( x, y );
            root -> display();
        }
    }

    Form* getCurrentForm() {
        map < int, Form* >::iterator found = _formMap.find( _currentForm );
        if ( found != _formMap.end() ) {
            return found -> second;
        }
        return NULL;
    }

    void setForm( int id ) {
        _currentForm = id;
        Form* now = getCurrentForm();
        if ( now ) {
            now -> reset();
        }
    }

    void moveIntoForm( int id ) {
        _formStack.push( _currentForm );
        setForm( id );
    }

    void backForm() {
        if ( !_formStack.empty() ) {
            setForm( _formStack.top() );
            _formStack.pop();
        }
    }

    private:

    static Form* _BuildFromFile( const char* file ) {
        FILE* handle = fopen( file, "r" );
        if ( handle ) {
            Form* f = new Form( 0.0, 0.0, 1.0, 1.0, "" );
            while ( !feof( handle ) ) {
                AbstractComponent* comp = NULL;
                int compID;
                if ( fscanf( handle, "%d", &compID ) == 1 ) {
                    switch ( compID ) {
                        case 1: { /// button
                            int order;
                            double x, y, h;
                            char text[ 1024 ];
                            double xt, yt, bo;
                            int logic;
                            if ( fscanf( handle, "%d %lg %lg %lg %s %lg %lg %lg %d", &order, &x, &y, &h, text, &xt, &yt, &bo, &logic ) == 9 ) { // wow
                                double aspect = double( glutGet( GLUT_WINDOW_WIDTH ) ) / double( glutGet( GLUT_WINDOW_HEIGHT ) ) / 0.75;
                                double w = h * double( Button::BgWidth() ) / double( Button::BgHeight() ) / aspect;
                                int j = strlen( text );
                                for ( int i = 0; i < j; i++ ) {
                                    if ( text[ i ] == '~' ) {
                                        text[ i ] = ' ';
                                    }
                                }
                                comp = new Button( x, y, w, h, text, xt, yt, bo, _LOGIC[ logic ], order );
                            }
                            break;
                        }
                    }
                }
                if ( comp ) {
                    f -> addComponent( comp );
                }
            }
            fclose( handle );
            return f;
        }
        return NULL;
    }

    map < int, Form* > _formMap;
    int _currentForm;
    stack < int > _formStack;

    static void ( *_LOGIC[] )( Button*, int );

};

void ( *Menu::_LOGIC[] )( Button*, int ) = {
        MENU_BUTTON_START,
        MENU_BUTTON_LOAD,
        MENU_BUTTON_OPTIONS,
        MENU_BUTTON_EXIT,
        MENU_BUTTON_RETURN
    };
