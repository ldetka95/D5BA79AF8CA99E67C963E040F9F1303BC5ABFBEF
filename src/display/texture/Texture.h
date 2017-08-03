#pragma once

#include "../utils/SDLLink.h"

#include <glut/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define Uint32 unsigned int

#include <stdlib.h>

#define TEXTURE_STD_BPP 4

#define BITMAP_HEADER_LENGTH 54

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <map>
using namespace std;

#include <math.h>

/**
    Class holding bitmap data. Only 24-bit regular bitmaps are supported. Use clear white color (0x00FFFFFF) to mark transparent places.
*/
/*class Texture {
    private:
    Texture() {
        _pixels = NULL;
        _w = 0;
        _h = 0;
        _texture = 0;
    }
    Texture( int w, int h ) {
        _pixels = NULL;
        _w = 0;
        _h = 0;
        _texture = 0;
        resize( w, h );
    }
    Texture( GLuint image, int w, int h ) {
        _pixels = NULL;
        _w = 0;
        _h = 0;
        _texture = image;
        resize( w, h );
    }
    public:
    static Texture* create( int w, int h ) {
        if ( ( w >= 0 ) && ( h >= 0 ) ) {
            return new Texture( w, h );
        }
        return NULL;
    }

    static Texture* createFromGlTexture( GLuint image, int w, int h ) {
        if ( ( w >= 0 ) && ( h >= 0 ) ) {
            return new Texture( image, w, h );
        }
        return NULL;
    }
    ~Texture() {
        if ( _pixels ) {
            free( _pixels );
        }
        if ( _texture ) {
            glDeleteTextures( 1, &_texture );
        }
    }

    Uint32 getPixel( int x, int y ) {
        if ( ( x >= 0 ) && ( y >= 0 ) && ( x < _w ) && ( y < _h ) ) {
            return *( Uint32* )( int( _pixels ) + ( y * _w + x ) * TEXTURE_STD_BPP );
        }
        return 0xFFFFFFFF;
    }

    void setPixel( int x, int y, Uint32 color ) {
        if ( ( x >= 0 ) && ( y >= 0 ) && ( x < _w ) && ( y < _h ) ) {
            *( Uint32* )( int( _pixels ) + ( y * _w + x ) * TEXTURE_STD_BPP ) = color;
        }
    }

    void resize( int width, int height, bool fillBlack = true ) {
        if ( ( width < 0 ) || ( height < 0 ) ) {
            return;
        }
        _pixels = realloc( _pixels, width * height * TEXTURE_STD_BPP );
        if ( fillBlack ) {
            for ( int iy = 0; iy < _h; iy++ ) {
                for ( int ix = _w; ix < width; ix++ ) {
                    setPixel( ix, iy, 0xFF000000 );
                }
            }
            for ( int ix = 0; ix < width; ix++ ) {
                for ( int iy = _h; iy < height; iy++ ) {
                    setPixel( ix, iy, 0xFF000000 );
                }
            }
        }
        _w = width;
        _h = height;
    }

    void copyPixels32( Texture* dst, int x1, int y1, int x2, int y2, int w, int h ) {
        if ( !dst ) {
            return;
        }
        for ( int x = 0; x < w; x++ ) {
            int xsrc = x1 + x;
            int xdst = x2 + x;
            for ( int y = 0; y < h; y++ ) {
                int ysrc = y1 + y;
                int ydst = y2 + y;
                dst -> setPixel( xdst, ydst, this -> getPixel( xsrc, ysrc ) );
            }
        }
    }

    GLuint glCreateImage() {
        if ( !_texture ) {
            glGenTextures( 1, &_texture );
            glBindTexture( GL_TEXTURE_2D, _texture );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }
        glUseImage();
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels );
        return _texture;
    }

    bool glUseImage() {
        if ( _pixels ) {
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, _texture );
            return true;
        }
        return false;
    }

    Texture* scaleTo( int w, int h ) {
        double wf = double( _w ) / double( w );
        double hf = double( _h ) / double( h );
        double wfr = 1.0 / wf;
        double hfr = 1.0 / hf;
        Texture* ret = create( w, h );
        if ( ret ) {
            for ( int i = 0; i < w; i++ ) {
                for ( int j = 0; j < h; j++ ) {
                    int sum[ 4 ] = { 0, 0, 0, 0 };
                    int cnt = 0;
                    for ( int k = 0; k < wfr; k++ ) {
                        for ( int l = 0; l < hfr; l++ ) {
                            unsigned int px = getPixel( i * wf + k, j * hf + l );
                            for ( int m = 0; m < 4; m++ ) {
                                sum[ m ] += ( px >> ( ( 3 - m ) << 3 ) ) & 0xFF;
                            }
                            cnt++;
                        }
                    }
                    unsigned int color = 0;
                    for ( int m = 0; m < 4; m++ ) {
                        sum[ m ] = double( sum[ m ] ) / double( cnt );
                        color = ( color << 8 ) + ( sum[ m ] & 0xFF );
                    }
                    ret -> setPixel( i, j, color );
                }
            }
        }
        return ret;
    }

    int getWidth() {
        return _w;
    }
    int getHeight() {
        return _h;
    }
    void* getPixels() {
        return _pixels;
    }

    void refreshPixelsFromScreen() {
        glBindTexture( GL_TEXTURE_2D, _texture );
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels );
    }

    void makeRoundBorders( double smoothness ) {
        glBindTexture( GL_TEXTURE_2D, _texture );
        int mx = _w >> 1;
        int my = _h >> 1;
        int mxs = mx * ( 1.0 - smoothness );
        double a = - 1.0 / ( double( mx ) * smoothness );
        double b = 1.0 / smoothness;
        for ( int i = 0; i < _w; i++ ) {
            for ( int j = 0; j < _h; j++ ) {
                double dmx = mx - i;
                double dmy = my - j;
                double d = sqrt( dmx * dmx + dmy * dmy );
                if ( d > mx ) {
                    setPixel( i, j, 0 );
                } else if ( d > mxs ) {
                    unsigned int pixel = getPixel( i, j );
                    int mv = ( a * d + b ) * 256.0;
                    if ( mv > 0xFF ) {
                        mv = 0xFF;
                    }
                    mv &= 0xFF;
                    pixel &= ( mv << 24 ) + 0x00FFFFFF;
                    setPixel( i, j, pixel );
                }
            }
        }
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels );
    }

    protected:
    #pragma pack(push, 1)
    struct Header {
        public:
        uint16_t signature;
        uint32_t filesize;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t startOffset;
        uint32_t bihSize;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bpp;
        uint32_t compression;
        uint32_t imagesize;
        uint32_t hdpi;
        uint32_t vdpi;
        uint32_t numberOfColors;
        uint32_t numberOfImportantColors;
    };
    #pragma pack(pop)
    class ReaderException {
        public:
        ReaderException( int code ) {
            _c = code;
        }
        void print() {
            printf( "Reader Exception occured; code: %d\n", _c );
        }
        private:
        int _c;
    };
    static const int TEX_ALPHA_MASK = 0xFF000000;
    static const int TEX_R_MASK = 0x00FF0000;
    static const int TEX_G_MASK = 0x0000FF00;
    static const int TEX_B_MASK = 0x000000FF;
    public:
    static Texture* Load( const char* path ) {
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            Texture* texture = NULL;
            try {
                Header header;
                if ( fread( &header, 1, sizeof( header ), handle ) != sizeof( header ) ) {
                    throw new ReaderException( 1 );
                }
                if ( header.signature != 0x4D42 ) {
                    throw new ReaderException( 2 );
                }
                if ( header.bihSize != 40 ) {
                    //throw new ReaderException( 3 );
                }
                if ( ( header.width < 0 ) || ( header.height < 0 ) ) {
                    throw new ReaderException( 4 );
                }
                if ( header.planes != 1 ) {
                    throw new ReaderException( 5 );
                }
                /// forced right now
                if ( header.bpp != 24 ) {
                    throw new ReaderException( 6 );
                }
                texture = Texture::create( header.width, header.height );
                uint32_t size = header.width * header.height * 3;
                unsigned char* buffer = ( unsigned char* ) malloc( size );
                fseek( handle, header.startOffset, SEEK_SET );
                if ( fread( buffer, 1, size, handle ) != size ) {
                    free( buffer );
                    throw new ReaderException( 7 );
                }
                unsigned char* cursor = buffer;
                for ( uint32_t y = 0; y < header.height; y++ ) {
                    for ( uint32_t x = 0; x < header.width; x++ ) {
                        Uint32 color = TEX_ALPHA_MASK +
                            ( ( int( cursor[ 0 ] ) << 16 ) & TEX_R_MASK ) +
                            ( ( int( cursor[ 1 ] ) << 8 ) & TEX_G_MASK ) +
                            ( ( int( cursor[ 2 ] ) ) & TEX_B_MASK );
                        if ( ( color & 0x00FFFFFF ) == 0x00FFFFFF ) {
                            color = 0x00000000;
                        }
                        texture -> setPixel( x, y, color );
                        cursor += 3;
                    }
                }
                free( buffer );
            } catch ( ReaderException* e ) {
                if ( texture ) {
                    delete texture;
                    texture = NULL;
                }
                e -> print();
                delete e;
            }
            fclose( handle );
            texture -> glCreateImage();
            return texture;
        }
        return NULL;
    }

    static bool saveRGB( Texture* img, const char* path, int mask = 0x00FFFFFF ) {
        FILE* handle = fopen( path, "wb" );
        if ( handle ) {
            int w = img -> getWidth();
            int h = img -> getHeight();
            Header header = ___defaultHeaderRGB( w, h );
            fwrite( &header, sizeof( header ), 1, handle );
            unsigned char* px = ( unsigned char* ) img -> getPixels();
            int rgbSize = w * h * 3;
            int rgbaSize = w * h * 4;
            unsigned char* buffer3 = ( unsigned char* ) malloc( rgbSize );
            unsigned char* buffer3c = buffer3;
            double m1 = double( ( mask & TEX_R_MASK ) >> 16 ) / 255.0;
            double m2 = double( ( mask & TEX_G_MASK ) >> 8 ) / 255.0;
            double m3 = double( ( mask & TEX_B_MASK ) ) / 255.0;
            for ( int i = 0; i < rgbaSize; i += 4 ) {
                buffer3c[ 0 ] = px[ i + 2 ] * m3;
                buffer3c[ 1 ] = px[ i + 1 ] * m2;
                buffer3c[ 2 ] = px[ i + 0 ] * m1;
                buffer3c += 3;
            }
            fwrite( buffer3, rgbSize, 1, handle );
            free( buffer3 );
            fclose( handle );
            return true;
        }
        return false;
    }

    //Texture* clone();

    private:
    int _w, _h;
    GLuint _texture;
    void* _pixels;

    static Header ___defaultHeaderRGB( int w, int h ) {
        Header header;
        header.signature = 0x4D42;
        header.filesize = w * h * 3 + sizeof( header );
        header.reserved1 = 0;
        header.reserved2 = 0;
        header.startOffset = sizeof( header );
        header.bihSize = 40;
        header.width = w;
        header.height = h;
        header.planes = 1;
        header.bpp = 24;
        header.compression = 0;
        header.imagesize = w * h * 3;
        header.hdpi = 0;
        header.vdpi = 0;
        header.numberOfColors = 0;
        header.numberOfImportantColors = 0;
        return header;
    }

};*/

