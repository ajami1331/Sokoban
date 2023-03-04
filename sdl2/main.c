#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <emscripten.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Context 
{
    int x;
};

struct App 
{
    SDL_Renderer *renderer;
    SDL_Window *window;
} app;



void doInput(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                exit(0);
                break;
            default:
                break;
        }
    }
}

void prepareScene(void)
{
    SDL_SetRenderDrawColor(app.renderer, 96, 128, 255, 255);
    SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
    SDL_RenderPresent(app.renderer);
}

void game_loop(void *arg) 
{
    prepareScene();

    doInput();

    presentScene();

    struct Context *ctx = arg;

    printf("x: %d\n", ctx->x);

    if (ctx->x >= 100) 
    {
        emscripten_cancel_main_loop();
        printf("got here...\n");
        exit(0);
        return;
    }

    ctx->x += 1;

    SDL_Delay(16);
}

void init() 
{
    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = 0;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(0);
    }
    
    //Create window
    app.window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
    if (app.window == NULL) 
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(0);
    }
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

    if (!app.renderer) 
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
}

void cleanup() 
{
    printf("cleaning up");
    SDL_DestroyWindow(app.window);
    SDL_Quit();
}

int main() 
{
	memset(&app, 0, sizeof(app));
    init();
    atexit(&cleanup);
    struct Context ctx;
    int simulate_infinite_loop = 1;
    ctx.x = 0;
    emscripten_set_main_loop_arg(game_loop, &ctx, -1, simulate_infinite_loop);
    printf("exiting");
    return 0;
}