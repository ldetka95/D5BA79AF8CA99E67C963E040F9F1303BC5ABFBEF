#pragma once

#define ENTITY_CLASS 11942

#include "Inventory.h"

#include <math.h>

class Entity : public Restorable, public EntityInterface {
    protected:
    Entity( CLASS classID ) : Restorable( classID ), EntityInterface() {
        _id = 0;
        ___varCount = 0;
        ___var = NULL;
        ___pos = Vector3D( 0.0, 0.0, 0.0 );
        ___movement = Vector3D( 0.0, 0.0, 0.0 );
        ___movementDerv = Vector3D( 0.0, 0.0, 0.0 );
        ___angles = PolarVector3D( 1.0, 0.0, 0.0 );
        ___inventory = new Inventory();
        ___wallJoint = Vector3D( 0.0, 0.0, 0.0 );
        initVar( ENTITY_VAR_NUMBER );
    }

    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        Restorable* build() {
            return new Entity();
        }
    };

    Entity() : Restorable( ENTITY_CLASS ), EntityInterface() {
        _id = 0;
        ___varCount = 0;
        ___var = NULL;
        ___pos = Vector3D( 0.0, 0.0, 0.0 );
        ___movement = Vector3D( 0.0, 0.0, 0.0 );
        ___movementDerv = Vector3D( 0.0, 0.0, 0.0 );
        ___angles = PolarVector3D( 1.0, 0.0, 0.0 );
        ___inventory = new Inventory();
        ___wallJoint = Vector3D( 0.0, 0.0, 0.0 );
        initVar( ENTITY_VAR_NUMBER );
    }

    public:
    virtual ~Entity() {
        initVar( 0 );
        if ( ___inventory ) {
            delete ___inventory;
        }
    }

    static void Register() {
        Restorable::Register( ENTITY_CLASS, new Builder() );
        Inventory::Register();
        ___varConverter -> appendMapping( EV_MODELID, "MODEL_ID" );
        ___varConverter -> appendMapping( EV_MAXHP, "HP" );
        ___varConverter -> appendMapping( EV_SPEED, "SPEED" );
        ___varConverter -> appendMapping( EV_CYLINDER_RADIUS, "CYLINDER_RADIUS" );
        ___varConverter -> appendMapping( EV_CYLINDER_HEIGHT, "CYLINDER_HEIGHT" );
        ___varConverter -> appendMapping( EV_HOTSPOTHEIGHT, "HOTSPOT_HEIGHT" );
        ___varConverter -> appendMapping( EV_JUMP, "JUMP" );
        ___varConverter -> appendMapping( EV_IS_PLAYER, "IS_PLAYER" );
        ___varConverter -> appendMapping( EV_SPEED_SPRINT, "SPRINT_SPEED" );
        ___varConverter -> appendMapping( EV_CAN_JUMP_ON_WALL, "IS_SPIDER" );
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        stream -> add( _id );
        stream -> add( ___pos );
        stream -> add( ___movement );
        stream -> add( ___movementDerv );
        stream -> add( ___angles );
        stream -> add( ___wallJoint );
        stream -> add( ___varCount );
        for ( int i = 0; i < ___varCount; i++ ) {
            stream -> add( ___var[ i ] );
        }
        Restorable::Serialize( ___inventory, stream );
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        _id = stream -> get < ID > ();
        ___pos = stream -> get < Vector3D > ();
        ___movement = stream -> get < Vector3D > ();
        ___movementDerv = stream -> get < Vector3D > ();
        ___angles = stream -> get < PolarVector3D > ();
        ___wallJoint = stream -> get < Vector3D > ();
        ___varCount = stream -> get < int > ();
        initVar( ___varCount );
        for ( int i = 0; i < ___varCount; i++ ) {
            ___var[ i ] = stream -> get < double > ();
        }
        Inventory* inv = Restorable::TryDeserialize < Inventory > ( stream );
        if ( inv ) {
            if ( ___inventory ) {
                delete ___inventory;
            }
            ___inventory = inv;
        }
        return true;
    }

    static Entity* ReadRaw( const char* path ) {
        FILE* handle = fopen( path, "r" );
        if ( handle ) {
            Entity* ent = new Entity();
            //int id = -1;
            while ( !feof( handle ) ) {
                char buffer[ 4096 ];
                double value;
                if ( fscanf( handle, "%s = %lf", buffer, &value ) == 2 ) {
                    /*if ( strcmp( buffer, "ID" ) == 0 ) {
                        id = value;
                    } else {*/
                        string word = string( buffer );
                        int i = ___varConverter -> getID( word );
                        if ( i >= 0 ) {
                            ent -> ___var[ i ] = value;
                        }
                    //}
                }
            }
            /*if ( id >= 0 ) {
                ___ENTITYTYPES[ id ] = ent;
            } else {
                delete ent;
                ent = NULL;
                throw SaginaException( string( "unspecified ID in entity: " ) + string( path ) );
            }*/
            fclose( handle );
            return ent;
        }
        return NULL;
    }

    static Entity* ReadRaw( string path ) {
        return ReadRaw( path.c_str() );
    }

    void initVar( int size ) {
        ___var = ( double* ) realloc( ___var, size * sizeof( double ) );
        for ( int i = ___varCount; i < size; i++ ) {
            ___var[ i ] = 0.0;
        }
        ___varCount = size;
    }

    double getVar( int i ) {
        if ( ( i >= 0 ) && ( i < ___varCount ) ) {
            return ___var[ i ];
        }
        return NAN;
    }

    void setVar( int i, double v ) {
        if ( ( i >= 0 ) && ( i < ___varCount ) ) {
            ___var[ i ] = v;
        }
    }

    int getVarCount() {
        return ___varCount;
    }

    /*void print() {
        printf( "Entity %p\n", this );
        for ( int i = 0; i < ___varCount; i++ ) {
            printf( "   var[ %d ] = %lf\n", i, ___var[ i ] );
        }
        printf( "\n" );
    }*/

    Entity* clone() {
        Entity* ent = new Entity();
        ent -> _id = _id;
        ent -> ___pos = ___pos;
        for ( int i = 0; i < ___varCount; i++ ) {
            ent -> ___var[ i ] = ___var[ i ];
        }
        return ent;
    }

    bool isPlayer() {
        return ( int( round( getVar( EV_IS_PLAYER ) ) ) == 1 );
    }

    static void Init( const char* slfPath ) {
        StringLinker* slf = StringLinker::FromFile( slfPath );
        if ( slf ) {
            for ( int i = 0; i < slf -> entryCount(); i++ ) {
                string path = slf -> getEntryString( i );
                Entity* entity = ReadRaw( path );
                if ( entity ) {
                    int id = slf -> getEntryID( i );
                    entity -> _id = id;
                    ___ENTITYTYPES[ id ] = entity;
                }
            }
            delete slf;
        }
    }

    static Entity* Create( int id, Vector3D initPos ) {
        Entity* ent = NULL;
        map < int, Entity* >::iterator found = ___ENTITYTYPES.find( id );
        if ( found != ___ENTITYTYPES.end() ) {
            ent = found -> second -> clone();
            ent -> ___pos = initPos;
            /// custom initialization
            ent -> ___var[ EV_HP ] = ent -> ___var[ EV_MAXHP ];
        }
        return ent;
    }

    Vector3D* getPosition() {
        return &___pos;
    }

    Vector3D* getMovement() {
        return &___movement;
    }

    PolarVector3D* getAngles() {
        return &___angles;
    }

    Vector3D* getWallJoint() {
        return &___wallJoint;
    }

    void lookAt( Vector3D posToReach ) {
        ___angles = PolarVector3D( posToReach - ___pos );
    }

    void tryReach( Map* m, Container* allEntityContainer, Vector3D posToReach, double speed ) {
        lookAt( posToReach );
        tryReachTarget( m, allEntityContainer, speed );
    }

    void tryReachNoLook( Map* m, Vector3D posToReach, double speed ) {
        PolarVector3D anglesOrig = ___angles;
        lookAt( posToReach );
        move( m, speed, 0.0, NULL );
        anglesOrig = ___angles;
    }

    void tryReachTarget( Map* m, Container* allEntityContainer, double speed ) {
        move( m, speed, 0.0, allEntityContainer );
    }

    void move( Map* m, double fw, double lr, Container* allEntityContainer ) {
        double isOnWall = getVar( EVT_IS_ON_WALL );
        if ( isnan( isOnWall ) || isOnWall == 0.0 ) {
            moveNormal( m, fw, lr, allEntityContainer );
        } else if ( isOnWall == 1.0 ) {
            moveSpider( m, fw, lr, allEntityContainer );
        }
    }

    void moveNormal( Map* m, double fw, double lr, Container* allEntityContainer ) {
        Vector3D axisWeight = Vector3D( 1.0, 0.0, 1.0 );
        Vector3D cartesianMove = Vector3D( ___angles ).normalize();
        Vector3D cartesianWeightedMove = Vector3D( cartesianMove.x * axisWeight.x, cartesianMove.y * axisWeight.y, cartesianMove.z * axisWeight.z ).normalize();
        ___movement = ___movement.balance( Vector3D(
            cartesianWeightedMove.x * fw - cartesianWeightedMove.z * lr,
            - cartesianWeightedMove.y * fw,
            cartesianWeightedMove.z * fw + cartesianWeightedMove.x * lr
        ), Physics::MOVEMENT_INERTIA );
        Vector3D XZMovOrig = ___movement.normalize();
        XZMovOrig.y = 0.0;
        Vector3D simplifiedPosAfterXZ = ___pos + XZMovOrig;
        if ( allEntityContainer ) {
            for ( Container::Iterator i = allEntityContainer -> getIterator(); i.hasNext(); i.next() ) {
                Entity* ent = dynamic_cast < Entity* > ( i.get() );
                if ( ( ent != this ) && ( ent != NULL ) ) {
                    Vector3D itDif = *( ent -> getPosition() ) - ___pos;
                    itDif.y = 0.0;
                    //if ( itDif.dist3D() < getVar( EV_CYLINDER_RADIUS ) ) {
                    if ( CollisionPoints::InsideCylinder( getHotSpot(), ent -> getHotSpot(), ent -> getVar( EV_CYLINDER_RADIUS ), ent -> getVar( EV_CYLINDER_HEIGHT ) ) ) {
                        Vector3D movement = - itDif.normalize();
                        ___movement += movement * PUSHED_COEFF;
                        ent -> tryReachNoLook( m, ent -> ___pos + itDif, movement.dist3D() * PUSH_BACK_COEFF );
                    }
                }
            }
        }
        ___checkModelMapCollision( m );
        ___movement = Physics::AffectGravity( ___movement, ___movementDerv );
        Vector3D checker = Vector3D(
            ___pos.x,
            ___pos.y + ___movement.y + ___movementDerv.y,
            ___pos.z
        );
        if ( m -> pointCollides( checker ) ) {
            ___pos.y = floor( ___pos.y );
            ___movementDerv.y = 0.0;
            ___movement.y = 0.0;
        } else if ( m -> pointCollides( simplifiedPosAfterXZ ) && ( getVar( EV_CAN_JUMP_ON_WALL ) == 1.0 ) ) {
            // catch the wall
            ___wallJoint = XZMovOrig.axisVector();
            setVar( EVT_IS_ON_WALL, 1.0 );
            //printf( "CAUGHT THE WALL: %g, %g, %g\n", ___wallJoint.x, ___wallJoint.y, ___wallJoint.z );
        }
        ___pos += ___movement;
        ___rpos = ___pos;
        ___rpos.y += getVar( EV_HOTSPOTHEIGHT );
    }

    void moveSpider( Map* m, double fw, double lr, Container* allEntityContainer ) {
        Vector3D axisWeight = Vector3D( absf( ___wallJoint.z ), 0.0, absf( ___wallJoint.x ) );
        Vector3D cartesianMove = Vector3D( ___angles ).normalize();
        Vector3D cartesianWeightedMove = Vector3D( cartesianMove.x * axisWeight.x, cartesianMove.y * axisWeight.y, cartesianMove.z * axisWeight.z ).normalize();
        ___movement = ___movement.balance( Vector3D(
            cartesianWeightedMove.x * fw,
            - cartesianWeightedMove.y * fw,
            cartesianWeightedMove.z * fw
        ), Physics::MOVEMENT_INERTIA );
        //Vector3D nextPos = ___pos + ___movement;
        /*Vector3D jumpOff = ___movement.multiplyElements( ___wallJoint );
        if ( jumpOff.dist3D() > ( 0.5 * ___movement.dist3D() ) ) {
            ___wallJoint = Vector3D( 0.0, 0.0, 0.0 );
            setVar( EVT_IS_ON_WALL, 0.0 );
            ___movement = jumpOff * ___movement.dist3D();
            printf( "OFF THE WALL\n" );
        }*/
        if ( !m -> pointCollides( ___pos + ___movement + ___wallJoint ) ) {
            jump( m, getVar( EV_JUMP ) );
        }
        ___checkModelMapCollision( m );
        ___pos += ___movement;
        ___rpos = ___pos;
        ___rpos.y += getVar( EV_HOTSPOTHEIGHT );
    }

    void jump( Map* m, double power ) {
        bool isOnWall = ( isnan( getVar( EVT_IS_ON_WALL ) ) || getVar( EVT_IS_ON_WALL ) == 0.0 );
        if ( isOnWall ) {
            Vector3D posBelow = ___pos + Vector3D( 0.0, - 0.01, 0.0 );
            if ( m -> pointCollides( posBelow ) ) {
                ___movementDerv.y = 0.0;
                ___movement.y = power;
            }
        } else {
            ___movement -= ___wallJoint * getVar( EV_JUMP ) * 2.0;
            ___movement.y += getVar( EV_JUMP );
            ___wallJoint = Vector3D( 0.0, 0.0, 0.0 );
            setVar( EVT_IS_ON_WALL, 0.0 );
            //printf( "OFF THE WALL\n" );
        }
    }

    void display() {
        double modelID = getVar( EV_MODELID );
        if ( !isnan( modelID ) ) {
            ModelData* model = BlockModel::getModel( modelID );
            if ( model ) {
                glPushMatrix();
                Vector3D spot = getHotSpot();
                glTranslated( spot.x, spot.y, spot.z );
                glRotated( degr( - ___angles.i ), 0.0, 1.0, 0.0 );
                model -> display();
                glPopMatrix();
            }
        }
    }

    void cameraView() {
        getHotSpot();
        Camera::Link( &___rpos, &___angles );
    }

    Vector3D getHotSpot() {
        ___rpos = ___pos;
        ___rpos.y += getVar( EV_HOTSPOTHEIGHT );
        return ___rpos;
    }

    Ammo* shoot() {
        if ( !___inventory ) {
            return NULL;
        }
        Weapon* w = ___inventory -> getSelectedWeapon();
        if ( !w ) {
            return NULL;
        }
        return w -> shoot( this );
    }

    bool reloadWeapon() {
        if ( !___inventory ) {
            return false;
        }
        Weapon* w = ___inventory -> getSelectedWeapon();
        if ( w ) {
            return w -> reload();
        }
        return false;
    }

    void idleTick() {
        if ( ___inventory ) {
            Weapon* w = ___inventory -> getSelectedWeapon();
            if ( w ) {
                w -> reloadTick();
            }
        }
    }

    Inventory* getInventory() {
        return ___inventory;
    }

    static const double PUSHED_COEFF = 0.04;
    static const double PUSH_BACK_COEFF = 0.03;

    private:

    void ___checkModelMapCollision( Map* m ) {
        double v = getVar( EV_MODELID );
        ModelData* model = NULL;
        if ( !isnan( v ) ) {
            model = BlockModel::getModel( v );
        }
        CollisionPoints* points = NULL;
        if ( model ) {
            Vector3D overallMovement = ___movement;
            points = model -> getCollisionPoints();
            list < Vector3D* >* lst = points -> getList();
            list < Vector3D* >::iterator it = lst -> begin();
            while ( it != lst -> end() ) {
                Vector3D point = *( *it );
                Vector3D ptMovement = m -> checkDerivativeCollision( ___pos + point, ___movement );
                overallMovement = overallMovement.applySlowDown( ptMovement );
                it++;
            }
            ___movement = overallMovement;
        } else {
            ___movement = m -> checkDerivativeCollision( ___pos, ___movement );
        }
    }

    static StringLinker* ___varConverter;
    static map < int, Entity* > ___ENTITYTYPES;

    ID _id;
    Vector3D ___rpos;
    Vector3D ___pos;
    Vector3D ___movement;
    Vector3D ___movementDerv;
    PolarVector3D ___angles;
    Vector3D ___wallJoint;
    double* ___var;
    int ___varCount;
    Inventory* ___inventory;

};

StringLinker* Entity::___varConverter = new StringLinker();
map < int, Entity* > Entity::___ENTITYTYPES;

