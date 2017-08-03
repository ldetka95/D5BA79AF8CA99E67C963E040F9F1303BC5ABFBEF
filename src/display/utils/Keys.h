#pragma once

#define KEY_INPUT_NUM 256
#define MOUSE_INPUT_NUM 16

#define KEY_MASK_HOLD 0x00000001
#define KEY_MASK_STRUCK 0x00000002

#define MOUSE_MASK_HOLD 0x00000001
#define MOUSE_MASK_CLICKED 0x00000002
#define MOUSE_MASK_RELEASED 0x00000004

/**
    Wrapper-like class responsible for holding keys current state.
*/
class Keys {
    private:
    Keys() {}
    ~Keys() {}
    public:

    static bool isPressed( int key ) {
        if ( ( key >= 0 ) && ( key < KEY_INPUT_NUM ) ) {
            return ( _k[ key ] & ( KEY_MASK_HOLD | KEY_MASK_STRUCK ) ) > 0;
        }
        return false;
    }

    static bool isStrucked( int key ) {
        if ( ( key >= 0 ) && ( key < KEY_INPUT_NUM ) ) {
            return ( _k[ key ] & ( KEY_MASK_STRUCK ) ) > 0;
        }
        return false;
    }

    static bool isMousePressed( int button ) {
        if ( ( button >= 0 ) && ( button < MOUSE_INPUT_NUM ) ) {
            return ( _m[ button ] & ( MOUSE_MASK_HOLD ) ) > 0;
        }
        return false;
    }

    static bool isMouseClicked( int button ) {
        if ( ( button >= 0 ) && ( button < MOUSE_INPUT_NUM ) ) {
            return ( _m[ button ] & ( MOUSE_MASK_CLICKED ) ) > 0;
        }
        return false;
    }

    static bool isMouseReleased( int button ) {
        if ( ( button >= 0 ) && ( button < MOUSE_INPUT_NUM ) ) {
            return ( _m[ button ] & ( MOUSE_MASK_RELEASED ) ) > 0;
        }
        return false;
    }

    static void press( int key ) {
        if ( ( key >= 0 ) && ( key < KEY_INPUT_NUM ) ) {
            _k[ key ] |= ( KEY_MASK_HOLD | KEY_MASK_STRUCK );
        }
    }

    static void release( int key ) {
        if ( ( key >= 0 ) && ( key < KEY_INPUT_NUM ) ) {
            _k[ key ] &= ( ~KEY_MASK_HOLD );
        }
    }

    static void controlMouse( int button, int state ) {
        if ( ( button >= 0 ) && ( button < MOUSE_INPUT_NUM ) ) {
            switch ( state ) {
                case GLUT_DOWN: {
                    _m[ button ] |= ( MOUSE_MASK_CLICKED | MOUSE_MASK_HOLD );
                    break;
                }
                case GLUT_UP: {
                    _m[ button ] &= ( ~MOUSE_MASK_HOLD );
                    _m[ button ] |= ( MOUSE_MASK_RELEASED );
                    break;
                }
            }
        }
    }

    static void setMouse( int x, int y ) {
        _mx = x;
        _my = y;
    }
    static int* getMouseX() {
        return &_mx;
    }
    static int* getMouseY() {
        return &_my;
    }

    static void invalidStruckStates() {
        for ( int i = 0; i < KEY_INPUT_NUM; i++ ) {
            _k[ i ] &= ( ~KEY_MASK_STRUCK );
        }
        for ( int i = 0; i < MOUSE_INPUT_NUM; i++ ) {
            _m[ i ] &= ( ~MOUSE_MASK_CLICKED );
            _m[ i ] &= ( ~MOUSE_MASK_RELEASED );
        }
    }

    static void resetKeys() {
        for ( int i = 0; i < KEY_INPUT_NUM; i++ ) {
            _k[ i ] = 0;
        }
    }

    static void resetMouse() {
        for ( int i = 0; i < MOUSE_INPUT_NUM; i++ ) {
            _m[ i ] = 0;
        }
    }

    private:
    static int _k[ KEY_INPUT_NUM ];
    static int _m[ MOUSE_INPUT_NUM ];
    static int _mx;
    static int _my;
};

int Keys::_k[ KEY_INPUT_NUM ];
int Keys::_m[ MOUSE_INPUT_NUM ];
int Keys::_mx = -1;
int Keys::_my = -1;


