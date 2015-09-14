//load all the headers and the precompiled headers
#include "stdwx.h"
#include "AppFrame.h"

//define the image size
#define GRID_SIZE_WIDTH     50
#define GRID_SIZE_HEIGHT    15

//define the grid cells for the game
#define LEVEL_X             16
#define LEVEL_Y             56

//implement all the events
wxBEGIN_EVENT_TABLE(AppFrame, wxFrame)
    EVT_MENU(wxID_NEW, AppFrame::onNew)
    EVT_MENU(wxID_OPEN, AppFrame::onLoad)
    EVT_MENU(wxID_ABOUT, AppFrame::onAbout)
    EVT_MENU(wxID_EXIT, AppFrame::onExit)
    EVT_TIMER(ID_TIMER, AppFrame::onTimer)
wxEND_EVENT_TABLE()

#define UPDATE_TIME     2

//our main appFrame class
//flag the style not to resize the window so we dont have to deal with redrawing and scaling the images
AppFrame::AppFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
            :wxFrame(NULL, wxID_ANY, title, pos, size, 
                    wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    //set all of our pointers to NULL
    backBuffer = NULL;
    player = NULL;
    ball = NULL;
    level = NULL;
    
    //increase our curr level to 1
    currLevel = 1;
    //set our score to 0
    score = 0;
    
    //WINDOW DRAWING AND MANIPULATING COMMANDS
    wxMenuBar *menuBar = new wxMenuBar;
    
    //setup the main menu File
    wxMenu *menuFile = new wxMenu;
    
    menuFile->Append(wxID_NEW, "&New");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_OPEN, "&Load");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_ABOUT, "&About");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "E&xit");
    
    menuBar->Append(menuFile, "&File");
    
    SetMenuBar(menuBar);
    
    CreateStatusBar();
    SetStatusText("Welcome to Pong game!");
    
    //create the main window panel
    m_panel = new wxPanel(this, wxID_ANY);
    
    //create the info panel                                         wxSize(width,height)
    m_info_panel = new wxPanel(m_panel, wxID_ANY, wxDefaultPosition, wxSize(200,840), wxBORDER_THEME);
    
    //create the game window
    gameWindow = new wxWindow(m_panel, wxID_ANY, wxDefaultPosition, wxSize(800,840), wxBORDER_THEME);
    
    //setup window painting event
    gameWindow->Connect(wxID_ANY, -1, wxEVT_PAINT, 
                            (wxObjectEventFunction)& AppFrame::paintEvent, NULL, this);
    //setup key capturing event
    gameWindow->Connect(wxID_ANY, -1, wxEVT_KEY_DOWN,
                            (wxObjectEventFunction)& AppFrame::onKey, NULL, this);
    
    
    m_playerLives = new wxStaticText(m_info_panel, wxID_ANY, "Lives: ", 
                                    wxDefaultPosition, wxSize(200,15), wxST_NO_AUTORESIZE);
    
    m_blocksLeft = new wxStaticText(m_info_panel, wxID_ANY, "Blocks left: ", 
                                    wxDefaultPosition, wxSize(200,15), wxST_NO_AUTORESIZE);
    
    m_score = new wxStaticText(m_info_panel, wxID_ANY, "Score: ", 
                                    wxDefaultPosition, wxSize(200,15), wxST_NO_AUTORESIZE);
    
    m_currentLevel = new wxStaticText(m_info_panel, wxID_ANY, "Level: ", 
                                    wxDefaultPosition, wxSize(200,15), wxST_NO_AUTORESIZE);
    
    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(gameWindow, 0, wxGROW|wxALL|wxSTRETCH_NOT, 0);
    mainSizer->Add(m_info_panel, 0 , wxGROW|wxALL|wxSTRETCH_NOT, 0);
    
    wxBoxSizer *subsizer = new wxBoxSizer(wxVERTICAL);
    subsizer->Add(m_playerLives, 0, wxLEFT|wxTOP, 5);
    subsizer->Add(m_blocksLeft, 0, wxLEFT|wxTOP, 5);
    subsizer->Add(m_score, 0, wxLEFT|wxTOP, 5);
    subsizer->Add(m_currentLevel, 0, wxLEFT|wxTOP, 5);
    
    m_panel->SetAutoLayout(true);
    m_panel->SetSizer(mainSizer);
    
    m_info_panel->SetAutoLayout(true);
    m_info_panel->SetSizer(subsizer);
    
    //setup our image handlers so we can load our images
    wxInitAllImageHandlers();
    
    //setup our drawing area
    backBuffer = new wxBitmap(GRID_SIZE_WIDTH * LEVEL_X, GRID_SIZE_HEIGHT * LEVEL_Y);
    
    //load our images
    wxImage emptyBlock, filledBlock, pongPlayer, ballImg;
    emptyBlock.LoadFile("Data/emptyBlock.tif");
    filledBlock.LoadFile("Data/block50x15.bmp");
    
    pongPlayer.LoadFile("Data/pongPlayer.tif");
    ballImg.LoadFile("Data/ball15x15.tif");
    
    //when creating the draw area, we now forward the backBuffer for init
    drawArea = new DrawEngine(LEVEL_X, LEVEL_Y, backBuffer);
    
    drawArea->createSprite(&pongPlayer, SPRITE_PLAYER);
    drawArea->createSprite(&ballImg, SPRITE_BALL);
    
    drawArea->createBgTile(&emptyBlock, TILE_EMPTY);
    drawArea->createBgTile(&filledBlock, TILE_BLOCK);
    
    //new timer
    //whenever the timers UPDATE_TIME is true, 
    //we run the AppFrame::onTimer function
    m_timer = new wxTimer(this, ID_TIMER);
    m_timer->Start(UPDATE_TIME);
    
    //randomize seed based on the elapsed time
