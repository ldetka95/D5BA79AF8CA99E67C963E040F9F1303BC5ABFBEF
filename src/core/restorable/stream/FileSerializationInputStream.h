#pragma once

#define FILEBINARYOUTPUTSTREAM_CONST_ALLOCATION_SIZE 4096

class FileSerializationInputStream : public AbstractSerializationInputStream {
    protected:
    FileSerializationInputStream( FILE* handle, const char* path ) : AbstractSerializationInputStream() {
        ___handle = handle;
        fseek( ___handle, 0, SEEK_END );
        ___fsize = ftell( ___handle );
        fseek( ___handle, 0, SEEK_SET );
        ___path = string( path );
        ___lastBuffer = NULL;
        ___allocated = malloc( FILEBINARYOUTPUTSTREAM_CONST_ALLOCATION_SIZE );
    }
    public:
    static FileSerializationInputStream* create( const char* path ) {
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            return new FileSerializationInputStream( handle, path );
        }
        return NULL;
    }
    ~FileSerializationInputStream() {
        fclose( ___handle );
        if ( ___lastBuffer ) {
            free( ___lastBuffer );
        }
        if ( ___allocated ) {
            free( ___allocated );
        }
    }

    void* getMemory( int bytes ) {
        int positionAfter = getCursor() + bytes;
        if ( positionAfter > getSize() ) {
            char msg[ 4096 ];
            sprintf( msg, "Deserialization exception: expected [%d] bytes, got [%d] bytes (lacking %d)", bytes, getSize() - getCursor(), positionAfter - getSize() );
            throw SaginaSerializationException( msg );
        }
        void* alc;
        if ( bytes >= FILEBINARYOUTPUTSTREAM_CONST_ALLOCATION_SIZE ) {
            alc = malloc( bytes );
            if ( ___lastBuffer ) {
                free( ___lastBuffer );
            }
            ___lastBuffer = alc;
        } else {
            alc = ___allocated;
        }
        fread( alc, bytes, 1, ___handle );
        setCursor( positionAfter );
        return alc;
    }

    const char* getPath() {
        return ___path.c_str();
    }

    int getSize() {
        return ___fsize;
    }

    void seek( int position ) {
        setCursor( position );
        fseek( ___handle, position, SEEK_SET );
    }

    private:

    FILE* ___handle;
    string ___path;
    int ___fsize;
    void* ___lastBuffer;
    void* ___allocated;

};
