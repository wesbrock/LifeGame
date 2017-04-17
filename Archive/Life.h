#pragma once

//#define DEBUG_VERBOSE

#include <vector>
#include <set>
#include <map>
#include "Cell.h"

class Life
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
	typedef std::set<Cell*, CellComparer> CellGrid;

	Life(const std::vector<std::pair<int64_t, int64_t>> &coords);		// Initialize space from array
	Life(const char* path);												// Initialize space from file
	virtual ~Life();

	void Step();					// Runs a step of the simulation

	const CellGrid& GetCells() const { return m_cells; }

private:
	void SetCellState(int64_t x, int64_t y, bool alive);
	void SetCellState(Cell* c, bool alive);
	void FindAndSetNeighbors(Cell *cell);

	CellGrid m_cells;
};