#include <inttypes.h>
/*
class TextureLoader {
    private:
    TextureLoader() {
    }
    protected:
    #pragma pack(push, 1)
    struct Header {
        public:
        uint16_t signature;
        uint32_t filesize;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t startOffset;
        uint32_t bihSize;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bpp;
        uint32_t compression;
        uint32_t imagesize;
        uint32_t hdpi;
        uint32_t vdpi;
        uint32_t numberOfColors;
        uint32_t numberOfImportantColors;
    };
    #pragma pack(pop)
    class ReaderException {
        public:
        ReaderException( int code ) {
            _c = code;
        }
        void print() {
            printf( "Reader Exception occured; code: %d\n", _c );
        }
        private:
        int _c;
    };
    static const int TEX_ALPHA_MASK = 0xFF000000;
    static const int TEX_R_MASK = 0x00FF0000;
    static const int TEX_G_MASK = 0x0000FF00;
    static const int TEX_B_MASK = 0x000000FF;
    public:
    static Texture* load( const char* path ) {
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            Texture* texture = NULL;
            try {
                Header header;
                if ( fread( &header, 1, sizeof( header ), handle ) != sizeof( header ) ) {
                    throw new ReaderException( 1 );
                }
                if ( header.signature != 0x4D42 ) {
                    throw new ReaderException( 2 );
                }
                if ( header.bihSize != 40 ) {
                    //throw new ReaderException( 3 );
                }
                if ( ( header.width < 0 ) || ( header.height < 0 ) ) {
                    throw new ReaderException( 4 );
                }
                if ( header.planes != 1 ) {
                    throw new ReaderException( 5 );
                }
                /// forced right now
                if ( header.bpp != 24 ) {
                    throw new ReaderException( 6 );
                }
                texture = Texture::create( header.width, header.height );
                uint32_t size = header.width * header.height * 3;
                unsigned char* buffer = ( unsigned char* ) malloc( size );
                fseek( handle, header.startOffset, SEEK_SET );
                if ( fread( buffer, 1, size, handle ) != size ) {
                    free( buffer );
                    throw new ReaderException( 7 );
                }
                unsigned char* cursor = buffer;
                for ( uint32_t y = 0; y < header.height; y++ ) {
                    for ( uint32_t x = 0; x < header.width; x++ ) {
                        Uint32 color = TEX_ALPHA_MASK +
                            ( ( int( cursor[ 0 ] ) << 16 ) & TEX_R_MASK ) +
                            ( ( int( cursor[ 1 ] ) << 8 ) & TEX_G_MASK ) +
                            ( ( int( cursor[ 2 ] ) ) & TEX_B_MASK );
                        if ( ( color & 0x00FFFFFF ) == 0x00FFFFFF ) {
                            color = 0x00000000;
                        }
                        texture -> setPixel( x, y, color );
                        cursor += 3;
                    }
                }
                free( buffer );
            } catch ( ReaderException* e ) {
                if ( texture ) {
                    delete texture;
                    texture = NULL;
                }
                e -> print();
                delete e;
            }
            fclose( handle );
            texture -> glCreateImage();
            return texture;
        }
        return NULL;
    }
    static bool saveRGB( Texture* img, const char* path, int mask = 0x00FFFFFF ) {
        FILE* handle = fopen( path, "wb" );
        if ( handle ) {
            int w = img -> getWidth();
            int h = img -> getHeight();
            Header header = ___defaultHeaderRGB( w, h );
            fwrite( &header, sizeof( header ), 1, handle );
            unsigned char* px = ( unsigned char* ) img -> getPixels();
            int rgbSize = w * h * 3;
            int rgbaSize = w * h * 4;
            unsigned char* buffer3 = ( unsigned char* ) malloc( rgbSize );
            unsigned char* buffer3c = buffer3;
            double m1 = double( ( mask & TEX_R_MASK ) >> 16 ) / 255.0;
            double m2 = double( ( mask & TEX_G_MASK ) >> 8 ) / 255.0;
            double m3 = double( ( mask & TEX_B_MASK ) ) / 255.0;
            for ( int i = 0; i < rgbaSize; i += 4 ) {
                buffer3c[ 0 ] = px[ i + 2 ] * m3;
                buffer3c[ 1 ] = px[ i + 1 ] * m2;
                buffer3c[ 2 ] = px[ i + 0 ] * m1;
                buffer3c += 3;
            }
            fwrite( buffer3, rgbSize, 1, handle );
            free( buffer3 );
            fclose( handle );
            return true;
        }
        return false;
    }
    static bool performScreenshot() {
        Texture* texture = getScreenshot();
        if ( !texture ) {
            return false;
        }
        int t = time( NULL );
        int t2 = glutGet( GLUT_ELAPSED_TIME );
        char fileName[ 512 ];
        //char fileName2[ 512 ];
        //char fileName3[ 512 ];
        //char fileName4[ 512 ];
        sprintf( fileName, "data/screenshot/%d__ingame%d.bmp", t, t2 );
        //sprintf( fileName2, "data/screenshot/%d__ingame%d_R.bmp", t, t2 );
        //sprintf( fileName3, "data/screenshot/%d__ingame%d_G.bmp", t, t2 );
        //sprintf( fileName4, "data/screenshot/%d__ingame%d_B.bmp", t, t2 );
        bool saved = saveRGB( texture, fileName, 0x00FFFFFF );
        //saved &= saveRGB( texture, fileName2, 0x00FF0000 );
        //saved &= saveRGB( texture, fileName3, 0x0000FF00 );
        //saved &= saveRGB( texture, fileName4, 0x000000FF );
        delete texture;
        return saved;
    }
    static Texture* getScreenshot() {
        int f[ 4 ];
        glGetIntegerv( GL_VIEWPORT, f );
        Texture* texture = Texture::create( f[ 2 ] - f[ 0 ], f[ 3 ] - f[ 1 ] );
        if ( texture ) {
            glReadPixels( f[ 0 ], f[ 1 ], f[ 2 ], f[ 3 ], GL_RGBA, GL_UNSIGNED_BYTE, texture -> getPixels() );
            texture -> glCreateImage();
        }
        return texture;
    }
    static Texture* loadJoinedTexture( const char* path ) {
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            int w;
            if ( fscanf( handle, "%d %d %d", &_W, &_H, &w ) == 3 ) {
                Texture* tex = Texture::create( _W * w, _H * w );
                if ( tex ) {
                    while ( !feof( handle ) ) {
                        int id;
                        char bufferPath[ 4096 ];
                        if ( fscanf( handle, "%d %s", &id, bufferPath ) == 2 ) {
                            Texture* texPart = load( bufferPath );
                            if ( texPart ) {
                                int xc = ( id % _W ) * w;
                                int yc = ( id / _W ) * w;
                                Texture* tscaled = texPart -> scaleTo( w, w );
                                tscaled -> copyPixels32( tex, 0, 0, xc, yc, w, w );
                                setPartialTexture( id, texPart );
                                delete tscaled;
                            }
                        }
                    }
                    return tex;
                }
            }
            fclose( handle );
        }
        return NULL;
    }

    static int getLastJoinedHorizontalCount() {
        return _W;
    }

    static int getLastJoinedVerticalCount() {
        return _H;
    }

    static Texture* getPartialTexture( int id ) {
        map < int, Texture* >::iterator found = _m.find( id );
        if ( found != _m.end() ) {
            return found -> second;
        }
        return NULL;
    }

    static void setPartialTexture( int id, Texture* texture ) {
        _m[ id ] = texture;
    }

    private:

    static Header ___defaultHeaderRGB( int w, int h ) {
        Header header;
        header.signature = 0x4D42;
        header.filesize = w * h * 3 + sizeof( header );
        header.reserved1 = 0;
        header.reserved2 = 0;
        header.startOffset = sizeof( header );
        header.bihSize = 40;
        header.width = w;
        header.height = h;
        header.planes = 1;
        header.bpp = 24;
        header.compression = 0;
        header.imagesize = w * h * 3;
        header.hdpi = 0;
        header.vdpi = 0;
        header.numberOfColors = 0;
        header.numberOfImportantColors = 0;
        return header;
    }

    static int _W;
    static int _H;

    static map < int, Texture* > _m;

};

int TextureLoader::_W = -1;
int TextureLoader::_H = -1;

map < int, Texture* > TextureLoader::_m;

Texture* Texture::clone() {
    Texture* t = new Texture();
    t -> resize( _w, _h, false );
    int wh = _w * _h;
    int* src = ( int* ) _pixels;
    int* dst = ( int* ) t -> _pixels;
    for ( int i = 0; i < wh; i++ ) {
        dst[ i ] = src[ i ];
    }
    t -> glCreateImage();
    return t;
}*/


