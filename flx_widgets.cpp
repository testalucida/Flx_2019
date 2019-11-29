/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "flx/flx_widgets.h"

#include <FL/Fl_Group.H>
#include "flx/fontsncolors.h"

using namespace std;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Flx_Group::Flx_Group(Flx_Group* pParent, int row, int col)
: Fl_Group(0, 0, 0, 0, NULL)
{
    
}

void Flx_Group::add(Fl_Widget& w) {
    //_grid.calculatePosition(w);
    Fl_Group::add(w);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Flx_Window::Flx_Window(int w, int h, const char* pLabel) 
: Fl_Window(w, h, pLabel)
{
   
}

void Flx_Window::end() {
    Fl_Window::end();
    Flx_GridManager::inst().doLayout(this);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Flx_Button::Flx_Button( const char* pLbl, int row, int col)
: Fl_Button(0, 0, 0, 0, pLbl), GridInfo()
{
    this->pWidget = this;
    this->row = row;
    this->column = col;
    int W = 0, H = 0;
    this->measure_label(W, H);
    h(H+8);
    w(W+8);
    Flx_GridManager::inst().add(parent(), (GridInfo*)this);
    
    //color(buttoncolor);
}