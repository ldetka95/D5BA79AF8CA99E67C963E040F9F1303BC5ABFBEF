#pragma once

#include "../core/common/Timer.h"
#include "../core/common/TimerFactory.h"

/** CUSTOM DEFINITIONS */

#define PERSPECTIVE_LEVEL_ID 1

#define PERSPECTIVE_MENU_ID 2
#define PERSPECTIVE_MENU_OPTIONS_ID 3

/** CUSTOM DEFINITIONS END */

class Perspective {
    protected:
    Perspective( int perspectiveID, Timer* timer ) {
        _perspectiveID = perspectiveID;
        _timer = timer;
        if ( _timer ) {
            _timer -> setTime();
        }
        _ct = 0;
    }

    Perspective( int perspectiveID, TimerFactory* tf ) {
        _perspectiveID = perspectiveID;
        if ( tf ) {
            _timer = tf -> buildTimer();
        } else {
            _timer = NULL;
        }
        if ( _timer ) {
            _timer -> setTime();
        }
        _ct = 0;
    }

    public:
    virtual ~Perspective() {
        if ( _timer ) {
            delete _timer;
        }
    }

    static Perspective* Setup( Perspective* perspective ) {
        if ( !perspective ) {
            return NULL;
        }
        map < int, Perspective* >::iterator found = _map.find( perspective -> _perspectiveID );
        if ( found == _map.end() ) {
            pair < int, Perspective* > p;
            p.first = perspective -> _perspectiveID;
            p.second = perspective;
            _map.insert( p );
            return perspective;
        }
        return NULL;
    }

    virtual void onCreate() = 0;
    virtual void onDestroy() = 0;
    virtual void onTick() = 0;

    int getPerspectiveID() {
        return _perspectiveID;
    }

    static Perspective* Current() {
        return _current;
    }

    static void ChangeTo( int npID ) {
        if ( _current ) {
            _current -> onDestroy();
            _current = NULL;
        }
        map < int, Perspective* >::iterator found = _map.find( npID );
        if ( found != _map.end() ) {
            _current = found -> second;
        }
        if ( _current ) {
            _current -> onCreate();
        }
    }

    void tick( double fps ) {
        /*static int cnt = 0;
        static int fpss = 0;
        static int t = glutGet( GLUT_ELAPSED_TIME );
        cnt++;
        if ( glutGet( GLUT_ELAPSED_TIME ) > t + 1000 ) {
            t = glutGet( GLUT_ELAPSED_TIME );
            fpss = cnt;
            cnt = 0;
            printf( "FPS: %d\n", fpss );
        }*/
        if ( _timer ) {
            _timer -> setTime();
            onTick();
            int ms = 1000.0 / fps;
            while ( !_timer -> hasPassed( ms ) );
            _ct += ( _timer -> getTime() - _ctOut );
        } else {
            onTick();
        }
    }

    static Perspective* GetPerspective( int id ) {
        map < int, Perspective* >::iterator found = _map.find( id );
        if ( found != _map.end() ) {
            return found -> second;
        }
        return NULL;
    }

    int currentTime() {
        if ( _timer ) {
            return _ct;
        }
        return -1;
    }

    static void Exception( const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsprintf( buffer, fmt, args );
        va_end( args );
        throw SaginaException( buffer );
    }

    private:

    int _perspectiveID;
    Timer* _timer;
    int _ct;
    int _ctOut;

    static Perspective* _current;
    static map < int, Perspective* > _map;

};

Perspective* Perspective::_current = NULL;
map < int, Perspective* > Perspective::_map;

