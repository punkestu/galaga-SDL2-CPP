#include <SDL.h>
#include <iostream>
#include <vector>

struct vect2d{
    int x,y;
};

class player{
private:
    SDL_Rect body[2];
    std::vector<vect2d> bullets;
    bool left,right;
public:
    player(){
        body[0] = {210, 440, 60, 20};
        body[1] = {230, 420, 20, 20};
        left = right = 0;
    }

    void control(SDL_Event* event){
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.sym == SDLK_a){
                left = 1;
            }
            if(event->key.keysym.sym == SDLK_d){
                right = 1;
            }
        }
        if(event->type == SDL_KEYUP){
            if(event->key.keysym.sym == SDLK_a){
                left = 0;
            }
            if(event->key.keysym.sym == SDLK_d){
                right = 0;
            }
        }
    }

    void update()
    {
        if(left){
            body[0].x-=5;
            body[1].x-=5;
        }
        if(right){
            body[0].x+=5;
            body[1].x+=5;
        }
    }

    void render(SDL_Renderer* renderer){
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderFillRects(renderer, body, 2);
    }

};

int main(int argc, char* argv[])
{
    SDL_Window* window = SDL_CreateWindow("apps", 100,100,480,480,false);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    player mplayer = player();

    SDL_Event event;
    int start = SDL_GetTicks();
    while(true){
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        mplayer.render(renderer);

        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){
            break;
        }
        mplayer.control(&event);

        if(SDL_GetTicks()-start>=1000/60){
            mplayer.update();
            start = SDL_GetTicks();
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
