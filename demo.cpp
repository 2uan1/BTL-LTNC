#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

int LEVEL_WIDTH = 2560;
int LEVEL_HEIGHT = 1440;

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

int SLIME_SPRITE_COUNT = 11;
SDL_Rect slimeSprite[11];

int SNAKE_SPRITE_COUNT = 2;
SDL_Rect snakeSprite[2];

int HEART_SPRITE_COUNT = 5;
SDL_Rect heartSprite[5];

int COIN_SPRITE_COUNT = 6;
SDL_Rect coinSprite[6];

SDL_Rect spriteWalkdown[11];
SDL_Rect spriteWalkright[11];
SDL_Rect spriteWalkleft[11];
SDL_Rect spriteWalkup[11];
SDL_Rect* direction[4] = {spriteWalkup, spriteWalkleft, spriteWalkdown, spriteWalkright};
enum dir
{
    up,
    left,
    down,
    right
};

enum enemyType
{
    SLIME,
    SNAKE,
    ENEMY_TYPES
};

enum atkdir
{
    _up,
    upleft,
    _left,
    downleft,
    _down,
    downright,
    _right,
    upright
};

enum weapon
{
    sword,
    bow
};

Mix_Chunk* footstep = NULL;

//classes
class Texture;
class Object;
class Player;
class Boss;
class projectile;
class Slime;
class coin;
class Snake;

//vector arrays
std::vector<Object*> objects;
std::vector<projectile*> projectiles;

//function prototypes
bool init(std::string path);
bool loadAsset();
void quit();
bool checkCollision(SDL_Rect a, SDL_Rect b);
bool checkCollisionObj (SDL_Rect a, std::vector<Object*> objs);
bool checkCollisionEnemy (SDL_Rect a, std::vector<Slime*> enemies);
float angleBetween(Object& a, Object& b);
float angleBetweenRect(SDL_Rect a, SDL_Rect b);
float distanceBetween(Object& a, Object& b);
float distanceBetweenRect(SDL_Rect a, SDL_Rect b);
void checkAlive();
void spawnEnemy(int& time);

//class implementation
class Texture//following from lazyfoo tutorial^^
{
    public:
        Texture(int x = 0, int y = 0)
        {
            dTexture = NULL;
            tWidth = 0;
            tHeight = 0;
        }

        ~Texture()
        {
            free();
        }

        void render(int x = 0, int y = 0, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE )
        {
            SDL_Rect rect = {x, y, tWidth, tHeight};
            //SDL_Rect sized_rect = {x, y, 240*2, 208*2};
            if (clip != NULL)
            {
                rect.w = clip->w;
                rect.h = clip->h;
            }
            SDL_RenderCopyEx(renderer, dTexture, clip, &rect, angle, center, flip);
        }

        bool loadFile(std::string path)
        {
            free();
            SDL_Texture* convertedtexture = NULL;
            SDL_Surface* loadedsurface = IMG_Load(path.c_str());
            //dTexture = IMG_LoadTexture(path.c_str());
            if (loadedsurface == NULL)
                std::cout << "loadFile Error: can't load surface" << std::endl;
            else
            {
                convertedtexture = SDL_CreateTextureFromSurface(renderer, loadedsurface);
                if (convertedtexture == NULL)
                    std::cout << "loadFile Error: can't convert surface" << std::endl;
                else
                {
                    tWidth = loadedsurface->w;
                    tHeight = loadedsurface->h;

                }
                SDL_FreeSurface(loadedsurface);
            }
            dTexture = convertedtexture;
            //SDL_DestroyTexture(convertedtexture);
            if (dTexture == NULL)
                return false;
            else
                return true;
        }

        void free()
        {
            SDL_DestroyTexture(dTexture);
            dTexture = NULL;
            tWidth = 0;
            tHeight = 0;
        }

        int getWidth()
        {
            return tWidth;
        }

        int getHeight()
        {
            return tHeight;
        }

    private:
        int tWidth;
        int tHeight;

        SDL_Texture* dTexture;
};

//sprites
//Texture dumb;
Texture link;
Texture background;
Texture arrow;
Texture slime;
Texture Coin;
Texture Heart;
Texture snake;

class Object
{
public:
    Object(int x = 0, int y = 0, int inithealth = 1)
    {
        xPos = x;
        yPos = y;
        txtr = NULL;
        health = inithealth;
        objects.push_back(this);
    }

    void setPos(float x, float y)
    {
        xPos = x;
        yPos = y;
        cbox.x = xPos;
        cbox.y = yPos;
    }

//    ~Object()
//    {
//        txtr->free();
//        objects.erase(std::remove(objects.begin(), objects.end(), this), objects.end());
//    }

    virtual void updateStatus(int damage)
    {
        health -= damage;

    }

    virtual void playDeathAnim()
    {

    }

    virtual void render()
    {
        txtr->render(xPos, yPos);
    }

    float getX()
    {
        return xPos;
    }

    float getY()
    {
        return yPos;
    }

    int getHeight()
    {
        return height;
    }

    int getWidth()
    {
        return width;
    }

    int getHealth()
    {
        return health;
    }

    SDL_Rect getColBox()
    {
        return cbox;
    }

    virtual void knockback(float angle)
    {

    }

//    SDL_Texture* getTexture()
//    {
//        return txtr;
//    }

protected:
    float xPos, yPos;
    int width, height;
    int health;
    SDL_Rect cbox;
    Texture* txtr;
};

class Boss : public Object
{
public:
    Boss(int x, int y, int health) : Object(x, y, health)
    {
        cbox.x = xPos;
        cbox.y = yPos;
//        txtr = &dumb;
        cbox.w = txtr->getWidth();
        cbox.h = txtr->getHeight();
    }