//    wxStopWatch *timer = new wxStopWatch();
//    //get the elapsed time
//    srand(timer->Time());
//    //reset the timer
//    timer->Start();
    
    //setup our ball and player pos in x,y
    vector ballPos, playerPos;
    ballPos.x = 400 - 7.5;
    ballPos.y = 760 - 15;
    
    playerPos.x = 400 - 37;
    playerPos.y = 760;
    
    //create a new level
    level = new Level(drawArea, false);
    //create a new player/pong at bottom screen center
    player = new Pongplayer(drawArea, level, 0, playerPos.x, playerPos.y);
    ball = new Ball(drawArea, level, 1, ballPos.x, ballPos.y);

    //add the player and the ball to our level
    level->addPlayer(player);
    level->addBall(ball);
    
    //draw the player and ball at their pos
    player->draw(playerPos.x, playerPos.y);
    ball->draw(ballPos.x, ballPos.y);
    
    //connect the ball to the player 
    player->setBall(ball);
    //connect the player to the ball
    ball->setPlayer(player);
    
    //draw our level
    level->draw();
    
    //set the game state to game in progress
    gameState = STATE_GAME_IN_PROGRESS;
    
//    w = new wxLogWindow(this, wxT("Log"), true, false);
}

//destroy and clear all our ptrs
AppFrame::~AppFrame()
{
    gameState = STATE_NULL;
    
    m_timer->Stop();
    
    delete m_timer;
    
    if(player)
        delete player;
    
    if(ball)
        delete ball;
    
    if(level)
        delete level;
    
}

/**********************wxWIDGET EVENT METHODS***************************/
//when File->New is selected
void AppFrame::onNew(wxCommandEvent& event)
{
    //reset the timer
    m_timer->Stop();
    
    //delete player
    if(player)
        delete player;
    
    //delete ball
    if(ball)
        delete ball;
    
    //delete level
    if(level)
        delete level;
    
    //start a brand new level
    startNewLevel(RANDOM_NEW_LEVEL);
    
    //increase our curr level to 1
    currLevel = 1;
    
    //change our gameState to indicate
    //we are in progress of playing a game
    gameState = STATE_GAME_IN_PROGRESS;
    
    //start the timer
    m_timer->Start(UPDATE_TIME);               
                
}

//when File->Load is selected
void AppFrame::onLoad(wxCommandEvent& event)
{

}

//when File->About is selected
void AppFrame::onAbout(wxCommandEvent& event)
{
    wxMessageBox("Pong Game, tried by nosferatu :)", "Pong Game Info", wxOK | wxICON_INFORMATION);
}

