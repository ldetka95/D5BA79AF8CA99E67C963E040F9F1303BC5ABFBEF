#pragma once

class AbstractComponent2D {
    public:
    AbstractComponent2D( SDL_Rect rect ) {
        _rect = rect;
    }
    virtual ~AbstractComponent2D() {
        list < AbstractComponent2D* >::iterator it = _childList.begin();
        while ( it != _childList.end() ) {
            delete ( *it );
            it++;
        }
    }

    void subComponent( AbstractComponent2D* comp ) {
        _childList.push_back( comp );
    }

    virtual void onDisplay( AbstractComponent2D* mother, SDL_Surface* surface ) = 0;

    virtual void onClick( int button, int state, int x, int y ) = 0;

    void display( SDL_Surface* surface ) {
        ___display( NULL, surface );
    }

    AbstractComponent2D* click( int button, int state, int x, int y ) {
        list < AbstractComponent2D* >::iterator it = _childList.begin();
        while ( it != _childList.end() ) {
            AbstractComponent2D* clicked = ( *it ) -> click( button, state, x, y );
            if ( clicked ) {
                return clicked;
            }
            it++;
        }
        if ( ( x >= _rect.x ) && ( y >= _rect.y ) && ( x < _rect.x + _rect.w ) && ( y < _rect.y + _rect.h ) ) {
            onClick( button, state, x, y );
            return this;
        }
        return NULL;
    }

    SDL_Rect getRect() {
        return _rect;
    }

    int getWidth() {
        return _rect.w;
    }

    int getHeight() {
        return _rect.h;
    }

    int getX() {
        return _rect.x;
    }

    int getY() {
        return _rect.y;
    }

    void setRect( SDL_Rect rect ) {
        _rect = rect;
    }

    void setWidth( int w ) {
        _rect.w = w;
    }

    void setHeight( int h ) {
        _rect.h = h;
    }

    void setX( int x ) {
        _rect.x = x;
    }

    void setY( int y ) {
        _rect.y = y;
    }

    private:

    void ___display( AbstractComponent2D* mother, SDL_Surface* surface ) {
        onDisplay( mother, surface );
        list < AbstractComponent2D* >::iterator it = _childList.begin();
        while ( it != _childList.end() ) {
            ( *it ) -> ___display( this, surface );
            it++;
        }
    }

    list < AbstractComponent2D* > _childList;
    SDL_Rect _rect;

};