    void move(Player p);

    void renderHealth()
    {
        SDL_Rect healthBar = {xPos-10-camera.x, yPos-10-camera.y, health/2, 5};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
        SDL_RenderFillRect(renderer, &healthBar);
    }

    float calculateAngle()
    {
        float deltaX = (xPos - xTargetDst);
        float deltaY = -(yPos - yTargetDst);//SDL cordinate system has y invert compare to descartes
        float angle = atan(deltaY/deltaX)/3.14159*180;
        if (xPos > xTargetDst && yPos > yTargetDst)
            return angle+180;
        else if (xPos > xTargetDst && yPos <yTargetDst)
            return angle-180;
        return angle;
    }

    void render()
    {
//        SDL_Rect colbox = {cbox.x - camera.x, cbox.y - camera.y, cbox.w, cbox.h};
//        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
//        SDL_RenderFillRect(renderer, &colbox);
        txtr->render(xPos-camera.x, yPos-camera.y);
        renderHealth();
    }

    void updateStatus(int damage)
    {
        std::cout << "reducing health by 5" << std::endl;
        health -= damage;
    }

    void handleInput(SDL_Event& e)
    {
        if (e.type == SDL_MOUSEBUTTONUP && e.key.repeat == 0)
        {
            switch(e.button.button)
            {
                case SDL_BUTTON_RIGHT:
                    xTargetDst = e.motion.x+camera.x;
                    yTargetDst = e.motion.y+camera.y;
////                    std::cout << "emotionx,emotiony) = (" << e.motion.x << "," << e.motion.y << ")" << std::endl;
//                    std::cout << "(xTarget,yTarget) = (" << xTargetDst << "," << yTargetDst << ")" << std::endl;
//                    std::cout << sin(90*3.14159/180);
//                    std::cout << calculateAngle();
                    break;
                case SDL_BUTTON_LEFT:
                    xAttackDst = e.motion.x+camera.x;
                    yAttackDst = e.motion.y+camera.y;
                    std::cout << "(xAttack,yAttack) = (" << xAttackDst << "," << yAttackDst << ")" << std::endl;
                    break;
            }
        }
    }

private:
    float xTargetDst, yTargetDst;
    float xAttackDst, yAttackDst;
    float Vel = 3;
//    SDL_Rect BossCollisionBox;
};

std::vector<Slime*> slimes;
class Slime : public Object
{
public:
    int SLIME_WIDTH = 42;
    int SLIME_HEIGHT = 30;
    int VEL = 3;
    Slime(int x, int y, int health) : Object(x, y, health)
    {
        txtr = &slime;
        cbox = {xPos + 6, yPos + 30, SLIME_WIDTH, SLIME_HEIGHT};
        xVel = 0;
        yVel = 0;
        visionRange = 200;
        attackRange = 100;
        chargingAmount = 0;
        chargingAttack = false;
        displace = false;
    }
//    ~Slime()
//    {
//        txtr->free();
//    }

    int frame = 0;
    void render()
    {
        renderHealth();
        SDL_Rect* CurrentClip;
        CurrentClip = &slimeSprite[frame/6];
        txtr->render(xPos - camera.x, yPos - camera.y, CurrentClip);

        frame++;
        if (frame / 6 >= SLIME_SPRITE_COUNT)
        {
            frame = 0;
        }
////        std::cout << "slimexy = (" << xPos << "," << yPos <<")" << std::endl;
    }
    void renderHealth()
    {
        SDL_Rect rendercbox = {cbox.x - camera.x, cbox.y - camera.y, cbox.w, cbox.h};
        SDL_SetRenderDrawColor(renderer, 128,0,0,0);
        SDL_RenderFillRect(renderer, &rendercbox);
        SDL_Rect healthBar = {xPos-camera.x, yPos-camera.y, health/2, 5};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
        SDL_RenderFillRect(renderer, &healthBar);
    }
    void move(Player& player);

    void attack(Player& player);

    void updateStatus(int damage)
    {
        health -= damage;
    }

    float knockbackamount = 50;
    void knockback(float angle)
    {
        displace = true;
        float displaceX = cos((angle)/180.0*3.14159) * knockbackamount;
        float displaceY = -sin((angle)/180.0*3.14159) * knockbackamount;
        xPos += displaceX; cbox.x = xPos+6;
        yPos += displaceY; cbox.y = yPos+30;

//        displace = false;
//        std::cout << "sin(135) = " << sin(135.0/180.0*3.14159) << std::endl;
    }
private:
    float xVel, yVel;
    bool displace;
    int visionRange;
    int attackRange;
    bool chargingAttack;
    int chargingAmount;
    int stunDuration;
};

std::vector<Snake*> snakes;
class Snake : public Object
{
public:
    int SNAKE_WIDTH = 44;
    int SNAKE_HEIGHT = 30;
    int VEL = 3;
    Snake(int x, int y, int health) : Object(x, y, health)
    {
        txtr = &snake;
        cbox = {xPos, yPos, SNAKE_WIDTH, SNAKE_HEIGHT};
        xVel = 0;
        yVel = 0;
        displace = false;
        currentDirection = rand() % 2;
    }

