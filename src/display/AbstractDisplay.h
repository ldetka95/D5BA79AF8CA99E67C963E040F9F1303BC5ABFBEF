#pragma once

#include "texture/TextureHolder.h"
#include "hud/Hud.h"
#include "../core/common/Vectors.h"
#include "utils/Settings.h"
#include "glew/FrameUnit.h"
#include "utils/Camera.h"
#include "utils/Keys.h"
#include "model/BlockModel.h"
//#include "component/AbstractComponent2D.h"

class AbstractDisplay {
    public:
    AbstractDisplay() {
        _list = 0;
    }
    virtual ~AbstractDisplay() {
        clearList();
    }

    static const int WITH_LIST = 1;
    static const int NO_LIST = 0;

    static bool Add( AbstractDisplay* display ) {
        if ( !display ) {
            ___ADL.push_back( display );
            return false;
        }
        return true;
    }

    static void DisplayAll() {
        list < AbstractDisplay* >::iterator it = ___ADL.begin();
        while ( it != ___ADL.end() ) {
            ( *it ) -> display();
            it++;
        }
    }

    virtual void display( int listWrap = WITH_LIST ) {
        if ( listWrap == WITH_LIST ) {
            if ( !_list ) {
                rebuild();
            }
            if ( _list ) {
                glCallList( _list );
            }
        } else if ( listWrap == NO_LIST ) {
            static int ch = 0;
            ch++;
            onDisplay();
        }
    }

    void rebuild() {
        clearList();
        _list = glGenLists( 1 );
        glNewList( _list, GL_COMPILE );
            glPushMatrix();
            onDisplay();
            glPopMatrix();
        glEndList();
    }

    void clearList() {
        if ( _list ) {
            glDeleteLists( _list, 1 );
            _list = 0;
        }
    }

    protected:

    virtual void onDisplay() = 0;

    private:

    GLuint _list;

    static list < AbstractDisplay* > ___ADL;

};

list < AbstractDisplay* > AbstractDisplay::___ADL;

