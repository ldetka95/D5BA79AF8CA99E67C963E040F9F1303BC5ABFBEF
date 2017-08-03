#pragma once

#include "AbstractMapLoader.h"
#include <stdarg.h>

#define BIOCORA_BLOCKSIZE 32.0

class BioCoraMapLoader : public AbstractMapLoader {
    protected:
    BioCoraMapLoader( FILE* handle, bool debug ) : AbstractMapLoader() {
        _debug = debug;
        _handle = handle;
        _map = NULL;
        _set = NULL;
        _read();
        fclose( handle );
    }
    public:
    virtual ~BioCoraMapLoader() {
        if ( _map ) {
            free( _map );
        }
        if ( _set ) {
            delete _set;
        }
        list < Entity* >::iterator it = _entList.begin();
        while ( it != _entList.end() ) {
            delete ( *it );
            it++;
        }
    }

    static const int SOLID_HEIGHT = 4;
    static const int SHOOTABLE_HEIGHT = 1;
    static const int WALKABLE_HEIGHT = 0;

    static BioCoraMapLoader* GetLoader( const char* path ) {
        FILE* handle = fopen( path, "rb" );
        if ( handle ) {
            return new BioCoraMapLoader( handle, true );
        }
        return NULL;
    }

    static BioCoraMapLoader* GetLoader( string path ) {
        return GetLoader( path.c_str() );
    }

    World* convert() {
        Map* m = Map::CreateMap( _mapX, _mapY, 16, 16 );
        Interactive* ia = new Interactive();
        if ( m ) {
            double stX = double( getStartPositionX() ) / BIOCORA_BLOCKSIZE;
            double stZ = double( getStartPositionY() ) / BIOCORA_BLOCKSIZE;
            //m -> startAt( stX, stZ );
            ia -> setPlayer( ia -> addEntity( Entity::Create( 0, Vector3D( stX, 2.0, stZ ) ) ) );
            int index = 0;
            for ( int j = 0; j < _mapY; j++ ) {
                for ( int i = 0; i < _mapX; i++ ) {
                    int field = _map[ index ];
                    int h = SOLID_HEIGHT;
                    TextureSet::Info info;
                    if ( _set -> get( field, &info ) ) {
                        if ( info.shootable ) {
                            h = SHOOTABLE_HEIGHT;
                        }
                        if ( info.walkable ) {
                            h = WALKABLE_HEIGHT;
                        }
                    }
                    for ( int k = h; k >= 0; k-- ) {
                        m -> setField( i, k, j, field );
                    }
                    index++;
                }
            }
        }
        if ( _set ) {
            m -> setTextures( _set -> clone() );
        }
        list < Entity* >::iterator it = _entList.begin();
        while ( it != _entList.end() ) {
            Entity* ent = ( *it ) -> clone();
            ia -> addEntity( ent );
            Vector3D p = *( ent -> getPosition() );
            _logInfo( "[SPAWNING] ( %lf %lf %lf )\n", p.x, p.y, p.z );
            it++;
        }
        World* w = World::CreateFromMap( m, ia );
        return w;
    }

    int getStartPositionX() {
        return _startX;
    }

    int getStartPositionY() {
        return _startY;
    }

    private:

    void _logInfo( const char* fmt, ... ) {
        if ( _debug ) {
            va_list args;
            va_start( args, fmt );
            vprintf( fmt, args );
            va_end( args );
        }
    }