    int frame = 0;
    void render()
    {
        renderHealth();
        SDL_Rect* CurrentClip;
        CurrentClip = &snakeSprite[frame/6];
        if (currentDirection == right)
            txtr->render(xPos - camera.x, yPos - camera.y, CurrentClip, 180, NULL,SDL_FLIP_VERTICAL);
        else if (currentDirection == left)
            txtr->render(xPos - camera.x, yPos - camera.y, CurrentClip);


        frame++;
        if (frame / 6 >= SNAKE_SPRITE_COUNT)
        {
            frame = 0;
        }
////        std::cout << "slimexy = (" << xPos << "," << yPos <<")" << std::endl;
    }
    void renderHealth()
    {
        SDL_Rect rendercbox = {cbox.x - camera.x, cbox.y - camera.y, cbox.w, cbox.h};
        SDL_SetRenderDrawColor(renderer, 128,0,0,0);
        SDL_RenderFillRect(renderer, &rendercbox);
        SDL_Rect healthBar = {xPos-10-camera.x, yPos-10-camera.y, health/2, 5};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
        SDL_RenderFillRect(renderer, &healthBar);

    }
    void move(Player& player);

    void attack()
    {

    }

    void updateStatus(int damage)
    {
        health -= damage;
    }

    float knockbackamount = 50;
    void knockback(float angle)
    {
        displace = true;
        float displaceX = cos((angle)/180.0*3.14159) * knockbackamount;
        float displaceY = -sin((angle)/180.0*3.14159) * knockbackamount;
        xPos += displaceX; cbox.x = xPos;
        yPos += displaceY; cbox.y = yPos;
    }
private:
    float xVel, yVel;
    int currentDirection;
    bool displace;
    int visionRange;
    int stunDuration;
};

std::vector<coin*> coins;

class heart
{
public:
    heart(int x, int y)
    {
        heartTexture = &Coin;
        collected = false;
        heartCollisionBox = {x, y, 40, 30};
    }
    int frame = 0;
    void render()
    {
        SDL_Rect* CurrentClip;
        CurrentClip = &heartSprite[frame/6];
        heartTexture->render(heartCollisionBox.x - heartCollisionBox.x, heartCollisionBox.y - camera.y, CurrentClip);
        frame++;
        if (frame/6 > HEART_SPRITE_COUNT)
            frame = 0;
    }
    void update(Player& plyr);
    bool gotCollected()
    {
        return collected;
    }
    SDL_Rect getHeartBox()
    {
        return heartCollisionBox;
    }
private:
    bool collected;
    SDL_Rect heartCollisionBox;
    Texture* heartTexture;
};

class coin
{
public:
    coin(int x, int y)
    {
        cointxtr = &Coin;
        collected = false;
        coinbox = {x, y, 40, 40};
    }

    int frame = 0;
    void render()
    {
        SDL_Rect* CurrentClip;
        CurrentClip = &coinSprite[frame/30];
        cointxtr->render(coinbox.x - camera.x, coinbox.y - camera.y, CurrentClip);
        frame++;
        if (frame/30 > COIN_SPRITE_COUNT)
            frame = 0;
    }

    void update(Player& plyr);

    bool gotCollected()
    {
        return collected;
    }

    SDL_Rect getCoinBox()
    {
        return coinbox;
    }
private:
    bool collected;
    SDL_Rect coinbox;
    Texture* cointxtr;
};

class Player : public Object
{
    public:
        const int playerSpriteWidth = 36;
        const int playerSpriteHeight = 46;

        Player(int x, int y, int inithealth) : Object(x, y, inithealth)
        {
            cbox.x = xPos+4;//plus offset
            cbox.y = yPos+6;
            xVel = 0;
            yVel = 0;
            xAcc = 0;
            yAcc = 0;
            money = 0;
            currentDirection = down;
            currentWeapon = sword;
            invincible = false;
            invincibilityTimer = 0;
            txtr = &link;
            cbox.w = playerSpriteWidth; //link width in pixel
            cbox.h = playerSpriteHeight;//link height
        }

        void move(std::vector<Object*> objs, std::vector<Slime*> enemies)
        {
            float xspeedDif = speedCap - abs(xVel);
            float xease = xspeedDif * xAcc/3;
            xVel += xease;
            if (-speedCap > xVel)
                xVel = -speedCap;
            if (xVel > speedCap)
                xVel = speedCap;
            float yspeedDif = speedCap - abs(yVel);
            float yease = yspeedDif * yAcc/3;
            yVel += yease;
            if (-speedCap > yVel)
                yVel = -speedCap;
            if (yVel > speedCap)
                yVel = speedCap;
            /*if (xAcc > 0)
                xAcc -= friction;
            else if (xAcc < 0)
                xAcc += friction;
            if (xAcc < friction && xAcc > -friction)
                xAcc = 0;*/
            //std::cout << "xVel: " << xVel << " xAcc = " << xAcc <<std::endl;
            //friction decelleration
            if (xVel > 0)
                xVel -= friction;
            else if (xVel < 0)
                xVel += friction;
            if (xVel < friction && xVel > -friction)
                xVel = 0;
            if (yVel > 0)
                yVel -= friction;
            else if (yVel < 0)
                yVel += friction;
            if (yVel < friction && yVel > -friction)
                yVel = 0;

            if (xVel > 0)
                currentDirection = right;
            else if (xVel < 0)
                currentDirection = left;
            if (yVel > 0)
                currentDirection = down;
            else if (yVel < 0)
                currentDirection = up;

            if (xVel > 0 && yVel == 0)
                currentAttackingDirection = _right;
            else if (xVel < 0 && yVel == 0)
                currentAttackingDirection = _left;
            if (yVel > 0 && xVel == 0)
                currentAttackingDirection = _down;
            else if (yVel < 0 && xVel == 0)
                currentAttackingDirection = _up;
            if (xVel > 0 && yVel > 0)
                currentAttackingDirection = downright;
            else if (xVel > 0 && yVel < 0)
                currentAttackingDirection = upright;
            else if (xVel < 0 && yVel < 0)
                currentAttackingDirection  = upleft;
            else if (xVel < 0 && yVel > 0)
                currentAttackingDirection = downleft;

            xPos += xVel;
            cbox.x = xPos+4;//plus offset
            if (xPos < 0 || (xPos + playerSpriteWidth) > LEVEL_WIDTH || checkCollisionObj(cbox, objs) || checkCollisionEnemy(cbox, slimes))
            {
//                std::cout << checkCollisionObj(cbox, objs);
                xPos -= xVel;
            }
            yPos += yVel;
            cbox.y = yPos+6;
            if (yPos < 0 || (yPos + playerSpriteHeight)> LEVEL_HEIGHT || checkCollisionObj(cbox, objs) )
            {
                yPos -= yVel;
            }
        }

