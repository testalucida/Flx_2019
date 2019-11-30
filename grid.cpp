#include "flx/grid.h"

#include <FL/Fl_Group.H>

using namespace std;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Layout::Layout(Fl_Group* pGrp):
_pGrp(pGrp)
{
    _pGrp = pGrp;
}

void Layout::add(GridInfo* pInfo) {
    _grid.addGridInfo(pInfo);
}

void Layout::layout() {
    //do the layout row by row:
    for(int r = 0, rmax = _grid.getRowCount(); r < rmax; r++) {
        for(int c = 0, cmax = _grid.getColumnCount(); c < cmax; c++) {
            Cell cell = _grid.getCell(r, c);
            const GridInfo* pInfo = _grid.getGridInfo(r, c);
            layoutWidget(pInfo->pWidget, *pInfo, cell);
        }
    }
    
    //_pGrp->size(x, _pGrp->h());
}

void Layout::layoutWidget(Fl_Widget* pW, 
                          const GridInfo& info, const Cell& cell) 
{
    //to do: consider row and column span
    
    bool stickyN = (info.sticky.find("n") != string::npos);
    bool stickyS = (info.sticky.find("s") != string::npos);
    bool stickyW = (info.sticky.find("w") != string::npos);
    bool stickyE = (info.sticky.find("e") != string::npos);
    //calculate the amount (dW) of how much greater is the cell's width
    //compared to the widget's with including west and east paddings:
    int dW = cell.w - (pW->w() + info.pad.w + info.pad.e); 
    //calculate the amount (dH) of how much greater is the cell's height
    //compared to the widget's height including north and south paddings:
    int dH = cell.h - (pW->h() + info.pad.n + info.pad.s);
    
    int x = 0; 
    if(stickyW) {
        x = cell.x + info.pad.w; //that's the minimum value of x
        if(stickyE) {
            //stretch widget west-east:
            int newW = pW->w() + dW;
            pW->size(newW, pW->h());
        }
    } else if(stickyE) {
        x = dW;
    } else {
        //center widget horizontally:
        x = dW/2;
    }
    
    int y = 0;
    if(stickyN) {
        
    } else if(stickyS) {
        
    } else {
        //center widget horizontally
    }
    
}

//void Layout::layout() {
//    //do the layout row by row:
//    int x
//    
//    for(int r = 0, rmax = _grid.getRowCount(); r < rmax; r++) {
//        x = 0;
//        for(int c = 0, cmax = _grid.getColumnCount(); c < cmax; c++) {
//            const GridInfo* pInfo = _grid.getGridInfo(r, c);
//            if(pInfo) {
//                x += pInfo->pad.e;
//                pInfo->pWidget->position(x, 0);
//                x += pInfo->pWidget->w();
//                x += pInfo->pad.w;
//            }
//        }
//    }
//    _pGrp->size(x, _pGrp->h());
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Column::add(const GridInfo* pInfo) {
    _gridInfos.push_back(pInfo);
    if( pInfo ) {
        int w = pInfo->pWidget->w() + pInfo->pad.e + pInfo->pad.w;
        _w = (w > _w) ? w : _w;
    }
}

int Column::getHeight(int row) const {
    const GridInfo* pInfo = _gridInfos.at(row);
    return pInfo ? 
        pInfo->pad.n + pInfo->pWidget->h() + pInfo->pad.s
            : 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Grid::addGridInfo(const GridInfo* pInfo) {
    int n = _columns.size();
    while(n++ <= pInfo->column) {
        //e.g.: if pInfo's column index == 3 and _columns' size == 1
        //(column index == 0) then add 3 empty columns
        _columns.push_back(new Column());
    }
    //add pInfo to the appropriate column (matching index):
    Column* pCol = _columns.at(pInfo->column);
    int nCells = pCol->getCount();
    while(nCells++ < pInfo->row) {
        //if pInfo's row index > pCol's size
        //then add 'cells' until pCol's size == pInfo's row index + 1
        pCol->add((GridInfo*)NULL);
    }
    pCol->add(pInfo);
}

int Grid::getColumnWidth(int col) const {
    return -1;
}

int Grid::getRowHeight(int row) const {
    int h = 0;
    //iterate over all columns and look at all widgets stored
    //on position row. The maximum height determines the height of row 'row'.
    for(auto* pColumn : _columns) {
        int H = pColumn->getHeight(row);
        h = (H > h) ? H : h;
    }
    
//    for(auto it = _columns.begin(); it != _columns.end(); it++) {
//        Column* pCol = *it;
//        const GridInfo* pInfo = pCol->getGridInfo(row);
//        if(pInfo) {
//            int H = pInfo->pWidget->h() + pInfo->pad.n + pInfo->pad.s;
//            h = (H > h) ? H : h;
//        }
//    }
    
    
    return h;
}

Cell Grid::getCell(int row, int col) const {
    Cell cell;
    //The cell's width is determined by the referenced column's width:
    cell.w = _columns[col]->getWidth();
    //The cell's height is determined by the maximum height of all 
    //columns in the specified row
    //Furthermore, we get the cell's x position by adding 
    //all left sided columns' widths:
    cell.h = 0;
    cell.x = 0;
    for(auto* pCol : _columns) {
        int H = pCol->getHeight(row);
        cell.h = (H > cell.h) ? H : cell.h;
        cell.x += pCol->getWidth();
    }
    
    //To get cell.y we have to iterate over all GridInfos of an
    //arbitrary column:
    Column* pCol = _columns[0];
    cell.y = 0;
    for(int r = 0, rmax = pCol->getCount(); r < rmax; r++) {
        cell.y += pCol->getHeight(r);
    }
    
    return cell;
}

const GridInfo* Grid::getGridInfo(int row, int col) {
    return _columns[col]->getGridInfo(row);
}

Fl_Widget* Grid::getWidget(int row, int col) {
    const GridInfo* pInfo = getGridInfo(row, col);
    return pInfo->pWidget;
}

Grid::~Grid() {
    for(auto it = _columns.begin(); it != _columns.end(); it++) {
        delete *it;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Flx_GridManager::add(Fl_Group* pGrp, GridInfo* pInfo) {
    //does this pGrp already have a Layout?
    for(auto it = _layouts.begin(); it != _layouts.end(); it++) {
        Layout* pLay = *it;
        if(pLay->getGroup() == pGrp) {
            pLay->add(pInfo);
            return;
        }
    }
    
    Layout* pLay = new Layout(pGrp);
    pLay->add(pInfo);
    _layouts.push_back(pLay);
}

/**
 * Iterates over the given Fl_Group's Flx_...-children.
 * Determines the number of rows and columns and their heights resp. widths by
 * using each Flx-object's GridInfo.
 * Sets each child's width and height and clearance to its neighbours.
 * @param pGrp parent group to be layed out.
 */
void Flx_GridManager::doLayout(Fl_Group* pGrp) {
    for(auto it = _layouts.begin(); it != _layouts.end(); it++) {
        Layout *pLay = *it;
        if(pLay->getGroup() == pGrp) {
            pLay->layout();
            return;
        }
    }
    //todo throw LayoutNotFoundException
}

Layout* Flx_GridManager::getLayout(Fl_Group* pGrp) {
    for(auto it = _layouts.begin(); it != _layouts.end(); it++) {
        Layout* pLay = *it;
        if(pLay->getGroup() == pGrp) return pLay;
    }
    return NULL;
}

Flx_GridManager::~Flx_GridManager() {
    for( auto it = _layouts.begin(); it != _layouts.end(); it++) {
        delete *it;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

