#include "stdwx.h"
//#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "fitrange.h"
#include "ball.h"
#include "character.h"

//blocks grid size
#define GRID_SIZE_WIDTH     50
#define GRID_SIZE_HEIGHT    15

//ball speed
#define FACING_MULT         1

Ball::Ball(DrawEngine* de, Level* lvl, int index, float x, float y, 
        int numlives, char left_key, char right_key) : 
        Sprite(de, lvl, index, x, y, numlives)
{
    //set our random direction
    srand(time(NULL));
    facingDir.y = -(0.5 * ((double) rand() / (RAND_MAX)) + 0.5);
    facingDir.x = (1 - 2 * ((rand() % 100)/100.0));
//    facingDir.x = 0;
//    facingDir.y = -1;
//    
    //normalize and mult based off of our speed facing mult
    facingDir = normalize(facingDir);
    facingDir.x *= FACING_MULT;
    facingDir.y *= FACING_MULT;
    
    //init ballOn to false, make the ball sit on pong
    ballOn = false;
    
    leftKey = left_key;
    rightKey = right_key;
    
    moveCounter = 0;
    classID = BALL_CLASSID;
    hitSide = HIT_NOHIT;
    //set the number of lives 
    lives = 3;
    //remember starting pos
    startPos.x = x;
    startPos.y = y;
    
}
//set the pointer to player
void Ball::setPlayer(Character* p)
{
    player = p;
}

//update the balls position
void Ball::update()
{
    if(ballOn)
    {
        //signal that we have started to move from the pong
        if(moveCounter < 2)
        {
            moveCounter++;
        }
        //get the future/new position
        vector newpos;
        facingDir = normalize(facingDir);
        facingDir.x *= FACING_MULT;
        facingDir.y *= FACING_MULT;
        
        newpos.x = (int)(pos.x + facingDir.x);
        newpos.y = (int)(pos.y + facingDir.y);

        //check for collision
        int collision = checkMove(newpos.x, newpos.y);
        
        //are we are not hitting anything
        if(collision == 2)
        {
            //if we are not hitting the bricks
            if(level->getLevelInfo(newpos.x, newpos.y) == TILE_EMPTY)
            {
                //keep moving
                move(facingDir.x, facingDir.y);
            }
            //if we are hitting the BLOCKS
            else
            {
               
                //get the current blocks normal
                vector normDir;
                normDir = level->getEnemiesNormal((newpos.x / GRID_SIZE_WIDTH), 
                                                  (newpos.y / GRID_SIZE_HEIGHT));
           
                //clamp the facingDir and store it in tmp
                //so when we move we dont visually overlap with the block we just hit
                vector tmp = clampBlocks(newpos.x, newpos.y);
                
                //move the ball on top of the block
                move(tmp.x,tmp.y);
                
                //kill the block
                level->emptyBlock(newpos.x / GRID_SIZE_WIDTH, newpos.y / GRID_SIZE_HEIGHT);

                //update our blocks and empty spaces
                level->updateEnemies();
                
                //draw our ball
                drawArea->drawSprite(SPRITE_BALL, pos.x + tmp.x, pos.y + tmp.y);

                //bounce the ball off of the blocks normal and set
                //the last member to true meaning that we are conducting bounce off blocks
                bounceBall(normDir.x, normDir.y, true);
            }
        }
        //if we are hitting the pong player
        else if(collision == 1)
        {
            //bounce the ball based on the side of the player we hit
            bounceBall(0,0,false);
        }
        //if we are hitting the wall
        else if(collision == 0)
        {
            //just bounce the ball
            bounceBall(0,0,false);
        }
        //if we have died
        else if(collision == -1)
        {
            //if player is alive
            if(player->isAlive())
            {
                //remove balls lives/players lives
                player->addLives(-1);
                addLives(-1);

                //reset the ball position and the players pos
                resetPos();
                player->resetPos();
            }
        }
    }
}

