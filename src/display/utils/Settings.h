#pragma once

class Settings {
    private:
    class Resolution {
        public:
        Resolution() {
        }
        Resolution( int width, int height ) {
            w = width;
            h = height;
        }
        short w;
        short h;
    };
    class LinearParameter {
        public:
        LinearParameter() {
        }
        LinearParameter( int pid, double mn, double mx, double init, double jump ) {
            id = pid;
            minValue = mn;
            maxValue = mx;
            currentValue = init;
            valueJump = jump;
        }
        void write( FILE* handle ) {
            fwrite( &id, 1, sizeof( int ), handle );
            fwrite( &minValue, 1, sizeof( double ), handle );
            fwrite( &maxValue, 1, sizeof( double ), handle );
            fwrite( &currentValue, 1, sizeof( double ), handle );
            fwrite( &valueJump, 1, sizeof( double ), handle );
        }
        bool read( FILE* handle ) {
            bool ok = true;
            ok &= ( fread( &id, 1, sizeof( int ), handle ) == sizeof( int ) );
            ok &= ( fread( &minValue, 1, sizeof( double ), handle ) == sizeof( double ) );
            ok &= ( fread( &maxValue, 1, sizeof( double ), handle ) == sizeof( double ) );
            ok &= ( fread( &currentValue, 1, sizeof( double ), handle ) == sizeof( double ) );
            ok &= ( fread( &valueJump, 1, sizeof( double ), handle ) == sizeof( double ) );
            return ok;
        }
        int id;
        double minValue;
        double maxValue;
        double currentValue;
        double valueJump;
    };

    Settings() {
    }
    ~Settings() {
    }
    public:

    static const int SETTING_ID_FOV = 1;
    static const int SETTING_ID_VSPEED = 2;
    static const int SETTING_ID_VINVERT = 3;
    static const int SETTING_ID_HSPEED = 4;
    static const int SETTING_ID_HINVERT = 5;
    static const int SETTING_ID_ASPECT = 6;

    static void LoadSettings( const char* resolutions_path, const char* linear_path, const char* outputPath ) {
        Settings* s = new Settings();
        /// resolutions
        FILE* handle = fopen( resolutions_path, "r" );
        if ( handle ) {
            int w;
            int h;
            while ( fscanf( handle, "%d %d", &w, &h ) == 2 ) {
                Resolution res = Resolution( w, h );
                s -> ___reslist.push_back( res );
            }
            fclose( handle );
        }
        /// linears
        handle = fopen( linear_path, "r" );
        if ( handle ) {
            int id;
            double mn;
            double mx;
            double cur;
            double jmp;
            while ( fscanf( handle, "%d %lf %lf %lf %lf", &id, &mn, &mx, &cur, &jmp ) == 5 ) {
                LinearParameter linear = LinearParameter( id, mn, mx, cur, jmp );
                s -> ___linearList.push_back( linear );
            }
            fclose( handle );
        }
        /// saving all
        handle = fopen( outputPath, "wb" );
        if ( handle ) {
            int n = s -> ___reslist.size();
            fwrite( &n, 1, sizeof( int ), handle );
            for ( int i = 0; i < n; i++ ) {
                Resolution res = s -> ___reslist[ i ];
                fwrite( &( res.w ), 1, sizeof( short ), handle );
                fwrite( &( res.h ), 1, sizeof( short ), handle );
            }
            int m = s -> ___linearList.size();
            fwrite( &m, 1, sizeof( int ), handle );
            for ( int i = 0; i < m; i++ ) {
                LinearParameter linear = s -> ___linearList[ i ];
                linear.write( handle );
            }
            fclose( handle );
        }
        /// exchange
        Exchange( s );
    }

    static void LoadBinarySettings( const char* path ) {
        Settings* s = new Settings();
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            int n;
            Resolution res;
            fread( &n, 1, sizeof( int ), handle );
            for ( int i = 0; i < n; i++ ) {
                fread( &( res.w ), 1, sizeof( short ), handle );
                fread( &( res.h ), 1, sizeof( short ), handle );
                s -> ___reslist.push_back( res );
            }
            fread( &n, 1, sizeof( int ), handle );
            for ( int i = 0; i < n; i++ ) {
                LinearParameter linear;
                linear.read( handle );
                s -> ___linearList.push_back( linear );
            }
            fclose( handle );
        }
        /// exchange
        Exchange( s );
    }

    static Settings* getSettings() {
        return ___CURRENT_SETTINGS;
    }

    static void Exchange( Settings* settings ) {
        if ( ___CURRENT_SETTINGS ) {
            delete ___CURRENT_SETTINGS;
        }
        ___CURRENT_SETTINGS = settings;
    }

    int resolutionCount() {
        return ___reslist.size();
    }

    bool getResolution( int i, int* w, int* h ) {
        if ( ( i < 0 ) || ( i >= resolutionCount() ) ) {
            return false;
        }
        Resolution res = ___reslist[ i ];
        if ( w ) {
            ( *w ) = res.w;
        }
        if ( h ) {
            ( *h ) = res.h;
        }
        return true;
    }

    double getSettingValue( int id ) {
        LinearParameter* linear = ___findLinear( id );
        if ( linear ) {
            return linear -> currentValue;
        }
        return NAN;
    }

    double getSettingMinValue( int id ) {
        LinearParameter* linear = ___findLinear( id );
        if ( linear ) {
            return linear -> minValue;
        }
        return NAN;
    }

    double getSettingMaxValue( int id ) {
        LinearParameter* linear = ___findLinear( id );
        if ( linear ) {
            return linear -> maxValue;
        }
        return NAN;
    }

    void modifySettingScale( int id, double scale ) {
        if ( ( scale < 0.0 ) || ( scale > 1.0 ) ) {
            return;
        }
        LinearParameter* linear = ___findLinear( id );
        if ( linear ) {
            volatile int scaleJump = ( linear -> maxValue - linear -> minValue ) * scale / linear -> valueJump;
            linear -> currentValue = linear -> minValue + double( scaleJump ) * linear -> valueJump;
        }
    }

    void modifySetting( int id, int jumpI ) {
        LinearParameter* linear = ___findLinear( id );
        if ( linear ) {
            volatile int scaleJump = ( linear -> currentValue - linear -> minValue ) / linear -> valueJump;
            scaleJump += jumpI;
            linear -> currentValue = min( linear -> maxValue, max( linear -> minValue, linear -> minValue + double( scaleJump ) * linear -> valueJump ) );
        }
    }

    private:

    LinearParameter* ___findLinear( int id ) {
        for ( int i = 0; i < ( int ) ___linearList.size(); i++ ) {
            LinearParameter linear = ___linearList[ i ];
            if ( linear.id == id ) {
                return &___linearList[ i ];
            }
        }
        return NULL;
    }

    vector < Resolution > ___reslist;

    vector < LinearParameter > ___linearList;

    static Settings* ___CURRENT_SETTINGS;

};

Settings* Settings::___CURRENT_SETTINGS = NULL;

