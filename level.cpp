#include "stdwx.h"
#include <stdlib.h>
#include <iterator>
#include "level.h"
#include "character.h"
#include "ball.h"
#include "blocks.h"

/*
 * BLOCK_SIZE is the area of w,h a block can take
 * previously one block was one char (x) and one block could be defined with multiple chars
 * (xxx)->one block if the size is 3, this allowed us to setup ids on blocks to reflect this
 * when a ball collides
 * 
 * BLOCK_WIDTH is the number of blocks that we will place width wise in our level
 * BLOCK_HEIGHT is the number of blocks that we will place height wise in our level
 * BLOCK_HOFFSET is the offset in Y(HEIGHT) that blocks will have from the center
 * 
 * example: 
 * BLOCK_WIDTH 3
 * BLOCK_HEIGHT 5
 * BLOCK_HOFFSET 2
 * 
 * ------------------
 * |    xxx         |
 * |    xxx         |
 * |    xxx         |
 * |    xxx         |
 * |    xxx         |
 * |    |center     |
 * |    |           |
 * |    BLOCK_HOFF  |
 * |                |
 * |                |
 * ------------------
 */
#define BLOCK_SIZE      1
#define BLOCK_WIDTH     5
#define BLOCK_HEIGHT    10
#define BLOCK_HOFFSET   2

//blocks grid size in pixels (bitmap size)
#define GRID_SIZE_WIDTH     50
#define GRID_SIZE_HEIGHT    15

//define the grid cells for the game (w,h) = (x,y)
#define LEVEL_X             16
#define LEVEL_Y             56

Level::Level(DrawEngine* de, bool randLevel)
{
    drawArea = de;
    
    width = de->getScreenW();
    height = de->getScreenH();
    
    //create the level walls 
    //AND the blocks pong will destroy
    level = new char*[width];
        
    for(int w = 0; w < width; w++)
    {
        level[w] = new char[height];
    }
        
    player = NULL;
    ball = NULL;
    totalBlocks = 0;
    //create our level
    createLevel(randLevel);

    //set the map to point to level construct
    drawArea->setMap(level);
    
}

//clear all the level maps and npcs
Level::~Level()
{
    for(int w = 0; w < width; w++)
    {
        delete [] level[w];
    }
    delete [] level;
    
    for(iter = npc.begin(); iter != npc.end(); ++iter)
    {
        delete *iter;
    }
}


//add a player to the level
void Level::addPlayer(Character* p)
{
    player = p;
}

//add the ball to the level
void Level::addBall(Ball* b)
{
    ball = b;
}

//create our level!!
void Level::createLevel(bool randomLevel)
{
    //if we need to create the random level
    if(randomLevel)
    {
        setRandomLevel();
    }
    //if not then regular
    else
    {
        setRegularLevel();
    }   
}

//create a random level 
void Level::setRandomLevel()
{
    int countw ,counth;
    countw = counth = 0;

    //iterate thru our full screen size
    //for the entire width
    for(int w = 0; w < width; w++)
    {
        //for the entire height
        for(int h = 0; h < height; h++)
        {
            //if our height is at a specified value
            if(h >= 10 && h < LEVEL_Y - 20)
            {
                int random = rand() % 100;
                
                if(random < 50 && w < LEVEL_X)
                {
                    counth++;
                    //create a new block
                    //set the value to block
                    level[w][h] = TILE_BLOCK;
                    //create a new block object inside a list and set its isBlock = true
                    npc.push_back(new Blocks(drawArea, this, SPRITE_CLASSID, w, h, 1, counth, true));
                }
                else
                {
                    //set the value to block
                    level[w][h] = TILE_EMPTY;
                    //create a new block object inside a list and set its isBlock = true
                    npc.push_back(new Blocks(drawArea, this, SPRITE_CLASSID, w, h, 0, -1, false));
                }
            
            }
            //else fill it up with empty blocks
            else
            {
                //set the value to block
                level[w][h] = TILE_EMPTY;
                //create a new block object inside a list and set its isBlock = true
                npc.push_back(new Blocks(drawArea, this, SPRITE_CLASSID, w, h, 0, -1, false));
            }
        }
    }
    totalBlocks = counth;
}

