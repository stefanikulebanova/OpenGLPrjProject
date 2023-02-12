#include "game.h"
#include "resource.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include <iostream>
#include <random>
using namespace std;
#include <text_renderer.h>
#include <sstream>
#include <irrKlang/irrKlang.h>
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
SpriteRenderer* Renderer;
GameObject* Player;
GameObject* Coin;
GameObject* Box;
TextRenderer* Text;
float COIN_VELOCITY(100.0f);
float PLAYER_VELOCITY(500.0f);

int random(int low, int high)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(low, high);
    return dist(gen);
}
float floatRandom(float low, float high)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(low, high);
    return dist(gen);
}

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height), NumOfCoins(0)
{

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Box;
    delete Coin;
    delete Text;
}

void Game::Init()
{
    //load shaders
    ResourceManager::LoadShader("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/shaders/sprite.vs", "C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // load textures
    ResourceManager::LoadTexture("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/textures/blinky.png", true, "blinky");
    ResourceManager::LoadTexture("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/textures/school-hallway.jpeg", false, "background");
    ResourceManager::LoadTexture("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/textures/coins.png", true, "coin");
    ResourceManager::LoadTexture("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/textures/project.png", true, "box");
    ResourceManager::LoadTexture("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/textures/Rectangle2.png", true, "rectangle");
    // set render-specific controls
    Renderer = new SpriteRenderer(myShader);
    Text = new TextRenderer(this->Width, this->Height);
    // configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("blinky"));
    glm::vec2 coinPos;
    //generate random number for x position
    float xRand = floatRandom(1.5, 2.5);
    coinPos = glm::vec2(this->Width / xRand - COIN_SIZE.x / xRand, this->Height - COIN_SIZE.y * 12.0f);
    Coin = new GameObject(coinPos, COIN_SIZE, ResourceManager::GetTexture("coin"));
    this->Coins.push_back(*Coin);

    glm::vec2 boxPos;
    //generate random number for x position
    xRand = floatRandom(1.5, 2.5);
    boxPos = glm::vec2(this->Width / xRand - BOX_SIZE.x / xRand, 240.0f);
    Box = new GameObject(boxPos, BOX_SIZE, ResourceManager::GetTexture("box"));
    this->Boxes.push_back(*Box);
    if (!SoundEngine)
        std::cout << "error sound engine" << std::endl;
    // audio
    SoundEngine->play2D("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/audio/background.mp3", true);
}
int frames = 0;
int box = 7000;

void Game::Update(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        this->DoCollisions();
        //put new coin
        if (frames % 6000 == 0) {
            glm::vec2 pos;
            float xRand = floatRandom(1.5, 2.5);
            pos = glm::vec2(this->Width / xRand - COIN_SIZE.x / xRand, this->Height - COIN_SIZE.y * 12.0f);
            Coin = new GameObject(pos, COIN_SIZE, ResourceManager::GetTexture("coin"));
            this->Coins.push_back(*Coin);
        }
        //put new box
        if (frames % box == 0) {
            glm::vec2 boxPos;
            //generate random number for x position
            float xRand = floatRandom(1.5, 2.5);
            boxPos = glm::vec2(this->Width / xRand - BOX_SIZE.x / xRand, 240.0f);
            Box = new GameObject(boxPos, BOX_SIZE, ResourceManager::GetTexture("box"));
            this->Boxes.push_back(*Box);
        }
        //elevate velocity through time
        if (frames % 30000 == 0 && COIN_VELOCITY<300) {
            COIN_VELOCITY += 20;
            PLAYER_VELOCITY += 20;
            if(box >= 1000)
            box -= 200;
        }
        // update objects
        for (GameObject& coin : this->Coins) {
            coin.Update(COIN_VELOCITY * dt);
        }
        for (GameObject& box : this->Boxes) {
            box.Update(COIN_VELOCITY * dt);
        }      
    }
    frames++;  
}

float jump_velocity = 0;

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float playerVelocity = PLAYER_VELOCITY * dt;
        float coinVelocity = COIN_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_LEFT])
        {
            if (Player->Position.x >= 0.0f)
                Player->Position.x -= playerVelocity;
        }
        if (this->Keys[GLFW_KEY_RIGHT])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += playerVelocity;
        }
        for (GameObject& coin : this->Coins) {
            if (coin.Position.y >= this->Height) {
                this->Coins.erase(this->Coins.begin());
            }
        }
        for (GameObject& box : this->Boxes) {
            if (box.Position.y >= this->Height) {
                this->Boxes.erase(this->Boxes.begin());
            }
        }
    }
    if (this->State != GAME_ACTIVE) {

        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->NumOfCoins = 0;
            this->Coins.clear();
            this->Boxes.clear();
            COIN_VELOCITY = 100.0f;
            PLAYER_VELOCITY = 500.0f;
        }
    }
}

void Game::Render()
{
    // draw background
    Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    // draw player
    Player->Draw(*Renderer);

    if (this->State == GAME_ACTIVE)
    {
        //draw coins
        for (GameObject& coin : this->Coins) {
            if (!coin.Destroyed) {
                 coin.Draw(*Renderer);
               }
        }
        //draw boxes
        for (GameObject& box : this->Boxes) {
            if (!box.Destroyed) {
                box.Draw(*Renderer);
            }
        }
        std::stringstream ss; ss << this->NumOfCoins;
        Text->RenderText("Coins: " + ss.str(), 5.0f, 5.0f, 1.0f);;
    }
    if (this->State == GAME_MENU)
    {
        //draw rectangle
        Renderer->DrawSprite(ResourceManager::GetTexture("rectangle"), glm::vec2(this->Width / 2 - 200.0f, this->Height / 2 - 100.0f), glm::vec2(400.0f, 200.0f), 0.0f);
        //render text
        Text->Load("C:/Windows/Fonts/JOKERMAN.TTF", 30);
        Text->RenderText("Press ENTER to start", 250.0f, Height / 2 - 10.0f, 1.0f);
    }
    if (this->State == GAME_OVER) {
        //draw rectangle
        Renderer->DrawSprite(ResourceManager::GetTexture("rectangle"), glm::vec2(this->Width / 2 - 200.0f, this->Height / 2 - 100.0f), glm::vec2(400.0f, 200.0f), 0.0f);
        //render text
        Text->Load("C:/Windows/Fonts/JOKERMAN.TTF", 48);
        Text->RenderText("GAME OVER", 250.0f, Height / 2 - 35.0f, 1.0f);
        Text->Load("C:/Windows/Fonts/JOKERMAN.TTF", 30);
        Text->RenderText("Press ENTER to start over", 210.0f, Height / 2 + 30.0f, 1.0f, glm::vec3(0.47f, 1.0f, 0.0f));
    }
}

void Game::DoCollisions()
{
    for (GameObject& coin : this->Coins)
    {
        if (!coin.Destroyed)
        {
            if (CheckCollision(*Player, coin))
            {
                this->NumOfCoins++;
                SoundEngine->play2D("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/audio/coin.wav", false);
                coin.Destroyed = true;
            }
        }
    }

    for (GameObject& coin : this->Boxes)
    {
        if (!coin.Destroyed)
        {
            if (CheckCollision(*Player, coin))
            {
                SoundEngine->play2D("C:/Users/Stefani K/Source/Repos/OpenGLPrjProject/audio/bump.mp3", false);
                coin.Destroyed = true;
                this->State = GAME_OVER;
            }
        }
    }
}

bool Game::CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}