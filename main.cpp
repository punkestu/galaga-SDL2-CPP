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
    bool left,right,shoot,skey;
    Uint32 start;
public:
    player(){
        body[0] = {210, 440, 60, 20};
        body[1] = {230, 420, 20, 20};
        left = right = shoot = skey = 0;
        start = 99999999;
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
                skey = 1;
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
                skey = 0;
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
        if(skey){
            if(SDL_GetTicks()-start>=500){
                //std::cout<<SDL_GetTicks()-start<<std::endl;
                shoot = 1;
                start = SDL_GetTicks();
            }else{
                shoot = 0;
            }
        }else{
            shoot = 0;
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
    SDL_Rect getBody(){return {body[0].x,body[1].y,body[0].w,body[0].h+body[1].h};}
};

class enemy{
private:
    SDL_Rect body;
    std::vector<vect2d> bullets;
    bool left,right,shoot;
    int start, moving, movHor;
public:
    enemy(vect2d startPos, int moveD){
        body.x = startPos.x;
        body.y = startPos.y;
        body.w = 60;
        body.h = 60;
        left = right = shoot = 0;
        if(moveD == 1){
            right = 1;
        }else{
            left = 1;
        }
        start = SDL_GetTicks();
        moving = SDL_GetTicks();
        movHor = SDL_GetTicks();
    }

    void moveF(){
        body.y+=70;
    }

    void update(){
        if(SDL_GetTicks()-moving >= 10*1000){
            moveF();
            moving = SDL_GetTicks();
        }
        if(SDL_GetTicks()-movHor >= 250){
            SDL_Log("move");
            if(right){
                body.x+=10;
                if(body.x+body.h >= 470){
                    right = 0;
                    left = 1;
                }
            }
            if(left){
                body.x-=10;
                if(body.x <= 10){
                    right = 1;
                    left = 0;
                }
            }
            movHor = SDL_GetTicks();
        }
    }

    void render(SDL_Renderer* renderer){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &body);
    }

    std::vector<vect2d>* getBullets(){return &bullets;}
    SDL_Rect getBody(){return body;}

};

int main(int argc, char* argv[])
{
    SDL_Window* window = SDL_CreateWindow("apps", 100,100,480,480,false);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    player mplayer = player();
    std::vector<enemy*> enemies;
    enemies.push_back(new enemy({{10,10},1}));

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
            for(unsigned int i = 0; i < enemies.size(); i++){
                enemies[i]->update();
            }
            start = SDL_GetTicks();
        }
        //

        //render
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        mplayer.render(renderer);
        for(unsigned int i = 0; i < enemies.size(); i++){
            enemies[i]->render(renderer);
        }
        //

        unsigned int i = 0;
        for(std::vector<vect2d>::iterator it = mplayer.getBullets()->begin(); i < mplayer.getBullets()->size() ; it++){
            if(it->y<=10){
                mplayer.getBullets()->erase(it);
                it--;
                i--;
            }
            for(unsigned int i = 0; i < enemies.size(); i++){
                if(it->x<=enemies[i]->getBody().x+60&&it->x+10>=enemies[i]->getBody().x){
                    if(it->y<=enemies[i]->getBody().y+60){
                        delete enemies[i];
                        enemies.clear();
                        mplayer.getBullets()->erase(it);
                        it--;
                        i--;
                        break;
                    }
                }
            }
            i++;
        }

        SDL_RenderPresent(renderer);
    }

    for(unsigned int i = 0; i < enemies.size(); i++){
        delete enemies[i];
        enemies.clear();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
