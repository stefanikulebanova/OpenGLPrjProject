#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <game_object.h>
// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_OVER
};
// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 100.0f);
const glm::vec2 COIN_SIZE(30.0f, 30.0f);
const glm::vec2 BOX_SIZE(50.0f, 50.0f);

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    unsigned int            Width, Height;
    unsigned int            NumOfCoins;
    std::vector<GameObject> Coins;
    std::vector<GameObject> Boxes;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    bool CheckCollision(GameObject& one, GameObject& two);
};

#endif