//check if we can move 
//return 2 if the space is empty, we havent died and hit the pong player
//return 1 if we have hit the pong player
//return 0 if we have hit a wall
//return -1 if we went down too far and have died
int Ball::checkMove(int posx, int posy)
{
    
    //check if the space is empty, by checking
    //if we are not hitting the wall or have died
    bool wall = isNotHittingWall(posx, posy);
    
    //we havent hit the wall
    if(wall)
    {
        //we can go and check 
        //if we are not hitting the pong player
        bool p = isNotHittingPlayer(posx, posy);
        
        //if we havent hit the player
        if(p)
        {
            //did we die?
            bool die = notDead(posx, posy);
            //if we didnt die
            if(die)
            {
                //signal that we can move (return 2)
                return 2;
            }
            //if we have died
            else
            {
                //signal that we have died (return -1)
                return -1;
            }
        }
        //if we have hit the player
        else
        {
            //signal that we have hit the player (return 1)
            return 1;
        }
    }
    //if we have hit the wall
    else
    {
        //signal that we have hit the wall (return 0)
        return 0;
    }
    
}

//are we dead?
bool Ball::notDead(int posx, int posy)
{
    if((int)posy < (player->getY()))
    {
        return true;
    }
    return false;
}

//are we hitting the player?
bool Ball::isNotHittingPlayer(int posx, int posy)
{
    //if we still havent moved from pongs pos
    //or we just moved a moment ago
    if(moveCounter > 1)
    {
        //if we are not dead, gone too low
        //or if we are not just above the pong player
        bool distOn = false;
        float distance;
        
        //aimPlayer is going to point towards the ball from player
        //or maybe its better to just use the normal of the player
        vector aimPlayer;
        aimPlayer.x = 0;
        aimPlayer.y = -1;
        //normalize our aim vector
        aimPlayer = normalize(aimPlayer);

        //get the dot product between the balls facingDir
        //and the vector from player to ball
        float dotProd = dotProduct(facingDir, aimPlayer);
        
        int i, tmpi;
        //lets just check to see if our ball is crossing the Y threshold
        if(player->getY() < posy + 15)
        {
            //get the distance from ball to player
            //we also need to sum up the distance from all the parts 
            //of the pong player and not only from the center
            float minDist = 100;
            for(i = 0; i <= 73; i++)
            {
                distance = sqrt(((player->getX() + i) - posx) * ((player->getX() + i) - posx) 
                            + (player->getY() - posy) * (player->getY() - posy));

                //if the current distance is less then our minDist
                if(distance < minDist)
                {
                    //set the minDist to current dist
                    minDist = distance;
                    //also remember which section of the pong player we were looking up
                    tmpi = i;
                }
                //if we have started increasing the distance to the ball
                else
                {
                    //get the hell out of here
                    break;
                }

            }
            
            //check if minDist is less than 15, basically see if ball is close
            if(minDist < 15)
            {
                distOn = true;
            }
        }
        //if dotProd < 0 - if the facingDir is facing towards the player
        //and if distance is pretty small
        //we are probably pretty low heading towards the bottom
        if(dotProd < 0 && distOn)
        {
            //if we are not dead
            if(notDead(posx, posy))
            {
                //if we are just above the player
                if((int)posy > (player->getY() - 15))
                {
                    //if we are not in pongs player width vicinity 
                    if(((int)posx > (73 + player->getX()) || (int)posx < (player->getX() - 15)))
                    {
                        //we havent hit the player
                        hitSide = HIT_NOHIT;
                        return true;
                    }
                    //save our hitSide so later we can control the reflection of the ball
                    //based off of side of the pong player it hit
                    player->setHitSide(tmpi);
                    //we have hit the player
                    hitSide = HIT_PLAYER;
                    return false;
                }
            }
            //we are either dead or have hit the player
            hitSide = HIT_NOHIT;
            return false;
        }
        hitSide = HIT_NOHIT;
        return true;
    }
    //we are not moving and havent hit anything
    hitSide = HIT_NOHIT;
    return true;
}

//are we hitting the wall?
//set BALLS hitSide using our hit enum
bool Ball::isNotHittingWall(int posx, int posy)
{
    //are we hitting left side
    if((int)posx <= 0)
    {
        hitSide = HIT_LSIDE;
        return false;
    }
    //or are we hitting right side
    if((int)posx >= drawArea->getAbsScreenW() - 15)
    {
        hitSide = HIT_RSIDE;
        return false;
    }
    //or are we hitting top
    if((int)posy <= 0)
    {
        hitSide = HIT_TOP;
        return false;
    }
    //or are we hitting bottom
    if((int)posy >= drawArea->getAbsScreenH() - 15)
    {
        hitSide = HIT_BOTTOM;
        return false;
    }
    //if not then we are not hitting the wall
    else
    {
        //we havent hit anything
        hitSide = HIT_NOHIT;
        return true;
    }
}