    void _read() {
        _mapName = _readString( _handle );
        _logInfo( "Map name: %s\n", _mapName.c_str() );
        _mapID = _readFloat( _handle );
        _logInfo( "Map ID: %d\n", _mapID );
        _textureSet = _readString( _handle );
        _logInfo( "Texture set: %s\n", _textureSet.c_str() );
        _set = _ldTextureSet( _textureSet.c_str() );
        if ( !_set ) {
            _logInfo( "[WARN] set does not exist.\n" );
        }
        _pathAudio = _readString( _handle );
        _logInfo( "Path audio: %s\n", _pathAudio.c_str() );
        _mapX = _readFloat( _handle );
        _mapY = _readFloat( _handle );
        _logInfo( "Map size: %d / %d\n", _mapX, _mapY );
        _map = ( int* )( calloc( _mapX * _mapY, sizeof( int ) ) );
        for ( int j = 0; j < _mapY; j++ ) {
            string buffered = _readString( _handle );
            const char* buffer = buffered.c_str();
            for ( int i = 0; i < _mapX; i++ ) {
                _map[ j * _mapX + i ] = buffer[ i ];
            }
        }
        _logInfo( "Map loaded.\n" );
        _startX = _readFloat( _handle );
        _startY = _readFloat( _handle );
        _logInfo( "Player starts at: %d / %d\n", _startX, _startY );
        int spawnMax = _readFloat( _handle );
        for ( int is = 0; is < spawnMax; is++ ) { // czytanie spawnu
            int idSpawned = _readFloat( _handle );
            double spawnX = _readFloat( _handle );
            double spawnY = _readFloat( _handle );
            double fMin = _readFloat( _handle );
            double fMax = _readFloat( _handle );
            double minDist = _readFloat( _handle );
            int spawnQTotal = _readFloat( _handle );
            _logInfo( "SPAWN[ %d ] = (%d [%lf/%lf]px, %lf - %lf, %lf, %d\n", is, idSpawned, spawnX, spawnY, fMin, fMax, minDist, spawnQTotal );
            Entity* spawned = Entity::Create( idSpawned, Vector3D( spawnX  / BIOCORA_BLOCKSIZE, 3.0, spawnY / BIOCORA_BLOCKSIZE ) );
            if ( spawned ) {
                _entList.push_back( spawned );
            } else {
                _logInfo( "[WARN] no entity with ID = %d\n", idSpawned );
            }
        }
    }

    string _readString( FILE* handle ) {
        char BUFFER[ 4096 ];
        fgets( BUFFER, 4096, handle );
        int len = strlen( BUFFER );
        for ( int i = 0; i < len; i++ ) {
            if ( ( BUFFER[ i ] == 13 ) || ( BUFFER[ i ] == 10 ) ) {
                BUFFER[ i ] = 0;
                break;
            }
        }
        return string( BUFFER );
    }

    double _readFloat( FILE* handle ) {
        string s = _readString( handle );
        return atof( s.c_str() );
    }

    FILE* _handle;
    bool _debug;

    int _mapX;
    int _mapY;
    int _startX;
    int _startY;

    string _mapName;
    int _mapID;
    string _textureSet;
    string _pathAudio;

    int* _map;
    TextureSet* _set;

    list < Entity* > _entList;

    TextureSet* _ldTextureSet( const char* file ) {
        FILE* handle = fopen( file, "r" );
        if ( handle ) {
            TextureSet* texSet = new TextureSet();
            // reading
            int texCnt = _readFloat( handle );
            for ( int i = 0; i < texCnt; i++ ) {
                TextureSet::Info info;
                info.id = _readFloat( handle );
                info.path = _readString( handle );
                info.destroyID = _readFloat( handle );
                info.walkable = _readFloat( handle );
                info.shootable = _readFloat( handle );
                info.transparent = _readFloat( handle );
                info.maxHp = _readFloat( handle );
                texSet -> add( info.id, info );
            }
            // closing
            fclose( handle );
            return texSet;
        }
        return NULL;
    }

