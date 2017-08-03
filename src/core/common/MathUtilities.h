#pragma once

#include <cstdlib>

double absf( double a ) {
    return ( a > 0.0 ? a : - a );
}

double signum( double a ) {
    return ( a == 0.0 ? 0.0 : a < 0.0 ? -1.0 : 1.0 );
}

double minAmp( double a, double b ) {
    double aa = ( a > 0.0 ) ? a : - a;
    double ba = ( b > 0.0 ) ? b : - b;
    if ( aa < ba ) {
        return a;
    } else if ( ba < aa ) {
        return b;
    }
    return ( a == b ) ? a : 0.0;
}

double maxAmp( double a, double b ) {
    double aa = ( a > 0.0 ) ? a : - a;
    double ba = ( b > 0.0 ) ? b : - b;
    if ( aa > ba ) {
        return a;
    } else if ( ba > aa ) {
        return b;
    }
    return ( a == b ) ? a : 0.0;
}

#define RANDOM_PRECISION 6
#define RANDOM_GENATT 100

double random( double range = 1.0 ) {
    double ret = 0.0;
    for ( int i = 0; i < RANDOM_PRECISION; i++ ) {
        ret = ( ret + double( rand() % ( RANDOM_GENATT + 1 ) ) ) / RANDOM_GENATT;
    }
    return ret * range;
}

double random( double a, double b ) {
    return a + random( b - a );
}

double sigmaBetween( int timeA, int timeB, double p ) {
    double a = timeA;
    double b = timeB;
    return 1.0 / ( 1.0 + exp( - ( ( double( glutGet( GLUT_ELAPSED_TIME ) ) - a ) / ( b - a ) - 0.5 ) * p ) );
}


