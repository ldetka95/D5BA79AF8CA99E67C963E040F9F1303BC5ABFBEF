#pragma once

#include "../../core/common/Timer.h"

class Camera {
    private:
    Camera() {
    }
    ~Camera() {
    }
    public:

    static const int NO_BOBBING = 0x00000001;

    static void SetView( int flags ) {
        double movDif = 0.0;
        double movDifY = 0.0;
        if ( _pos ) {
            movDifY = absf( _pos -> y - _oldPos.y );
            _oldPos.y = _pos -> y;
            movDif = _pos -> dist3D( _oldPos );
        }

        glEnable( GL_FOG );

        GLfloat color[ 3 ] = { 0.0, 0.0, 0.0 };
        glFogf( GL_FOG_DENSITY, 0.05 );
        glFogfv( GL_FOG_COLOR, color );

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluPerspective( _fov, _aspect * double( FrameUnit::getScreenWidth() ) / double( FrameUnit::getScreenHeight() ), 0.001, 1000.0 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        if ( ( flags & NO_BOBBING ) != NO_BOBBING ) {
            double sinOffset = sin( double( glutGet( GLUT_ELAPSED_TIME ) ) / 1000.0 * M_PI * 4.0 ) * movDif;
            glTranslated( sinOffset, - absf( sinOffset ) + movDifY, 0.0 );
        }
        if ( _angles ) {
            glRotated( _zRotation, 0.0, 0.0, 1.0 );
            glRotated( degr( _angles -> a ), 1.0, 0.0, 0.0 );
            glRotated( degr( _angles -> i ), 0.0, 1.0, 0.0 );
        }
        if ( _pos ) {
            glTranslated( -_pos -> x, -_pos -> y, -_pos -> z );
        }
    }

    static void NextTick() {
        _oldPos = ( *_pos );
    }

    static void Link( Vector3D* pos, PolarVector3D* angles ) {
        _pos = pos;
        _oldPos = ( *pos );
        _angles = angles;
    }

    static void SetViewByMouse( int* x, int* y, int flags ) {
        if ( ( _angles ) && ( _movementEnabled ) ) {
            double di = 0.0;
            double da = 0.0;
            if ( _lastMouseX >= 0 ) {
                di = _hSpeed * double( *x - _lastMouseX ) * 0.001;
                _angles -> i += di;
            }
            if ( _lastMouseY >= 0 ) {
                da = _vSpeed * double( *y - _lastMouseY ) * 0.001;
                _angles -> a += da;
                if ( _angles -> a < _verticalMin ) {
                    _angles -> a = _verticalMin;
                }
                if ( _angles -> a > _verticalMax ) {
                    _angles -> a = _verticalMax;
                }
            }
        }
        _lastMouseX = *x;
        _lastMouseY = *y;

        SetView( flags );

        if ( _movementEnabled ) {
            ___SetCursorMiddleIfOutsideBoundaries( x, y, 0.25f );
        }
    }

    static void Move( double fw, double lr ) {
        Vector3D axisWeight = Vector3D( 1.0, 0.0, 1.0 );
        ___Move( fw, lr, 0.0, axisWeight );
    }

    static void MoveFree( double fw, double lr ) {
        Vector3D axisWeight = Vector3D( 1.0, 1.0, 1.0 );
        ___Move( fw, lr, 0.0, axisWeight );
    }

    static bool isMovementEnabled() {
        return _movementEnabled;
    }

    static void setMovementEnabled( bool enabled ) {
        _movementEnabled = enabled;
    }

    static void SetPosition( Vector3D vec ) {
        if ( _pos ) {
            ( *_pos ) = vec;
        }
    }

    static void SetZRotation( double z ) {
        _zRotation = z;
    }

    static double GetFov() {
        return _fov;
    }

    static void SetFov( double fovValue ) {
        _fov = fovValue;
    }

    static void Setup() {
        Settings* settings = Settings::getSettings();
        if ( settings ) {
            _fov = settings -> getSettingValue( Settings::SETTING_ID_FOV );
            _vSpeed = settings -> getSettingValue( Settings::SETTING_ID_VSPEED ) * settings -> getSettingValue( Settings::SETTING_ID_VINVERT ) * VERTICAL_SPEED;
            _hSpeed = settings -> getSettingValue( Settings::SETTING_ID_HSPEED ) * settings -> getSettingValue( Settings::SETTING_ID_HINVERT ) * HORIZONTAL_SPEED;
            _aspect = settings -> getSettingValue( Settings::SETTING_ID_ASPECT );
        }
    }

    private:

    static Vector3D _oldPos;
    static Vector3D* _pos;
    static PolarVector3D* _angles;
    static bool _movementEnabled;
    static int _lastMouseX;
    static int _lastMouseY;
    static double _vSpeed;
    static double _hSpeed;
    static double _fov;
    static double _aspect;
    static double _zRotation;

    static double _verticalMin;
    static double _verticalMax;

    public:

    static const double VERTICAL_SPEED = 0.65;
    static const double HORIZONTAL_SPEED = 0.65;

    static void ___SetCursorMiddleIfOutsideBoundaries( int* x, int* y, float padding ) {
        float revpadding = 1.0f - padding;
        float wx = glutGet( GLUT_WINDOW_WIDTH );
        float wy = glutGet( GLUT_WINDOW_HEIGHT );
        float px = float( *x ) / wx;
        float py = float( *y ) / wy;
        if ( ( px < padding ) || ( py < padding ) || ( px > revpadding ) || ( py > revpadding ) ) {
            _lastMouseX = wx / 2;
            _lastMouseY = wy / 2;
            ( *x ) = _lastMouseX;
            ( *y ) = _lastMouseY;
            glutWarpPointer( _lastMouseX, _lastMouseY );
        }
    }

    static void ___Move( double fw, double lr, double ud, Vector3D axisWeight ) {
        if ( _movementEnabled ) {
            Vector3D cartesianMove = Vector3D( *_angles ).normalize();
            Vector3D cartesianWeightedMove = Vector3D( cartesianMove.x * axisWeight.x, cartesianMove.y * axisWeight.y, cartesianMove.z * axisWeight.z ).normalize();
            Vector3D _movement = Vector3D(
                cartesianWeightedMove.x * fw - cartesianWeightedMove.z * lr,
                - cartesianWeightedMove.y * fw + cartesianWeightedMove.y * ud,
                cartesianWeightedMove.z * fw + cartesianWeightedMove.x * lr
            );
            ( *_pos ) = ( *_pos ) + _movement;
        }
    }

};

Vector3D Camera::_oldPos;
Vector3D* Camera::_pos = NULL;
PolarVector3D* Camera::_angles = NULL;
bool Camera::_movementEnabled = true;
int Camera::_lastMouseX = -1;
int Camera::_lastMouseY = -1;
double Camera::_vSpeed = - Camera::VERTICAL_SPEED;
double Camera::_hSpeed = Camera::HORIZONTAL_SPEED;
double Camera::_fov = 80.0;
double Camera::_aspect = 1.0;
double Camera::_zRotation = 0.0;
double Camera::_verticalMin = radn( - 89.0 );
double Camera::_verticalMax = radn( 89.0 );

