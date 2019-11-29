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

class Fl_Widget;
class Fl_Group;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Padding {
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
    std::string sticky = "nswe"; //one of or combination of n, s, w, e
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Column {
public:
    Column(){}
    ~Column(){}
    
    void add(const GridInfo*);
    const GridInfo* getGridInfo(int column) const {return _gridInfos.at(column);}
    int getWidth() {return _w;}
    int getCount() {return _gridInfos.size();}
private:
    std::vector<const GridInfo*> _gridInfos;
    int _w = 0;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//typedef std::vector<const GridInfo*> Column;
typedef std::vector<Column*> Columns;

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
     * Retrieve layout of given group
     * @param grp Fl_Group whose layout is to find
     * @return found Layout or NULL
     */
    Fl_Group* getGroup() const {return _pGrp;}
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