    //int setupPlay( string mapPath, double customX, double customY ) {
        // Mapa
        /*cout << "Map reset... ";
        for (int ix = 0; ix < MAP_MAXX; ix++)
            for (int iy = 0; iy < MAP_MAXY; iy++)
                {
                    Map[ix][iy] = 0;
                }
        cout << "DONE. " << endl << "Texture clearing... ";
        for (int i = 0; i < MAX_TEXTURE; i++)
            {
                SDL_FreeSurface( texture[i].data );
                texture[i].data = NULL;
            }
        cout << "DONE. " << endl << "Removing map... ";
        if ( mapSurface )
            {
                SDL_FreeSurface( mapSurface );
                mapSurface = NULL;
            } else cout << "No need to remove. ";
        cout << "DONE. " << endl << "Reset pickup and entities... ";
        for (int i = 0; i < MAX_PICKUP; i++)
            pickEnt[i].active = false;
        for (int i = 0; i < MAX_ENTITY; i++)
            gameEnt[i].exist = false;
        cout << "DONE. " << endl << "Reset map Vars level 1... ";*/
        /*int MapX = 0;
        int MapY = 0;
        int startx = 0;
        int starty = 0;
        cout << "DONE. " << endl << "MAP READ: " << endl;*/
        /*SDL_Surface* tempHandleSurface = NULL;
        if ( tryopen_load( 1, mapPath ) ) // wczytywanie mapy
            {*/
                //cout << "   Stream opened. " << endl << "   Reset map Vars level 2... ";
                //MapName = readstring(1); // nazwa
                //cmapID = readfloat(1); // id mapy
                //string textureSet = readstring(1); // zestaw tekstur
                //cout << "DONE. " << endl << "   Loading audio and map size... ";
                //path_audio = readstring(1);
                //MapX = readfloat(1); // rozmiar X w blokach
                //MapY = readfloat(1); // rozmiar Y w blokach
                //cout << "DONE. " << endl << "   Preparing early map... ";
                /*ReloadSound( 128, path_audio.c_str() );
                mapSurface = SDL_CreateRGBSurface(0, MapX * BLOCKSIZE, MapY * BLOCKSIZE, 32, 0, 0, 0, 0);
                if ( mapSurface ) cout << "DONE. "; else cout << "CANNOT PREPARE MAP! ";
                cout << endl << "   Reading textures... " << endl;
                string buffer;*/
                /// @todo czytanie tekstur
                /*if ( tryopen_load( 2, textureSet ) ) // czytanie tekstur
                    {
                        int k = readfloat(2);
                        for (int ik = 0; ik < k; ik++)
                            {
                                int id = readfloat(2);
                                cout << "    - Holding texture: " << id;
                                string texturePath = readstring(2);
                                texture[id].destroyID = readfloat(2);
                                texture[id].walkable = readfloat(2);
                                texture[id].shootable = readfloat(2);
                                texture[id].transparent = readfloat(2);
                                texture[id].maxhp = readfloat(2);
                                cout << " [ DATA READING FROM " << texturePath << " ]";
                                if ( tempHandleSurface )
                                    {
                                        SDL_FreeSurface ( tempHandleSurface );
                                        tempHandleSurface = NULL;
                                    }
                                tempHandleSurface = SDL_LoadBMP( texturePath.c_str() );
                                if ( tempHandleSurface )
                                    {
                                        texture[id].data = rotozoomSurface( tempHandleSurface, 0.0, SCALEFACTOR, 0 );
                                        if ( !texture[id].data ) cout << " - NOT CREATED!"<< endl;
                                        if ( tempHandleSurface )
                                            {
                                                SDL_FreeSurface ( tempHandleSurface );
                                                tempHandleSurface = NULL;
                                            }
                                        cout << " - DONE. "<< endl;
                                    } else cout << " - NOT CREATED! "<< endl;
                            }
                        cout << "   DONE. ";
                        tryclose(2);
                    } else cout << "   CANNOT OPEN TEXTURE SET! ";*/