class Texture {
    private:

    /*#pragma pack(push, 1)
    struct Header {
        public:
        uint16_t signature;
        uint32_t filesize;
        uint16_t reserved1;
        uint16_t reserved2;
        uint32_t startOffset;
        uint32_t bihSize;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bpp;
        uint32_t compression;
        uint32_t imagesize;
        uint32_t hdpi;
        uint32_t vdpi;
        uint32_t numberOfColors;
        uint32_t numberOfImportantColors;
    };
    #pragma pack(pop)

    static Header ___defaultHeaderRGB( int w, int h ) {
        Header header;
        header.signature = 0x4D42;
        header.filesize = w * h * 3 + sizeof( header );
        header.reserved1 = 0;
        header.reserved2 = 0;
        header.startOffset = sizeof( header );
        header.bihSize = 40;
        header.width = w;
        header.height = h;
        header.planes = 1;
        header.bpp = 24;
        header.compression = 0;
        header.imagesize = w * h * 3;
        header.hdpi = 0;
        header.vdpi = 0;
        header.numberOfColors = 0;
        header.numberOfImportantColors = 0;
        return header;
    }*/

    Texture( SDL_Surface* surface, bool cleanup ) {
        _surface = surface;
        _cleanup = cleanup;
        SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat( _surface, SDL_PIXELFORMAT_RGBA32, 0 );
        if ( convertedSurface ) {
            if ( cleanup ) {
                SDL_FreeSurface( _surface );
            }
            _surface = convertedSurface;
        }
        _texture = 0;
        glCreateImage();
    }

