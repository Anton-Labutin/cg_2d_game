#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Map.h"
#include <stdexcept>


enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Player
{
  explicit Player(const Point& pos) :
                 coords(pos), old_coords(coords) {};

  bool Moved(const Image& screen, const TileMap& map);
  void ProcessInput(MovementDir dir);
  void Draw(Image &screen);
  void SetPlayerImg(Image &img);
  void SetBackgroundImg(Image &img);
  Point GetPos() const;
  const Image& GetImg() const;
  void SetPos(const Point& pos);

private:
  Point coords {.x = -1, .y = -1};
  Point old_coords {.x = 0, .y = 0};
  
  Image playerImg;
  Image backgroundImg;

  int move_speed = tileSize / 8;
};

#endif //MAIN_PLAYER_H