                //cout << endl << "   Preparing final map... ";
                //SDL_Rect dest;
                /*for (int j = 0; j < MapY; j++) // czytanie ustawienia blokowego na mapie
                    {
                        buffer = readstring(1);
                        for (int i = 0; i < MapX; i++)
                            {
                                Map[i][j] = buffer[i];
                                if ( ( Map[i][j] < MAX_TEXTURE ) && ( Map[i][j] >= 0 ) )
                                    {
                                        if ( texture[Map[i][j]].data ) {
                                            dest.x = i * BLOCKSIZE;
                                            dest.y = j * BLOCKSIZE;
                                            SDL_BlitSurface( texture[Map[i][j]].data, NULL, mapSurface, &dest );
                                        }
                                    }
                            }
                    }*/
                //cout << "DONE. " << endl << "   Reset map Vars level 3...  ";
                //startx = readfloat(1)*SCALEFACTOR; // odczyt pozycji X gracza
                //starty = readfloat(1)*SCALEFACTOR; // odczyt pozycji Y gracza
                /*if ( ( customX != -1 ) || ( customY != -1 ) ) {spawnMax
                    startx = customX;
                    starty = customY;
                }*/
                //cout << "DONE. " << endl << "   Reset spawn...  ";
                /*for (int is = 0; is < MAX_SPAWN; is++) spawn[is].idSpawned = -1;
                cout << "DONE. " << endl << "   Spawn reading...  ";
                int spawnMax = readfloat(1);
                for (int is = 0; is < spawnMax; is++) // czytanie spawnu
                    {
                        spawn[is].idSpawned = readfloat(1);
                        spawn[is].spawnX = readfloat(1);
                        spawn[is].spawnY = readfloat(1);
                        spawn[is].frequencyMin = readfloat(1);
                        spawn[is].frequencyMax = readfloat(1);
                        spawn[is].minDistWhenSpawn = readfloat(1);
                        spawn[is].spawnQuantityTotal = readfloat(1);
                        spawn[is].frequencyCounter = 0;
                        spawn[is].spawnedNow = false;
                    }
                cout << "DONE. " << endl << "   Teleports reading...  ";
                for (int it = 0; it < MAX_TELEPORT; it++)
                    teleport[it].active = false;
                int loadedTeleports = readfloat(1);
                for (int it = 0; it < loadedTeleports; it++) // czytanie teleportow
                    {
                        teleport[it].active = true;
                        teleport[it].map_path = readstring(1);
                        teleport[it].posX = readfloat(1);
                        teleport[it].posY = readfloat(1);
                        teleport[it].radius = readfloat(1);
                        teleport[it].destinyX = readfloat(1);
                        teleport[it].destinyY = readfloat(1);
                    }
                tryclose(1);*/
                //cout << "DONE. " << endl;
            //}
        /*else
            {
                cout << "Error: Cannot open map file " << mapPath << endl;
                return 1;
            }*/
        ///cout << "Quest initiation... ";
        // Questy
        ///questMain = mission[currentMissionSelect].path_quest;
        // --przeniesione do main.cpp
        // Postacie
        //int startx = 250*SCALEFACTOR;
        //int starty = 250*SCALEFACTOR;
        /**cout << "DONE. " << endl << "Spawn Player... ";
        gameEnt[PLAYER_ID].create( &gameObj[PLAYER_ID], startx, starty, PLAYER_ID );
        playerImmortal = 0;
        cout << "DONE. " << endl << "Getting weapons... ";
        for (int i = 0; i < MAX_WEAPON; i++)
            {
                if ( ammoWeapon[int(weapon[i].weaponVar[10])].max_storage < 0 )
                    weapon[i].current_clip = weapon[i].weaponVar[3];
            }
        for (int i = 0; i < MAX_SELECTION_WEAPON; i++)
            {
                for (int j = MAX_WEAPON - 1; j >= 0; j--)
                    {
                        if ( ( broughtWeapon[j] )
                             && ( weapon[j].texture )
                             && ( weapon[j].weaponVar[11] == i )
                             && ( ( i ) || ( ammoWeapon[int(weapon[j].weaponVar[10])].storage != 0 ) ) ) {
                            selectedWeapon[i] = j;
                            break;
                        }
                    }
            }
        //int mX = 250;*/
        /*for (int i = 0; i < mX; i++)
            {
                if ( i != PLAYER_ID )
                    {
                        int xf = startx;
                        int yf = starty;
                        while ( distance ( xf, yf, startx, starty ) <= 1000*SCALEFACTOR )
                            {
                                xf = rand() % int(BLOCKSIZE*(MapX+1));
                                yf = rand() % int(BLOCKSIZE*(MapY+1));
                            }
                        gameEnt[i].create( &gameObj[1], xf, yf );
                    }
                //else mX++;
            }*/
        /*for (int i = 0; i < MAX_ENTITY; i++)
            {
                if ( ( i != PLAYER_ID ) && ( gameEnt[i].isAlive() ) )
                    {
                        gameEnt[i].publicVar[14] = gameEnt[i].publicVar[14] + ( rand() % int( gameEnt[i].publicVar[14] * gameEnt[i].publicVar[24] + 1 ) ) - gameEnt[i].publicVar[14] * gameEnt[i].publicVar[24];
                        gameEnt[i].publicVar[15] = gameEnt[i].publicVar[15] + ( rand() % int( gameEnt[i].publicVar[15] * gameEnt[i].publicVar[24] + 1 ) ) - gameEnt[i].publicVar[15] * gameEnt[i].publicVar[24];
                        if ( gameEnt[i].publicVar[14] > gameEnt[i].publicVar[15] ) gameEnt[i].publicVar[14] = gameEnt[i].publicVar[15] + 1;
                    }
            }*/
        //cout << "DONE. " << endl << "Loaded map. " << endl << endl;
        //return 0;
    //}

};