//add/remove lives from the ball
void Ball::addLives(int num)
{
    Sprite::addLives(num);
}

//bounce the ball
//the most important part of the game
bool Ball::bounceBall(float x, float y, bool block)
{
    //if we are hitting an enemy block
    if(block)
    {
        //create temp vectors 
        vector collNorm, tempDir;
        
        //set the enemies collision normal
        collNorm.x = x;
        collNorm.y = y;
        //calc the reflection vector from our dir and 
        //enemies collision normal
        tempDir = reflect(facingDir, collNorm);
        
        //just check to see if our y is extremely low, cannot have that
//        if(fabs(tempDir.y) < 0.1)
//            tempDir.y += 0.15;
//        tempDir = normalize(tempDir);
        
        //set our facing dir
        facingDir = tempDir;
        //we are successful
        return true;
    }
    //if we are not hitting an enemy block
    //check what is it that we are hitting using our hit enum
    else
    {
         
        //are we hitting bottom wall
        if(hitSide == HIT_BOTTOM)
        {
            //clamp our facingDir and return as new vector
            //so we dont visually overlap with the wall when we paint the window
            //avoiding artifacts
            vector tmp = clampBounds();
            //first move to bottom side of the wall
            if((int)pos.y != drawArea->getAbsScreenH())
            {
                move(tmp.x, tmp.y);
            }
            
            //then update our facing dir
            //so next run we will bounce off of the wall
            vector bottom, tempDir;
            bottom.x = 0;
            bottom.y = -1;
            
            tempDir = reflect(facingDir, bottom);
            //just check to see if our y is extremely low, cannot have that
//            if(fabs(tempDir.y) < 0.1)
//                tempDir.y += 0.15;
//            tempDir = normalize(tempDir);
            
            facingDir = tempDir;
            
            return true;
        }
        //or top 
        else if(hitSide == HIT_TOP)
        {
            //clamp our facingDir and return as new vector
            //so we dont visually overlap with the wall when we paint the window
            //avoiding artifacts
            vector tmp = clampBounds();
            //first move to top side of the wall
            if((int)pos.y != 0)
            {
                move(tmp.x, tmp.y);
            }
            
            //then update our facing dir
            //so next run we will bounce off of the wall
            vector top, tempDir;
            top.x = 0;
            top.y = 1;
            
            tempDir = reflect(facingDir, top);
            //just check to see if our y is extremely low, cannot have that
//            if(fabs(tempDir.y) < 0.1)
//                tempDir.y += 0.15;
//            tempDir = normalize(tempDir);
            facingDir = tempDir;
            
            return true;
        }
        //or left side
        else if(hitSide == HIT_LSIDE)
        {
            //clamp our facingDir and return as new vector
            //so we dont visually overlap with the wall when we paint the window
            //avoiding artifacts
            vector tmp = clampBounds();
            //first move to left side of the wall
            if((int)pos.x != 0)
            {
                move(tmp.x, tmp.y);
            }
            
            //then update our facing dir
            //so next run we will bounce off of the wall
            vector lside, tempDir;
            lside.x = 1;
            lside.y = 0;
            
            tempDir = reflect(facingDir, lside);
            //just check to see if our y is extremely low, cannot have that
//            if(fabs(tempDir.y) < 0.1)
//                tempDir.y += 0.15;
//            tempDir = normalize(tempDir);
            facingDir = tempDir;
            
            return true;
        }
        //or right side
        else if(hitSide == HIT_RSIDE)
        {
            //clamp our facingDir and return as new vector
            //so we dont visually overlap with the wall when we paint the window
            //avoiding artifacts
            vector tmp = clampBounds();
            //first move to right side of the wall
            if((int)pos.x != drawArea->getScreenW())
            {
                move(tmp.x, tmp.y);
            }
            
            //then update our facing dir
            //so next run we will bounce off of the wall
            vector rside, tempDir;
            rside.x = -1;
            rside.y = 0;
            
            tempDir = reflect(facingDir, rside);
            //just check to see if our y is extremely low, cannot have that
//            if(fabs(tempDir.y) < 0.1)
//                tempDir.y += 0.15;
//            tempDir = normalize(tempDir);
            facingDir = tempDir;
            
            return true;
        }
        else if(hitSide == HIT_PLAYER)
        {
            //clamp our facingDir and return as new vector
            //so we dont visually overlap with the player when we paint the window
            //avoiding artifacts
            vector tmp = clampPlayer();
            //move our ball ontop of the player, since we have just clamped our facingDir into tmp
            if((int)pos.y != drawArea->getScreenH())
            {
                move(tmp.x, tmp.y);
            }
            
            vector normal, tempDir;
            //get the hitSide of the pong player so we can tweak the normal
            //the ball will bounce differently based on the side of the pong player it hit
            int side = player->getHitSide();
            //refit our side (since it goes from 0-74, we need it from -3,3)
            //and tweak the x component of the normal
            normal.x = fitrange(side, -3, 3, 0, 74) * (0.8 / 3.0);
            normal.y = -1;
            
            //normalize the normal just in case
            normal = normalize(normal);
            //reflect the facingDir against the tweaked normal
            tempDir = reflect(facingDir, normal);
            //just check to see if our y is extremely low, cannot have that
            if(fabs(tempDir.y) < 0.1)
                tempDir.y *= 5;
            tempDir = normalize(tempDir);
            //set the new facingDir
            facingDir = tempDir;
            
            return true;
            
        }
        //or maybe nothing
        else
        {
            return false;
        }
        
        
    }
}