        int frame = 4;
        void render()
        {
            //SDL_Rect* CurrentClip = &spriteWalkleft[frame / 4];

            SDL_Rect* CurrentClip;

            if (xVel != 0 || yVel != 0)
            {
                CurrentClip = &direction[currentDirection][frame/4];
                Mix_PlayChannel(-1, footstep, -1);
            }
            else
            {
                CurrentClip = &direction[currentDirection][0];
                Mix_HaltChannel(-1);
            }

//            std::cout << "xVel: " << xVel << " yVel: " << yVel << std::endl;
//            SDL_RenderFillRect(renderer, &cbox);//render underlying collisionbox
            txtr->render(xPos - camera.x, yPos - camera.y, CurrentClip);
            renderHealth();
            frame++;
            if (frame / 4 >= 11)
            {
                frame = 4;
            }
        }

        void renderHealth()
        {
            SDL_Rect healthBar = {xPos - camera.x, yPos - camera.y, health/2, 10};
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
            SDL_RenderFillRect(renderer, &healthBar);

//            for (int i = 0; i < 6; i++)
//            {
//                SDL_Rect healthBaroutline = {20-i, 20-i, 200+2*i, 30+2*i};
//                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
//                SDL_RenderDrawRect(renderer, &healthBaroutline);
//            }
        }

        void collectCoin()
        {
            money++;
            std::cout << "couin: " << money << std::endl;
        }

        void collectHeart()
        {
            health += 5;
        }

        void displayCoinCount()
        {

        }

        void update(int damage = 0)
        {
            std::cout <<"about to do somedamgeag to stupid" << std::endl;
            if (damage > 0 && invincible == false)
            {
                std::cout << "took damege" << std::endl;
                health -= damage;
                invincible = true;
            }
            if (invincible == true)
            {
                invincibilityTimer++;
            }
            if (invincibilityTimer > 120)
            {
                invincible = false;
                invincibilityTimer = 0;
            }
        }

        void attack(std::vector<Object*> objs)
        {
            SDL_Rect attackBox;
            int atkScale = 1;
            if (currentDirection == up)
                attackBox = {xPos-camera.x + 5, yPos - cbox.w/2 + 25 - camera.y - atkScale*cbox.h/2, cbox.w, cbox.h * atkScale};
            else if (currentDirection == down)
                attackBox = {xPos-camera.x + 5, yPos + cbox.w - camera.y - 10, cbox.w, cbox.h * atkScale};
            else if (currentDirection == right)
                attackBox = {xPos-camera.x + cbox.w, yPos - camera.y, cbox.w * atkScale, cbox.h};
            else if (currentDirection == left)
                attackBox = {xPos-camera.x - cbox.w + 20 - atkScale*cbox.w/2, yPos - camera.y, cbox.w * atkScale, cbox.h};

            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
            SDL_RenderFillRect(renderer, &attackBox);

            for (auto obj : objs)
            {
//                std::cout << obj->getX() << "," << obj->getY() << std::endl;
                SDL_Rect objBox = {obj->getColBox().x - camera.x, obj->getColBox().y - camera.y, obj->getColBox().w, obj->getColBox().h};
//                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 50);
//                SDL_RenderFillRect(renderer, &objBox);
                if(checkCollision(attackBox, objBox))
                {
                    if(obj != this)
                    {
                        obj->updateStatus(5);
                        SDL_Rect tmpcbox = {cbox.x + cbox.w/2, cbox.y + cbox.h/2, 0, 0};
                        SDL_Rect tmpobjbox = {obj->getColBox().x + obj->getColBox().w/2, obj->getColBox().y + obj->getColBox().h/2};
                        obj->knockback(angleBetweenRect(tmpcbox, tmpobjbox));
//                        std::cout << angleBetweenRect(cbox, obj->getColBox()) << std::endl;
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 50);
//                        int i = 0;
//                        while(i<10)
//                        {
//                            i++;
//                            std::cout << i << std::endl;
                            SDL_RenderFillRect(renderer, &objBox);
//                            SDL_Delay(1000);
//                        }
                    }
                }
            }
        }

        void shoot()
        {

        }

        void handleInput(SDL_Event& e)
        {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_w:
                        yAcc -= ACC;
                        break;
                    case SDLK_a:
                        xAcc -= ACC;
                        break;
                    case SDLK_s:
                        yAcc += ACC;
                        break;
                    case SDLK_d:
                        xAcc += ACC;
                        break;
                    case SDLK_r:
                        std::cout << currentWeapon <<std::endl;
                        if(currentWeapon == sword)
                            currentWeapon = bow;
                        else if (currentWeapon == bow)
                            currentWeapon = sword;
                        break;
                    case SDLK_v:
                        if (currentWeapon == sword)
                            attack(objects);
                        else if (currentWeapon == bow)
                            shoot();
                        charging = true;
                        std::cout << "pressing v" << std::endl;
                        break;
                    case SDLK_f:
                        ACC = 1;
                        std::cout << "pressing F" << std::endl;
                        break;
                }
            }
            if (e.type == SDL_KEYUP && e.key.repeat == 0)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_w:
                        yAcc += ACC;
                        break;
                    case SDLK_a:
                        xAcc += ACC;
                        break;
                    case SDLK_s:
                        yAcc -= ACC;
                        break;
                    case SDLK_d:
                        xAcc -= ACC;
                        break;
                    case SDLK_v:
                        charging = false;
                        std::cout << "releasing v" << std::endl;
                        break;
                    case SDLK_f:
                        ACC = 0.5;
                        std::cout << "release F" << std::endl;
                        break;
                }
            }
        }

        float getX()
        {
            return xPos;
        }

        float getY()
        {
            return yPos;
        }

        bool isInvincible()
        {
            return invincible;
        }
    private:
        float xVel, yVel;
        float xAcc, yAcc;
        float ACC = 0.5;
        float speedCap = 5;
        float friction = 0.3;
        //float Boost = 1;

        bool invincible;
        int invincibilityTimer;
        int money;
        int currentWeapon;
        bool charging;
        int currentDirection;
        int currentAttackingDirection;