//create a regular level using the previously defined variables for the console game
void Level::setRegularLevel()
{
    //since we want to be able to make each block consist of many chars
    //we setup an id matrix system where we determine the ids based off of BLOCK_SIZE
    //  EXAMPLE
    //BLOCK_SIZE = 1    //BLOCK_SIZE = 3
    //-------------     -------------------
    //| 1 | 4 | 7 |     | 1 | 1 | 1 | 4 | 4...
    //-------------     -------------------
    //| 2 | 5 | 8 |     | 2 | 2 | 2 | 5 | 5...
    //-------------     -------------------                    
    //| 3 | 6 | 9 |     | 3 | 3 | 3 | 6 | 6...
    //----------------------------------------
    int countw ,counth, lastCount, prevLastCount;
    countw = counth = lastCount = prevLastCount = 0;
    
    //iterate thru our full screen size
    for(int w = 0; w < width; w++)
    {
        //if we are at the beginning of our iteration
        //basically if we are at beginning of every 0th iteration of BLOCK_SIZE size
        //example BLOCK_SIZE = 1, iterations: 0-0-0-0-0-0-0-0-...
        //example BLOCK_SIZE = 2, iterations: 0-1-0-1-0-1-0-1-...
        //example BLOCK_SIZE = 3, iterations: 0-1-2-0-1-2-0-1-...
        if((w % ((width / 2) - BLOCK_WIDTH / 2)) % BLOCK_SIZE == 0)
        {
            //reset the counter to the last recorded one
            counth = lastCount;
        }
        //if we are every nth iteration, so not the 0th
        else
        {
            //reset the counter to the previous last count
            counth = prevLastCount;
        }
        
        for(int h = 0; h < height; h++)
        {
            //if we are within a certain limit of the width
            if((w >= ((width / 2) - BLOCK_WIDTH / 2) && w < ((width / 2) + BLOCK_WIDTH / 2)))
            {
                //this is a counter for every time we get successfully into our loop 
                countw = w  % ((width / 2) - BLOCK_WIDTH / 2);
                
                //if we within a limit of the height
                if(h >= ((height / 2) - ((BLOCK_HEIGHT / 2) + BLOCK_HOFFSET)) 
                        && h < (((height / 2) + (BLOCK_HEIGHT / 2)) - BLOCK_HOFFSET))
                {
                    //record the previous state of the lastCount
                    //every time we get to the last member of our array
                    //in the last iteration before reseting the iter to 0, 
                    //record the state of the lastCount
                    if((countw % BLOCK_SIZE == BLOCK_SIZE - 1) && 
                            (h == (((height / 2) + (BLOCK_HEIGHT / 2)) - BLOCK_HOFFSET) - 1))
                    {
                        prevLastCount = lastCount;
                    }
                    
                    //keep increasing our iterator
                    counth++;
                    //keep setting the last count to the current iter
                    //if we are at the first stage of iteration (0th iteration)
                    if(countw % BLOCK_SIZE == 0)
                    {
                        lastCount = counth;
                    }

                    //set the value to block
                    level[w][h] = TILE_BLOCK;
                    //create a new block object inside a list and set its isBlock = true
                    npc.push_back(new Blocks(drawArea, this, SPRITE_CLASSID, w, h, 1, counth, true));
                }
                else
                {
                    //set an empty block
                    level[w][h] = TILE_EMPTY;
                    //create a new block object inside a list and set its isBlock = false
                    npc.push_back(new Blocks(drawArea, this, SPRITE_CLASSID, w, h, 0, -1, false));
                }         
            }
            else
            {
                //set an empty block
                 level[w][h] = TILE_EMPTY;
                //create a new block object inside a list and set its isBlock = false
                npc.push_back(new Blocks(drawArea, this, SPRITE_CLASSID, w, h, 0, -1, false));
            }
        }
    }
    totalBlocks = counth;
}

//draw our level
void Level::draw()
{
    drawArea->drawLevel();
}

//check if we have pressed a key in the level
bool Level::keyPress(char c)
{
    //if we have a player
    if(player)
    {
        //if the key is pressed
        //and is one of the keys mapped to the player
        if(player->keyPress(c) || c == ' ')
        {
            //if the ball hasnt moved yet
            if(ball->getBallStatus() == false)
            {
                //move the ball with the player
                if(ball->keyPress(c))
                {
                    return true;
                }
                return false;
            }
            return true;
        }
        return false;
    }
    
}

