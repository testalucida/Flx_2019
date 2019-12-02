#include <typeinfo>
#include <string>
#include <vector>
#include <algorithm>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "flx/flx_widgets.h"
#include "flx/grid.h"

using namespace std;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


int main (int argc, char ** argv) {
 
  Fl::scheme("gtk+");
  
  Flx_Window* pWin = new Flx_Window(200, 200);
  Flx_Button* pBtn = new Flx_Button("button to click", 0, 0);
  pBtn->setPaddingHorizontal(5);
  
  pBtn = new Flx_Button("another button", 0, 1);
  pBtn->setPaddingHorizontal(10, 5);
  
  pBtn = new Flx_Button("button in second row", 1, 0);
  pBtn->setPaddingHorizontal(5);
  
  pWin->end();
  pWin->show (argc, argv);

  return(Fl::run());
}