    Texture( GLuint imggl, int w, int h ) {
        _surface = SDL_CreateRGBSurface( 0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
        _cleanup = true;
        _texture = imggl;
    }

    bool _cleanup;
    SDL_Surface* _surface;
    GLuint _texture;

    public:

    ~Texture() {
        SDL_FreeSurface( _surface );
        glDeleteTextures( 1, &_texture );
    }

    static Texture* Load( const char* path ) {
        SDL_Surface* surface = SDL_LoadBMP( path );
        if ( surface ) {
            return new Texture( surface, true );
        }
        return NULL;
    }

    static Texture* Create( int w, int h ) {
        SDL_Surface* surface = SDL_CreateRGBSurface( 0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
        if ( surface ) {
            return new Texture( surface, true );
        }
        return NULL;
    }

    static Texture* Create( SDL_Surface* s ) {
        if ( s ) {
            //printf( "Returning from %p\n", s );
            return new Texture( s, false );
        }
        return NULL;
    }

    static Texture* CreateFromGlTexture( GLuint imggl, int w, int h ) {
        if ( ( imggl ) && ( w > 0 ) && ( h > 0 ) ) {
            return new Texture( imggl, w, h );
        }
        return NULL;
    }

    SDL_Surface* getSurface() {
        return _surface;
    }

    int getWidth() {
        return _surface -> w;
    }
    int getHeight() {
        return _surface -> h;
    }

    void makeOpaque( Uint32 color ) {
        int wh = _surface -> w * _surface -> h;
        Uint32* pixels = ( Uint32* ) _surface -> pixels;
        for ( int i = 0; i < wh; i++ ) {
            if ( pixels[ i ] == color ) {
                pixels[ i ] = 0x00000000;
            }
        }
        glCreateImage();
    }

    GLuint glCreateImage() {
        glEnable( GL_TEXTURE_2D );
        if ( !_texture ) {
            glGenTextures( 1, &_texture );
            glBindTexture( GL_TEXTURE_2D, _texture );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }
        glUseImage();
        SDL_LockSurface( _surface );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _surface -> w, _surface -> h, 0, GL_RGBA, GL_UNSIGNED_BYTE, _surface -> pixels );
        SDL_UnlockSurface( _surface );
        return _texture;
    }

    bool glUseImage() {
        if ( _texture ) {
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, _texture );
            return true;
        }
        return false;
    }

