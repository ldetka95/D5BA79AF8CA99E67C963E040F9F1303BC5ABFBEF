#include "src/core/restorable/Restorable.h"
#include "src/core/common/StringLinker.h"

#include "src/display/glew/glew.c"

#include <gl/gl.h>
#include <glut/glut.h>

#include "src/world/World.h"

#include "src/perspective/Level.h"
#include "src/perspective/Menu.h"

#include "src/display/texture/Texture.h"

#define MAP_ROOTPATH "data/map/"

#include <time.h>

FrameUnit* window = NULL;

static int RESOLUTION_SETTING_INIT = 0;//42;

#define TIME_QUANT ( 1000 / 60 )

void onDisplay() {
    /*static int NEXT_TIME = glutGet( GLUT_ELAPSED_TIME ) + TIME_QUANT;
    while ( NEXT_TIME >= glutGet( GLUT_ELAPSED_TIME ) );
    NEXT_TIME = glutGet( GLUT_ELAPSED_TIME ) + TIME_QUANT;*/
    if ( !window ) {
        //FrameUnit::setResolution( 1, 1 );
        //FrameUnit::setResolution( 2560, 1440 );
        window = FrameUnit::CreateUnitFromSettings( RESOLUTION_SETTING_INIT );//FrameUnit::getDefault(); // glutGet( GLUT_WINDOW_WIDTH ), glutGet( GLUT_WINDOW_HEIGHT )
        /**
            Resolutions:
            0: ( default )
            1: 320x180
            2: 320x240
            3: 480x270
            4: 480x320
            5: 640x360
            6: 640x480
            7: 800x450
            8: 800x480
            9: 800x600
            10: 1024x576
            10: 1024x600
            11: 1024x768
            12: 1280x720
            13: 1280x800
            14: 1280x1024
            15: 1366x768
            16: 1400x1050
            17: 1440x900
            19: 1600x900
            20: 1600x1024
            21: 1600x1200
            22: 1920x1080
            23: 1920x1200
            24: 2048x1024
            24: 2048x1152
            25: 2048x1536
            26: 2560x1080
            27: 2560x1440
            28: 2560x1600
            29: 2560x2048
            30: 3072x1536
            31: 3072x1728
            32: 4096x2048
            33: 4096x2304
            34: 8192x4096
            35: 8192x4608
            36: 16384x8192
            37: 16384x9216
        */
    }
    window -> openFrame();
        glClearColor( 0.0, 0.0, 0.0, 1.0 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        Perspective* current = Perspective::Current();
        try {
            if ( current ) {
                current -> tick( 60 );//onTick();
            }
        } catch ( SaginaException se ) {
            se.log();
            exit( 0 );
        }
    window -> closeFrame();
    window -> renderFrame();
    Keys::invalidStruckStates();
    glutSwapBuffers();
    glutPostRedisplay();
}

void onReshape( int x, int y ) {
    glViewport( 0, 0, x, y );
    onDisplay();
}

void onSpecialKey( int k, int x, int y ) {
    if ( k == GLUT_KEY_F12 ) {
        char buffer[ 4096 ];
        sprintf( buffer, "data/screenshot/SCREENSHOT_%ld_%d.bmp", time( NULL ), glutGet( GLUT_ELAPSED_TIME ) );
        window -> getScreen() -> refreshPixelsFromScreen();
        window -> getScreen() -> saveRGB( buffer );
    }
}

void onKeyStruck( unsigned char k, int x, int y ) {
    /*if ( k == 27 ) {
        exit( 0 );
    }*/
    Keys::press( k );
}

void onKeyRelease( unsigned char k, int x, int y ) {
    Keys::release( k );
}

void onMouseMotion( int x, int y ) {
    Keys::setMouse( x, y );
}

void onMouseClick( int button, int state, int x, int y ) {
    Keys::controlMouse( button, state );
}

const char* SETTINGS_PATH_RESOLUTION_CONFIG = "data/config/resolution.dat";
const char* SETTINGS_PATH_LINEAR_PARAMETER_CONFIG = "data/config/linear.dat";
const char* SETTINGS_PATH_DEFAULT = "data/config/Settings.dat";

void setupView() {
    glutIgnoreKeyRepeat( 1 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    Settings::LoadSettings( SETTINGS_PATH_RESOLUTION_CONFIG, SETTINGS_PATH_LINEAR_PARAMETER_CONFIG, SETTINGS_PATH_DEFAULT );
    Settings::LoadBinarySettings( SETTINGS_PATH_DEFAULT );

    Camera::Setup();

    //Camera::Link( &POS, &ANGLES );
    Container::Register();
    Level::Register();

    Entity::Init( "data/entity/Entity.slf" );
    BlockModel::Init( "data/model/Model.slf" );
    TextureHolder::InitGlobalHolder( "data/textures/map.slf" );

    BioCoraMapLoader* loader = BioCoraMapLoader::GetLoader( "data/oldMap/hospital1.txt" );
    if ( loader ) {
        World* w = loader -> convert();
        //FileSerializationOutputStream* stream = FileSerializationOutputStream::create( "data/map/Level.dat" );
        //Restorable::Serialize( m, stream );
        //delete m;
        w -> saveWorld( "data/map/Level.dat" );
        delete w;
        delete loader;
    }

    string pth = MAP_ROOTPATH;
    pth += "map.slf";
    Level* level = new Level( pth.c_str() );
    if ( !level -> changeLevel( MAP_ROOTPATH, 1 ) ) {
        printf( "WARN: can't change level to id 1\n" );
    }

    Weapon::Init( "data/weapon/Weapon.slf", "data/weapon/Ammo.slf" );

    level -> debugInitWeapon();

    Menu* menu = new Menu( 1, "data/gui/GUI.slf" );

    Perspective::Setup( level );
    Perspective::Setup( menu );
    //Perspective::ChangeTo( PERSPECTIVE_LEVEL_ID );
    Perspective::ChangeTo( PERSPECTIVE_MENU_ID );
}

static void InitSDL() {
    SDL_Init( 0 );
    Text::Init( "data/font/Font.ttf", 20 );
}

int main( int argc, char** argv ) {

    glutInit( &argc, argv );

    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    InitSDL();
    srand( time( NULL ) );

    glutInitWindowSize( 800, 600 );
    glutCreateWindow( "MapView" );

    glutReshapeFunc( onReshape );
    glutDisplayFunc( onDisplay );
    glutKeyboardFunc( onKeyStruck );
    glutKeyboardUpFunc( onKeyRelease );
    glutMotionFunc( onMouseMotion );
    glutPassiveMotionFunc( onMouseMotion );
    glutMouseFunc( onMouseClick );
    glutSpecialFunc( onSpecialKey );

    glewInit();

    setupView();

    glutFullScreen();

    glutMainLoop();

    return 0;
}