//        SDL_Rect playerCollisionBox;
};

void coin::update(Player& plyr)
{
    if (checkCollision(coinbox, plyr.getColBox()) && !collected)
    {
        plyr.collectCoin();
        collected = true;
    }
}

void heart::update(Player& plyr)
{
    if (checkCollision(heartCollisionBox, plyr.getColBox()) && !collected)
    {
        plyr.collectHeart();
        collected = true;
    }
}

void Slime::move(Player& player)
{
    if (displace == true)
    {
        stunDuration++;
//        std::cout << stunDuration << std::endl;
        if (stunDuration >= 60)
        {
            displace = false;
            stunDuration = 0;
        }

    }
    if (distanceBetweenRect(cbox, player.getColBox()) < 500 && displace == false)
    {
        float deltaX = abs(xPos - player.getColBox().x);
        float deltaY = abs(yPos - player.getColBox().y);
        if (xPos > player.getColBox().x) xVel = -VEL;
        else if (xPos < player.getColBox().x) xVel = VEL;
        if (yPos > player.getColBox().y) yVel = -VEL;
        else if (yPos < player.getColBox().y) yVel = VEL;
        if (xPos != player.getColBox().x || yPos != player.getColBox().y)
        {
            if (deltaX > deltaY)
            {
                if (xPos != player.getColBox().x)
                {
                    xPos += xVel;
                    if (checkCollision(cbox, player.getColBox()))
                        xPos -= xVel;
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel*(deltaY/deltaX);
                    if (checkCollision(cbox, player.getColBox()))
                        yPos -= yVel*(deltaY/deltaX);
                }
            }
            else if (deltaY > deltaX)
            {
                if (xPos != player.getColBox().x)
                {
                    xPos += xVel*(deltaX/deltaY);
                    if (checkCollision(cbox, player.getColBox()))
                        xPos -= xVel*(deltaX/deltaY);
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel;
                    if (checkCollision(cbox, player.getColBox()))
                        yPos -= yVel;
                }
            }
            cbox.x = xPos+6;
            cbox.y = yPos+30;
//            SDL_Rect rendercbox = {cbox.x - camera.x, cbox.y - camera.y, cbox.w, cbox.h};
//            SDL_SetRenderDrawColor(renderer, 128,0,0,0);
//            SDL_RenderFillRect(renderer, &rendercbox);
        }
    }
}

void Slime::attack(Player& player)
{
    SDL_Rect attackZone;
    SDL_Rect renderAttackZone;
    if(distanceBetweenRect(cbox, player.getColBox()) <= attackRange && !chargingAttack)
    {
        std::cout << "chargiong" << std::endl;
        displace = true;
        chargingAttack = true;
        attackZone = player.getColBox();
        renderAttackZone = {attackZone.x - camera.x, attackZone.y - camera.y, attackZone.w, attackZone.h};
    }
    if (chargingAttack == true)
    {
        chargingAmount++;
        SDL_SetRenderDrawColor(renderer, 255, 0, (int)(chargingAmount/200.0*255.0), 255);
        SDL_RenderFillRect(renderer, &renderAttackZone);
//        std::cout << (int)(chargingAmount/120.0*255.0) << std::endl;
    }
    if (chargingAmount >= 200)
    {
        std::cout << checkCollision(player.getColBox(), attackZone) << std::endl;
        SDL_Rect renderPlayerBox = {player.getColBox().x - camera.x, player.getColBox().y - camera.y, player.getColBox().w, player.getColBox().h};
        if (checkCollision(renderPlayerBox, renderAttackZone) == true)
        {
            std::cout << "atking plyr" <<std::endl;
            player.update(5);
        }
        chargingAmount = 0;
        chargingAttack = false;
    }
}

