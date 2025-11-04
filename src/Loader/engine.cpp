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

    if (!_drawer->Init()) { fprintf(stderr, "Error initializing Drawer\n"); return EXIT_FAILURE; }

    // Create the Camera
    _cam = new Camera(0, 0);

    if (!_cam) { fprintf(stderr, "Error creating camera object\n"); return EXIT_FAILURE; }

    // Load Assets
    _dataHandler = new DataHandler();

    if (!_dataHandler) { fprintf(stderr, "Error creating DataHandler\n"); return EXIT_FAILURE; }
    if (_dataHandler->ReadFile(_dataPath)) { printf("Error loading assets\n"); return EXIT_FAILURE; }

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

    if (_controller) { printf("%s connected\n", SDL_GameControllerName(_controller)); }

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
                        _cam->SetXSpeed(- 4);
                        break;
                    }

                    case SDLK_RIGHT:
                    {
                        _cam->SetXSpeed(4);
                        break;
                    }

                    case SDLK_DOWN:
                    {
                        _cam->SetYSpeed(4);
                        break;
                    }

                    case SDLK_UP:
                    {
                        _cam->SetYSpeed(- 4);
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

void Engine::Update()
{
    _cam->Move();
    if (_level) _level->DrawScene(_drawer, _cam->GetViewRect());
}

void Engine::Render(CustomRenderFunc crf)
{
    _drawer->DrawFrame();
    
    if (crf)
        crf(PRE_RENDER, this);
    if (_level) _level->DrawScene(_drawer, _cam->GetViewRect());
    if (crf)
        crf(POST_RENDER, this);
    
    SDL_GL_SwapWindow(_drawer->GetWindow());
}

void Engine::Run(EventPolling ep, CustomRenderFunc crf)
{
    constexpr uint32_t PS1_HZ              = 50;
    const uint64_t     FREQ                = SDL_GetPerformanceFrequency();
    const uint64_t     TICK_CNT            = FREQ / PS1_HZ;
    const uint32_t     TICK_FRAC           = (uint32_t) (FREQ % PS1_HZ);
    const uint64_t     CNT_PER_MS          = FREQ / 1000;
    constexpr int      MAX_TICKS_PER_FRAME = 8;
    constexpr int      MAX_BACKLOG_TICKS   = 8;

    if (!_initialized) { printf("Warning: The engine must be initialized before running\n"); return; }

    printf("Running the engine...\n");
    _running = true;

    // Logic clock
    uint64_t nextTick = SDL_GetPerformanceCounter() + TICK_CNT;
    uint32_t frac     = 0;

    // Main game loop
    while (_running)
    {
        ProcessInput(ep);
        //CheckCollisions();

        bool newFrame = false;
        uint64_t now = SDL_GetPerformanceCounter();
        int steps = 0;

        // Ticks
        while (now >= nextTick && steps < MAX_TICKS_PER_FRAME)
        {
            bool lastTick = ((now - nextTick) < TICK_CNT) || (steps+1 == MAX_TICKS_PER_FRAME);

            // Send prims if ready
            _drawer->SetReady(lastTick);
            if (lastTick) _drawer->ClearQueues();
            Update();
            _drawer->SetReady(false);

            // Bresenham clocking
            nextTick += TICK_CNT;
            frac     += TICK_FRAC;
            if (frac >= PS1_HZ) { nextTick += 1; frac -= PS1_HZ; }

            newFrame |= lastTick;
            ++steps;
            now = SDL_GetPerformanceCounter();
        }

        // PANIC DROP
        if (now >= nextTick)
        {
            uint64_t lateness = now - nextTick;
            if (lateness >= (uint64_t)MAX_BACKLOG_TICKS * TICK_CNT)
            {
                nextTick = now + TICK_CNT;
                frac = 0;
            }
        }

        // Render on new frame
        if (newFrame) { Render(crf); }
        else
        {
            uint64_t wait = (nextTick > now) ? (nextTick - now) : 0;

            // Idle
            while (wait > CNT_PER_MS)
            {
                SDL_Delay(1);
                now  = SDL_GetPerformanceCounter();
                wait = (nextTick > now) ? (nextTick - now) : 0;
            }

            // Late CPU spin to close the phase
            while (SDL_GetPerformanceCounter() < nextTick) {}
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

