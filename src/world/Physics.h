#pragma once

#include "Map.h"

class Physics {
    private:
    Physics() {}
    ~Physics() {}
    public:

    static Vector3D AffectGravity( Vector3D vec, Vector3D & vecDerv ) {
        /*static double FPS = 60.0;
        static double vMaxPerFrame = VMAX / FPS;
        static double accPerFrame = GRAVITY / ( FPS * FPS );
        double accMul = 1.0 - absf( vec.y ) / vMaxPerFrame;
        double accMulForce = accMul >= 0.0 ? pow( accMul, 0.1 ) : 0.0;
        vec.y += accPerFrame * accMulForce;// * pow( accMul, 0. );
        static int i = 0;
        printf( "%d : %lf\n", i, vec.y * 60.0 );
        i++;
        return vec;*/

        //printf( "%lf\n", vecDerv.y * 60.0 );
        vec.y += vecDerv.y;
        double grv = GRAVITY / 60.0;
        vecDerv.y = vecDerv.y * GRAVITY_INERTIA + ( grv ) * ( 1.0 - GRAVITY_INERTIA );
        return vec;
    }

    static const double GRAVITY = - 9.81 * 2.0;// / 30.0;
    static const double VMAX = 180.0; // m/s
    static const double GRAVITY_INERTIA = 0.99;
    static const double MOVEMENT_INERTIA = 0.9;

    private:
};

//const double Physics::GRAVITY = 0.1;
//const double Physics::GRAVITY_INERTIA = 0.1;
