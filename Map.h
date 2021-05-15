#include <vector>
#include <fstream>
#include "Image.h"

using std::vector;
using std::ifstream;


enum class GameChar {
    HOLE = ' ', 
    WALL = '#', 
    FLOOR = '.', 
    PLAYER = '@', 
    LEVEL_EXIT = 'x', 
    TRAP = 'T', 
};


class TileMap {
public:
    
    TileMap(size_t mapSizeX, size_t mapSizeY) 
    {
        map = vector<vector<GameChar>> (mapSizeY, vector<GameChar>(mapSizeX, GameChar::FLOOR));
    }

    bool 
    Create(ifstream& finput);

    GameChar 
    DefineTile(const Image& screen, const Point& p) const;

    GameChar
    GetTile(size_t mapPosX, size_t mapPosY) const;

    int TilesPerX() const;
    int TilesPerY() const;
    int TilesCnt() const;

private:

    vector<vector<GameChar>> map;
};
