#include "engine.h"
#include <cstring>

Engine::Engine(const char* dataPath)
      : _initialized(false), _running(false), _levelDex(0),
        _drawer(nullptr), _dataHandler(nullptr), _level(nullptr),
        _cam(nullptr), _controller(nullptr)
{
    strcpy(_dataPath, dataPath);
}

int Engine::Init()
{
    // Create the Drawer
    _drawer = new Drawer();

    if (!_drawer->Init())
    {
        fprintf(stderr, "Error initializing Drawer\n");
        return EXIT_FAILURE;
    }

    // Create the Camera
    _cam = new Camera(0, 0);

    if (!_cam)
    {
        fprintf(stderr, "Error creating camera object\n");
        return EXIT_FAILURE;
    }

    // Load Assets
    _dataHandler = new DataHandler();

    if (!_dataHandler)
    {
        fprintf(stderr, "Error creating DataHandler\n");
        return EXIT_FAILURE;
    }

    if (_dataHandler->ReadFile(_dataPath))
    {
        printf("Error loading assets\n");
        return EXIT_FAILURE;
    }

    // Look for controllers
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    int numJoysticks = SDL_NumJoysticks();

    for (int i = 0; i < numJoysticks; i++)
    {
        if (SDL_IsGameController(i))
        {
            _controller = SDL_GameControllerOpen(i);
            break;
        }
    }

    if (_controller)
    {
        printf("%s connected\n", SDL_GameControllerName(_controller));
    }

    _initialized = true;
    printf("Engine successfully initialized\n");

    // Load debug level
    _level = _dataHandler->LoadLevel(_levelDex);
    if (_level) _level->Init(_drawer);

    return 0;
}

void Engine::ProcessInput(EventPolling ep)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (ep)
            ep(&event);
        switch (event.type)
        {
            case SDL_QUIT:
            {
                printf("Shutting down the engine...\n");
                _running = false;
                break;
            }
            
            // Exit engine if main window is closing
            case SDL_WINDOWEVENT: 
            {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(_drawer->GetWindow())) {
                    SDL_Event quit_event;
                    quit_event.type = SDL_QUIT;
                    SDL_PushEvent(&quit_event);
                }
                break;
            }

            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    {
                        printf("Shutting down the engine...\n");
                        _running = false;
                        break;
                    }

                    case SDLK_a:
                    {
                        _drawer->ToggleAspectRatio();
                        break;
                    }

                    case SDLK_f:
                    {
                        _drawer->ToggleFullscreen();
                        break;
                    }

                    case SDLK_LEFT:
                    {
                        _cam->SetXSpeed(- 6);
                        break;
                    }

                    case SDLK_RIGHT:
                    {
                        _cam->SetXSpeed(6);
                        break;
                    }

                    case SDLK_DOWN:
                    {
                        _cam->SetYSpeed(6);
                        break;
                    }

                    case SDLK_UP:
                    {
                        _cam->SetYSpeed(- 6);
                        break;
                    }

                    case SDLK_m:
                    {
                        SetLevelIndex(_levelDex - 1);
                        break;
                    }

                    case SDLK_p:
                    {
                        SetLevelIndex(_levelDex + 1);
                        break;
                    }

                }   break;
            }

            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                    {
                        _cam->SetXSpeed(0);
                        break;
                    }

                    case SDLK_DOWN:
                    case SDLK_UP:
                    {
                        _cam->SetYSpeed(0);
                        break;
                    }

                }   break;
            }

            case SDL_CONTROLLERBUTTONDOWN:
            {
                printf("Controller button pressed: %d\n", event.cbutton.button);
                break;
            }

            case SDL_CONTROLLERBUTTONUP:
            {
                printf("Controller button released: %d\n", event.cbutton.button);
                break;
            }

            case SDL_CONTROLLERAXISMOTION:
            {
                break;
            }
        }
    }
}

void Engine::Update(float deltaTime)
{
    _cam->Move();
}

void Engine::Render(CustomRenderFunc crf)
{
    if (crf)
        crf(PRE_RENDER, this);
    if (_level) _level->DrawScene(_drawer, _cam->GetViewRect());
    if (crf)
        crf(POST_RENDER, this);
    SDL_GL_SwapWindow(_drawer->GetWindow());
}

void Engine::Run(EventPolling ep, CustomRenderFunc crf)
{
    if (!_initialized)
    {
        printf("Warning: The engine must be initialized before running\n");
        return;
    }

    printf("Running the engine...\n");
    _running = true;

    // 25 FPS (PAL)
    const float frameDuration = 0.04f;

    // Record the time of the last frame
    clock_t lastFrameTime = clock();

    // Main game loop
    while (_running)
    {
        // Capture the current time and calculate delta time (in seconds)
        clock_t currentTime = clock();
        float deltaTime = (float) (currentTime - lastFrameTime) * 0.001f;
        lastFrameTime = currentTime;

        // Core
        ProcessInput(ep);
        //CheckCollisions();
        Update(deltaTime);
        Render(crf);

        // Frame limiting using SDL_Delay() to wait until the frame duration is met
        clock_t frameEndTime = clock();
        float frameElapsed = (float) (frameEndTime - currentTime) * 0.001f;

        if (frameElapsed < frameDuration)
        {
            // Calculate the remaining time to wait in milliseconds.
            uint32_t delayMs = (uint32_t) ((frameDuration - frameElapsed) * 1000);
            SDL_Delay(delayMs);
        }
    }
}

SDL_Window* Engine::GetWindow()
{
    if (!_drawer)
        return nullptr;
    return _drawer->GetWindow();
}

SDL_GLContext* Engine::GetContext()
{
    if (!_drawer)
        return nullptr;
    return _drawer->GetContext();
}

int Engine::SetLevelIndex(int level)
{
    if (level < 0)
        _levelDex = Level::LEVEL_MAX_INDEX;
    else if (level > Level::LEVEL_MAX_INDEX)
        _levelDex = 0;
    else
        _levelDex = level;
    delete _level;
    _level = _dataHandler->LoadLevel(_levelDex);
    if (_level) _level->Init(_drawer);
    return _levelDex;
}

Camera* Engine::GetCamera()
{
    return _cam;
}

Engine::~Engine()
{
    delete _drawer;
    delete _level;
    delete _dataHandler;
    delete _cam;
    SDL_GameControllerClose(_controller);
    SDL_Quit();
}