//update our ball trajectory 
void Level::update()
{
    //if the ball has left the pong
    if(ball->getBallStatus())
    {
        //update our balls info
        ball->update();
    }
}

/*
 * return the level info
 * true for TILE_BLOCK
 * false for TILE_EMPTY
 * we also do checks against every corner of the ball, since its the balls coord we are providing as args
 * and balls orig corner is topleft, we have to check for possibility of other corners colliding 
*/
bool Level::getLevelInfo(float& x, float& y)
{
    //vars to convert from x,y to h,w
    int w,h;
    //tmp vars to hold the original x,y
    int tmpx, tmpy;
    
    tmpx = x;
    tmpy = y;
    
    //convert to w,h
    w = x / GRID_SIZE_WIDTH;
    h = y / GRID_SIZE_HEIGHT;
    
    //check the top left corner
    if(level[w][h] == TILE_BLOCK)
    {
        //return a block
        return level[w][h];
    }
    
    //check the top right corner
    x = x + 15;
    w = x / GRID_SIZE_WIDTH;
    
    if(level[w][h] == TILE_BLOCK)
    {
        //return a block
        return level[w][h];
    }
    
    //check the bottom right corner
    y = y + 15;
    h = y / GRID_SIZE_HEIGHT;
    
    if(level[w][h] == TILE_BLOCK)
    {
        //return a block
        return level[w][h];
    }
    
    //check the bottom left corner
    x = x - 15;
    w = x / GRID_SIZE_WIDTH;
    
    if(level[w][h] == TILE_BLOCK)
    {
        //return a block
        return level[w][h];
    }
    
    if(level[w][h] == TILE_EMPTY)
    {
        x = tmpx;
        y = tmpy;
        //return empty
        return level[w][h];
    }
    
}

