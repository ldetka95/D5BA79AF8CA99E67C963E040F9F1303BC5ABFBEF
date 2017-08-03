#pragma once

class Restorable;

class RestorableBuilder {
    public:
    RestorableBuilder() {}
    virtual ~RestorableBuilder() {}
    virtual Restorable* build() = 0;
};


