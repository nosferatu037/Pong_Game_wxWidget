#include "stdwx.h"
#include "character.h"

Character::Character(DrawEngine* de, Level *lvl, int index, float x, float y, int numlives, 
                        char left_key, char right_key) 
                        : Sprite(de, lvl, index, x, y, numlives)
{
    hitSide = -1;
    //set our move keys
    leftKey = left_key;
    rightKey = right_key;
    //set our classID
    classID = CHAR_CLASSID;
    
    //set our start pos we can use later for reseting purposes
    startPos.x = x;
    startPos.y = y;
    
}
//move the pong
//overwritten pure virtual method from Sprite class
bool Character::keyPress(char c)
{
    if(c == leftKey)
    {
        return move(-15, 0);
    }
    else if(c == rightKey)
    {
        return move(15,0);
    }
    else
    {
        return false;
    }
}

//this implementation is when the ball is not caught
void Character::addLives(int num)
{
    //remove lives
    //for now the pong can only lose lives
    Sprite::addLives(num);
    //if we are still alive
    if(Sprite::isAlive())
    {
        //reset pongs pos
        resetPos();
    }
}

//draw the pong character
void Character::draw(float x, float y)
{
    //for the entire length of pong draw the pong
//    for(int i = CHAR_LEFT2; i <= CHAR_RIGHT2; i++){
        drawArea->drawSprite(spriteIndex, (int)(x),(int) y); 
//    drawArea->drawSprite(spriteIndex, x, y);
//    }
}

//erase the pong character
void Character::erase(float x, float y)
{
    //for the entire length of pong erase the pong
    drawArea->drawBgRegion(wxPoint(x, y), wxSize(74,15));
//    drawArea->deleteSprite(spriteIndex, int(x),(int)y); 
}

//check if we are hitting walls
bool Character::isValidMove(int posx, int posy)
{
    //we need to consider the offset for the pongs length
    if(posx >=0 && posy >= 0)
    {
        if((posx + 74) < drawArea->getAbsScreenW())
        {
            return true;
        }
    }
    return false;
}

//reset our position to the bottom center part of screen
void Character::resetPos()
{
    //erase current pos
    erase(pos.x, pos.y);
    //set the pos to bottom center of screen
    pos.x = startPos.x;
    pos.y = startPos.y;
    //draw our new pos
    draw(pos.x, pos.y);
}

//return hitSide of the char
int Character::getHitSide()
{
    return hitSide;
}

//set the hitSide of the char
void Character::setHitSide(int side)
{
    hitSide = side;
}

