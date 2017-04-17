#pragma once

//#define DEBUG_VERBOSE

#include <vector>
#include <set>
#include <unordered_map>
#include "Cell.h"

/**
 * Stores representation of game board and cells.  Cells are stored in nested unordered maps, where <x,y> maps to the Cell
 */
class LifeMap
{
private:
	// Comparison function for cell pointers
	struct CellComparer
	{
		bool operator() (const Cell* lhs, const Cell * rhs) const
		{
			return *lhs < *rhs;
		}
	};

public:
	typedef std::unordered_map<int64_t, std::unordered_map<int64_t, Cell*>> CellGrid;

	LifeMap(const std::vector<std::pair<int64_t, int64_t>> &coords);		// Initialize space from array
	LifeMap(const char* path);												// Initialize space from file
	virtual ~LifeMap();

	void Step();					// Runs a step of the simulation

	const CellGrid& GetCells() const { return m_cells; }

private:
	void SetCellState(int64_t x, int64_t y, bool alive);
	void SetCellState(Cell* c, bool alive);
	void FindAndSetNeighbors(Cell *cell);
	
	CellGrid m_cells;
};