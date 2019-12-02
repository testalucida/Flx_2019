/* 
 * File:   grid.h
 * Author: martin
 *
 * Created on 27. November 2019, 23:25
 */

#ifndef GRID_H
#define GRID_H

#include <string>
#include <vector>
#include <memory>

class Fl_Widget;
class Fl_Group;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Padding {
    // padding defines a minimum clearance to the respective cell border, 
    // e.g.: e == 2 means a minimum distance of 2 pixels 
    // to the right cell border.
    // If however a small widget width is specified (maybe together 
    // with sticky="w") the clearance might be greater.
    int n = 2; //north
    int e = 2;
    int s = 2;
    int w = 2; //west
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class GridInfo {
public:
    GridInfo() {};
public:
    Fl_Widget* pWidget = NULL;
    int row = 0;
    int column = 0;
    int rowspan = 1;
    int columnspan = 1;
    Padding pad;
    /**
     * Sets horizontal padding.
     * If e is not given (or -1) it is ignored
     * @param w left padding
     * @param e right padding
     */
    void setPaddingHorizontal(int w, int e = -1);
    
    /**
     * Sets vertical padding.
     * If s is not given (or -1) it is ignored
     * @param n upper padding
     * @param s lower padding
     */
    void setPaddingVertical(int n, int s = -1);
    
    Padding& getPadding() {return pad;}
    
    /**
     * sticky describes a widget's size and position within a cell.
     * The widget's minimum clearance to the surrounding cell borders
     * is determined by the 4 padding values (n, e, s, w).
     * sticky prevails the widgets width attribute, e.g.:
     * column width = 100, widget's width = 70, sticky = "we":
     * the widget will be drawn with a width of 100 minus pad.w minus 
     * pad.e
     * So, if you want to keep your widget its width of 70 you
     * have to specifiy *either* sticky = "w" or sticky = "e".
     * Or you give up specifying a west or east value and your 
     * widget will keep its width and will be drawn in the center of the
     * cell.
     */
    std::string sticky = "nswe";
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Column {
public:
    Column(){}
    Column(int rows);
    ~Column(){}
    
    /**
     * Adds a GridInfo object to this column.
     * @param pointer to GridInfo - may be NULL
     */
    void add(const GridInfo*);
    
    /**
     * Sets a GridInfo into the given cell 
     * @param pInfo pointer to GridInfo to set
     * @param row index of the cell
     */
    void setGridInfo(const GridInfo* pInfo, int row);
    
    /**
     * Gets the GridInfo object of a given 'row' index
     * @param row index of this column's element
     * @return pointer to GridInfo
     */
    const GridInfo* getGridInfo(int row) const {return _gridInfos.at(row);}
    
    /**
     * Gets width of column.
     * The width is determined by the widget requiring the maximum widhth.
     * It is calculated by adding pad.e, widget.w() and pad.e.
     * @return width
     */
    int getWidth() const;
    
    /**
     * Gets the height of the row with the given row index.
     * The height is determined by the widget requiring the maximum height.
     * It is calculated by adding pad.n, widget.h() and pad.s.
     * @param row row index
     * @return height
     */    
    int getHeight(int row) const;
    
    /**
     * Gets the number of stored widgets in this column, 
     * i.e. the number of "rows"
     * @return number of rows
     */
    int getCount() const {return _gridInfos.size();}
    
private:
    std::vector<const GridInfo*> _gridInfos;
    //int _w = 0;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef std::vector<Column*> Columns;

struct Cell {
    int x, y;
    int w, h;
};

typedef std::unique_ptr<Cell> CellPtr;

class Grid {
public:
    Grid() {}
    virtual ~Grid();
    
    /**
     * Adds a widget's GridInfo object to a cell of a column
     * of this grid. The cell is determined by
     * GridInfo.row and GridInfo.column.
     * @param pInfo GridInfo to add
     */
    void addGridInfo(const GridInfo* pInfo);
    
    /**
     * Gets the width of the given column which is determined by
     * the max width of this column's widgets plus their 
     * eastern and western padding.
     * @param col 
     * @return max width
     */
    int getColumnWidth(int col) const;
    
    /**
     * Gets the height of the given row.
     * The height is determined by considering all height attributes
     * of the widgets contained in all columns at position 'row' plus 
     * their northern and eastern padding.
     * @param row 
     * @return height of the given row
     */
    int getRowHeight(int row) const;
    
    /**
     * Gets the cell specified by row/col.
     * The cell's height is determined by the row's widget requiring
     * maximum height by considering widget.h(), pad.n and pad.s
     * @param row cell's row index
     * @param col cell's column index
     * @return Cell object at row/col
     */
    Cell getCell(int row, int col) const;
    
    CellPtr getCellPtr(int row, int col) const;
    
    /**
     * Gets the number of columns contained in this grid.
     * @return number of columns
     */
    int getColumnCount() const {return _columns.size();}
    
    /**
     * Gets the number of rows in this grid.
     * @return number of rows
     */
    int getRowCount() const {return (getColumnCount() > 0) ? 
                             _columns[0]->getCount() : 0;}
    
    /**
     * Gets the GridInfo at a given position
     * @param row row index
     * @param col col index
     * @return GridInfo* at position row/col
     */
    const GridInfo* getGridInfo(int row, int col);
    
    /**
     * Gets the widget at the specified position
     * @param row cell's row index
     * @param col cell's column index
     * @return pointer to Fl_Widget at row/col
     */
    Fl_Widget* getWidget(int row, int col);
    
    const Columns& getColumns() const {return _columns;}
    
private:
    /**
     * Adds nCols columns, each containing nRows rows to _columns
     * @param n
     */
    void addColumns(int nCols, int nRows);
    
    /**
     * Adds nRows cells to each column in _columns
     * @param nRows
     */
    void addRows(int nRows);
    
private:
    Columns _columns;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Layout {
public:
    Layout(Fl_Group* pGrp);
    
    /**
     * iterates over all children of this layout's Fl_Group
     * and positions them on Fl_Group using the GridInfo objects 
     * contained in _grid.
     */
    void layout();
    
    /**
     * Adds a widget's GridInfo object to this layout.
     * @param Pointer to GridInfo to add
     */
    void add(GridInfo*);
    
    /**
     * Sets size and position of the given Widget using
     * the given GridInfo and cell infos.
     * @param pW Fl_Widget to position
     * @param info GridInfo containing padding and sticky
     * @param cell Cell object containing needed information
     */
    void layoutWidget(Fl_Widget* pW, const GridInfo& info, const Cell& cell);
    
    /**
     * Retrieve layout of given group
     * @param grp Fl_Group whose layout is to find
     * @return found Layout or NULL
     */
    Fl_Group* getGroup() const {return _pGrp;}
    
    const Grid& getGrid() const {return _grid;}
    
private:
    Fl_Group* _pGrp;
    Grid _grid;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Flx_GridManager {
public:
    static Flx_GridManager& inst() {
        static Flx_GridManager _instance;
        return _instance;
    }
    ~Flx_GridManager();
    
    void add(Fl_Group*, GridInfo*);
    void doLayout(Fl_Group*);
    Layout* getLayout(Fl_Group* );
private:
    Flx_GridManager(){}
    Flx_GridManager(const Flx_GridManager&);
    Flx_GridManager & operator = (const Flx_GridManager&);
private:
    std::vector<Layout*> _layouts;
};

#endif /* GRID_H */