    bool refreshPixelsFromScreen() {
        if ( _texture ) {
            glBindTexture( GL_TEXTURE_2D, _texture );
            int n = _surface -> w * _surface -> h;
            Uint32* _pixels = ( Uint32* ) malloc( sizeof( Uint32 ) * n );
            glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels );
            Uint8* _surfPixels = ( Uint8* ) _surface -> pixels;
            for ( int i = 0; i < _surface -> w; i++ ) {
                for ( int j = 0; j < _surface -> h; j++ ) {
                    Uint32* pixelSrc = _pixels + j * _surface -> w + i;
                    Uint32* pixelDst = ( Uint32* )( _surfPixels + ( ( _surface -> h ) - j - 1 ) * _surface -> pitch + i * _surface -> format -> BytesPerPixel );
                    Uint32 pixel = ( *pixelSrc );
                    pixel = ( pixel & 0xFF00FF00 ) + ( ( pixel << 16 ) & 0x00FF0000 ) + ( ( pixel >> 16 ) & 0x000000FF );
                    ( *pixelDst ) = pixel;
                }
            }
            free( _pixels );
            return true;
        }
        return false;
    }

    void copyPixels32( Texture* dst, int x1, int y1, int x2, int y2, int w, int h ) {
        if ( !dst ) {
            return;
        }
        SDL_Rect srcRect;
        srcRect.x = x1;
        srcRect.y = y1;
        srcRect.w = w;
        srcRect.h = h;
        SDL_Rect dstRect;
        dstRect.x = x2;
        dstRect.y = y2;
        dstRect.w = w;
        dstRect.h = h;
        SDL_BlitSurface( _surface, &srcRect, dst -> _surface, &dstRect );
    }

    void saveRGB( const char* path ) {
        //printf( "PATH: [%s] - SURFACE [%p]\n", path, _surface );
        if ( SDL_SaveBMP( _surface, path ) ) {
            printf( "ERR: %s\n", SDL_GetError() );
        } else {
            printf( "SAVED.\n" );
        }
    }

};