void Snake::move(Player& player)
{
    if (displace == true)
    {
        stunDuration++;
//        std::cout << stunDuration << std::endl;
        if (stunDuration >= 200)
        {
            displace = false;
            stunDuration = 0;
        }
    }
    if (distanceBetweenRect(cbox, player.getColBox()) < 500 && displace == false)
    {
        float deltaX = abs(xPos - player.getColBox().x);
        float deltaY = abs(yPos - player.getColBox().y);
        if (xPos > player.getColBox().x) xVel = -VEL;
        else if (xPos < player.getColBox().x) xVel = VEL;
        if (yPos > player.getColBox().y) yVel = -VEL;
        else if (yPos < player.getColBox().y) yVel = VEL;
        if (xPos != player.getColBox().x || yPos != player.getColBox().y)
        {
            if (deltaX > deltaY)
            {
                if (xPos != player.getColBox().x)
                {
                    xPos += xVel;
                    if (checkCollision(cbox, player.getColBox()))
                        xPos -= xVel;
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel*(deltaY/deltaX);
                    if (checkCollision(cbox, player.getColBox()))
                        yPos -= yVel*(deltaY/deltaX);
                }
            }
            else if (deltaY > deltaX)
            {
                if (xPos != player.getColBox().x)
                {
                    xPos += xVel*(deltaX/deltaY);
                    if (checkCollision(cbox, player.getColBox()))
                        xPos -= xVel*(deltaX/deltaY);
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel;
                    if (checkCollision(cbox, player.getColBox()))
                        yPos -= yVel;
                }
            }
            if (xVel > 0)
                currentDirection = right;
            else
                currentDirection = left;
            cbox.x = xPos;
            cbox.y = yPos;
//            SDL_Rect rendercbox = {cbox.x - camera.x, cbox.y - camera.y, cbox.w, cbox.h};
//            SDL_SetRenderDrawColor(renderer, 128,0,0,0);
//            SDL_RenderFillRect(renderer, &rendercbox);
        }
    }
}

void Boss::move(Player p)
{
    {
        float deltaX = abs(xPos - xTargetDst);
        float deltaY = abs(yPos - yTargetDst);
        if (xPos != xTargetDst || yPos != yTargetDst)
        {
            if (deltaX > deltaY)
            {
                if (abs(xPos - xTargetDst) < Vel)
                    xPos = xTargetDst;
                else if (xPos > xTargetDst)
                    xPos -= Vel;
                else if (xPos < xTargetDst)
                    xPos += Vel;
                if (abs(yPos - yTargetDst) < Vel)
                    yPos = yTargetDst;
                else if(yPos > yTargetDst)
                    yPos -= Vel*(deltaY/deltaX);
                else if(yPos < yTargetDst)
                    yPos += Vel*(deltaY/deltaX);
            }
            else if (deltaY > deltaX)
            {
                if (abs(xPos - xTargetDst) < Vel)
                    xPos = xTargetDst;
                else if (xPos > xTargetDst)
                    xPos -= Vel*(deltaX/deltaY);
                else if (xPos < xTargetDst)
                    xPos += Vel*(deltaX/deltaY);
                if (abs(yPos - yTargetDst) < Vel)
                    yPos = yTargetDst;
                else if(yPos > yTargetDst)
                    yPos -= Vel;
                else if(yPos < yTargetDst)
                    yPos += Vel;
            }
            if (xTargetDst < 0)
                xTargetDst =  0;
            else if (xTargetDst + cbox.w > LEVEL_WIDTH)
                xTargetDst = LEVEL_WIDTH - cbox.w;
            if (yTargetDst < 0)
                yTargetDst = 0;
            else if (yTargetDst + cbox.h > LEVEL_HEIGHT)
                yTargetDst = 0;
//            if (checkCollision(cbox, p.getColBox()) && xTargetDst < xPos)
//                xTargetDst = xPos+6;
//            else if (checkCollision(cbox, p.getColBox()) && xTargetDst > xPos)
//                xTargetDst = xPos-6;
//            if (abs(xPos - xTargetDst) < 6)
//                xPos = xTargetDst;
            cbox.x = xPos;
            cbox.y = yPos;
//            std::cout << "(Bossx,Boosy) = (" << xPos << "," << yPos << ")" << std::endl;
        }
    }
}

class projectile : public Object
{
    public:
        projectile()
        {
            xVel = 0;
            yVel = 0;
            txtr = nullptr;
            colBox = { xPos, yPos, 10, 10};//temporaly
            projectiles.push_back(this);
        }

        void render()
        {
            txtr->render(xPos - camera.x, yPos - camera.y);
        }

        int getAngle()
        {
            return angle;
        }

        SDL_Rect getColBox()
        {
            return colBox;
        }
    private:
        float xVel, yVel;
        int angle;
        SDL_Rect colBox;

};

