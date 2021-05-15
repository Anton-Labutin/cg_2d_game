#include "common.h"
#include "Player.h"

#include <fstream>
#include <map>
#include <unistd.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>

using std::ifstream;
using std::ios_base;
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::cerr;

constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

void processPlayerMovement(Player &player)
{
  if (Input.keys[GLFW_KEY_W] || Input.keys[GLFW_KEY_UP])
    player.ProcessInput(MovementDir::UP);
  else if (Input.keys[GLFW_KEY_S] || Input.keys[GLFW_KEY_DOWN])
    player.ProcessInput(MovementDir::DOWN);
  if (Input.keys[GLFW_KEY_A] || Input.keys[GLFW_KEY_LEFT])
    player.ProcessInput(MovementDir::LEFT);
  else if (Input.keys[GLFW_KEY_D] || Input.keys[GLFW_KEY_RIGHT])
    player.ProcessInput(MovementDir::RIGHT);
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cerr << "Failed to initialize OpenGL context" << endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: "<< std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
    std::cout << "W, A, S, D - movement  "<< std::endl;
    std::cout << "press ESC to exit" << std::endl;

	return 0;
}


void 
drawTile(Image& screen, const Point& pos, const Image& tile)
{
    Pixel tilePixel;

    for (int y = pos.y; y < pos.y + tile.Height() && y < screen.Height(); ++y) {
        for (int x = pos.x; x < pos.x + tile.Width() && x < screen.Width(); ++x) {
            tilePixel = tile.GetPixel(x - pos.x, y - pos.y);
            screen.PutPixel(x, y, tilePixel);
        }
    }
}


Point
drawMap(Image& screen, const TileMap& gameMap, const map<GameChar, Image>& tiles) 
{
    Point playerPos = {-1, -1};
    GameChar drawnTile;
    
    for (int yTileIdx = 0; yTileIdx < gameMap.TilesPerY(); ++yTileIdx) {
        for (int xTileIdx = 0; xTileIdx < gameMap.TilesPerX(); ++xTileIdx) {
            drawnTile = gameMap.GetTile(xTileIdx, yTileIdx);

            if (tiles.count(drawnTile)) {
                if (drawnTile == GameChar::PLAYER) {
                    playerPos.x = xTileIdx * tileSize;
                    playerPos.y = yTileIdx * tileSize;
                }
            
                drawTile(screen, {xTileIdx * tileSize, yTileIdx * tileSize}, tiles.at(drawnTile));
            } else {
                throw std::runtime_error("Undefined tile on the map");
            }
        }
    }

    return playerPos;
}


bool 
GotOnTile(const Image& screen, const TileMap& map, const Player& player, const GameChar& tile) 
{
    Point playerPos = player.GetPos();

    if (map.DefineTile(screen, {playerPos.x + player.GetImg().Width() / 8, playerPos.y + player.GetImg().Height() / 8}) == tile || 
        map.DefineTile(screen, {playerPos.x + 7 * player.GetImg().Width() / 8, playerPos.y + player.GetImg().Height() / 8}) == tile ||
        map.DefineTile(screen, {playerPos.x + player.GetImg().Width() / 8, playerPos.y + 7 * player.GetImg().Height() / 8}) == tile || 
        map.DefineTile(screen, {playerPos.x + 7 * player.GetImg().Width() / 8, playerPos.y + 7 * player.GetImg().Height() / 8}) == tile
    ) {
        return true;
    } else {
        return false;
    }
}


void 
createLevels(const vector<const char*>& levelFilePaths, vector<TileMap>& levels)
{
    for (const auto& filePath: levelFilePaths) {
        ifstream level(filePath, ios_base::in);
        
        if (! level.is_open()) {
            throw std::runtime_error("Failed to open the input file");
        }

        levels.push_back(TileMap(WINDOW_WIDTH / tileSize, WINDOW_HEIGHT / tileSize));
        
        if (! (levels.rbegin() -> Create(level))) {
            throw std::runtime_error("No player on the map");
        }

        level.close();
    }
}


int main(int argc, char** argv)
{
	if(!glfwInit())
        return -1;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  

	if(initGL() != 0) {
        return -1;
    }
    
    //Reset any OpenGL errors which coiuld be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

    vector<TileMap> levels;
    vector<const char*> levelFilePaths;
    levelFilePaths.push_back("./resources/level1.txt");
    levelFilePaths.push_back("./resources/level2.txt");
   
    createLevels(levelFilePaths, levels);
    size_t currLevelIdx = 0;    
	
    Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    
    Image imgFloor("./resources/floor.png");
    Image imgWall("./resources/wall.png");
    Image imgPlayer("./resources/hero.png");
    Image imgLevelExit("./resources/new_level.png");
    Image imgTrap("./resources/trap.png");
    Image imgGameOver("./resources/game_over.png");
    Image imgHole("./resources/hole.png");
    Image imgMovingToNewLevel("./resources/moving_to_new_level.png");
    Image imgEnd("./resources/end.png");

    map<GameChar, Image> tiles;
    tiles.emplace(GameChar::FLOOR, imgFloor);
    tiles.emplace(GameChar::WALL, imgWall);
    tiles.emplace(GameChar::PLAYER, imgPlayer);
    tiles.emplace(GameChar::LEVEL_EXIT, imgLevelExit);
    tiles.emplace(GameChar::TRAP, imgTrap);
    tiles.emplace(GameChar::HOLE, imgHole);


    Player player { drawMap(screenBuffer, levels[currLevelIdx], tiles) };
    player.SetPlayerImg(tiles[GameChar::PLAYER]);
    player.SetBackgroundImg(tiles[GameChar::FLOOR]);
   
    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
    
    //game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        glfwPollEvents(); // the number of seconds since the library was initialized with glfwInit. 

        processPlayerMovement(player);
        if (player.Moved(screenBuffer, levels[currLevelIdx])) {
            player.Draw(screenBuffer);
            
            if (GotOnTile(screenBuffer, levels[currLevelIdx], player, GameChar::TRAP) || 
                GotOnTile(screenBuffer, levels[currLevelIdx], player, GameChar::HOLE)) {
                drawTile(screenBuffer, {(WINDOW_WIDTH - imgGameOver.Width()) / 2, (WINDOW_HEIGHT - imgGameOver.Height()) / 2}, imgGameOver);
			    glfwSetWindowShouldClose(window, GL_TRUE);
             } else {
                if (GotOnTile(screenBuffer, levels[currLevelIdx], player, GameChar::LEVEL_EXIT)) { 
                    if (currLevelIdx + 1 != levels.size()) {
                        drawTile(screenBuffer, {(WINDOW_WIDTH - imgMovingToNewLevel.Width()) / 2, (WINDOW_HEIGHT - imgMovingToNewLevel.Height()) / 2}, imgMovingToNewLevel);
		                
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
                        glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
		                glfwSwapBuffers(window);
                        
                        sleep(3);

                        player.SetPos( drawMap(screenBuffer, levels[++currLevelIdx], tiles) );
                    } else {
                        drawTile(screenBuffer, {(WINDOW_WIDTH - imgEnd.Width()) / 2, (WINDOW_HEIGHT - imgMovingToNewLevel.Height()) / 2}, imgEnd);
			            glfwSetWindowShouldClose(window, GL_TRUE);
                    }
                }
             }
        }

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
        glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
		glfwSwapBuffers(window);
	}

    sleep(3); 
	glfwTerminate();
	return 0;
}
