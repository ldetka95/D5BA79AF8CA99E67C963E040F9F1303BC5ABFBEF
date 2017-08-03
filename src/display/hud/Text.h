#pragma once

#include <SDL2/SDL_ttf.h>

class Text {
    private:
    Text() {
    }
    ~Text() {
    }
    public:

    static void Init( const char* fontPath, int fontSize ) {
        TTF_Init();
        if ( _font ) {
            TTF_CloseFont( _font );
        }
        _font = TTF_OpenFont( fontPath, fontSize );
        Color( 255, 255, 255 );
    }

    static void Write( SDL_Surface* surface, int x, int y, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsprintf( buffer, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = TTF_RenderText_Solid( _font, buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = x;
        textPos.y = y;
        SDL_BlitSurface( tx, 0, surface, &textPos );
        SDL_FreeSurface( tx );
    }

    static SDL_Surface* RenderOnly( const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsprintf( buffer, fmt, args );
        va_end( args );
        //printf( "Rendering [%s]\n", buffer );
        /*SDL_Color color;
        color.r = 1 - _textColor.r;
        color.b = 1 - _textColor.g;
        color.g = 1 - _textColor.b;
        color.a = 0;*/
        SDL_Surface* tx = TTF_RenderText_Solid( _font, buffer, _textColor );
        //printf( "BPP = %d\n", tx -> format -> BytesPerPixel );
        //SDL_Surface* conv = SDL_ConvertSurfaceFormat( tx, SDL_PIXELFORMAT_RGBA32, 0 );
        //printf( "BPP_Converted = %d\n", conv -> format -> BytesPerPixel );
        //SDL_FreeSurface( tx );
        /*SDL_SetColorKey( tx, SDL_TRUE, SDL_MapRGB( tx -> format, 0, 0, 0 ) );
        SDL_Surface* back = SDL_CreateRGBSurface( 0, tx -> w, tx -> h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
        SDL_FillRect( back, NULL, SDL_MapRGBA( back -> format, 0, 0, 0, 0 ) );
        SDL_BlitSurface( tx, NULL, back, NULL );
        SDL_FreeSurface( tx );*/
        /*int wh = conv -> w * tx -> h;
        Uint32* pixels = ( Uint32* ) conv -> pixels;
        //Uint32 color = ( _
        for ( int i = 0; i < wh; i++ ) {
            if ( ( pixels[ i ] & 0x00FFFFFF ) == 0 ) {
                //printf( "set\n" );
                pixels[ i ] = 0x00000000; // xBGR
            }
        }*/
        return tx;//conv;
    }

    static void Write( SDL_Renderer* renderer, int x, int y, const char* fmt, ... ) {
        char buffer[ 4096 ];
        va_list args;
        va_start( args, fmt );
        vsprintf( buffer, fmt, args );
        va_end( args );
        SDL_Surface* tx;
        tx = TTF_RenderText_Solid( _font, buffer, _textColor );
        SDL_Rect textPos;
        textPos.x = x;
        textPos.y = y;
        textPos.w = tx -> w;
        textPos.h = tx -> h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, tx );
        SDL_FreeSurface( tx );
        SDL_RenderCopy( renderer, texture, 0, &textPos );
        SDL_DestroyTexture( texture );
    }

    static void Color( int r, int g, int b ) {
        _textColor.r = r;
        _textColor.g = g;
        _textColor.b = b;
    }

    private:

    static TTF_Font* _font;
    static SDL_Color _textColor;

};

TTF_Font* Text::_font = NULL;
SDL_Color Text::_textColor;


