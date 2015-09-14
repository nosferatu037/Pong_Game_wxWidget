/* 
 * File:   pongplayer.h
 * Author: nosferatu
 *
 * Created on June 27, 2015, 8:23 PM
 */

#ifndef PONGPLAYER_H
#define	PONGPLAYER_H
#include "character.h"
#include "ball.h"

class Pongplayer : public Character
{
    public:
        Pongplayer(DrawEngine *de, Level *lvl, int index, float x = 1, float y = 1, int numlives = 3,
                char left_key = WXK_LEFT, char right_key = WXK_RIGHT, char spell = WXK_SPACE);
        
        //catch users input key
        bool keyPress(char c);
        //set the ball players ptr
        void setBall(Ball *b);

        
    protected:
        //if we have hit space then make the ball move
        void castSpell();
            
    private:
        char spellKey;
        Ball *ball;

};


#endif	/* PONGPLAYER_H */

