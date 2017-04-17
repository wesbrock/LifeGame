#pragma once

#include "Life.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

Life::Life(const std::vector<std::pair<int64_t, int64_t>> &coords) 
{ 
	for (int64_t i = 0; i < coords.size(); i++)
	{
		SetCellState(coords[i].first, coords[i].second, true);
	}
}

// Expects a file with one coordinate pair per line, with coordinates separated by commas.
//Ex: 399,-7000034\n
Life::Life(const char* path)
{
	std::cout << "Reading in file " << path << "..." << std::endl;
	std::ifstream file(path);
	if (file.fail())
	{
		std::cerr << "Failed to read file " << path << "please retry with valid file." << std::endl;
	}

	std::string str, left, right;
	int64_t left64, right64;
	int count = 0;
	while (std::getline(file, str))
	{
		int comma = str.find(",");
		if (comma == -1 || comma >= str.length() - 1)
		{
			std::cerr << "Invalid coordinate format on line " << count << "." << std::endl;
		}

		left = str.substr(0, comma);
		right = str.substr(comma + 1);

		left64 = std::stoll(left);
		right64 = std::stoll(right);

		SetCellState(left64, right64, true);
		count++;
	}
	std::cout << "Found " << count << " coordinate pairs in file." << std::endl;
}

Life::~Life()
{
	for (CellGrid::iterator it = m_cells.begin(); it != m_cells.end(); it++)
	{
		delete (*it);
	}
	m_cells.clear();
}

void Life::SetCellState(int64_t x, int64_t y, bool alive)
{
	// Dead is the default state of the map, really shouldn't be creating these
	if (!alive)
	{
		std::cerr << "Attempting to create dead cell at (" << x << ", " << y << ")." << std::endl;
		return;
	}

	Cell* cell = new Cell(x, y, alive);
	std::pair<CellGrid::iterator, bool> insertResult = m_cells.insert(cell);
	// Cell already existed
	if (!insertResult.second)
	{
		CellGrid::iterator cellIt = m_cells.find(cell);
		(*cellIt)->SetAlive(alive);
		delete cell;
		cell = *cellIt;
	}
	FindAndSetNeighbors(cell);
}

void Life::SetCellState(Cell* c, bool alive)
{
	c->SetAlive(alive);

	if (alive)
	{
		FindAndSetNeighbors(c);
	}
	else
	{
		// Clean up cell
	}
}

void Life::FindAndSetNeighbors(Cell* cell)
{
	int64_t x, y;
	// After creating a new cell, make sure it's neighbors exist, attempt to insert each neighbor, if it does not exist
	for (int64_t i = -1; i <= 1; i++)
	{
		for (int64_t j = -1; j <= 1; j++)
		{
			// Bounds checking, don't check the element itself
			if ((i == 0 && j == 0) || (cell->GetX() == INT64_MIN && i < 0) || (cell->GetY() == INT64_MIN && j < 0) || (cell->GetX() == INT64_MAX && i > 0) || (cell->GetY() == INT64_MAX && j >0))
			{
				continue;
			}
			x = cell->GetX() + i;
			y = cell->GetY() + j;
			
			// If neighbors already exist, skip the check
			if (cell->HasNeighborAtPosition(x, y))
			{
				continue;
			}
			
			Cell* neighbor = new Cell(x, y, false);
			CellGrid::iterator foundNeighbor = m_cells.find(neighbor);
			// Cell already exists, so add it as a neighbor
			if (foundNeighbor != m_cells.end())
			{
				cell->AddNeighbor(*foundNeighbor);
				delete neighbor;
				neighbor = nullptr;
			}
			// Alive cells should create neighbors that don't already exists
			else if (cell->IsAlive())
			{
				m_cells.insert(neighbor);
				cell->AddNeighbor(neighbor);
				// Recurse so newly created dead cell can check for other live neighbors
				FindAndSetNeighbors(neighbor);
			}
		}
	}
}

void Life::Step()
{
	std::vector<Cell*> changingCells;
	int count = 0;
	for (CellGrid::iterator it = m_cells.begin(); it != m_cells.end(); it++)
	{
		int livingNeighbors = (*it)->LivingNeighborsCount();

		if ((*it)->IsAlive())
		{
#ifdef DEBUG_VERBOSE
			std::cout << **it << " is alive" << std::endl;
#endif
			count++;
		}

		if ((*it)->IsAlive() && (livingNeighbors < 2 || livingNeighbors > 3))
		{    
			//std::cout << "Change cell " << **it << std::endl;
			changingCells.push_back(*it);
		}
		else if (!(*it)->IsAlive() && livingNeighbors == 3)
		{
			//std::cout << "Change cell " << **it << std::endl;
			changingCells.push_back(*it);
		}
	}

	std::cout << count << " cells alive." << std::endl;
	std::cout << changingCells.size() << " cells changing." << std::endl;

	// Process changing cells queue
	for (int i = 0; i < changingCells.size(); i++)
	{
		SetCellState(changingCells[i], !changingCells[i]->IsAlive());
	}
}