/* 
 * File:   drawEngine.h
 * Author: nosferatu
 *
 * Created on June 26, 2015, 4:19 PM
 */

#ifndef DRAWENGINE_H
#define	DRAWENGINE_H


class DrawEngine
{
public:
    DrawEngine(int sizeW, int sizeH, wxBitmap *canvas);
    DrawEngine(int sizeW, int sizeH);
    
    ~DrawEngine();
    
    //create and manipulate drawing of the Sprites
    int createSprite(wxImage *sprite, int index);
    int createEmptySprite(wxImage *sprite, int index);
    void drawSprite(int index, int posx, int posy);
    void deleteSprite(int index, int posx, int posy);
    
    
    //create and manipulate drawing of the level tiles
    int createBgTile(wxImage *sprite, int index);
//    void drawTile(int index, int posx, int posy);
    void deleteTile(int posx, int posy);
    //set the tile to be empty at specific location
    void emptyTile(int posx, int posy);
    
    //get screen size
    float getScreenW();
    float getScreenH();
    //get the screen abs size x,y
    float getAbsScreenW();
    float getAbsScreenH();
    
    //point to the level map pointer
    void setMap(char **data);
    //draw our tiles method 
    void drawLevel();
    
    //connect our winCanvas to a bitmap
    void setWindow(wxBitmap *window, int width, int height);
    //clear the given region with the bg
    void drawBgRegion(const wxPoint& pos, const wxSize& size);
    //draw our background
    void drawBg();
    
private:
    
    
    
protected:
    //data members for holding draw related info
    //we will hold max of 16 different spriteImages (players and non players)
    wxBitmap *spriteImage[16];
    //we will hold max of 16 different empty sprites
    wxBitmap *emptySpriteImage[16];
    //we will hold max of 16 different tileImages (walls, blocks and so on)
    wxBitmap *tileImage[16];
    //this is the bitmap canvas we will paint our images on
    wxBitmap *winCanvas;
    //this is the background bitmap that we will keep on all the time
    wxBitmap *bg;
    
    char **map;
    int screenWidth, screenHeight;
};

#endif	/* DRAWENGINE_H */

