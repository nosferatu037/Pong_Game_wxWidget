#include "stdwx.h"
//#include <curses.h>

#include "drawEngine.h"
#include "sprite.h"
#include "level.h"

#define GRID_SIZE_WIDTH     50
#define GRID_SIZE_HEIGHT    15

//constructor, init the screen size and canvas
DrawEngine::DrawEngine(int sizeW, int sizeH, wxBitmap *canvas) {
    
    setWindow(canvas, sizeW, sizeH);
    drawBg();
}
//different constructor for when we dont need to init the canvas
//when we are leveling up, we might need this ctor
DrawEngine::DrawEngine(int sizeW, int sizeH)
{
    screenHeight = sizeH;
    screenWidth = sizeW;
    
    map = NULL;
}

//destructor set the curs back
DrawEngine::~DrawEngine()
{

}

//create the sprite, or assign a char for sprite drawing
int DrawEngine::createSprite(wxImage *sprite, int index)
{
    //we can only hold 16 sprites, make sure thats the case
    if(index >= 0 && index < 16)
    {   
        //set the sprite to the current bitmap
        spriteImage[index] = new wxBitmap(*sprite);

        return index;
    }
    return -1;
    
}

//draw the curr sprite at specific pos
void DrawEngine::drawSprite(int index, int posx, int posy)
{
    //memory device context for drawing the bitmap
    wxMemoryDC dc;
    //associate the dc with our canvas or where we want to draw the bitmap
    dc.SelectObject(*winCanvas);
    dc.DrawBitmap(*spriteImage[index], wxPoint(posx, posy), true);
    //deassociate the bitmap with the device context
    dc.SelectObject(wxNullBitmap);
}

//erase the curr sprite
//this is not the best way to do it, as a matter of fact it doesnt even work :)
void DrawEngine::deleteSprite(int index, int posx, int posy)
{
    wxMemoryDC dc;
    dc.SelectObject(*winCanvas);
    dc.DrawBitmap(*emptySpriteImage[index], wxPoint(posx, posy), true);
    dc.SelectObject(wxNullBitmap);
}

//we create an empty sprite image, the logic is that this is the sprite we keep paintin
//when we want to delete a block or move the ball or player
//it doesnt work very well
int DrawEngine::createEmptySprite(wxImage* sprite, int index)
{
    if(index >= 0 && index < 16)
    {
        emptySpriteImage[index] = new wxBitmap(*sprite);
        return index;
    }
    return -1;
}

//create a sprite for our blocks walls or empty spaces
//this is the stuff ball will be colliding against
int DrawEngine::createBgTile(wxImage *sprite, int index)
{
    if(index >= 0 && index < 16)
    {
        tileImage[index] = new wxBitmap(*sprite);
        return index;
    }
    return -1;
    
}

//delete our block tiles
//the logic is that we allocate an area thats the size of our block
//and we paint the bg into that area, which in general cleares or deletes the tile
void DrawEngine::deleteTile(int posx, int posy)
{
    drawBgRegion(wxPoint(posx * GRID_SIZE_WIDTH, posy * GRID_SIZE_HEIGHT), wxSize(50, 15));
    map[posx][posy] = TILE_EMPTY;
}


//set the tile to be empty at specific location
void DrawEngine::emptyTile(int posx, int posy)
{
    map[posx][posy] = TILE_EMPTY;
}

//get the screen width
float DrawEngine::getScreenW()
{
    return screenWidth;
}

//get the screen height
float DrawEngine::getScreenH()
{
    return screenHeight;
}

//get the absolute screen size in x,y
float DrawEngine::getAbsScreenH()
{
    return screenHeight * GRID_SIZE_HEIGHT;
}
//get the absolute screen size in x,y
float DrawEngine::getAbsScreenW()
{
    return screenWidth * GRID_SIZE_WIDTH;
}

//set the map to point to level construct
void DrawEngine::setMap(char **data)
{
    map = data;
}

//draw our tiles method
void DrawEngine::drawLevel()
{
    //since we will be drawing everything in the winCanvas
    //and we will not display it until after
    //we need a memoryDC
    wxMemoryDC dc;
    
    dc.SelectObject(*winCanvas);
    
    if(map)
    {
        for(int w = 0; w < screenWidth; w++)
        {    
            for(int h = 0; h < screenHeight; h++)
            {
                dc.DrawBitmap(*tileImage[map[w][h]], 
                        wxPoint(w * GRID_SIZE_WIDTH, h * GRID_SIZE_HEIGHT), true);
            }
        }
    }
    dc.SelectObject(wxNullBitmap);
}

//set the bitmap we created in the main class to the draw engine
//so we know where to draw stuff
void DrawEngine::setWindow(wxBitmap* window, int width, int height)
{
    winCanvas = window;
    screenWidth = width;
    screenHeight = height;
}

//init and always draw the background
void DrawEngine::drawBg()
{
    //set and load the new background in memory
    bg = new wxBitmap();
    bg->LoadFile("Data/pongBG.bmp", wxBITMAP_DEFAULT_TYPE);
    wxMemoryDC background;
    background.SelectObject(*winCanvas);
    background.DrawBitmap(*bg, wxPoint(0,0));
    background.SelectObject(wxNullBitmap);
}

//just draw our object at a specific region of the bg
void DrawEngine::drawBgRegion(const wxPoint& pos, const wxSize& size)
{
    
    //get a rectangle region of our current objs pos
    wxRect region(pos, size);

    //new dc for painting
    wxMemoryDC paintReg;
    paintReg.SelectObject(*winCanvas);
    //extract a section of the bg at the specific region
    wxBitmap newbmp = bg->GetSubBitmap(region);
    //paint that section
    paintReg.DrawBitmap(newbmp, pos, true);
    paintReg.SelectObject(wxNullBitmap);
    
}
