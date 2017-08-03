#pragma once

class FileSerializationOutputStream : public AbstractSerializationOutputStream {
    protected:
    FileSerializationOutputStream( FILE* handle, const char* path ) : AbstractSerializationOutputStream() {
        ___handle = handle;
        ___path = string( path );
        ___total = 0;
    }
    public:
    static FileSerializationOutputStream* create( const char* path ) {
        FILE* handle = fopen( path, "wb" );
        if ( handle ) {
            return new FileSerializationOutputStream( handle, path );
        }
        return NULL;
    }
    static FileSerializationOutputStream* createAppendable( const char* path ) {
        FILE* handle = fopen( path, "ab" );
        if ( handle ) {
            return new FileSerializationOutputStream( handle, path );
        }
        return NULL;
    }
    ~FileSerializationOutputStream() {
        fclose( ___handle );
    }

    void addMemory( const void* src, int srcLength ) {
        fwrite( src, srcLength, 1, ___handle );
        ___total = ftell( ___handle );
    }

    void addBuffer( ByteBuffer* buffer ) {
        if ( buffer ) {
            fwrite( buffer -> getRawData(), buffer -> getSize(), 1, ___handle );
            ___total = ftell( ___handle );
        }
    }

    FileSerializationInputStream* toInputStream() {
        if ( ___handle ) {
            fclose( ___handle );
            ___handle = 0;
        }
        return FileSerializationInputStream::create( ___path.c_str() );
    }

    const char* getPath() {
        return ___path.c_str();
    }

    FILE* getFileHandle() {
        return ___handle;
    }

    int getFileSize() {
        return ___total;
    }

    private:

    FILE* ___handle;
    string ___path;
    int ___total;

};



