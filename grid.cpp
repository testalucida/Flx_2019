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
    int x = 0;
    for(int r = 0, rmax = _grid.getRowCount(); r < rmax; r++) {
        x = 0;
        for(int c = 0, cmax = _grid.getColumnCount(); c < cmax; c++) {
            const GridInfo* pInfo = _grid.getGridInfo(r, c);
            if(pInfo) {
                x += pInfo->pad.e;
                pInfo->pWidget->position(x, 0);
                x += pInfo->pWidget->w();
                x += pInfo->pad.w;
            }
        }
    }
    _pGrp->size(x, _pGrp->h());
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Column::add(const GridInfo* pInfo) {
    _gridInfos.push_back(pInfo);
    if( pInfo ) {
        int w = pInfo->pWidget->w() + pInfo->pad.e + pInfo->pad.w;
        _w = (w > _w) ? w : _w;
    }
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
    //iterate through all columns and look at all widgets stored
    //on position row. The highest height determines the height of row 'row'.
    for(auto it = _columns.begin(); it != _columns.end(); it++) {
        Column* pCol = *it;
        const GridInfo* pInfo = pCol->getGridInfo(row);
        if(pInfo) {
            int H = pInfo->pWidget->h() + pInfo->pad.n + pInfo->pad.s;
            h = (H > h) ? H : h;
        }
    }
    return h;
}

const GridInfo* Grid::getGridInfo(int row, int col) {
    return _columns[col]->getGridInfo(row);
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
 * Iterates through the given Fl_Group's Flx_...-children.
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

