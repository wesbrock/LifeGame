#include "Cell.h"
#include <stdlib.h>
#include <iostream>

Cell::Cell(int64_t x, int64_t y, bool alive) : m_x(x), m_y(y), m_isAlive(alive) {}

Cell::~Cell() 
{
	// Clean up neighbors' references to this cell
	for (NeighborList::iterator it = m_neighbors.begin(); it != m_neighbors.end(); it++)
	{
		for (NeighborList::const_iterator neighborIt = (*it)->m_neighbors.begin(); neighborIt != (*it)->m_neighbors.end(); neighborIt++)
		{
			if (*neighborIt == this)
			{
				(*it)->m_neighbors.erase(neighborIt);
				break;
			}
		}
	}
	m_neighbors.clear();
}

int Cell::LivingNeighborsCount() const
{
	int count = 0;
	for (int i = 0; i < m_neighbors.size(); i++)
	{
		if (m_neighbors[i]->IsAlive())
		{
			count++;
		}
	}
	return count;
}

bool Cell::HasNeighborAtPosition(int64_t x, int64_t y) const
{
	for (int i = 0; i < m_neighbors.size(); i++)
	{
		if (m_neighbors[i]->m_x == x && m_neighbors[i]->m_y == y)
		{
			return true;
		}
	}
	return false;
}

void Cell::AddNeighbor(Cell* neighbor)
{
	if (abs(m_x - neighbor->m_x) > 1 || abs(m_y - neighbor->m_y) > 1)
	{
		std::cerr << "Invalid neighbors (" << m_x << ", " << m_y << ") and (" << neighbor->m_x << ", " << neighbor->m_y << ")" << std::endl;
		return;
	}

	// Neighbor is a reflexive relationship
	m_neighbors.push_back(neighbor);
	neighbor->m_neighbors.push_back(this);
}

void Cell::RemoveNeighbor(Cell* neighbor)
{
	for (NeighborList::const_iterator it = m_neighbors.begin(); it != m_neighbors.end(); it++)
	{
		if (*it == neighbor)
		{
			m_neighbors.erase(it);
			break;
		}
	}

	for (NeighborList::const_iterator it = neighbor->m_neighbors.begin(); it != neighbor->m_neighbors.end(); it++)
	{
		if (*it == this)
		{
			neighbor->m_neighbors.erase(it);
			break;
		}
	}
}

bool Cell::operator<(const Cell& b) const
{
	return this->m_x < b.m_x || (this->m_x == b.m_x && m_y < b.m_y);
}

std::ostream& operator<<(std::ostream& os, const Cell& c)
{
	return os << "(" << c.GetX() << ", " << c.GetY() << "): " << c.IsAlive() << " | " << c.LivingNeighborsCount();
}