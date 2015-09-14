/* 
 * File:   main.cpp
 * Author: nosferatu
 *
 * Created on July 10, 2015, 7:04 PM
 */


#include "stdwx.h"

#include "AppFrame.h"

class PongGame : public wxApp
{
    //initialize our application
    virtual bool OnInit();
};

wxIMPLEMENT_APP(PongGame);

bool PongGame::OnInit()
{
    AppFrame *frame = new AppFrame("Pong game!", wxPoint(100, 100), wxSize(1000, 840));
    
    frame->Show();
    
    SetTopWindow(frame);
    
    return true;
}