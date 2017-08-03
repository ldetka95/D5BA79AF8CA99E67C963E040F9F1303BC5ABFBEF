#pragma once

class SaginaSerializationException : public SaginaException {
    public:
    SaginaSerializationException( string s ) : SaginaException( s ) {
    }
    SaginaSerializationException( const char* s ) : SaginaException( s ) {
    }
    virtual ~SaginaSerializationException() {
    }
    private:
};

