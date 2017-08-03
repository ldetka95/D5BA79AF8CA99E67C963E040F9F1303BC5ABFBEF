#pragma once

#include <vector>

class StringLinker {
    protected:

    #pragma pack( push, 1 )
    struct FileHeader {
        public:
        int size;
    };
    #pragma pack( pop )

    static void DefaultHeader( FileHeader* header ) {
        header -> size = 0;
    }

    public:
    StringLinker() {
        _size = 0;
        DefaultHeader( &_header );
    }
    ~StringLinker() {
        _resize( 0 );
    }

    static StringLinker* FromFile( const char* path ) {
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            StringLinker* slf = new StringLinker();
            // load
            char* buffer = ( char* ) malloc( 64 * 1024 + 1 );
            try {
                if ( fread( &( slf -> _header ), 1, sizeof( &( slf -> _header ) ), handle ) != sizeof( &( slf -> _header ) ) ) {
                    throw -1; // all these creepy throws used to fail over ._.
                }
                slf -> _resize( ( slf -> _header ).size );
                int32_t id;
                uint16_t sl;
                for ( int i = 0; i < slf -> _size; i++ ) {
                    if ( fread( &id, 1, 4, handle ) != 4 ) {
                        throw i;
                    }
                    if ( fread( &sl, 1, 2, handle ) != 2 ) {
                        throw i;
                    }
                    if ( fread( buffer, 1, sl, handle ) != sl ) {
                        throw i;
                    }
                    buffer[ sl ] = 0;
                    slf -> _setMapping( i, id, string( buffer ) );
                }
            } catch ( int i ) {
                delete slf;
                slf = NULL;
            }
            free( buffer );
            fclose( handle );
            return slf;
        }
        return NULL;
    }

    static StringLinker* FromFile( string path ) {
        return FromFile( path.c_str() );
    }

    void save( const char* path ) {
        FILE* handle = fopen( path, "wb" );
        if ( handle ) {
            // save
            _header.size = _size;
            fwrite( &_header, sizeof( _header ), 1, handle );
            for ( int i = 0; i < _size; i++ ) {
                fwrite( &( _intEntry[ i ] ), 4, 1, handle );
                const char* cc = _stringEntry[ i ].c_str();
                uint16_t len = strlen( cc );
                fwrite( &len, 2, 1, handle );
                fwrite( cc, len, 1, handle );
            }
            fclose( handle );
        }
    }

    void save( string path ) {
        save( path.c_str() );
    }

    int entryCount() {
        return _size;
    }

    int getEntryID( int index ) {
        if ( ( index >= 0 ) && ( index < _size ) ) {
            return _intEntry[ index ];
        }
        return -1;
    }

    string getEntryString( int index ) {
        if ( ( index >= 0 ) && ( index < _size ) ) {
            return _stringEntry[ index ];
        }
        return "";
    }

    int getID( string value ) {
        map < string, int >::iterator found = _stringMap.find( value );
        if ( found != _stringMap.end() ) {
            return found -> second;
        }
        return -1;
    }

    string getString( int value ) {
        map < int, string >::iterator found = _intMap.find( value );
        if ( found != _intMap.end() ) {
            return found -> second;
        }
        return "";
    }

    void appendMapping( int k, string v ) {
        int i = _size;
        _resize( _size + 1 );
        _setMapping( i, k, v );
    }

    private:

    void _setMapping( int i, int k, string v ) {
        if ( ( i >= 0 ) && ( i < _size ) ) {
            _intEntry[ i ] = k;
            _stringEntry[ i ] = v;
            _intMap[ k ] = v;
            _stringMap[ v ] = k;
        }
    }

    void _resize( int s ) {
        _size = s;
        _intEntry.resize( s );
        _stringEntry.resize( s );
    }

    map < int, string > _intMap;
    map < string, int > _stringMap;
    vector < int > _intEntry;
    vector < string > _stringEntry;
    int _size;
    FileHeader _header;

};

