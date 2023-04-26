#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

int LEVEL_WIDTH = 2560;
int LEVEL_HEIGHT = 1440;

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int TOTAL_TILES = 3600;
const int TOTAL_TILE_SPRITES = 176;

SDL_Rect TileClips[ TOTAL_TILE_SPRITES ];

enum TileNumber
{
    topLeftGrassPatch,
    topRightGrassPatch,
    bottomLeftGrassPatch,
    bottomRightGrassPatch,
    blackHole,
    top4GrassPatch,
    full8GrassPatch,
    full82Flower,
    full81Flower,
    BrownStud,
    aaa,
    water1, water2, water3, water4, water5,

    basicGrass,
    grassPatch1, grassPatch2, grassPatch3, bush, bottom4GrassPatch, starthing, seed, melon, brownThing, rock, w6,w7,w8,w9,w10,

    pathVariation1,pathVariation2, topRoad,sdr,btl,btr,sbl,leftRoad,rightRoad,wierd,wierdWithMold,w11,w12,w13,w14,w15,

    pathVariation3,basicPath,botRoad,str,bbl,bbr,stl, treeStud,twofence,sign,box,onefence,you,board,w16,w17,

    b1,b2,b3,b4,b5,b6,h1,h2,h3,h4,h5,h6,lightGrassPatch,plant,bombplant,bbb,

    b7,b8,b9,b10,b11,b12,h7,h8,h9,h10,h11,h12,t1,t2,t3,t4,

    b13,b14,b15,b16,b17,b18,h13,h14,h15,h16,h17,h18,t5,t6,t7,t8,

    b19,b20,b21,b22,b23,b24,h19,h20,h21,h22,h23,h24,t9,t10,t11,t12,

    b25,b26,b27,b28,b29,b30,h25,h26,h27,h28,h29,h30,t13,t14,t15,t16,

    b31,b32,b33,b34,b35,b36,h31,h32,h33,h34,h35,h36,t17,t18,t19,t20,

    b37,b38,b39,b40,b41,b42,ccc,ddd,pathIntoHomeLeft,pathIntoHomeRight,waterrock1,waterrock2,waterrock3,waterrock4,waterrock5,waterrock6
};

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

const int WALL_OBJECT_COUNT = 23;
SDL_Rect colliable[23] = { {92,33,193,159},
                           {351,0,190,158},
                           {672,194,16,27},
                           {737,0,192,94},
                           {1024,0,182,96},
                           {1375,0,129,126},
                           {1569,32,191,160},
                           {2431,446,129,163},{1375,289,192,187-32},{1216,258,137,156},{833,449,256,64},
                           {768,513,256,95},{0,322,96,158},{94,388,131,156},{159,546,130,156},{128,704,128,156},{31,1122,195,156},{288,1057,193,156},{2049, 800,511,226},{1665,801,192,63},{1504,864,352,193},{1275,831,197,194}, {957,899,324,156} };

int SLIME_SPRITE_COUNT = 11;
SDL_Rect slimeSprite[11];

int SNAKE_SPRITE_COUNT = 2;
SDL_Rect snakeSprite[2];

int HEART_SPRITE_COUNT = 5;
SDL_Rect heartSprite[5];

int COIN_SPRITE_COUNT = 6;
SDL_Rect coinSprite[6];

SDL_Rect attacksprite;
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
class heart;
//class Tile;

//vector arrays
std::vector<Object*> objects;
std::vector<projectile*> projectiles;

//function prototypes
bool init(std::string path);
bool loadAsset(/*Tile* tiles[]*/);
void quit(/*Tile* tiles[]*/);
bool checkCollision(SDL_Rect a, SDL_Rect b);
bool checkCollisionObj (SDL_Rect a, std::vector<Object*> objs);
bool checkCollisionEnemy (SDL_Rect a, std::vector<Slime*> enemies);
float angleBetween(Object& a, Object& b);
float angleBetweenRect(SDL_Rect a, SDL_Rect b);
float distanceBetween(Object& a, Object& b);
float distanceBetweenRect(SDL_Rect a, SDL_Rect b);
void checkAlive();
void spawnEnemy(int& time, Player& p1);
bool checkWallCollision (SDL_Rect source);
//bool touchesWall( SDL_Rect box, Tile* tiles[] );
////bool setTiles( Tile *tiles[] );

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
Texture TileTexture;
Texture link;
Texture background;
Texture arrow;
Texture slime;
Texture Coin;
Texture Heart;
Texture snake;
Texture HUD;

