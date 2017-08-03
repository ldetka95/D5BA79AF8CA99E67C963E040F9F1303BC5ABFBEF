#pragma once

class EntityInterface {
    public:
    EntityInterface() {
    }
    virtual ~EntityInterface() {
    }

    virtual Vector3D getHotSpot() = 0;
    virtual double getVar( int index ) = 0;
    virtual void setVar( int index, double value ) = 0;
    virtual PolarVector3D* getAngles() = 0;

};