//when File->Exit is selected
void AppFrame::onExit(wxCommandEvent& event)
{
    Close(true);
}


//events functions
//function when our window is being redrawn
void AppFrame::paintEvent(wxPaintEvent& event)
{
    //set the paint device context to our gameWindow
    wxPaintDC dc(gameWindow);
    //clear it just in case
    dc.Clear();
    //update our gameWindow/ paint it
    updateView();

}

//on every hit key
void AppFrame::onKey(wxKeyEvent& event)
{
    //if we are playing a game
    if(gameState == STATE_GAME_IN_PROGRESS)
    {
        //get our key press
        level->keyPress(event.GetKeyCode());
    }
    //update our view, repaint the window
    updateView();
    
}
//on time function
void AppFrame::onTimer(wxTimerEvent& event)
{
    //if a game is in progress
    if(gameState == STATE_GAME_IN_PROGRESS)
    {
        //update our game 
        updateGame();
        //keep our score, so it can go to the next level
        score = (level->getTotalBlocks() - level->blocksCount()) * 150;
    }
    
}

/**********************wxWIDGET EVENT METHODS***************************/

//start a fresh new level from scratch
bool AppFrame::startNewLevel(LevelType ltype)
{
    //reset the score back to 0
    score = 0;
    
    //delete our backbuffer
    if(backBuffer)
        delete backBuffer;
    
    //dependent on what we pass as a level type
    switch(ltype)
    {
        case RANDOM_NEW_LEVEL:
        {
            //create a new random level
            level = new Level(drawArea, true);
            break;
        }
        case REGULAR_LEVEL:
        {
            //create a new regular level
            level = new Level(drawArea, false);
            break;
        }
    }
    
    //create our backBuffer bitmap 
    backBuffer = new wxBitmap(GRID_SIZE_WIDTH * LEVEL_X, GRID_SIZE_HEIGHT * LEVEL_Y);
    //connect our drawArea to backBuffer again since we are starting a brand new level
    drawArea->setWindow(backBuffer, LEVEL_X, LEVEL_Y);
    //draw the background constantly
    drawArea->drawBg();
    
    //create a new player/pong at bottom screen center
    player = new Pongplayer(drawArea, level, 0, 400 - 37, 760);
    ball = new Ball(drawArea, level, 1, 400 - 7.5, 760-15);
    
    //connect the player and the ball to our level
    level->addPlayer(player);
    level->addBall(ball);
    //and draw them at their pos
    player->draw(400 - 37, 760);
    ball->draw(400 - 7.5, 760-15);
    //connect the ball to the player 
    player->setBall(ball);
    //connect the player to the ball
    ball->setPlayer(player);
    //draw our level
    level->draw();

    return true;
}

//level up after finishing previous level
bool AppFrame::levelUp(LevelType ltype)
{
    //delete the previous level
    if(level)
        delete level;
    
    //delete the previous buffer
    if(backBuffer)
        delete backBuffer;
    
    //dependent on what we pass as a level type
    switch(ltype)
    {
        case RANDOM_NEW_LEVEL:
        {
            //create a new random level
            level = new Level(drawArea, true);
            break;
        }
        case REGULAR_LEVEL:
        {
            //create a new regular level
            level = new Level(drawArea, false);
            break;
        }
    }
    //create a new backbuffer
    backBuffer = new wxBitmap(GRID_SIZE_WIDTH * LEVEL_X, GRID_SIZE_HEIGHT * LEVEL_Y);
    //reconnect the drawArea to the buffer
    drawArea->setWindow(backBuffer, LEVEL_X, LEVEL_Y);
    //draw the bg
    drawArea->drawBg();
    //reconnect the player and the ball to the level
    level->addPlayer(player);
    level->addBall(ball);
    //reset player and ball pos
    player->resetPos();
    ball->resetPos();

    //draw the level
    level->draw();

    return true;
}

