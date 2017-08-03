#pragma once

#include "ModelData.h"

class BlockModel {
    private:
    BlockModel() {
    }
    ~BlockModel() {
    }
    public:
    /**
        Maps model to specified key. Key values must be unique.
        @param key Key to map.
        @param model Model to map.
        @return true if model was added (there was no key collision), false otherwise.
    */
    static bool addModel( int key, ModelData* model ) {
        if ( getModel( key ) ) {
            return false;
        }
        pair < int, ModelData* > p;
        p.first = key;
        p.second = model;
        _m.insert( p );
        return true;
    }

    /**
        Gets mapped model.
        @see addModel()
        @param key Specified key.
        @return concrete instance of ModelData, or NULL if not present.
    */
    static ModelData* getModel( int key ) {
        return ___peek( key, false );
    }

    /**
        Removes mapped model. THis method does not delete any instance of ModelData.
        @see addModel()
        @param key Specified key.
        @return concrete instance of ModelData, or NULL if not present.
    */
    static ModelData* removeModel( int key ) {
        return ___peek( key, true );
    }

    /**
        Adds models loaded from specified directory. Models must be defined using special language.
        @see ModelData::getModelsFrom()
        @param path Path to file containing proper parsable text.
        @return count of models read.
    */
    static int loadModels( const char* path ) {
        map < int, ModelData* >* m = ModelData::getModelsFrom( path );
        if ( m ) {
            int cntAdded = 0;
            map < int, ModelData* >::iterator it = m -> begin();
            while ( it != m -> end() ) {
                if ( addModel( it -> first, it -> second ) ) {
                    cntAdded++;
                }
                it++;
            }
            delete m;
            return cntAdded;
        }
        return -1;
    }

    static void Init( const char* slfPath ) {
        StringLinker* slf = StringLinker::FromFile( slfPath );
        if ( slf ) {
            for ( int i = 0; i < slf -> entryCount(); i++ ) {
                //int id = slf -> getEntryID( i );
                string path = slf -> getEntryString( i );
                loadModels( path.c_str() );
            }
            delete slf;
        }
    }

    /**
        Overridden from AbstractPathReader.
        @see AbstractPathReader
        @param path File path found during read().
        @return true if file was successfully read; false otherwise.
    */
    /*bool singleFileRead( const char* path ) {
        if ( loadModels( path ) < 0 ) {
            //printf( " > BlockModel::singleFileRead : at \"%s\"\n", path );
            return false;
        }
        return true;
    }*/

    /**
        Calls read() from AbstractPathReader
        @see AbstractPathReader::read()
        @param path Root directory of seeking.
        @param regexp Will search for files which matches this searching sequence.
    */
    /*static void onRead( const char* path, const char* regexp ) {
        BlockModel* modelLoader = new BlockModel();
        modelLoader -> read( path, regexp, true );
        delete modelLoader;
    }*/

    private:
    static ModelData* ___peek( int key, bool eraseMe ) {
        map < int, ModelData* >::iterator found = _m.find( key );
        if ( found != _m.end() ) {
            if ( eraseMe ) {
                _m.erase( found );
            }
            return found -> second;
        }
        return NULL;
    }
    //static BlockModel* _inst;
    static map < int, ModelData* > _m;
};

map < int, ModelData* > BlockModel::_m;

//BlockModel* BlockModel::_inst = NULL;