//update our enemies facingDirection
//basically its a normal pointing from the enemy to outter space
void Level::updateEnemiesDir()
{
    //update the enemies status
    updateEnemies();
    
    int count = 0;
    //for the entire level 
    for(int w = 0; w < width; w++)
    {
        for(int h = 0; h < height; h++)
        {   
            //if a block is not empty
            if(level[w][h] == TILE_BLOCK)
            {
                //update its normal
                updateDirectionLaplace(count);
                
            }
            count++;
        }
    }
    
//    avgFacingDir();
    
}
//======================================================================
//LAPLACIAN normal direction of enemies
//update our normal direction of the enemy
//by sampling the neighbourhood of 8 cells around the current enemy
//|--------|--------|--------|
//| pt+w-1 | pt + w | pt+w+1 |
//|--------|--------|--------|
//| pt - 1 |   pt   | pt + 1 |
//|--------|--------|--------|
//| pt-w-1 | pt - w | pt-w+1 |
//|--------|--------|--------|
//
//      pt is the current enemy 
//if the cell is empty then add it to accum, since we want our normal
//to point to the mean pos of the empty cells
//its like a laplacian in houdini vex
//==================================
void Level::updateDirectionLaplace(int index)
{
    int currIndex = index;
    //set the iterator to first block
    iter = npc.begin();
    
    //recalc the pos
    //get the width in the 2d array
    int scW = index / height;
    //get the height in the 2d array
    int scH = index % height;
    
    //===========================================
    //**********GET THE SURROUNDING BLOCKS VALUE
    bool neigh[8];
    int emptyCount = 0;
    int accumH = 0;
    int accumW = 0;
    
    //get the top left block (scW - 1,scH - 1)
    //formula for converting 2d array to lists index
    //list index = (currentWidth +/- offsetW) * fullHeight + (currentHeight +/- offsetH)
    index = (scW - 1) * height + (scH - 1);
    
    //set the iterator to point at the top left enemy
    advance(iter, index);
    
    //set its isBlock(true/false) value to the array
    neigh[0] = (*iter)->isBlock;
    
    //if its not a block (isBlock = false), it must be empty
    if(neigh[0] == 0)
    {
        //accumulate the empty blocks pos cooord
        accumH += (scH - 1);
        accumW += (scW - 1);
    }
    //increase the empty counter if we have found an empty block
    emptyCount += (neigh[0] == 0);
    
    //!!!!!REPEAT THE ABOVE PROCEDURE FOR EVERY SURROUNDING CELL!!!!
    
    
    //get the bottom left block (scW - 1,scH + 1)
    index = (scW - 1) * height + (scH + 1);
    iter = npc.begin();
    advance(iter, index);
    
    neigh[1] = (*iter)->isBlock;
    if(neigh[1] == 0)
    {
        accumH += (scH + 1);
        accumW += (scW - 1);
    }
    emptyCount += (neigh[1] == 0);
    
    //get the top right block (scW + 1,scH - 1)
    index = (scW + 1) * height + (scH - 1);
    iter = npc.begin();
    advance(iter, index);
    
    neigh[2] = (*iter)->isBlock;
    if(neigh[2] == 0)
    {
        accumH += (scH - 1);
        accumW += (scW + 1);
    }
    emptyCount += (neigh[2] == 0);
    
    //get the bottom right block (scW + 1,scH + 1)
    index = (scW + 1) * height + (scH + 1);
    iter = npc.begin();
    advance(iter, index);
    
    neigh[3] = (*iter)->isBlock;
    if(neigh[3] == 0)
    {
        accumH += (scH + 1);
        accumW += (scW + 1);
    }
    emptyCount += (neigh[3] == 0);
    
    //get the top block (scW ,scH - 1)
    index = (scW) * height + (scH - 1);
    iter = npc.begin();
    advance(iter, index);
    
    neigh[4] = (*iter)->isBlock;
    if(neigh[4] == 0)
    {
        accumH += (scH - 1);
        accumW += (scW);
    }
    emptyCount += (neigh[4] == 0);
    
    //get the bottom block (scW ,scH + 1)
    index = (scW) * height + (scH + 1);
    iter = npc.begin();
    advance(iter, index);
    
    neigh[5] = (*iter)->isBlock;
    if(neigh[5] == 0)
    {
        accumH += (scH + 1);
        accumW += (scW);
    }
    emptyCount += (neigh[5] == 0);
    
    //get the right block (scW + 1,scH)
    index = (scW + 1) * height + (scH);
    iter = npc.begin();
    advance(iter, index);
    
    neigh[6] = (*iter)->isBlock;
    if(neigh[6] == 0)
    {
        accumH += (scH);
        accumW += (scW + 1);
    }
    emptyCount += (neigh[6] == 0);
    
    //get the left block (scW - 1,scH)
    index = (scW - 1) * height + (scH);
    iter = npc.begin();
    advance(iter, index);
    
    neigh[7] = (*iter)->isBlock;
    if(neigh[7] == 0)
    {
        accumH += (scH);
        accumW += (scW - 1);
    }
    emptyCount += (neigh[7] == 0);
    
    //===========================================
    //we have finished the accumulation
    //now lets calculate the normal
    int newH = 0;
    int newW = 0;
    
    //if there are empty blocks
    //take their summed up coords and get an average by div with the total number of empty blocks
    if(emptyCount > 0)
    {
        newH = accumH / emptyCount;
        newW = accumW / emptyCount;
    
        //have the iterator point to the current block
        iter = npc.begin();
        advance(iter, currIndex);
        //set the current blocks facingDir to be the avg normal we just calculated
        (*iter)->setFacingDir(newW - ((*iter)->getX()), newH - ((*iter)->getY()));
    }
    
    
}

//update our enemies info
//maybe some blocks have been destroyed, mark them as empty 
//so we dont collide again
void Level::updateEnemies()
{
    int count = 0;
    //set the iterator to the beginnig of npc
    iter = npc.begin();
    //iterate thru the screen
    for(int w = 0; w < width; w++)
    {
        for(int h = 0; h < height; h++)
        {   
            //set the iterator to beginning every time..
            //cause it will just add to the count 
            iter = npc.begin();
            advance(iter, count);
            
            //set the current blocks info empty or a block
            (*iter)->isBlock = level[w][h];
            count++;
        }
    }
}