//draw the ball
void Ball::draw(float x, float y)
{
    drawArea->drawSprite(SPRITE_BALL, x, y);
}

//erase the ball
void Ball::erase(float x, float y)
{
    //we erase the sprite by drawing the bg over a region that the ball takes
    //so instead of deleting anything, we just paint the bg over the ball
    drawArea->drawBgRegion(wxPoint(x, y), wxSize(15,15));
}

//move the ball if it hasnt been launched yet
bool Ball::keyPress(char c)
{
    //move it with the player
    if(c == leftKey)
    {
        return move(-15, 0);
    }
    else if(c == rightKey)
    {
        return move(15, 0);
    }
    else
    {
        return false;
    }
}

//set the ball status
//ballOn = true if the ball has been launched
//ballOn = false if its still sitting on the player
void Ball::setBallOn(bool set)
{
    ballOn = set;
}

//return the ball status
bool Ball::getBallStatus()
{
    return ballOn;
}

//move our ball based off of facingDir
//this is a relative move
bool Ball::move(float x, float y)
{
    //this is a relative move
    //meaning we move by increment (accel/facingDir)
    //rather than moving to actual pos
    erase(pos.x, pos.y);

    //just add to our current pos
    pos.x += x;
    pos.y += y;

    draw(pos.x, pos.y);

    return true;
}

//move the ball at specific location
//this is an absolute move
bool Ball::moveToPos(float x, float y)
{
    //this is an absolute move
    //meaning we draw the ball at a specific location
    //without updating the facingDir
    //first erase the ball at current pos
    erase(pos.x, pos.y);
    
    //then update our future pos
    pos.x = x;
    pos.y = y;
    
    //draw us at the future pos 
    draw(pos.x, pos.y);
    
    return true;
}

//reset the balls position and the center bottom screen 
//on top of the pong player, also reset the facingDirection
void Ball::resetPos()
{
    //erase ball from the dead pos
    erase(pos.x, pos.y);
    //reset the pos
    pos.x = startPos.x;
    pos.y = startPos.y;
    //draw at the default pos
    draw(pos.x, pos.y);
    //regenerate the facingDir
    srand(time(NULL));
    facingDir.x = (1 - 2 * ((rand() % 100)/100.0));
    facingDir.y = -2 * ((rand() % 100)/100.0);
    
    facingDir = normalize(facingDir);
    facingDir.x *= FACING_MULT;
    facingDir.y *= FACING_MULT;
    
    //reset the pos on top of the pong
    ballOn = false;   
    //and our move counter
    moveCounter = 0;
    //and our hitSide
    hitSide = HIT_NOHIT;
}

