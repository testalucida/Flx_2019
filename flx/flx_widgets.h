/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   flx_widgets.h
 * Author: martin
 *
 * Created on 29. November 2019, 09:36
 */

#ifndef FLX_WIDGETS_H
#define FLX_WIDGETS_H

#include <FL/Fl_Button.H>
#include "grid.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Flx_Group : public Fl_Group {
public:
    Flx_Group(Flx_Group* pParent, int row, int col);
    void add(Fl_Widget& w);
private:
    //Flx_Grid _grid;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Flx_Window : public Fl_Window {
    public:
        Flx_Window(int w, int h, const char *pLabel=0);
        void end();
private:
    //Flx_Grid _grid;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Flx_Button : public Fl_Button, public GridInfo {
public:
    Flx_Button(const char* pLbl, int row, int col);
    
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif /* FLX_WIDGETS_H */

