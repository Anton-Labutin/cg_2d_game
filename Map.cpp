#include "Map.h"
#include <iostream>

using std::cerr;
using std::endl;


bool
TileMap::Create(ifstream& finput)
{
    int xIdx = 0;
    int yIdx = TilesPerY() - 1;
    bool isPlayerFound = false;
    int ch;

    while ((ch = finput.get()) != EOF) {
        if (ch != '\n') {
            if (static_cast<GameChar>(ch) == GameChar::PLAYER) {
                isPlayerFound = true;
            }

            map[yIdx][xIdx++] = static_cast<GameChar>(ch);
        } else {
            if (xIdx != TilesPerX()) {
                cerr <<"Incorrect count of characters in the input row " << TilesPerY() - yIdx << ": requiered " << TilesPerX() << ", got " << xIdx << endl; 
                throw std::runtime_error("");
            }

            --yIdx;
            xIdx = 0;
        }
    }
    
    if (yIdx + 1) {
        cerr <<"Incorrect count of rows in the input: requiered " << TilesPerY() << ", got " << TilesPerY() + yIdx + 1 << endl; 
        throw std::runtime_error("");
    }

    return isPlayerFound;
}


GameChar 
TileMap::DefineTile(const Image& screen, const Point& p) const 
{
    if (p.x > screen.Width() || p.x < 0 || p.y > screen.Height() || p.y < 0) {
        throw std::out_of_range("Point is out of the screen");
    }

    return map[p.y < screen.Height() ? p.y / tileSize : TilesPerY() - 1][p.x < screen.Width() ? p.x / tileSize : TilesPerX() - 1];
}


GameChar 
TileMap::GetTile(size_t mapPosX, size_t mapPosY) const 
{
    if (mapPosX >= TilesPerX() || mapPosY >= TilesPerY()) {
        throw std::out_of_range("Tile is out of map");
    }

    return map[mapPosY][mapPosX];
}


int
TileMap::TilesPerX() const 
{
    return static_cast<int>(map[0].size());
}


int
TileMap::TilesPerY() const 
{
    return static_cast<int>(map.size());
}


int
TileMap::TilesCnt() const 
{
    return TilesPerX() * TilesPerY();
}
