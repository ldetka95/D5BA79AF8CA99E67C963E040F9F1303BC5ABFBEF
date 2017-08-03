#pragma once

#include <math.h>
#include "MathUtilities.h"

class PolarVector3D;

/**
    Basic three-dimensional Cartesian vector.
    High-end may change over lifetime.
    Low-end is always a ( 0, 0, 0 ) vector.
*/
class Vector3D {
    public:

    Vector3D operator + ( Vector3D vec ) {
        return Vector3D( x + vec.x, y + vec.y, z + vec.z );
    }

    Vector3D operator - ( Vector3D vec ) {
        return Vector3D( x - vec.x, y - vec.y, z - vec.z );
    }

    Vector3D operator -() {
        return Vector3D( - x, - y, - z );
    }

    Vector3D operator * ( double a ) {
        return Vector3D( x * a, y * a, z * a );
    }

    Vector3D operator / ( double a ) {
        return Vector3D( x / a, y / a, z / a );
    }

    Vector3D operator += ( Vector3D vec ) {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return * this;
    }

    Vector3D operator -= ( Vector3D vec ) {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return * this;
    }

    Vector3D operator *= ( double a ) {
        x *= a;
        y *= a;
        z *= a;
        return * this;
    }

    Vector3D operator /= ( double a ) {
        x /= a;
        y /= a;
        z /= a;
        return * this;
    }

    /**
        Creates a vector where high-end and low-end overlaps themselves.
    */
    Vector3D() {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    /**
        Creates a vector with specified high-end.
        @param x X coordinate.
        @param y Y coordinate.
        @param z Z coordinate.
    */
    Vector3D( double x, double y, double z ) {
        this -> x = x;
        this -> y = y;
        this -> z = z;
    }

    /**
        Creates a vector which is clone of specified Cartesian vector.
        @param vec Vector to clone.
    */
    Vector3D( const Vector3D& vec ) {
        x = vec.x;
        y = vec.y;
        z = vec.z;
    }

    /**
        Creates a vector which is clone of specified Cartesian vector passed as array of doubles. Result is undefined if array is shorter than number of dimensions in vector. Will fail if argument is NULL.
        @param vArray Pointer to array.
    */
    Vector3D( double* vArray ) {
        x = vArray[ 0 ];
        y = vArray[ 1 ];
        z = vArray[ 2 ];
    }

    /**
        Creates a vector basing on polar vector.
        @see PolarVector3D
        @param pvec Polar vector to convert to Cartesian.
    */
    Vector3D( PolarVector3D pvec );
    ~Vector3D() {
    }

    /**
        Gets content of this vector as array of doubles.
        @return content.
    */
    double* getFields() {
        return ( double* ) this;
    }

    /**
        Calculates distance to other vector's high-end.
        @param vec Vector to calculate distance to.
        @return distance.
    */
    double dist3D( Vector3D vec ) {
        double dx = x - vec.x;
        double dy = y - vec.y;
        double dz = z - vec.z;
        return _length( dx, dy, dz );
    }

    /**
        Calculates length of this vector (distance from high-end to low-end).
        @return length.
    */
    double dist3D() {
        return _length( x, y, z );
    }

    /**
        Returns normalized form of this vector.
        @return normalized form of this vector.
    */
    Vector3D normalize() {
        double l = _length( x, y, z );
        if ( l > 1E-12 ) {
            return Vector3D( x / l, y / l, z / l );
        }
        return ZERO;
    }

    /**
        Calculates coordinates of point situated between this and specified vector.
        @param vec Other vector.
        @param inertia Situation offset for point; does not have to be in range [0, 1]. Passing "0" will cause to return this vector, "1" - to return second vector, "0.5" - to return point straight between the two vectors etc.
        @return point.
    */
    Vector3D balance( Vector3D vec, double inertia ) {
        return ( *this ) * inertia + vec * ( 1.0 - inertia );
    }

    /**
        Calculates coordinates of point situated between this and specified vector, including also weights of x, y and z axes of vector.
        @see balance()
        @param vec Other vector.
        @param inertia Situation offset for point; does not have to be in range [0, 1]. Passing "0" will cause to return this vector, "1" - to return second vector, "0.5" - to return point straight between the two vectors etc.
        @param powerWeight Vector containing weights for axes.
        @return point.
    */
    Vector3D balance( Vector3D vec, double inertia, Vector3D powerWeight ) {
        Vector3D inertiaVector = powerWeight * ( 1.0 - inertia );
        return Vector3D(
            x * ( 1.0 - inertiaVector.x ) + vec.x * inertiaVector.x,
            y * ( 1.0 - inertiaVector.y ) + vec.y * inertiaVector.y,
            z * ( 1.0 - inertiaVector.z ) + vec.z * inertiaVector.z
        );
    }

    Vector3D multiplyElements( Vector3D vec ) {
        return Vector3D(
            x * vec.x,
            y * vec.y,
            z * vec.z
        );
    }

    Vector3D applyThrust( Vector3D thrust ) {
        return Vector3D(
            maxAmp( x, thrust.x ),
            maxAmp( y, thrust.y ),
            maxAmp( z, thrust.z )
        );
    }

    Vector3D applySlowDown( Vector3D thrust ) {
        return Vector3D(
            minAmp( x, thrust.x ),
            minAmp( y, thrust.y ),
            minAmp( z, thrust.z )
        );
    }

    Vector3D axisVector() {
        if ( dist3D() == 0.0 ) {
            return Vector3D( 0.0, 0.0, 0.0 );
        }
        double mx = max( absf( x ), max( absf( y ), absf( z ) ) );
        if ( mx == absf( x ) ) {
            return Vector3D( signum( x ), 0.0, 0.0 );
        }
        if ( mx == absf( y ) ) {
            return Vector3D( 0.0, signum( y ), 0.0 );
        }
        if ( mx == absf( z ) ) {
            return Vector3D( 0.0, 0.0, signum( z ) );
        }
        throw SaginaException( "Unknown axisVector cast" );
    }

    double x;
    double y;
    double z;

    static Vector3D ZERO;
    static Vector3D X;
    static Vector3D Y;
    static Vector3D Z;

    private:

    inline double _length( double x, double y, double z ) {
        return sqrt( x * x + y * y + z * z );
    }
};

Vector3D Vector3D::ZERO = Vector3D();
Vector3D Vector3D::X = Vector3D( 1.0, 0.0, 0.0 );
Vector3D Vector3D::Y = Vector3D( 0.0, 1.0, 0.0 );
Vector3D Vector3D::Z = Vector3D( 0.0, 0.0, 1.0 );


/**
    Vector expressed in sphere coordinates (aka polar coordinates in 2D).
*/
class PolarVector3D {
    public:

