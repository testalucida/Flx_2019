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
    int h = 0;
    for(int r = 0, rmax = _grid.getRowCount(); r < rmax; r++) {
        h += _grid.getRowHeight(r);
        for(int c = 0, cmax = _grid.getColumnCount(); c < cmax; c++) {
            Cell cell = _grid.getCell(r, c);
            const GridInfo* pInfo = _grid.getGridInfo(r, c);
            if(pInfo) {
                layoutWidget(pInfo->pWidget, *pInfo, cell);
            }
        }
    }
    
    Cell mostRight = _grid.getCell(0, _grid.getColumnCount()-1);
    int w = mostRight.x + mostRight.w;
    
    _pGrp->size(w, h);
}

void Layout::layoutWidget(Fl_Widget* pW, 
                          const GridInfo& info, const Cell& cell) 
{
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
        y = cell.y + info.pad.n;
        if(stickyS) {
            //stretch widget north-south
            int newH = pW->h() + dH;
            pW->size(pW->w(), newH);
        }
    } else if(stickyS) {
        y = dH;
    } else {
        //center widget horizontally
        y = dH/2;
    }
    
    pW->position(x, y);
    
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void GridInfo::setPaddingHorizontal(int w, int e) {
    pad.w = w;
    if(e > -1) pad.e = e;
}

void GridInfo::setPaddingVertical(int n, int s) {
    pad.n = n;
    if(s > -1) pad.s = s;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Column::Column(int rows) {
    for(int r = 0; r < rows; r++) {
        _gridInfos.push_back((GridInfo*)NULL);
    }
}

void Column::add(const GridInfo* pInfo) {
    _gridInfos.push_back(pInfo);
    if( pInfo ) {
//        int w = pInfo->pWidget->w() + pInfo->pad.e + pInfo->pad.w;
//        _w = (w > _w) ? w : _w;
    }
}

void Column::setGridInfo(const GridInfo* pInfo, int row) {
    _gridInfos.at(row) = pInfo;
}

int Column::getWidth() const {
    int w = 0;
    for(auto* pInfo : _gridInfos) {
        int W = pInfo ? 
                pInfo->pWidget->w() + pInfo->pad.e + pInfo->pad.w :
                0;
        w = (W > w) ? W : w;
    }
    return w;
}

int Column::getHeight(int row) const {
    const GridInfo* pInfo = _gridInfos.at(row);
    return pInfo ? 
        pInfo->pad.n + pInfo->pWidget->h() + pInfo->pad.s
            : 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Grid::addColumns(int nCols, int nRows) {
    for(int c = 0; c < nCols; c++) {
        Column *pCol = new Column(nRows);
        _columns.push_back(pCol);
    }
}

void Grid::addRows(int nRows) {
    for(auto* pCol : _columns) {
        for(int r = 0; r < nRows; r++) {
            pCol->add((GridInfo*)NULL);
        }
    }
}

void Grid::addGridInfo(const GridInfo* pInfo) {
    //add columns and rows as needed:
    int nColsNeeded = pInfo->column - _columns.size() + 1;
    int nRows = getRowCount();
    int nRowsNeeded = pInfo->row - nRows + 1;
    if(nColsNeeded > 0) {
        addColumns(nColsNeeded, nRows);
    }
    if(nRowsNeeded > 0) {
        addRows(nRowsNeeded);
    }
    
    Column* pCol = _columns.at(pInfo->column);
    pCol->setGridInfo(pInfo, pInfo->row);
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

    return h;
}

Cell Grid::getCell(int row, int col) const {
    Cell cell;
    //The cell's width is determined by the referenced column's width:
    cell.w = _columns[col]->getWidth();
    // check columnspan and extend cell.w if necessary:
    const GridInfo* pInfo = _columns[0]->getGridInfo(row);
    if(pInfo->columnspan > 1) {
        //get width of next cell(s) to the right:
        int colcnt = getColumnCount();
        int cmax = col + pInfo->columnspan;
        cmax = (cmax > colcnt) ? colcnt : cmax;
        for(int c=col+1; c < cmax; c++) {
            cell.w += _columns[c]->getWidth();
        }
    }
    
    //The cell's height is determined by the maximum height of all 
    //columns in the specified row
    //Furthermore, we get the cell's x position by adding 
    //all left sided columns' widths:
    cell.h = _columns[col]->getHeight(row);
    cell.x = 0;
    for(int c = 0; c < col; c++) { 
        Column* pCol = _columns[c];
        cell.x += pCol->getWidth();;
        int H = pCol->getHeight(row);
        cell.h = (H > cell.h) ? H : cell.h;
    }
    
    //To get cell.y we have to iterate over all rows:
    cell.y = 0;
    for(int r = 0; r < row; r++) {
        cell.y += getRowHeight(r);
    }
    
    return cell;
}

CellPtr Grid::getCellPtr(int row, int col) const {
    CellPtr p(new Cell());
    CellPtr p2(nullptr);
    return p2;
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

