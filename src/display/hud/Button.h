#pragma once

#include "AbstractComponent.h"

class Button : public AbstractComponent {
    protected:
    static const int STATES_NUM = 6;
    static const int OFF = 0;
    static const int PRESSED = 1;
    static const int HOLDING = 2;
    static const int OVER = 3;
    static const int SELECTION = 4;
    static const int DISABLED = 5;
    static void InitButtons() {
        _BUTTONINIT = true;
        _BUTTON_BG[ SELECTION ] = Texture::Load( "data/textures/gui/buttonSelection.bmp" );
        _BUTTON_BG[ OFF ] = Texture::Load( "data/textures/gui/button_Idle.bmp" );
        _BUTTON_BG[ PRESSED ] = Texture::Load( "data/textures/gui/button_Pressed.bmp" );
        _BUTTON_BG[ HOLDING ] = Texture::Load( "data/textures/gui/button_Pressed.bmp" );
        _BUTTON_BG[ OVER ] = Texture::Load( "data/textures/gui/button_Idle.bmp" );
        _BUTTON_BG[ DISABLED ] = Texture::Load( "data/textures/gui/button_Inactive.bmp" );
        for ( int i = 0; i < STATES_NUM; i++ ) {
            if ( _BUTTON_BG[ i ] ) {
                _BUTTON_BG[ i ] -> makeOpaque( 0xFFFFFFFF );
            }
        }
    }
    public:
    Button( double x, double y, double w, double h, string t, double xt, double yt, double bo, void ( *onclickcallback ) ( Button*, int ), int order ) : AbstractComponent( x, y, w, h, t ) {
        if ( !_BUTTONINIT ) {
            InitButtons();
        }
        SDL_Surface* text = Text::RenderOnly( t.c_str() );
        _textRendered[ 0 ] = Texture::Create( text );
        if ( _textRendered[ 0 ] -> getWidth() > _textRendered[ 0 ] -> getHeight() ) {
            _aspectW = 1.0;
            _aspectH = double( _textRendered[ 0 ] -> getHeight() ) / double( _textRendered[ 0 ] -> getWidth() ) * w / h;
        } else {
            _aspectW = double( _textRendered[ 0 ] -> getWidth() ) / double( _textRendered[ 0 ] -> getHeight() ) * h / w;
            _aspectH = 1.0;
        }
        _state = OFF;
        _over = false;
        _xt = xt;
        _yt = yt;
        _borderOffset = bo;
        _onClick = onclickcallback;
        _enabled = true;
        _atx = - double( order ) * 0.1;
        _orig_atx = - double( order ) * 0.1;
    }
    ~Button() {
        for ( int i = 0; i < STATES_NUM; i++ ) {
            if ( _textRendered[ i ] ) {
                delete _textRendered[ i ];
            }
        }
    }

    class Builder {
        public:
        Builder( double w, double h, double xt, double yt, double bo ) {
            _w = w;
            _h = h;
            _xt = xt;
            _yt = yt;
            _bo = bo;
        }
        ~Builder() {
        }
        template < class T > T* build( double x, double y, string text ) {
            return new T( x, y, _w, _h, text, _xt, _yt, _bo );
        }
        private:
        double _w;
        double _h;
        double _xt;
        double _yt;
        double _bo;
    };

    void onMousePressed( int button, int x, int y ) {
        if ( _enabled ) {
            _state = PRESSED;
        }
    }

    void onMouseReleased( int button, int x, int y ) {
        if ( _enabled ) {
            if ( _onClick ) {
                _onClick( this, button );
            }
        }
        _state = OFF;
    }

    void onMouseReleasedNoClick( int button, int x, int y ) {
        _state = OFF;
    }

    void onMouseIn( int x, int y ) {
        if ( _enabled ) {
            if ( _state == HOLDING ) {
                _state = PRESSED;
            } else {
                _state = OVER;
            }
        }
        _over = true;
        _over_startTime = glutGet( GLUT_ELAPSED_TIME );
    }

    void onMouseOut( int x, int y ) {
        if ( ( _state == PRESSED ) && ( _enabled ) ) {
            _state = HOLDING;
        } else {
            _state = OFF;
        }
        _over = false;
    }

    void onDisplay() {
        double matx = 0.025;
        double jmpFrameIn = 0.015;
        double jmpIn = 0.01;
        double jmpOut = 0.004;
        if ( _over ) {
            if ( _atx < matx ) {
                _atx = min( _atx + jmpIn, matx );
            } else {
                _atx = matx;
            }
        } else {
            if ( _atx > 0.0 ) {
                _atx = max( _atx - jmpOut, 0.0 );
            } else if ( _atx < 0.0 ) {
                _atx += jmpFrameIn;
            }
        }
        double tx = getComponentPosX();
        double ty = getComponentPosY();
        double tw = getComponentWidth() * _aspectW / 2.0;
        double th = getComponentHeight() * _aspectH / 2.0;
        tx = tx + getComponentWidth() / 2.0;
        ty = ty + getComponentHeight() / 2.0;
        double off = ( 1.0 - _borderOffset ) * 2.0;
        double off2 = off / 2.0;
        if ( _enabled ) {
            DisplayGuiTexture( _BUTTON_BG[ _state ], getComponentPosX() + _atx, getComponentPosY(), getComponentWidth(), getComponentHeight() );
        } else {
            DisplayGuiTexture( _BUTTON_BG[ DISABLED ], getComponentPosX() + _atx, getComponentPosY(), getComponentWidth(), getComponentHeight() );
        }
        DisplayGuiTexture( _textRendered[ 0 ], ( tx + _atx ) + _xt * tw - tw * off2, ty + _yt * th - th * off2, tw * off, th * off );
        if ( _over ) {
            DisplayGuiTexture( _BUTTON_BG[ SELECTION ], getComponentPosX() + _atx, getComponentPosY(), getComponentWidth(), getComponentHeight() );
        }
    }

    void onReset() {
        _atx = _orig_atx;
    }

    void enable( bool en ) {
        _enabled = en;
    }

    bool isEnabled() {
        return _enabled;
    }

    static int BgWidth() {
        if ( !_BUTTONINIT ) {
            InitButtons();
        }
        if ( _BUTTON_BG[ OFF ] ) {
            return _BUTTON_BG[ OFF ] -> getWidth();
        }
        return -1;
    }

    static int BgHeight() {
        if ( !_BUTTONINIT ) {
            InitButtons();
        }
        if ( _BUTTON_BG[ OFF ] ) {
            return _BUTTON_BG[ OFF ] -> getHeight();
        }
        return -1;
    }

    private:

    Texture* _textRendered[ 1 ];
    double _aspectW;
    double _aspectH;
    int _state;
    bool _over;
    int64_t _over_startTime;

    double _atx;
    double _orig_atx;
    double _xt;
    double _yt;
    double _borderOffset;

    static bool _BUTTONINIT;
    static Texture* _BUTTON_BG[ STATES_NUM ];

    void ( *_onClick )( Button*, int );
    bool _enabled;

};

bool Button::_BUTTONINIT = false;
Texture* Button::_BUTTON_BG[ STATES_NUM ] = { NULL, NULL, NULL, NULL };

