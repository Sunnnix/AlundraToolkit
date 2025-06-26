#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include "datahandler.h"
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <ctime>

enum RenderState {
    PRE_RENDER,
    POST_RENDER
};

class Engine;

typedef void (*EventPolling)(SDL_Event*);
typedef void (*CustomRenderFunc)(RenderState, Engine*);

class Camera
{
    private:

    Point _position;                // Position (pixels)
    static const int _width = 336;
    static const int _height = 256;
    int _dx;
    int _dy;

    public:

    /// Constructor
    Camera(int startX, int startY) : _position(Point { startX, startY }), _dx(0), _dy(0) {}

    // Getters/Setters
    Rect GetViewRect() const { return { _position.X, _position.Y, _width, _height }; }
    void SetPosition(Point position) { _position = position; }
    void SetXSpeed(int dx) { _dx = dx; }
    void SetYSpeed(int dy) { _dy = dy; }

    /// Move the camera using delta
    void Move()
    {
        _position.X += _dx;
        _position.Y += _dy;
    }

    /// Destructor
    ~Camera() = default;
};

class Engine
{
    private:

    char _dataPath[260];
    bool _initialized;
    bool _running;
    uint16_t _levelDex;

    Drawer* _drawer;
    Camera* _cam;
    DataHandler* _dataHandler;
    Level* _level;

    SDL_GameController* _controller;

    /// Processes user input
    void ProcessInput(EventPolling ep);

    /// Updates the game state
    void Update(float deltaTime);

    /// Renders game objects on the screen
    void Render(CustomRenderFunc crf);

    /// Checks for collisions between game objects
    void CheckCollisions();

    public:

    /// Constructor
    Engine(const char* dataPath);

    /// Initializes the game engine
    int Init();

    /// Starts the main game loop
    void Run(EventPolling ep = 0, CustomRenderFunc func = 0);

    SDL_Window* GetWindow();

    SDL_GLContext* GetContext();

    int GetLevelIndex() { return _levelDex; }
    int SetLevelIndex(int level);

    Camera* GetCamera();

    /// Destructor
    ~Engine();
};

#endif // ENGINE_H_INCLUDED
