#include "Player.h"

#include <iostream>

void 
drawTile(Image& screen, const Point& pos, const Image& tile);


bool 
Player::Moved(const Image& screen, const TileMap& map)
{
    if ((coords.x - old_coords.x > 0) && 
        (coords.x + tileSize <= screen.Width()) && 
        (map.DefineTile(screen, {old_coords.x + tileSize, coords.y + 2}) != GameChar::WALL) && 
        (map.DefineTile(screen, {old_coords.x + tileSize, coords.y + tileSize - 3}) != GameChar::WALL)
    ) {
        // can move right
        return true;
    }

    if ((coords.x - old_coords.x < 0) && 
        (coords.x >= 0) && 
        (map.DefineTile(screen, {coords.x, coords.y + 2}) != GameChar::WALL) && 
        (map.DefineTile(screen, {coords.x, coords.y + tileSize - 3}) != GameChar::WALL) 
    ) {
        // can move left 
        return true;
    } 

    if ((coords.y - old_coords.y > 0) && 
        (coords.y + tileSize <= screen.Height()) && 
        (map.DefineTile(screen, {coords.x + 2, old_coords.y + tileSize}) != GameChar::WALL) && 
        (map.DefineTile(screen, {coords.x + tileSize - 3, old_coords.y + tileSize}) != GameChar::WALL)
    ) {  
        // can move up
        return true;
    }
        
    if ((coords.y - old_coords.y < 0) && 
        (coords.y >= 0) && 
        (map.DefineTile(screen, {coords.x + 2, coords.y}) != GameChar::WALL) && 
        (map.DefineTile(screen, {coords.x + tileSize - 3, coords.y}) != GameChar::WALL) 
    ) {
        // can move down
        return true;
    }

    coords = old_coords;
    return false;
}


void Player::ProcessInput(MovementDir dir)
{
  int move_dist = move_speed;
  switch(dir)
  {
    case MovementDir::UP:
      old_coords.y = coords.y;
      coords.y += move_dist;
      break;
    case MovementDir::DOWN:
      old_coords.y = coords.y;
      coords.y -= move_dist;
      break;
    case MovementDir::LEFT:
      old_coords.x = coords.x;
      coords.x -= move_dist;
      break;
    case MovementDir::RIGHT:
      old_coords.x = coords.x;
      coords.x += move_dist;
      break;
    default:
      break;
  }
}


void Player::SetPlayerImg(Image& img) 
{
    playerImg = img;
}


void Player::SetBackgroundImg(Image& img) 
{
    backgroundImg = img;
}


Point Player::GetPos() const 
{
    return coords;
}


const Image& Player::GetImg() const 
{
    return playerImg;
}


void Player::SetPos(const Point& pos) 
{
    old_coords = coords = pos;
}


void Player::Draw(Image &screen)
{
    static Point bgrImgPos = {0, 0};

    if (playerImg.Data() == nullptr) {
        throw std::runtime_error("Player image is undefined");
    }

    if (backgroundImg.Data() == nullptr) {
        throw std::runtime_error("Background image for player in undefined");
    }
        
    if (coords.x != old_coords.x) {
        if (coords.x > old_coords.x) {
            for (int y = old_coords.y; y < old_coords.y + tileSize; ++y) {
                for (int x = old_coords.x; x < coords.x; ++x) {
                    screen.PutPixel(x, y, backgroundImg.GetPixel((x - old_coords.x) + bgrImgPos.x, y - old_coords.y));
                }
            }

            bgrImgPos.x += coords.x - old_coords.x;
        } else {
            for (int y = old_coords.y; y < old_coords.y + tileSize; ++y) {
                for (int x = old_coords.x + tileSize - 1; x >= coords.x + tileSize; --x) {
                    screen.PutPixel(x, y, backgroundImg.GetPixel((x - old_coords.x) - bgrImgPos.x, y - old_coords.y));
                }

                bgrImgPos.x += old_coords.x - coords.x;
            }
        }

        bgrImgPos.x %= tileSize;
    } else {
        if (coords.y > old_coords.y) {
            for (int y = old_coords.y; y < coords.y; ++y) {
                for (int x = old_coords.x; x < old_coords.x + tileSize; ++x) {
                    screen.PutPixel(x, y, backgroundImg.GetPixel(x - old_coords.x, (y - old_coords.y) + bgrImgPos.y));
                }
            }

            bgrImgPos.y += coords.y - old_coords.y;
        } else {
            for (int y = old_coords.y + tileSize - 1; y >= coords.y + tileSize; --y) {
                for (int x = old_coords.x; x < old_coords.x + tileSize; ++x) {
                    screen.PutPixel(x, y, backgroundImg.GetPixel(x - old_coords.x, (y - old_coords.y) - bgrImgPos.y));
                }
            }

            bgrImgPos.y += old_coords.y - coords.y;
        }

        bgrImgPos.y %= tileSize;
    }

    drawTile(screen, coords, playerImg);
    old_coords = coords;
}
