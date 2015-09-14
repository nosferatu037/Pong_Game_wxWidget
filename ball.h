/* 
 * File:   ball.h
 * Author: nosferatu
 *
 * Created on June 27, 2015, 8:09 PM
 */

#ifndef BALL_H
#define	BALL_H

#include "sprite.h"
#include "character.h"

//#include "level.h"

//enums for collision normal detection
enum hit
{
    HIT_NOHIT = 0,
    HIT_LSIDE = 1,
    HIT_RSIDE,
    HIT_BOTTOM,
    HIT_TOP,
    HIT_PLAYER
};

class Ball : public Sprite
{
    public:
        Ball(DrawEngine *de, Level *lvl, int index, float x = 1, float y = 1, 
                int numlives = 1, char left_key = WXK_LEFT, char right_key = WXK_RIGHT);
        
        //method for updating balls information 
        void update();
        //set our player ptr to point to current player
        void setPlayer(Character *p);

        //move our ball based off of facingDir
        //this is a relative move
        bool move(float x, float y);
        //move the ball at specific location
        //this is an absolute move
        bool moveToPos(float x, float y);

        //check if we can move method
        int checkMove(int posx, int posy);
        
        //method for ball - wall collision detection
        //true - we are not hitting the wall, false - we are hitting wall
        bool isNotHittingWall(int posx, int posy);
        
        //method for ball - player collision detection
        //true - not hitting player, false - hitting a player
        bool isNotHittingPlayer(int posx, int posy);
        
        //method for ball life 
        //true - we are not dead, false - we are dead
        bool notDead(int posx, int posy);

        //add/remove lives from the ball
        void addLives(int num);
        
        //draw the ball method
        void draw(float x, float y);
        //erase the ball method
        void erase(float x, float y);
        
        //catch the users key input and
        //move the ball with pong if it hasnt been launched yet
        bool keyPress(char c);
        
        //set the ball status
        //ballOn = true if the ball has been launched
        //ballOn = false if its still sitting on the player
        void setBallOn(bool set);
        //return the ball status
        bool getBallStatus();
        
        //get facing direction
        vector getFacingDir();
        
        //reset the balls position and the center bottom screen 
        //on top of the pong player, also reset the facingDirection
        void resetPos();
        
        //return a dot product between two vectors
        float dotProduct(vector dir, vector norm);
       
        //calculate the reflection vector when we collide
        //between our direction and enemies normal
        vector reflect(vector dir, vector norm);
        
        //calc vectors magnitude
        float vecMag(vector Vec);
        
        //normalize a vector
        vector normalize(vector Vec);
        //get the balls centroid
        vector getCentroid();
        
    protected:
        //vector return methods
        //avoid ball to be drawn on top of the block, or player, or clipped by walls
        //clamp balls facingDir so it never overshoots the walls
        vector clampBounds();
        //clamp balls facingDir so it never overshoots the players pos
        vector clampPlayer();
        //clamp balls facingDir so it never overshoots the blocks pos
        vector clampBlocks(float x, float y);
        
        //return the sign of the number pos, neg
        int sign(float num);
        
        //members for storing ball related data
        //see if ballOn is true(has started moving)
        //or false(is still on top of player)
        bool ballOn;
        //catch the left right key
        char leftKey, rightKey;
        //ptr to player so we can communicate with him
        Character *player;
        //count if we have move from pongs pos
        //if so increase this so we dont go into specific loops later
        int moveCounter;
        //store the side ball hit
        hit hitSide;
        //ptr to blocks so we can query stuff and communicate
        Blocks *block;
        //set the start pos so we can easily reset to it
        vector startPos;
        
    private:
        //bounce the ball method calculation
        //the most important part of the game
        //bounce the ball at specific location,
        //and also flag true/false if we are bouncing off of a block we hit
        bool bounceBall(float x, float y, bool block);
                
};

#endif	/* BALL_H */