    PolarVector3D operator + ( PolarVector3D pvec ) {
        return PolarVector3D( ( r + pvec.r ) / 2.0, i + pvec.i, a + pvec.a );
    }
    PolarVector3D operator - ( PolarVector3D pvec ) {
        return PolarVector3D( ( r + pvec.r ) / 2.0, i - pvec.i, a - pvec.a );
    }

    PolarVector3D operator * ( double scale ) {
        return PolarVector3D( r * scale, i * scale, a * scale );
    }

    PolarVector3D operator / ( double scale ) {
        return PolarVector3D( r / scale, i / scale, a / scale );
    }

    PolarVector3D operator *= ( double scale ) {
        r *= scale;
        i *= scale;
        a *= scale;
        return * this;
    }

    PolarVector3D operator /= ( double scale ) {
        r /= scale;
        i /= scale;
        a /= scale;
        return * this;
    }

    /**
        Creates a new polar vector with zero angles and radius of one.
    */
    PolarVector3D() {
        r = 1.0;
        i = 0.0;
        a = 0.0;
    }

    /**
        Creates a new polar vector.
        @param r Radius of vector.
        @param i Inclination.
        @param a Azimuth.
    */
    PolarVector3D( double r, double i, double a ) {
        this -> r = r;
        this -> i = i;
        this -> a = a;
    }

    /**
        Creates a new polar vector.
        @param pvec Polar vector to clone.
    */
    PolarVector3D( const PolarVector3D& pvec ) {
        r = pvec.r;
        i = pvec.i;
        a = pvec.a;
    }

    /**
        Creates a new polar vector.
        @param vArray Array of doubles specifying ( r, i, a ) components.
    */
    PolarVector3D( double* vArray ) {
        r = vArray[ 0 ];
        i = vArray[ 1 ];
        a = vArray[ 2 ];
    }

    /**
        Creates a new polar vector.
        @param vec Vector expressed in Cartesian, to convert into sphere coordinates.
    */
    PolarVector3D( Vector3D vec );
    ~PolarVector3D() {
    }

    double* getFields() {
        return ( double* ) this;
    }

    PolarVector3D balance( PolarVector3D pvec, double inertia ) {
        return PolarVector3D( r * inertia + pvec.r * ( 1.0 - inertia ), i * inertia + pvec.i * ( 1.0 - inertia ), a * inertia + pvec.a * ( 1.0 - inertia ) );
    }

    PolarVector3D normalize() {
        return PolarVector3D( 1.0, i, a );
    }

    /**
        Calculates angular distance basing of differences.
        @param pvec Other polar vector.
        @return angular distance.
    */
    double angularDistance( PolarVector3D pvec ) {
        double di = i - pvec.i;
        double da = a - pvec.a;
        return sqrt( di * di + da * da );
    }

    double r;
    double i;
    double a;

    static PolarVector3D R_NORMAL;

    static PolarVector3D getRandom( double rmax, double imax, double amax ) {
        double rr = random( rmax );
        double ri = random( imax );
        double ra = random( amax ) - amax / 2.0;
        return PolarVector3D( rr, ri, ra );
    }

};

PolarVector3D PolarVector3D::R_NORMAL = PolarVector3D();


#define DEGREE_RADIAN_RATIO 180.0 / M_PI
#define RADIAN_DEGREE_RATIO M_PI / 180.0

Vector3D::Vector3D( PolarVector3D pvec ) {
    y = sin( pvec.a ) * pvec.r;
    double rxz = sqrt( pvec.r * pvec.r - y * y );
    x = sin( pvec.i ) * rxz;
    z = - cos( pvec.i ) * rxz;
}

PolarVector3D::PolarVector3D( Vector3D vec ) {
    r = vec.dist3D();
    i = atan2( vec.x, - vec.z );
    if ( i < 0.0 ) {
        i += 2 * M_PI;
    }
    double d = sqrt( vec.x * vec.x + vec.z * vec.z );
    a = atan2( vec.y, d );
}


double degr( double valueRadians ) {
    return valueRadians * DEGREE_RADIAN_RATIO;
}

double radn( double valueDegrees ) {
    return valueDegrees * RADIAN_DEGREE_RATIO;
}


