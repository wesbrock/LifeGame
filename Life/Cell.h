#pragma once

#include <stdint.h>
#include <set>
#include <vector>
#include <iostream>

/**
 * Represents a single cell on the board.
 */
class Cell
{
public:
	Cell(int64_t x, int64_t y, bool alive);
	virtual ~Cell();

	void AddNeighbor(Cell* neighbor);
	void RemoveNeighbor(Cell* neighbor);
	void SetAlive(bool alive) { m_isAlive = alive; }

	int64_t GetX() const { return m_x; }
	int64_t GetY() const { return m_y; }
	bool IsAlive() const { return m_isAlive; }
	int LivingNeighborsCount() const;
	bool HasNeighborAtPosition(int64_t x, int64_t y) const;

	bool operator<(const Cell& b) const;
	friend std::ostream& operator<<(std::ostream& os, const Cell& c);


private:
	typedef std::vector<Cell*> NeighborList;
	NeighborList m_neighbors;
	int64_t m_x;
	int64_t m_y;
	bool m_isAlive;
};