//return enemies X direction based on [w][h] coord
float Level::getEnemiesDirX(int w, int h)
{
    //set the iterator to the beginning
    iter = npc.begin();
    //formula for converting 2d array to lists index
    //list index = currentWidth * fullHeight + currentHeight
    int index = w * height + h;
    advance(iter, index);
    
    //return the facingDir x component
    return (*iter)->getFacingDirX();

        
}

//return enemies X direction based on [w][h] coord
float Level::getEnemiesDirY(int w, int h)
{
    iter = npc.begin();
    int index = w * height + h;
    advance(iter, index);
    

    return (*iter)->getFacingDirY();

}

//return enemies X pos based on [w][h] coord
float Level::getEnemiesPosX(int w, int h)
{
    iter = npc.begin();
    int index = w * height + h;
    advance(iter, index);
    
    return (*iter)->getX() * GRID_SIZE_WIDTH;
}

//return enemies Y pos based on [w][h] coord
float Level::getEnemiesPosY(int w, int h)
{
    iter = npc.begin();
    int index = w * height + h;
    advance(iter, index);
    
    return (*iter)->getY() * GRID_SIZE_HEIGHT;
}

//empty this block
bool Level::emptyBlock(int x, int y)
{
    //get to the current member
    iter = npc.begin();
    int member = x * height + y;
    advance(iter, member);
    
    //get our id
    int id = (*iter)->getID();

    //empty our block 
    (*iter)->isBlock = false;
    (*iter)->setID(-1);
    level[x][y] = TILE_EMPTY;
    
    //delete our block
    drawArea->deleteTile(x, y);
        
}

//return the current blocks normal for bouncing
//we check where the ball has collided against the current block
//and based off of that we calc a normal for bouncing
vector Level::getEnemiesNormal(int w, int h)
{
    //set our iter to the current block
    iter = npc.begin();
    int index = w * height + h;
    advance(iter, index);
    
    //vars for storing querried values
    vector ballPrevPos, blockPos, tmp;
    
    //get the balls previous pos by stepping back in time thru curr pos and facingDir
    ballPrevPos.x = (int)(ball->getX() - ball->getFacingDir().x);
    ballPrevPos.y = (int)(ball->getY() - ball->getFacingDir().y);
    
    //get current blocks pos in x,y
    blockPos.x = (*iter)->getX() * GRID_SIZE_WIDTH;
    blockPos.y = (*iter)->getY() * GRID_SIZE_HEIGHT;
    
    //check if the ball is above the block
    if(ballPrevPos.y <= blockPos.y)
    {
        //check if its on the left or right side
        if(ballPrevPos.x + 15 <= blockPos.x)
        {
            //we are left above the block
            tmp.x = -1;
            tmp.y = 0;
            //set our current blocks hitSide, so we know where we hit it
            (*iter)->setHitSide(HIT_LSIDE);
            
            return tmp;
        }
        else if(ballPrevPos.x >= blockPos.x + 50)
        {
            //we are right above the block
            tmp.x = 1;
            tmp.y = 0;
            //set our current blocks hitSide, so we know where we hit it
            (*iter)->setHitSide(HIT_RSIDE);
            
            return tmp;
        }
        //or directly above within block width range
        else
        {
            //we are directly above the block within its width range
            tmp.x = 0;
            tmp.y = -1;
            //set our current blocks hitSide, so we know where we hit it
            (*iter)->setHitSide(HIT_TOP);
            
            return tmp;
        }
        
        
    }
    //if not then its below the block
    else
    {
        //check if its on the left or right side
        if(ballPrevPos.x + 15 <= blockPos.x)
        {
            //we are left above the block
            tmp.x = -1;
            tmp.y = 0;
            //set our current blocks hitSide, so we know where we hit it
            (*iter)->setHitSide(HIT_LSIDE);
            
            return tmp;
        }
        else if(ballPrevPos.x >= blockPos.x + 50)
        {
            //we are right above the block
            tmp.x = 1;
            tmp.y = 0;
            //set our current blocks hitSide, so we know where we hit it
            (*iter)->setHitSide(HIT_RSIDE);
            
            return tmp;
        }
        //or directly above within block width range
        else
        {
            //we are directly above the block within its width range
            tmp.x = 0;
            tmp.y = 1;
            //set our current blocks hitSide, so we know where we hit it
            (*iter)->setHitSide(HIT_BOTTOM);
            
            return tmp;
        }
    }
    
}

