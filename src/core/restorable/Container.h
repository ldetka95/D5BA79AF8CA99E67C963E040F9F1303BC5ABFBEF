#pragma once

const CLASS CONTAINER_CLASS = 5176;

#include <stdint.h>

typedef int64_t ID;

#include "AbstractRestorableOperation.h"

class Container : public Restorable {
    public:

    class Iterator {
        public:
        Iterator( Container* container ) {
            _c = container;
            _begin = ( _c -> _map ).begin();
            _end = ( _c -> _map ).end();
            reset();
        }
        ~Iterator() {
        }

        bool hasNext() {
            if ( _it == _end ) {
                return false;
            }
            return true;
            /*map < ID, Restorable* >::iterator it2 = _it;
            it2++;
            return ( it2 != _end );*/
        }

        void next() {
            if ( hasNext() ) {
                _it++;
            }
        }

        ID getID() {
            if ( _it != _end ) {
                return _it -> first;
            }
            return -1;
        }

        Restorable* get() {
            if ( _it != _end ) {
                return _it -> second;
            }
            return NULL;
        }

        Restorable* remove() {
            Restorable* r = get();
            if ( r ) {
                ID id = getID();
                _it++;
                _c -> remove( id );
            }
            return r;
        }

        void reset() {
            _it = _begin;
        }

        void end() {
            _it = _end;
        }

        Container* getContainer() {
            return _c;
        }

        private:

        Container* _c;
        map < ID, Restorable* >::iterator _begin;
        map < ID, Restorable* >::iterator _it;
        map < ID, Restorable* >::iterator _end;

    };


    Container() : Restorable( CONTAINER_CLASS ) {
        _isMapDirty = true;
        _counter = 0;
    }
    virtual ~Container() {
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( ( unsigned int ) _map.size() );
        stream -> add( ( ID ) _counter );
        map < ID, Restorable* >::iterator it = _map.begin();
        map < Restorable*, ID > referenceMap;
        while ( it != _map.end() ) {
            stream -> add( ( ID ) it -> first );
            Restorable* res = it -> second;
            map < Restorable*, ID >::iterator itref = referenceMap.find( res );
            if ( itref == referenceMap.end() ) {
                referenceMap[ res ] = it -> first;
                stream -> add( ( char ) 0 );
                res -> serialize( stream );
            } else {
                stream -> add( ( char ) 1 );
                stream -> add( ( ID ) itref -> second );
            }
            it++;
        }
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        unsigned int cnt = stream -> get < unsigned int > ();
        _counter = stream -> get < ID > ();
        try {
            for ( unsigned int i = 0; i < cnt; i++ ) {
                ID id = stream -> get < ID > ();
                char ex = stream -> get < char > ();
                Restorable* restorable = NULL;
                if ( ex == 0 ) {
                    restorable = Restorable::Deserialize( stream );
                } else if ( ex == 1 ) {
                    ID id2 = stream -> get < ID > ();
                    map < ID, Restorable* >::iterator found = _map.find( id2 );
                    if ( found != _map.end() ) {
                        restorable = found -> second;
                    } else {
                        char msg[ 4096 ];
                        sprintf( msg, "Deserialization exception: dangling reference: %d", ( int ) id2 );
                        throw SaginaSerializationException( msg );
                    }
                } else {
                    char msg[ 4096 ];
                    sprintf( msg, "Deserialization exception: invalid container linking: %d", ( int ) ex );
                    throw SaginaSerializationException( msg );
                }
                if ( restorable ) {
                    pair < ID, Restorable* > p;
                    p.first = id;
                    p.second = restorable;
                    _map.insert( p );
                    _addOccurrence( restorable );
                }
            }
        } catch ( SaginaException se ) {
            deleteAll();
            throw se;
        }
        return true;
    }

    ID add( Restorable* r ) {
        if ( r ) {
            ID id = _counter;
            _map[ id ] = r;
            _addOccurrence( r );
            _counter++;
            while ( _get( _counter, false ) ) {
                _counter++;
            }
            _isMapDirty = true;
            return id;
        }
        return -1;
    }

    void add( ID id, Restorable* r ) {
        if ( ( r ) && ( id >= 0 ) ) {
            _map[ id ] = r;
            _addOccurrence( r );
            while ( _get( _counter, false ) ) {
                _counter++;
            }
            _isMapDirty = true;
        }
    }

    Restorable* get( ID index ) {
        return _get( index, false );
    }

    Restorable* remove( ID index ) {
        return _get( index, true );
    }

    int size() {
        return _map.size();
    }

    void clear() {
        _map.clear();
        _mapOccurrenceCounter.clear();
        _isMapDirty = true;
    }

    void deleteAll() {
        map < ID, Restorable* >::iterator it = _map.begin();
        while ( it != _map.end() ) {
            if ( _getOccurrenceCount( it -> second ) > 0 ) {
                _mapOccurrenceCounter[ it -> second ] = 0;
                delete it -> second;
            }
            it++;
        }
        clear();
    }

    void forEach( AbstractRestorableOperation* operation ) {
        if ( operation ) {
            map < ID, Restorable* >::iterator it = _map.begin();
            while ( it != _map.end() ) {
                operation -> perform( it -> second );
                it++;
            }
        }
    }

    void forEachUnique( AbstractRestorableOperation* operation ) {
        if ( operation ) {
            map < Restorable*, bool > feMap;
            map < Restorable*, bool >::iterator feMapFound;
            map < ID, Restorable* >::iterator it = _map.begin();
            while ( it != _map.end() ) {
                Restorable* r = it -> second;
                feMapFound = feMap.find( r );
                if ( feMapFound == feMap.end() ) {
                    operation -> perform( r );
                    feMap[ r ] = true;
                }
                it++;
            }
        }
    }

    /*template < class T > list < T* >* getList() {
        //if ( _isMapDirty ) {
        list < T* >* l = new list < T* > ();
            //_list.clear();
            map < Restorable*, int >::iterator it = _map.begin();
            while ( it != _map.end() ) {
                T* t = dynamic_cast < T* > ( it -> second );
                if ( t ) {
                    l -> push_back( t );
                }
                it++;
            }
            _isMapDirty = false;
        //}
        return l;
    }*/

    map < ID, Restorable* >* getMap() {
        return &_map;
    }

    Iterator getIterator() {
        return Iterator( this );
    }

    static void Register() {
        Restorable::Register( CONTAINER_CLASS, new Builder() );
    }

    protected:

    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        virtual ~Builder() {}
        Restorable* build() {
            return new Container();
        }
    };

    private:

    inline Restorable* _get( ID index, bool rem ) {
        map < ID, Restorable* >::iterator found = _map.find( index );
        if ( found != _map.end() ) {
            Restorable* r = found -> second;
            if ( rem ) {
                _map.erase( found );
                _subOccurrence( r );
                _isMapDirty = true;
            }
            return r;
        }
        return NULL;
    }

    int _getOccurrenceCount( Restorable* r ) {
        map < Restorable*, int >::iterator found = _mapOccurrenceCounter.find( r );
        if ( found != _mapOccurrenceCounter.end() ) {
            return found -> second;
        }
        return 0;
    }

    void _addOccurrence( Restorable* r ) {
        _mapOccurrenceCounter[ r ] = _getOccurrenceCount( r ) + 1;
    }

    void _subOccurrence( Restorable* r ) {
        _mapOccurrenceCounter[ r ] = _getOccurrenceCount( r ) - 1;
    }

    map < ID, Restorable* > _map;
    map < Restorable*, int > _mapOccurrenceCounter;
    list < Restorable* > _list;
    bool _isMapDirty;
    ID _counter;

};

