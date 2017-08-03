#pragma once

#include <list>

using namespace std;

/**
    Class containing relative collision points of any entity or object.
*/
class CollisionPoints {
    public:
    /**
        Creates new instance of collision point list.
    */
    CollisionPoints( Vector3D dim, Vector3D center ) {
        _dim = dim;
        _center = center;
    }
    ~CollisionPoints() {
        list < Vector3D* >::iterator it = _vlist.begin();
        while ( it != _vlist.end() ) {
            delete ( *it );
            it++;
        }
    }
    /**
        Adds point to collision points list. Creates hard copy of this point.
        @param point Point to add to. May be local; only values are used.
    */
    void addPoint( Vector3D point ) {
        _vlist.push_back( new Vector3D( point ) );
    }

    /**
        Gets whole raw list of collision points for eventual updates. Must be not deleted.
        @return pointer to list containing all collision points which has been added to this instance, never NULL.
    */
    list < Vector3D* >* getList() {
        return &_vlist;
    }

    Vector3D getDimensions() {
        return _dim;
    }

    Vector3D getCenter() {
        return _center;
    }

    static bool InsideCylinder( Vector3D point, Vector3D cylinderHotspot, double cylinderRadius, double cylinderHeight ) {
        Vector3D cXZ = Vector3D( cylinderHotspot.x, 0.0, cylinderHotspot.z );
        Vector3D pXZ = Vector3D( point.x, 0.0, point.z );
        return ( ( cXZ.dist3D( pXZ ) < cylinderRadius ) && ( absf( cylinderHotspot.y - point.y ) < cylinderHeight / 2.0 ) );
    }

    private:
    list < Vector3D* > _vlist;

    Vector3D _dim;
    Vector3D _center;

};