//update the game function
void AppFrame::updateGame()
{
    //cat our strings
    //store the player lives, the block count, current level and score
    wxString infoLives = "Lives: " + wxString::Format("%d", player->getLives());
    wxString infoLevel = "Level: " + wxString::Format("%d", currLevel);
    wxString infoBlocks = "Blocks Left: " + wxString::Format("%d", level->blocksCount());
    wxString infoScore = "Score: " + wxString::Format("%d", score);
    
    //set our wxStaticText members to display current info
    m_playerLives->SetLabel(infoLives);
    m_currentLevel->SetLabel(infoLevel);
    m_blocksLeft->SetLabel(infoBlocks);
    m_score->SetLabel(infoScore);
    
    //if we have killed all enemies
    if(level->blocksCount() == 0)
    {
        //it means we won the level
        
        //stop the timer
        m_timer->Stop();
        
        //increase the level
        currLevel++;
        
        //start a new random level/ level up
        levelUp(RANDOM_NEW_LEVEL);
        
        //start our timer
        m_timer->Start(UPDATE_TIME);
    }
    //else if we have died
    else if(level->blocksCount() > 0 && !player->isAlive())
    {
        //set the game state
        gameState = STATE_GAME_OVER;
        //stop the timer
        m_timer->Stop();
    }
    //else we are still playing and fighting enemies
    else
    {
        //pass time in ms the program took to execute all the code from the constructor
        //where we init randtimer to 0, or restarted the stopwatch
        level->update();
    }
    
    //update/paint our view
    updateView();
}

//update our window pain
//basically this repaints the window constantly
void AppFrame::updateView()
{
    //create a client device context and connect it to gameWindow
    wxClientDC area(gameWindow);
//    wxSize clientArea = gameWindow->GetClientSize();
//    wxLogMessage(wxT("gridSizeX, gridSizeY : %i, %i"), clientArea.GetWidth(), clientArea.GetHeight());
    //if the game is still in progress
    if(gameState == STATE_GAME_IN_PROGRESS)
    {
        //keep painting our game
        area.DrawBitmap(*backBuffer, wxPoint(0,0));
    }
    //if it isnt
    else if(gameState == STATE_GAME_OVER)
    {
        //draw the info telling us what happened
        drawInfo();
    }
}

//draw the information on the gameWindow, to let user know what they need to do
void AppFrame::drawInfo()
{
    //get the size of the gameWindow
    wxSize clientArea = gameWindow->GetClientSize();
    if(finalBackBuffer)
        delete finalBackBuffer;

    //create a new finalback buffer with gameWindow size
    finalBackBuffer = new wxBitmap(clientArea.GetWidth(), clientArea.GetHeight());

    //create a finalDC to hold the obj
    wxMemoryDC finalDC;
    
    //connect it to finalBackBuffer
    finalDC.SelectObject(*finalBackBuffer);
    
    //clear if there was anything in the buffer
    finalDC.Clear();
    
    //draw the rectangle region 
    finalDC.DrawRectangle(0, 0, clientArea.GetWidth(), clientArea.GetHeight());
    
    //store the text message
    wxString message;
    
    //based on the gamestate we print a different message
    if(gameState == STATE_NULL)
    {
        message = "Go to File > New to start a new game!";
    }
    else if(gameState == STATE_PLAYER_WON)
    {
        message = "You have won the game!";
    }
    else if(gameState == STATE_GAME_OVER)
    {
        message = "GAME OVER!";
    }
        
    int txtWidth, txtHeight;
    
    //get the width and height of the message and place it in txtWidth/txtHeight
    finalDC.GetTextExtent(message, &txtWidth, &txtHeight);
    
    //create a wxPoint obj at specified center
    wxPoint center((clientArea.GetWidth() - txtWidth) / 2, (clientArea.GetHeight() - txtHeight) / 2);
    //draw the text at the specified center
    finalDC.DrawText(message, center);
    
    //this is where we are going to see the info
    wxClientDC screenDC(gameWindow);
    
    //since we stored everything in memoryDC(finalDC) we need to place it in our clientDC(screenDC)
    //basically we wont see the info message in our window unless we blit the memory to our screen
    //all we need to do is print it from the top left corner 0,0
    //first is destPos, clientArea is the newSize, &finalDC is the source we are looking from 
    screenDC.Blit(wxPoint(0,0), clientArea, &finalDC, wxPoint(0,0));
    
    //clear our memoryDC
    finalDC.SelectObject(wxNullBitmap);
}