int main(int argc, char* argv[])
{
    if (init("help") == false)
        std::cout << "Failed to initialized" << std::endl;
    else
    {
        if (loadAsset() == false)
            std::cout << "Failed to load asset" << std::endl;
        else
        {
            srand(time(0));

//            for (int i = 0; i < 10; i++)
//                std::cout << rand() % 2;

            bool exit = false;
            SDL_Event e;
//            coin* coins[10];
////            Boss boss(200, 200, 200);

            Player* p1 = new Player(1000, 200, 100);
//            Player* p2 = new Player(800, 150, 100);
//            Slime* slime = new Slime(600, 300, 5);
//            Slime* slime2 = new Slime(300, 300, 20);
//            Snake* sn = new Snake(800, 800, 20);

//            p1.getTexture();
//
//            p2.getTexture();
//            SDL_Rect testrect = {400,400,50,20};
            int enemyspawn = 0;
            //main loop
            while (!exit)
            {

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(renderer);

				camera.x = ( p1->getX()+ p1->getColBox().w / 2 ) - SCREEN_WIDTH / 2;
				camera.y = ( p1->getY() + p1->getColBox().h / 2 ) - SCREEN_HEIGHT / 2;
//				for(int i = 0; i < 1; i++)
//                    p1.collectCoin();

				//Keep the camera in bounds
				if( camera.x < 0 )
				{
					camera.x = 0;
				}
				if( camera.y < 0 )
				{
					camera.y = 0;
				}
				if( camera.x > LEVEL_WIDTH - camera.w )
				{
					camera.x = LEVEL_WIDTH - camera.w;
				}
				if( camera.y > LEVEL_HEIGHT - camera.h )
				{
					camera.y = LEVEL_HEIGHT - camera.h;
				}
				background.render(0, 0, &camera);
                //event loop
                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        exit = true;
                    }
                    p1->handleInput(e);
//                    boss.handleInput(e);
//                    p2.handleInput(e);
                }
                spawnEnemy(enemyspawn);

//				SDL_Rect testrect2 = {testrect.x - camera.x, testrect.y - camera.y, testrect.w, testrect.h};
//				SDL_SetRenderDrawColor(renderer, 255, 255, 0, 0);
//				SDL_RenderFillRect(renderer, &testrect2);

				for (coin* c : coins)
                {
                    c->render();
                    c->update(*p1);
                }
                for (Slime* e : slimes)
                {
                    e->attack(*p1);
                    e->move(*p1);
                    e->render();

                }
                for (Snake* s : snakes)
                {
                    s->move(*p1);
                    s->render();
                }

//                sn->move(*p1);
//                sn->render();
//				slime->move(*p1);
//				slime2->move(*p1);
//				slime->render();
//				slime2->render();
				p1->move(objects, slimes);
				p1->render();
//				p2->render();

				checkAlive();

//				std::cout << checkCollision(p1.getColBox(), boss.getColBox());

//                p2.move();

//                std::cout << "p1: " << p1.getX() << "," << p1.getY() << std::endl;
//                std::cout << "p2: " << p2.getX() << "," << p2.getY() << std::endl;
//				std::cout << distanceBetween(p1, p2) << std::endl;

				SDL_RenderPresent(renderer);



            }
        }
    }
    quit();
    return 0;
}