//clamp our facingDir vector tmp to screen bounds
//so on the next move iteration it snaps on to the sides instead of overshooting bounds
vector Ball::clampBounds()
{
    vector tmp = facingDir;
    
    //clamp our facingDir in case it goes out of screen bounds
    //if we are going out of max bounds on the right side of the screenWidth
    if((int)(facingDir.x + pos.x) >= drawArea->getAbsScreenW() - 15)
    {
        tmp.x = ((drawArea->getAbsScreenW() - 15) - pos.x);
    }
    //or if we are going out of min bounds on the left side of the screenWidth
    if((facingDir.x + pos.x < 0))
    {
        tmp.x = -pos.x;
    }
    //if we are going out of max bounds on the bottom side of the screenHeight
    if((int)(facingDir.y + pos.y) >= drawArea->getAbsScreenH() - 15)
    {
        tmp.y = ((drawArea->getAbsScreenH() - 15) - pos.y);
    }
    //or if we are going out of min bounds on the top side of the screenHeight
    if(facingDir.y + pos.y < 0)
    {
        tmp.y = - pos.y;
    }
    
    return tmp;
}

//clamp our facingDir vector tmp to player pos in Y
//so on the next move iteration it snaps on to the player
vector Ball::clampPlayer()
{
    vector tmp = facingDir;
    //if we are overshooting the safe drawing distance to the player
    if(facingDir.y + pos.y >= player->getY() - 15)
    {
        //clamp the y so we snap onto the player
        tmp.y = (player->getY() - 15) - pos.y;
    }
    return tmp;
}

//clamp the facing dir so it doesnt overshoot the block
vector Ball::clampBlocks(float x, float y)
{
    //create tmp vectors for manipulation
    vector tmp = facingDir;
    vector nextpos;
    nextpos.x = (int)(facingDir.x + pos.x);
    nextpos.y = (int)(facingDir.y + pos.y);
    
    //get the currently hit blocks pos
    vector gridpos;
    gridpos.x = level->getEnemiesPosX(x / GRID_SIZE_WIDTH, y / GRID_SIZE_HEIGHT);
    gridpos.y = level->getEnemiesPosY(x / GRID_SIZE_WIDTH, y / GRID_SIZE_HEIGHT);
    
    //get the currently hit blocks hitSide
    int blocksHitSide = level->getEnemiesHit(x / GRID_SIZE_WIDTH, y / GRID_SIZE_HEIGHT);

    //if we have hit the left side of the block
    if(blocksHitSide == HIT_LSIDE)
    {
        tmp.x = (gridpos.x - 15) - nextpos.x;
    }
    //if we have hit the right side of the block
    if(blocksHitSide == HIT_RSIDE)
    {
        tmp.x = (gridpos.x + 50) - nextpos.x;
    }
    //if we have hit the bottom side of the block
    if(blocksHitSide == HIT_BOTTOM)
    {
        tmp.y = (gridpos.y + 15) - nextpos.y;
    }
    //if we have hit the top side of the block
    if(blocksHitSide == HIT_TOP)
    {
        tmp.y = (gridpos.y - 15) - nextpos.y;
    }
    
    return tmp;
}
//return a dot product between two vectors
float Ball::dotProduct(vector dir, vector norm)
{
//    float dirMag = sqrt(dir.x * dir.x + dir.y * dir.y);
//    float normMag = sqrt(norm.x * norm.x + norm.y * norm.y);
//    float costh = cos(dirMag/normMag);
    
//    return (dirMag * normMag * costh);
    return (dir.x * norm.x + dir.y * norm.y);
}

//calculate the reflection vector when we collide
//between our direction and enemies normal
vector Ball::reflect(vector dir, vector norm)
{
    vector temp;
    float dot = dotProduct(dir, norm);
    temp.x = dir.x - 2 * dot * norm.x;
    temp.y = dir.y - 2 * dot * norm.y;
    return temp;
}

//calc vectors magnitude
float Ball::vecMag(vector Vec)
{
    return (sqrt(Vec.x * Vec.x + Vec.y * Vec.y));
}

//normalize a vector
vector Ball::normalize(vector Vec)
{
    vector tmp;
    tmp.x = Vec.x / vecMag(Vec);
    tmp.y = Vec.y / vecMag(Vec);
    
    return tmp;
}

//get the facing direction
vector Ball::getFacingDir()
{
    return facingDir;
}

//get the balls centroid
//wxWidget draws 0,0 at top left corner
//so all the movement is based off of that
//we can return just the centroid if necessary 
vector Ball::getCentroid()
{
    vector center;
    center.x = pos.x + 7.5;
    center.y = pos.y + 7.5;
    
    return center;
}

//return the numbers sign + or -
int Ball::sign(float num)
{
    if(num < 0)
    {
        return -1;
    }
    if(num > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}