#pragma once

#include <list>
#include "AbstractComponent.h"

class Form : public AbstractComponent {
    public:
    Form( double x, double y, double w, double h, string title ) : AbstractComponent( x, y, w, h, title ) {
        if ( !_T ) {
            _T = Texture::Load( "Hello.bmp" );
        }
    }
    virtual ~Form() {
        //delete _t;
    }

    void onMousePressed( int button, int x, int y ) {
    }

    void onMouseReleased( int button, int x, int y ) {
    }

    void onMouseReleasedNoClick( int button, int x, int y ) {
    }

    void onMouseIn( int x, int y ) {
    }

    void onMouseOut( int x, int y ) {
    }

    void onDisplay() {
        DisplayGuiTexture( _T, getComponentPosX(), getComponentPosY(), getComponentWidth(), getComponentHeight() );
    }

    void onReset() {
    }

    private:

    static Texture* _T;

};

Texture* Form::_T = NULL;
