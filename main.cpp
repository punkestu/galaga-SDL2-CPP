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
    bool left,right,shoot;
    int start;
public:
    player(){
        body[0] = {210, 440, 60, 20};
        body[1] = {230, 420, 20, 20};
        left = right = shoot = 0;
        start = 1000;
    }

    void control(SDL_Event* event){
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.sym == SDLK_a){
                left = 1;
            }
            if(event->key.keysym.sym == SDLK_d){
                right = 1;
            }
            if(event->key.keysym.sym == SDLK_s){
                if(SDL_GetTicks()-start>=1000){
                    std::cout<<SDL_GetTicks()-start<<std::endl;
                    shoot = 1;
                    start = SDL_GetTicks();
                }else{
                    shoot = 0;
                }
            }

        }
        if(event->type == SDL_KEYUP){
            if(event->key.keysym.sym == SDLK_a){
                left = 0;
            }
            if(event->key.keysym.sym == SDLK_d){
                right = 0;
            }
            if(event->key.keysym.sym == SDLK_s){
                shoot = 0;
            }
        }
    }

    void update()
    {

        for(unsigned int i = 0; i < bullets.size(); i++){
            bullets[i].y-=2;
        }
        if(left){
            body[0].x-=5;
            body[1].x-=5;
        }
        if(right){
            body[0].x+=5;
            body[1].x+=5;
        }

        if(shoot){
            bullets.push_back({body[1].x+body[1].w/2-5, body[1].y-10});
            shoot = 0;
        }
    }

    void render(SDL_Renderer* renderer){
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderFillRects(renderer, body, 2);

        for(unsigned int i = 0; i < bullets.size(); i++){
            SDL_Rect bullet = {bullets[i].x,bullets[i].y,10,10};
            SDL_SetRenderDrawColor(renderer, 255,255,0,255);
            SDL_RenderFillRect(renderer, &bullet);
            std::cout<<bullets.size()<<std::endl;
        }
    }

    std::vector<vect2d>* getBullets(){return &bullets;}

};

int main(int argc, char* argv[])
{
    SDL_Window* window = SDL_CreateWindow("apps", 100,100,480,480,false);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    player mplayer = player();

    SDL_Event event;
    int start = SDL_GetTicks();
    while(true){

        //control
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){
            break;
        }
        mplayer.control(&event);
        //

        //update
        if(SDL_GetTicks()-start>=1000/60){
            mplayer.update();
            start = SDL_GetTicks();
        }
        //

        //render
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        mplayer.render(renderer);
        //

        unsigned int i = 0;
        for(std::vector<vect2d>::iterator it = mplayer.getBullets()->begin(); i < mplayer.getBullets()->size() ; it++){
            if(it->y<=120){
                mplayer.getBullets()->erase(it);
                it--;
                i--;
            }
            i++;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