bool init(std::string path)
{
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
		success = false;
	else
    {
		window = SDL_CreateWindow( path.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if(window == NULL)
			success = false;
		else
		{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if(renderer == NULL)
				return false;
			else
			{
				//SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init( imgFlags ) & imgFlags))
					success = false;
                if( Mix_OpenAudio( 24100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
			}
		}
    }
    return success;
}

bool loadAsset()
{
    bool success = true;
    /*face = IMG_Load("face.png");
    if (face == NULL)
        std::cout << "Failed to load texture. SDL Error: " << SDL_GetError() << std::endl;
    else
    {
        SDL_SetColorKey(face, SDL_TRUE, SDL_MapRGB(face->format, 0x00, 0xFF, 0xFF));
        texture = SDL_CreateTextureFromSurface(renderer, face);
    }*/
    //load sfx
    footstep = Mix_LoadWAV("minecraft-walking-By-Tuna2.mp3");
    if (footstep == NULL)
    {
        std::cout << "faield to load sound" << std::endl;
    }
    if (background.loadFile("testbg2560_1440.png") == false)
    {
        std::cout << "failed to load bg" << std::endl;
        success = false;
    }
    if (snake.loadFile("snake.png") == false)
    {
        std::cout << "Failed to load coins" << std::endl;
        success = false;
    }
    else
    {
        for (int i = 0; i < SNAKE_SPRITE_COUNT; i++)
            snakeSprite[i] = {44*i, 0, 44, 30};
    }
    if (Coin.loadFile("coin2.png") == false)
    {
        std::cout << "Failed to load coins" << std::endl;
        success = false;
    }
    else
    {
        for (int i = 0; i < COIN_SPRITE_COUNT; i++)
            coinSprite[i] = {40*i, 0, 40, 40};
    }
//    if(dumb.loadFile("dummy.png") == false)
//    {
//        std::cout << "Failed to loadFile" << std::endl;
//        success = false;
//    }
    if (slime.loadFile("slime.png") == false)
    {
        std::cout << "Failed to loadFile" << std::endl;
        success = false;
    }
    else
    {
        for (int i = 0; i < SLIME_SPRITE_COUNT; i++)
            slimeSprite[i] = {54*i, 0, 54, 66};
    }
    if (Heart.loadFile("heart.png") == false)
    {
        std::cout << "Failed to loadFile" << std::endl;
        success = false;
    }
    else
    {
        for (int i = 0; i < HEART_SPRITE_COUNT; i++)
            heartSprite[i] = {40*i, 0, 40, 30};
    }
    if (link.loadFile("link_sprite_new4.png") == false)
    {
        std::cout << "Failed to loadFile" << std::endl;
        success = false;
    }
    else
    {
        //default is spriteWalkdown[0], walk is 1 to 10
        spriteWalkup[0] = {0, 52*2, 48, 52};
        for (int i = 0; i < 10; i++)
        {
            spriteWalkup[i+1].x = i*48;
            spriteWalkup[i+1].y = 52*6;
            spriteWalkup[i+1].w = 48;
            spriteWalkup[i+1].h = 52;
        }

        spriteWalkleft[0] = {0, 52, 48, 52};
        for (int i = 0; i < 10; i++)
        {
            spriteWalkleft[i+1].x = i*48;
            spriteWalkleft[i+1].y = 52*5;
            spriteWalkleft[i+1].w = 48;
            spriteWalkleft[i+1].h = 52;
        }

        spriteWalkdown[0] = {0, 0, 48, 52};
        for (int i = 0; i < 10; i++)
        {
            spriteWalkdown[i+1].x = i*24*2;
            spriteWalkdown[i+1].y = 26*4*2;
            spriteWalkdown[i+1].w = 24*2;
            spriteWalkdown[i+1].h = 26*2;
        }

        spriteWalkright[0] = {0, 52*3, 48, 52};
        for (int i = 0; i < 10; i++)
        {
            spriteWalkright[i+1].x = i*48;
            spriteWalkright[i+1].y = 52*7;
            spriteWalkright[i+1].w = 48;
            spriteWalkright[i+1].h = 52;
        }

    }
    return success;
}

void quit()
{
    Mix_FreeChunk(footstep);
    footstep = NULL;
    link.free();
    snake.free();
    Coin.free();
    Heart.free();
    slime.free();
    background.free();
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_Quit();
}

void spawnEnemy(int& time)
{
    int SPAWNTIME = 60;
    if(time < SPAWNTIME)
    {
        time++;
//        std::cout << time <<std::endl;
    }
    else if (time >= SPAWNTIME)
    {
        float x = rand() % LEVEL_WIDTH;
        float y = rand() % LEVEL_HEIGHT;
        int enemy = rand() % ENEMY_TYPES;
        if (enemy == SLIME)
        {
            Slime* sl = new Slime(x, y, 20);
            slimes.push_back(sl);
        }
        else if (enemy == SNAKE)
        {
            Snake* sn = new Snake(x, y, 20);
            snakes.push_back(sn);
        }
        time = 0;
    }

}

void checkAlive()
{
//    for (auto obj : objects)
//    {
//        if (obj->getHealth() <= 0)
//        {
//            std::cout << "obj = {" << obj->getColBox().x << "," << obj->getColBox().y << "," << obj->getColBox().h << "," << obj->getColBox().h << "} " << "dead" << std::endl;
//            coin* c = new coin(obj->getColBox().x, obj->getColBox().y);
////            std::cout << "spowaing new coooin" <<std::endl;
//            coins.push_back(c);
//            obj->playDeathAnim();
////            obj->setPos(-100, -100);
//            objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
//            delete obj;
//        }
//    }
    for(Slime* sl : slimes)
    {
        if (sl->getHealth() <= 0)
        {
            coin* c = new coin(sl->getColBox().x, sl->getColBox().y);
            coins.push_back(c);
            slimes.erase(std::remove(slimes.begin(), slimes.end(), sl), slimes.end());
            delete sl;
        }
    }
    for(coin* c : coins)
    {
        if (c->gotCollected() == true)
        {
            coins.erase(std::remove(coins.begin(), coins.end(), c), coins.end());
            delete c;
        }
    }
    for(Snake* snake : snakes)
    {
        if (snake->getHealth() <= 0)
        {
            coin* c = new coin(snake->getColBox().x, snake->getColBox().y);
            coins.push_back(c);
            snakes.erase(std::remove(snakes.begin(), snakes.end(), snake), snakes.end());
            delete snake;
        }
    }
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
        return false;
    if( topA >= bottomB )
        return false;
    if( rightA <= leftB )
        return false;
    if( leftA >= rightB )
        return false;
    //If none of the sides from A are outside B
    return true;
}

bool checkCollisionObj (SDL_Rect a, std::vector<Object*> objs)
{
    for (auto obj : objs)
    {
//        std::cout << objs.capacity() << std::endl;
//        if (obj->getColBox())
//        std::cout << "obj = {" << obj->getColBox().x << "," << obj->getColBox().y << "," << obj->getColBox().h << "," << obj->getColBox().h << "}" << std::endl;
        if (obj->getColBox().x != a.x || obj->getColBox().y != a.y)
        {
//            std::cout << "obj = {" << obj->getColBox().x << "," << obj->getColBox().y << "," << obj->getColBox().h << "," << obj->getColBox().h << "}" << std::endl;
            return checkCollision(a, obj->getColBox());
        }
    }
    return false;
}

bool checkCollisionEnemy(SDL_Rect a, std::vector<Slime*> enemies)
{
    for (auto enemy : enemies)
        return checkCollision(a, enemy->getColBox());
    return false;
}

float angleBetween(Object& a, Object& b)
{
    float ax = (float)a.getColBox().x;
    float ay = (float)a.getColBox().y;
    float bx = (float)b.getColBox().x;
    float by = (float)b.getColBox().y;
    float deltaX = (ax - bx);
    float deltaY = -(ay - by);//SDL cordinate system has y invert compare to descartes
    float angle = atan(deltaY/deltaX)/3.14159*180;
    if (ax > bx && ay > by)
        angle = angle+180;
    else if (ax > bx && ay < by)
        angle = angle-180;
//    std::cout << "angle = " << angle << std::endl;
////    SDL_Delay(100);
    return angle;
}

float angleBetweenRect(SDL_Rect a, SDL_Rect b)
{
    float ax = (float)a.x;
    float ay = (float)a.y;
    float bx = (float)b.x;
    float by = (float)b.y;
    float deltaX = (ax - bx);
    float deltaY = -(ay - by);
    float angle = atan(deltaY/deltaX)/3.14159*180;
    if (ax > bx && ay > by)
        angle = angle+180;
    else if (ax > bx && ay < by)
        angle = angle-180;
    return angle;
}

float distanceBetween(Object& a, Object& b)
{
    float ax = (float)a.getColBox().x;
//    std::cout << "ax: " << ax << " ";
    float ay = (float)a.getColBox().y;
    float bx = (float)b.getColBox().x;
    float by = (float)b.getColBox().y;
    return sqrt((ax - bx)*(ax - bx) + (ay-by)*(ay-by));
}

float distanceBetweenRect(SDL_Rect a, SDL_Rect b)
{
    float ax = (float)a.x;
    float ay = (float)a.y;
    float bx = (float)b.x;
    float by = (float)b.y;
    return sqrt((ax - bx)*(ax - bx) + (ay-by)*(ay-by));
}
