#pragma once

class Timer {
    protected:

    Timer( int t ) {
        _time = t;
    }

    public:

    Timer() {
        //setTime();
    }

    virtual ~Timer() {
    }

    int getTime() {
        return _time;
    }

    void setTime() {
        _time = getCurrentTime();
    }

    void addTime( int ms ) {
        _time += ms;
    }

    bool hasPassed( int ms ) {
        return ( _time + ms ) <= getCurrentTime();
    }

    int lacking( int ms ) {
        int lack = ( _time + ms ) - getCurrentTime();
        return lack < 0 ? 0 : lack;
    }

    virtual Timer* clone() = 0;

    virtual int getCurrentTime() = 0;

    private:

    int _time;

};

class GlutTimer : public Timer {
    public:
    GlutTimer() : Timer() {
    }
    GlutTimer( int t ) : Timer( t ) {
    }
    ~GlutTimer() {
    }

    Timer* clone() {
        return new GlutTimer( getTime() );
    }

    int getCurrentTime() {
        return glutGet( GLUT_ELAPSED_TIME );
    }
};