bool checkWallCollision (SDL_Rect source)
{
    for (int i = 0; i < WALL_OBJECT_COUNT; i++)
    {
        if (checkCollision(source, colliable[i]))
            return true;
    }
    return false;
}

//class Tile
//{
//    public:
//		//Initializes position and type
//		Tile( int x, int y, int tileType )
//		{
//            //Get the offsets
//            mBox.x = x;
//            mBox.y = y;
//
//            //Set the collision box
//            mBox.w = TILE_WIDTH;
//            mBox.h = TILE_HEIGHT;
//
//            //Get the tile type
//            mType = tileType;
//        }
//
//		//Shows the tile
//		void render( SDL_Rect& camera )
//		{
//            //If the tile is on screen
//            if( checkCollision( camera, mBox ) )
//            {
//                //Show the tile
//                TileTexture.render( mBox.x - camera.x, mBox.y - camera.y, &TileClips[ mType ] );
//            }
//        }
//
//		//Get the tile type
//		int getType()
//		{
//		    return mType;
//		}
//
//		//Get the collision box
//		SDL_Rect getBox()
//		{
//		    return mBox;
//		}
//
//    private:
//		//The attributes of the tile
//		SDL_Rect mBox;
//
//		//The tile type
//		int mType;
//};

class Object
{
public:
    Object(int x = 0, int y = 0, int inithealth = 1)
    {
        xPos = x;
        yPos = y;
        txtr = NULL;
        health = inithealth;
//        objects.push_back(this);
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

    void updateStatus(int damage)
    {
        health -= damage;

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

protected:
    float xPos, yPos;
    int width, height;
    int health;
    SDL_Rect cbox;
    Texture* txtr;
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
    int VEL = 1;
    Snake(int x, int y, int health) : Object(x, y, health)
    {
        movementtimer = 0;
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

    void attack(Player& player);

    void updateStatus(int damage)
    {
        health -= damage;
    }

    float knockbackamount = 50;
    void knockback(float angle)
    {
        if (xVel != 0 || yVel != 0)
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
    int movementtimer;
};

std::vector<coin*> coins;

std::vector<heart*> heartcontainers;

class heart
{
public:
    heart(int x, int y)
    {
        heartTexture = &Heart;
        collected = false;
        heartCollisionBox = {x, y, 40, 30};
    }
    int frame = 0;
    void render()
    {
        SDL_Rect* CurrentClip;
        CurrentClip = &heartSprite[frame/6];
        heartTexture->render(heartCollisionBox.x - camera.x, heartCollisionBox.y - camera.y, CurrentClip);
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

        void setCamera( SDL_Rect& camera )
        {
            //Center the camera over the dot
            camera.x = ( cbox.x + playerSpriteWidth / 2 ) - SCREEN_WIDTH / 2;
            camera.y = ( cbox.y + playerSpriteHeight / 2 ) - SCREEN_HEIGHT / 2;

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
            if (xPos < 0 || (xPos + playerSpriteWidth) > LEVEL_WIDTH || checkCollisionObj(cbox, objs) || checkCollisionEnemy(cbox, slimes) || checkWallCollision(cbox))
            {
//                std::cout << checkCollisionObj(cbox, objs);
                xPos -= xVel;
            }
            yPos += yVel;
            cbox.y = yPos+6;
            if (yPos < 0 || (yPos + playerSpriteHeight)> LEVEL_HEIGHT || checkCollisionObj(cbox, objs) || checkWallCollision(cbox))
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
            renderHUD();
            renderHealth();
            frame++;
            if (frame / 4 >= 11)
            {
                frame = 4;
            }
        }

        void renderHealth()
        {
            SDL_Rect healthbar = {1008+1,20,(int)((float)health*221.0/100.0),27};
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
            SDL_RenderFillRect(renderer, &healthbar);
//            std::cout << (int)((float)health*221.0/100.0) << std::endl;

//            SDL_Rect healthBar = {xPos - camera.x, yPos - camera.y, health/2, 10};
//            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
//            SDL_RenderFillRect(renderer, &healthBar);

//            for (int i = 0; i < 6; i++)
//            {
//                SDL_Rect healthBaroutline = {20-i, 20-i, 200+2*i, 30+2*i};
//                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
//                SDL_RenderDrawRect(renderer, &healthBaroutline);
//            }
        }
        void renderHUD()
        {
            HUD.render(956,0);
        }

        void collectCoin()
        {
            money++;
            std::cout << "couin: " << money << std::endl;
        }

        void collectHeart()
        {
            if (health < 100)
            {
//                std::cout << "+5" <<std::endl;
                health += 5;
            }
        }

        void displayCoinCount()
        {

        }

        void update(int damage = 0)
        {
//            std::cout <<"about to do somedamgeag to stupid" << std::endl;
            if (damage > 0 && invincible == false)
            {
//                std::cout << "took damege" << std::endl;
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
        SDL_Rect attackBox, renderattackBox;
        int atkScale = 1;
        int angle;
        if (currentDirection == up)
        {
            attackBox = {xPos+5,yPos-cbox.w/2 + 25 -atkScale*cbox.h/2, cbox.w, cbox.h*atkScale};
//            renderattackBox = {xPos-camera.x + 5, yPos - cbox.w/2 + 25 - camera.y - atkScale*cbox.h/2, cbox.w, cbox.h * atkScale};
            angle= 0 ;
        }
        else if (currentDirection == down)
        {
            attackBox = {xPos+5,yPos+cbox.w , cbox.w, cbox.h*atkScale};
//            renderattackBox = {xPos-camera.x + 5, yPos + cbox.w - camera.y - 10, cbox.w, cbox.h * atkScale};
            angle = 180;
        }
        else if (currentDirection == right)
        {
            attackBox = {xPos+cbox.w,yPos, cbox.w*atkScale, cbox.h};
            angle = 90;
        }
        else if (currentDirection == left)
        {
            attackBox = {xPos-cbox.w +20-atkScale*cbox.w/2,yPos, cbox.w*atkScale, cbox.h};

            angle = -90;
        }
//        SDL_Rect tmp = {attackBox.x - camera.x, attackBox.y - camera.y, 32,32};
        renderattackBox = {attackBox.x-camera.x, attackBox.y - camera.y, attackBox.w, attackBox.h};

        for (int i = 0; i < 1000; i++)
            txtr->render(renderattackBox.x, renderattackBox.y, &attacksprite, angle);
//        SDL_RenderFillRect(renderer, &tmp);

//        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x0F, 0xFF);
//        SDL_RenderFillRect(renderer, &attackBox);

        for (Snake* sn :snakes)
        {
            if (checkCollision(attackBox, sn->getColBox()))
            {
                SDL_Rect tmpcbox = {cbox.x + cbox.w/2, cbox.y + cbox.h/2, 0, 0};
                SDL_Rect tmpobjbox = {sn->getColBox().x + sn->getColBox().w/2, sn->getColBox().y + sn->getColBox().h/2};
                sn->updateStatus(5);
                sn->knockback(angleBetweenRect(tmpcbox, tmpobjbox));
            }
        }

        for (Slime* sl : slimes)
        {
            if (checkCollision(attackBox, sl->getColBox()))
            {
                SDL_Rect tmpcbox = {cbox.x + cbox.w/2, cbox.y + cbox.h/2, 0, 0};
                SDL_Rect tmpobjbox = {sl->getColBox().x + sl->getColBox().w/2, sl->getColBox().y + sl->getColBox().h/2};
                sl->updateStatus(5);
                sl->knockback(angleBetweenRect(tmpcbox, tmpobjbox));
            }
        }

        /*for (auto obj : objs)
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
            }*/
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
//                        std::cout << "pressing v" << std::endl;
                        break;
//                    case SDLK_f:
//                        ACC = 1;
////                        std::cout << "pressing F" << std::endl;
//                        break;
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
//                        std::cout << "releasing v" << std::endl;
                        break;
//                    case SDLK_f:
//                        ACC = 0.5;
////                        std::cout << "release F" << std::endl;
//                        break;
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
        std::cout << "collero" << std::endl;
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
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        xPos -= xVel;
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel*(deltaY/deltaX);
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        yPos -= yVel*(deltaY/deltaX);
                }
            }
            else if (deltaY > deltaX)
            {
                if (xPos != player.getColBox().x)
                {
                    xPos += xVel*(deltaX/deltaY);
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        xPos -= xVel*(deltaX/deltaY);
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel;
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        yPos -= yVel;
                }
            }
            cbox.x = xPos;
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

void Snake::attack(Player& player)
{
    if (checkCollision(cbox, player.getColBox()))
    {
        std::cout << "touching paye";
        player.update(5);
//        player->render()
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
    if (distanceBetweenRect(cbox, player.getColBox()) < 300 && displace == false)
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
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        xPos -= xVel;
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel*(deltaY/deltaX);
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        yPos -= yVel*(deltaY/deltaX);
                }
            }
            else if (deltaY > deltaX)
            {
                if (xPos != player.getColBox().x)
                {
                    xPos += xVel*(deltaX/deltaY);
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        xPos -= xVel*(deltaX/deltaY);
                }
                if(yPos != player.getColBox().y)
                {
                    yPos += yVel;
                    if (checkCollision(cbox, player.getColBox()) || checkWallCollision(cbox))
                        yPos -= yVel;
                }
            }
//            if (xVel == 0 && yVel == 0)
//            {
//                movementtimer++;
//                if (movementtimer >60)
//                {
//                    xPos = xPos + (rand() % 200- 100);
//                    yPos = yPos + (rand() % 200 -100);
//                    movementtimer = 0;
//                }
//            }
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

class projectile : public Object
{
    public:
        projectile()
        {
            xVel = 0;
            yVel = 0;
            txtr = nullptr;
            colBox = { xPos, yPos, 10, 10};//temporaly
//            projectiles.push_back(this);
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
        //The level tiles
//        Tile* tileSet[ TOTAL_TILES ];

        if (loadAsset() == false)
            std::cout << "Failed to load asset" << std::endl;
        else
        {
            std::cout << waterrock6;
            srand(time(0));

//            for (int i = 0; i < 10; i++)
//                std::cout << rand() % 2;

            bool exit = false;
            SDL_Event e;

            Player* p1 = new Player(1000, 200, 100);

//            SDL_Rect testrect = {400,400,50,20};
            int enemyspawn = 0;
            //main loop
            while (!exit)
            {

                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(renderer);

                p1->setCamera(camera);
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
                spawnEnemy(enemyspawn, *p1);

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
                    s->attack(*p1);
                    s->render();
                }
                for (heart* hr : heartcontainers)
                {
                    hr->update(*p1);
                    hr->render();
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


//                p2.move();

//                std::cout << "p1: " << p1.getX() << "," << p1.getY() << std::endl;
//                std::cout << "p2: " << p2.getX() << "," << p2.getY() << std::endl;
//				std::cout << distanceBetween(p1, p2) << std::endl;

				SDL_RenderPresent(renderer);



            }
        }
        quit();
    }
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
    //load sfx
    footstep = Mix_LoadWAV("minecraft-walking-By-Tuna2.mp3");
    if (footstep == NULL)
    {
        std::cout << "faield to load sound" << std::endl;
    }
    //Load tile texture
	if( !TileTexture.loadFile( "tileset.png" ) )
	{
		std::cout << "Failed to load tile set texture!\n";
		success = false;
	}
    //Load tile map
//	if( !setTiles( tiles ) )
//	{
//		std::cout << "Failed to load tile set!\n";
//		success = false;
//	}
    if( !HUD.loadFile( "HUD.png" ) )
	{
		std::cout << "Failed to load texture!\n";
		success = false;
	}
    if (background.loadFile("gamemap.png") == false)
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
        attacksprite = {48,0,32,32};
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
    //Deallocate tiles
//	for( int i = 0; i < TOTAL_TILES; ++i )
//	{
//		 if( tiles[ i ] != NULL )
//		 {
//			delete tiles[ i ];
//			tiles[ i ] = NULL;
//		 }
//	}
    Mix_FreeChunk(footstep);
    footstep = NULL;
//    TileTexture.free();
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
    IMG_Quit();
}

void spawnEnemy(int& time,Player& p1)
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
        SDL_Rect enemybox = {x,y,1,1};
        if (enemy == SLIME && !checkWallCollision(enemybox) && distanceBetweenRect(p1.getColBox(),enemybox) > 500)//not collisde
        {
            Slime* sl = new Slime(x, y, 20);
            slimes.push_back(sl);
            objects.push_back(sl);
        }
        else if (enemy == SNAKE && !checkWallCollision(enemybox)  && distanceBetweenRect(p1.getColBox(),enemybox) > 500)
        {
            Snake* sn = new Snake(x, y, 20);
            snakes.push_back(sn);
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
            if(rand() % 2 == 0)
            {
                coin* c = new coin(sl->getColBox().x, sl->getColBox().y);
                coins.push_back(c);
            }
            else if (rand()% 2== 1)
            {
                heart* hr = new heart(sl->getColBox().x,sl->getColBox().y);
                heartcontainers.push_back(hr);
            }

            objects.erase(std::remove(objects.begin(), objects.end(), sl), objects.end());
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
    for(heart* hr : heartcontainers)
    {
        if (hr->gotCollected() == true)
        {
            heartcontainers.erase(std::remove(heartcontainers.begin(), heartcontainers.end(), hr), heartcontainers.end());
            delete hr;
        }
    }
    for(Snake* snake : snakes)
    {
        if (snake->getHealth() <= 0)
        {
            if(rand() % 2 == 0)
            {
                coin* c = new coin(snake->getColBox().x, snake->getColBox().y);
                coins.push_back(c);
            }
            else if (rand()% 2== 1)
            {
                heart* hr = new heart(snake->getColBox().x,snake->getColBox().y);
                heartcontainers.push_back(hr);
            }
            objects.erase(std::remove(objects.begin(), objects.end(), snake), objects.end());
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

//bool setTiles( Tile* tiles[] )
//{
//	//Success flag
//	bool tilesLoaded = true;
//
//    //The tile offsets
//    int x = 0, y = 0;
//
//    //Open the map
//    std::ifstream map( "output.txt" );
//
//    //If the map couldn't be loaded
//    if( map.fail() )
//    {
//		printf( "Unable to load map file!\n" );
//		tilesLoaded = false;
//    }
//	else
//	{
//		//Initialize the tiles
//		for( int i = 0; i < TOTAL_TILES; ++i )
//		{
//			//Determines what kind of tile will be made
//			int tileType = -1;
//
//			//Read tile from map file
//			map >> tileType;
//
//			//If the was a problem in reading the map
//			if( map.fail() )
//			{
//				//Stop loading map
//				printf( "Error loading map: Unexpected end of file!\n" );
//				tilesLoaded = false;
//				break;
//			}
//
//			//If the number is a valid tile number
//			if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
//			{
//				tiles[ i ] = new Tile( x, y, tileType );
//			}
//			//If we don't recognize the tile type
//			else
//			{
//				//Stop loading map
//				printf( "Error loading map: Invalid tile type at %d!\n", i );
//				tilesLoaded = false;
//				break;
//			}
//
//			//Move to next tile spot
//			x += TILE_WIDTH;
//
//			//If we've gone too far
//			if( x >= LEVEL_WIDTH )
//			{
//				//Move back
//				x = 0;
//
//				//Move to the next row
//				y += TILE_HEIGHT;
//			}
//		}
//
//		//Clip the sprite sheet
//		if( tilesLoaded )
//		{
//		    int c = 0;
//		    for (int i = 0; i < 11; i++)
//            {
//                for (int j = 0; j < 16; i++)
//                {
//                    TileClips[c] = {j*TILE_WIDTH, i*TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT};
//                    c++;
//                }
//            }
////			gTileClips[ TILE_RED ].x = 0;
////			gTileClips[ TILE_RED ].y = 0;
////			gTileClips[ TILE_RED ].w = TILE_WIDTH;
////			gTileClips[ TILE_RED ].h = TILE_HEIGHT;
////
////			gTileClips[ TILE_GREEN ].x = 0;
////			gTileClips[ TILE_GREEN ].y = 80;
////			gTileClips[ TILE_GREEN ].w = TILE_WIDTH;
////			gTileClips[ TILE_GREEN ].h = TILE_HEIGHT;
//		}
//	}
//
//    //Close the file
//    map.close();
//
//    //If the map was loaded fine
//    return tilesLoaded;
//}

//bool touchesWall( SDL_Rect box, Tile* tiles[] )
//{
//    //Go through the tiles
//    for( int i = 0; i < TOTAL_TILES; ++i )
//    {
//        //If the tile is a wall type tile
//        if( ( tiles[i]->getType() == melon )
//         || ( tiles[i]->getType() == rock )
//         || ( ( tiles[i]->getType() >= treeStud ) && ( tiles[i]->getType() <= board ) )
//         || ( ( tiles[i]->getType() >= h1 ) && ( tiles[i]->getType() <= h6 ) )
//         || ( ( tiles[i]->getType() >= h7 ) && ( tiles[i]->getType() <= t4 ) )
//         || ( ( tiles[i]->getType() >= h13 ) && ( tiles[i]->getType() <= t8 ) )
//         || ( ( tiles[i]->getType() >=  h19) && ( tiles[i]->getType() <= h12 ) )
//         || ( ( tiles[i]->getType() >= h25 ) && ( tiles[i]->getType() <= h30 ) )
//         || ( ( tiles[i]->getType() >= h31 ) && ( tiles[i]->getType() <= h36 ) )
//         || ( tiles[i]->getType() == t14 )
//         || ( tiles[i]->getType() == t15 )
//         || ( tiles[i]->getType() == t18 )
//         || ( tiles[i]->getType() == t19 )
//          )
//        {
//            //If the collision box touches the wall tile
//            if( checkCollision( box, tiles[ i ]->getBox() ) )
//            {
//                return true;
//            }
//        }
//    }
//
//    //If no wall tiles were touched
//    return false;
//}

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
    float angle;
    float deltaX = (ax - bx);
    float deltaY = -(ay - by);//SDL cordinate system has y invert compare to descartes
    if (deltaX > 5 || deltaX < 5)
    {
        angle = atan(deltaY/deltaX)/3.14159*180;
        if (ax > bx && ay > by)
            angle = angle+180;
        else if (ax > bx && ay < by)
            angle = angle-180;
    }

    if (deltaX <5 && deltaX >-5)
    {
        if (deltaY > 0)
            angle = -90;
        else
            angle = 90;
    }

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
    float angle;
    float deltaX = (ax - bx);
    float deltaY = -(ay - by);
    if (deltaX != 0)
    {
        angle = atan(deltaY/deltaX)/3.14159*180;
        if (ax > bx && ay > by)
            angle = angle+180;
        else if (ax > bx && ay < by)
            angle = angle-180;
    }

    if (deltaX == 0)
    {
        if (deltaY > 0)
            angle = -90;
        else
            angle = 90;
    }
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
