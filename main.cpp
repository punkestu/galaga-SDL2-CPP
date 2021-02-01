#include <SDL.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <time.h>

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
        if(left && body[0].x>=0+20){
            body[0].x-=5;
            body[1].x-=5;
        }
        if(right && body[0].x+60<=460){
            body[0].x+=5;
            body[1].x+=5;
        }
        if(skey){
            if(SDL_GetTicks()-start>=1000){
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
        SDL_RenderFillRect(renderer, &body[1]);
        SDL_SetRenderDrawColor(renderer, 0,0,255,255);
        SDL_RenderFillRect(renderer, &body[0]);

        for(unsigned int i = 0; i < bullets.size(); i++){
            SDL_Rect bullet = {bullets[i].x,bullets[i].y,10,10};
            SDL_SetRenderDrawColor(renderer, 255,255,0,255);
            SDL_RenderFillRect(renderer, &bullet);
            //std::cout<<bullets.size()<<std::endl;
        }
    }

    std::vector<vect2d>* getBullets(){return &bullets;}
    SDL_Rect getBody(){return {body[0].x,body[1].y,body[0].w,body[0].h+body[1].h};}
};

class enemy{
private:
    SDL_Rect body;
    std::vector<vect2d> bullets;
    bool left,right;
    int start, moving, movHor;
    int anchor, eNum;
public:
    enemy(int anchor, int eNum, vect2d startPos, int moveD){
        this->anchor = anchor;
        this->eNum = eNum;
        body.x = startPos.x;
        body.y = startPos.y;
        body.w = 30;
        body.h = 30;
        left = right = 0;
        if(moveD == 1){
            right = 1;
        }else{
            left = 1;
        }
        start = SDL_GetTicks();
        moving = SDL_GetTicks();
        movHor = SDL_GetTicks();
    }

    void update(bool shoot){
        for(unsigned int i = 0; i < bullets.size(); i++){
            bullets[i].y+=2;
        }

        if(SDL_GetTicks()-moving >= 15*1000){
            body.y+=40;
            moving = SDL_GetTicks();
        }
        if(SDL_GetTicks()-movHor >= 250){
            if(right){
                body.x+=10;
                if(body.x+body.h >= 480-(2*30*(eNum-anchor))){
                    right = 0;
                    left = 1;
                }
            }
            if(left){
                body.x-=10;
                if(body.x <= 0+(2*30*anchor)+10){
                    right = 1;
                    left = 0;
                }
            }
            movHor = SDL_GetTicks();
        }
        if(shoot){
            if(SDL_GetTicks()-start>=2000){
                bullets.push_back({body.x+body.w/2+5,body.y+body.h});
                start = SDL_GetTicks();
            }
        }
    }

    void render(SDL_Renderer* renderer){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &body);
        for(unsigned int i = 0; i < bullets.size(); i++){
            SDL_Rect bullet = {bullets[i].x,bullets[i].y,10,10};
            SDL_RenderFillRect(renderer, &bullet);
        }
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

    SDL_Event event;
    int start;
    int p;
    bool dead;
    bool retry;

    re:
    start = SDL_GetTicks();
    p = 0;
    dead = false;
    retry = false;

    for(int i = 0; i < 7; i++){
        enemies.push_back(new enemy(i,6,{i*30+i*30+20,20},-1));
    }
    for(int i = 0; i < 7; i++){
        enemies.push_back(new enemy(i,6,{i*30+i*30+20,60},1));
    }
    for(int i = 0; i < 7; i++){
        enemies.push_back(new enemy(i,6,{i*30+i*30+20,100},-1));
    }
    for(int i = 0; i < 7; i++){
        enemies.push_back(new enemy(i,6,{i*30+i*30+20,140},1));
    }

    while(true){
        srand(time(NULL)+rand());

        //control
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)){
            break;
        }
        mplayer.control(&event);
        //

        //update
        if(!dead){
            if(SDL_GetTicks()-start>=1000/60){
                mplayer.update();
                for(unsigned int i = 0; i < enemies.size(); i++){
                    if((int)i >= ((int)(enemies.size()/7)-1)*7){
                        for(int j = 0; j < 25; j++){
                            p = rand();
                        }
                        if(p%100 >= 99){
                            enemies[i]->update(true);
                        }else{
                            enemies[i]->update(false);
                        }
                    }else{
                        enemies[i]->update(false);
                    }
                }
                if(enemies.size()==0){
                    if(event.type == SDL_KEYDOWN){
                        if(event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_RETURN){
                            break;
                        }
                        if(event.key.keysym.sym == SDLK_r){
                            retry = true;
                            break;
                        }
                    }
                }
                start = SDL_GetTicks();
            }
        }else{
            for(unsigned int i = 0; i < enemies.size(); i++){
                enemies[i]->getBullets()->clear();
            }
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_RETURN){
                    break;
                }
                if(event.key.keysym.sym == SDLK_r){
                    retry = true;
                    break;
                }
            }
        }
        //

        //render
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        if(!dead){
            mplayer.render(renderer);
        }
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
            for(std::vector<enemy*>::iterator ite = enemies.begin(); ite != enemies.end(); ite++){
                if(it->x<=(*ite)->getBody().x+(*ite)->getBody().w&&it->x+10>=(*ite)->getBody().x){
                    if(it->y<=(*ite)->getBody().y+(*ite)->getBody().h && it->y>=(*ite)->getBody().y){
                        delete *ite;
                        enemies.erase(ite);
                        mplayer.getBullets()->erase(it);
                        it--;
                        i--;
                        break;
                    }
                }
            }
            i++;
        }
        for(unsigned int ene = 0; ene < enemies.size(); ene++){
            i=0;
            if(enemies[ene]->getBody().y+enemies[ene]->getBody().h>=mplayer.getBody().y){
                dead = true;
            }

            for(std::vector<vect2d>::iterator it = enemies[ene]->getBullets()->begin(); i < enemies[ene]->getBullets()->size() ; it++){
                if(it->y>=465){
                    //std::cout<<"pop"<<std::endl;
                    enemies[ene]->getBullets()->erase(it);
                    it--;
                    i--;
                }
                if(it->x<mplayer.getBody().x+mplayer.getBody().w && it->x+10>mplayer.getBody().x){
                    if(it->y<=mplayer.getBody().y+mplayer.getBody().h && it->y+10>=mplayer.getBody().y+20){
                        dead = true;
                        enemies[ene]->getBullets()->erase(it);
                        it--;
                        i--;
                    }
                }
                i++;
            }
        }

        SDL_RenderPresent(renderer);
    }

    for(unsigned int i = 0; i < enemies.size(); i++){
        delete enemies[i];
        enemies.clear();
    }
    mplayer.getBullets()->clear();

    if(retry){
        goto re;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
