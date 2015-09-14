/* 
 * File:   AppFrame.h
 * Author: nosferatu
 *
 * Created on July 10, 2015, 7:51 PM
 */

#ifndef APPFRAME_H
#define	APPFRAME_H
#include "stdwx.h"

#include "level.h"
#include "drawEngine.h"
#include "ball.h"
#include "pongplayer.h"

//the ids for events in wxWidgets
enum ID_List
{
    ID_TIMER = 400,
};
//the ids for the state of the game
enum GameState
{
    STATE_NULL = 0,
    STATE_GAME_IN_PROGRESS,
    STATE_GAME_OVER,
    STATE_PLAYER_WON
};
//the id for the level types
enum LevelType
{
    REGULAR_LEVEL = 0,
    RANDOM_NEW_LEVEL,
    NEXT_PACKAGE_LEVEL
};

struct Level_info
{
    int grid_x;
    int grid_y;
    char **grid;
};

typedef list<Level_info*> Package;


class AppFrame : public wxFrame
{
public:
    AppFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~AppFrame();
    
    //event based methods
    //when user selects new, load, about or exit from the menu
    void onNew(wxCommandEvent& event);
    void onLoad(wxCommandEvent& event);
    void onAbout(wxCommandEvent& event);
    void onExit(wxCommandEvent& event);
    
    //when user presses a key 
    void onKey(wxKeyEvent& event);
    //or a time event condition is satisfied
    void onTimer(wxTimerEvent& event);
    //every time we repaint the window
    void paintEvent(wxPaintEvent& event);
    //if a window is resized
    void onSize(wxSizeEvent& event);
    
private:
    //go up one level when we have successfully completed the current level
    bool levelUp(LevelType ltype);
    //start a fresh new level
    bool startNewLevel(LevelType ltype);
    //update our game logic
    void updateGame();
    //update our game view
    void updateView();
    //draw the game information
    void drawInfo();
    
private:    
    //data members
    //main panel
    wxPanel *m_panel;
    wxPanel *m_info_panel;
    wxWindow *gameWindow;
        
    //info keeping 
    wxStaticText *m_playerLives;
    wxStaticText *m_blocksLeft;
    wxStaticText *m_score;
    wxStaticText *m_currentLevel;
    //keep the score and the current level info
    int currLevel;
    int score;
    
    //game specific variables
    Level *level;
    DrawEngine *drawArea;
    Pongplayer *player;
    Ball *ball;
    
    wxTimer *m_timer;
    //this is what we draw, everything is drawn here 
    //before its displayed on the screen
    //to prevent flickering  
    wxBitmap *backBuffer;
    wxBitmap *finalBackBuffer;
    GameState gameState;
    
    wxLogWindow *w;
    
private:
    wxDECLARE_EVENT_TABLE();
};


#endif	/* APPFRAME_H */

