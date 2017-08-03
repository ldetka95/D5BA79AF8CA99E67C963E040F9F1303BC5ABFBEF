#pragma once

#include "RestorableBuilder.h"
#include "SaginaObject.h"

#include "stream/AbstractSerializationInputStream.h"
#include "stream/AbstractSerializationOutputStream.h"

#include "stream/MemorySerializationInputStream.h"
#include "stream/MemorySerializationOutputStream.h"

#include "stream/FileSerializationInputStream.h"
#include "stream/FileSerializationOutputStream.h"

class Restorable : public SaginaObject {
    public:
    Restorable( CLASS classID ) : SaginaObject( classID ) {
    }
    virtual ~Restorable() {
    }

    virtual bool onSerialize( AbstractSerializationOutputStream* stream ) = 0;
    virtual bool onDeserialize( AbstractSerializationInputStream* stream ) = 0;

    static bool Serialize( Restorable* restorable, AbstractSerializationOutputStream* stream ) {
        if ( ( stream ) && ( restorable ) ) {
            stream -> add( restorable -> getClass() );
            return restorable -> onSerialize( stream );
        }
        return false;
    }

    static Restorable* Deserialize( AbstractSerializationInputStream* stream ) {
        if ( stream ) {
            int cur = stream -> getCursor();
            Restorable* restorable = NULL;
            try {
                CLASS classID = stream -> get < CLASS > ();
                restorable = ___build( classID );
                if ( restorable ) {
                    restorable -> onDeserialize( stream );
                }
                return restorable;
            } catch ( SaginaException se ) {
                se.log();
                if ( restorable ) {
                    delete restorable;
                }
                stream -> seek( cur );
            }
        }
        return NULL;
    }

    template < class T > static T* TryDeserialize( AbstractSerializationInputStream* stream ) {
        try {
            return TryDeserializeThrow < T > ( stream );
        } catch ( SaginaSerializationException sse ) {
            sse.log();
            //throw sse;
            return NULL;
        }
    }

    template < class T > static T* TryDeserializeThrow( AbstractSerializationInputStream* stream ) {
        int cur = stream -> getCursor();
        Restorable* restorable = Deserialize( stream );
        T* restorableCast = dynamic_cast < T* > ( restorable );
        if ( restorableCast ) {
            return restorableCast;
        }
        stream -> seek( cur );
        char msg[ 4096 ];
        if ( restorable ) {
            sprintf( msg, "Invalid deserialized class: %d, expected another (?)", restorable -> getClass() );
        } else {
            sprintf( msg, "Invalid deserialized class: NULL, expected another (?)" );
        }
        throw SaginaSerializationException( msg );
    }

    static void Register( CLASS classID, RestorableBuilder* builder ) {
        if ( !builder ) {
            return;
        }
        map < CLASS, RestorableBuilder* >::iterator found = ______BUILDERMAP.find( classID );
        if ( found == ______BUILDERMAP.end() ) {
            pair < CLASS, RestorableBuilder* > p;
            p.first = classID;
            p.second = builder;
            ______BUILDERMAP.insert( p );
        }
    }

    static void UnregisterAll() {
        map < CLASS, RestorableBuilder* >::iterator it = ______BUILDERMAP.begin();
        while ( it != ______BUILDERMAP.end() ) {
            delete it -> second;
            it++;
        }
        ______BUILDERMAP.clear();
    }

    bool serialize( AbstractSerializationOutputStream* stream ) {
        return Serialize( this, stream );
    }

    private:

    static Restorable* ___build( CLASS classID ) {
        map < CLASS, RestorableBuilder* >::iterator found = ______BUILDERMAP.find( classID );
        if ( found != ______BUILDERMAP.end() ) {
            return ( found -> second ) -> build();
        }
        return NULL;
    }

    static map < CLASS, RestorableBuilder* > ______BUILDERMAP;

};

map < CLASS, RestorableBuilder* > Restorable::______BUILDERMAP;

#include "Container.h"

