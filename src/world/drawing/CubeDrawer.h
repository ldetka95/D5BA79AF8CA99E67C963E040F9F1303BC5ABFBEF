#pragma once

#include "../../display/AbstractDisplay.h"

class CubeDrawer {
    protected:
    CubeDrawer() {
    }
    ~CubeDrawer() {
    }
    public:

    /**
        Draws quad (side) of single cube.
        @param x X position of cube, absolute.
        @param y Y position of cube, absolute.
        @param z Z position of cube, absolute.
        @param textureID ID of the texture used for drawing.
        @param side Side of cube to draw. May be one of following:
            1 - Z front,
            2 - Z back,
            3 - X front,
            4 - X back,
            5 - Y top,
            6 - Y bottom.
    */
    static void DrawSide( int x, int y, int z, int textureID, int side ) {
        double texJumpX = 1.0 / double( textureCountX );
        double texJumpY = 1.0 / double( textureCountY );
        double texidX = double( textureID % textureCountX ) * texJumpX;
        double texidY = double( textureID / textureCountX ) * texJumpY;
        Vector3D normal = sideOffset[ side ];
        glNormal3f( normal.x, normal.y, normal.z );
        for ( int i = 0; i < 4; i++ ) {
            int* vertexSideIndex = cubeVertexIndex[ side ];
            glTexCoord2d( texidX + cubeTexCoord[ i ][ 0 ] * texJumpX, texidY + cubeTexCoord[ i ][ 1 ] * texJumpY );
            Vector3D vertex = cubeVertex[ vertexSideIndex[ i ] ];
            glVertex3f( vertex.x + x, vertex.y + y, vertex.z + z );
        }
    }

    static const int textureCountX = 16;
    static const int textureCountY = 16;
    static Vector3D sideOffset[ 6 ];
    static Vector3D cubeVertex[ 8 ];
    static int cubeVertexIndex[ 6 ][ 4 ];
    static double cubeTexCoord[ 4 ][ 2 ];

    private:
    static TextureHolder* _holder;

    static int _matrixGetField( char* matrix, int i, int j, int k, int x, int y, int z ) {
        if ( ( i >= 0 ) && ( i < z ) && ( j >= 0 ) && ( j < y ) && ( k >= 0 ) && ( k < x ) ) {
            return matrix[ i * x * y + j * x + k ] & 0x000000FF;
        }
        return -1;
    }

};



Vector3D CubeDrawer::sideOffset[ 6 ] = {
    Vector3D( 0.0, 0.0, -1.0 ),
    Vector3D( 0.0, 0.0, 1.0 ),
    Vector3D( 1.0, 0.0, 0.0 ),
    Vector3D( -1.0, 0.0, 0.0 ),
    Vector3D( 0.0, 1.0, 0.0 ),
    Vector3D( 0.0, -1.0, 0.0 )
};

Vector3D CubeDrawer::cubeVertex[ 8 ] = {
    Vector3D( 0.0, 0.0, 0.0 ),
    Vector3D( 0.0, 0.0, 1.0 ),
    Vector3D( 0.0, 1.0, 0.0 ),
    Vector3D( 0.0, 1.0, 1.0 ),
    Vector3D( 1.0, 0.0, 0.0 ),
    Vector3D( 1.0, 0.0, 1.0 ),
    Vector3D( 1.0, 1.0, 0.0 ),
    Vector3D( 1.0, 1.0, 1.0 )
};

int CubeDrawer::cubeVertexIndex[ 6 ][ 4 ] = {
    { 6, 2, 0, 4 },
    { 3, 7, 5, 1 },
    { 7, 6, 4, 5 },
    { 2, 3, 1, 0 },
    { 2, 6, 7, 3 },
    { 4, 0, 1, 5 }
};

double CubeDrawer::cubeTexCoord[ 4 ][ 2 ] = {
    { 0.0, 1.0 },
    { 1.0, 1.0 },
    { 1.0, 0.0 },
    { 0.0, 0.0 }
};

TextureHolder* CubeDrawer::_holder = NULL;
