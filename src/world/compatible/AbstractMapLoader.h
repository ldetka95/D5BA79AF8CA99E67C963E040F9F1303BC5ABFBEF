#pragma once

class AbstractMapLoader {
    protected:
    AbstractMapLoader() {
    }
    virtual ~AbstractMapLoader() {
    }

    public:

    virtual World* convert() = 0;

    private:
};


