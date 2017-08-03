#pragma once

class AbstractRestorableOperation {
    public:
    AbstractRestorableOperation() {}
    virtual ~AbstractRestorableOperation() {}
    virtual void perform( Restorable* restorable ) = 0;
};