//see if there is a block left of current blocks coord
int Level::getBlockLeft(int w, int h)
{
    iter = npc.begin();
    int curIndex = w * height + h;
    advance(iter, curIndex);
    
    int curId = (*iter)->getID();
    int counter = 0;
    
    while(counter < BLOCK_SIZE)
    {
        //see if there is anything left of us
        iter = npc.begin();
        int index = (w - (counter + 1)) * height + h;
        advance(iter, index);

        //if the block on our left is empty space
        if((*iter)->getID() == -1)
        {
            if(counter > 0)
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
                return counter;
            }
            else
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
                return -1;
            }
        }
        //if its the same id as we are
        else if((*iter)->getID() == curId)
        {
            counter++;
        }
        else
        {
            if(counter > 0)
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
                return counter;
            }
            else
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
                return counter =  BLOCK_SIZE + 1;
            }
        }
    }
}
//see if there is a block right of current blocks coord
int Level::getBlockRight(int w, int h)
{
    iter = npc.begin();
    int curIndex = w * height + h;
    advance(iter, curIndex);
    
    int curId = (*iter)->getID();
    int counter = 0;
    
    while(counter < BLOCK_SIZE)
    {
        //see if there is anything right of us
        iter = npc.begin();
        int index = (w + (counter + 1)) * height + h;
        advance(iter, index);

        //if the block on our left is empty space
        if((*iter)->getID() == -1)
        {
            if(counter > 0)
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
                
                return counter;
            }
            else
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
                
                return -1;
            }
        }
        //if its the same id as we are
        else if((*iter)->getID() == curId)
        {
            counter++;
        }
        else
        {
            if(counter > 0)
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
                
                return counter;
            }
            else
            {
                //reset the iter to the current block
                iter = npc.begin();
                int curIndex = w * height + h;
                advance(iter, curIndex);
        
                return counter =  BLOCK_SIZE + 1;
            }
        }
    }
}
//if there is a block under us
int Level::getBlockDown(int w, int h)
{
    iter = npc.begin();
    int curIndex = w * height + (h + 1);
    advance(iter, curIndex);
    
    if((*iter)->isBlock)
    {
        //reset the iter to the current block
        iter = npc.begin();
        int curIndex = w * height + h;
        advance(iter, curIndex);
        
        return 1;
    }
    else
    {
        //reset the iter to the current block
        iter = npc.begin();
        int curIndex = w * height + h;
        advance(iter, curIndex);
        
        return 0;
    }
}
//if there is a block above us
int Level::getBlockUp(int w, int h)
{
    iter = npc.begin();
    int curIndex = w * height + (h - 1);
    advance(iter, curIndex);

    if((*iter)->isBlock)
    {
        //reset the iter to the current block
        iter = npc.begin();
        int curIndex = w * height + h;
        advance(iter, curIndex);
        
        return 1;
    }
    else
    {
        //reset the iter to the current block
        iter = npc.begin();
        int curIndex = w * height + h;
        advance(iter, curIndex);
        
        return 0;
    }
}

//get the hitSide of the block based on blocks pos
int Level::getEnemiesHit(int w, int h)
{
    iter = npc.begin();
    int curIndex = w * height + h;
    advance(iter, curIndex);
    
    int side = (*iter)->getHitSide();
    
    return side;
}

//get blocks centroid in x,y based off of pos w,h
vector Level::getEnemiesCentroid(int w, int h)
{
//    iter = npc.begin();
//    int curIndex = w * height + h;
//    advance(iter, curIndex);
    float x = w * GRID_SIZE_WIDTH;
    float y = h * GRID_SIZE_HEIGHT;
    
    vector center;
    center.x = x + 25;
    center.y = y + 7.5;
    
    return center;
}

//get the remaining number of blocks
int Level::blocksCount()
{
    int count = 0;
    for(iter = npc.begin(); iter != npc.end(); iter++)
    {
        if((*iter)->isBlock)
        {
            count++;
        }
    }
    return count;
}
//return the total number of created blocks
int Level::getTotalBlocks()
{
    return totalBlocks;
}
