#pragma once

#include "LifeMap.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

/**
 * Initalize with a vector of (x,y) pairs.
 */
LifeMap::LifeMap(const std::vector<std::pair<int64_t, int64_t>> &coords)
{
	for (int64_t i = 0; i < coords.size(); i++)
	{
		SetCellState(coords[i].first, coords[i].second, true);
	}
}

/**
 * Initialize by a file, expected format is one coordinate pair per line, with coordinates separated by commas, parantheses are optional.
 * Ex: 399,-7000034\n
 */
LifeMap::LifeMap(const char* path)
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
		int index = str.find("(");
		if (index != -1)
		{
			str = str.substr(index + 1);
		}
		index = str.find(")");
		if (index != -1)
		{
			str = str.substr(0, index);
		}

		index = str.find(",");
		if (index == -1 || index >= str.length() - 1)
		{
			std::cerr << "Invalid coordinate format on line " << count << "." << std::endl;
			continue;
		}

		left = str.substr(0, index);
		right = str.substr(index + 1);

		left64 = std::stoll(left);
		right64 = std::stoll(right);

		SetCellState(left64, right64, true);
		count++;
	}
	std::cout << "Found " << count << " coordinate pairs in file." << std::endl;
}

LifeMap::~LifeMap()
{
	for (CellGrid::iterator it = m_cells.begin(); it != m_cells.end(); it++)
	{
		for (std::unordered_map<int64_t, Cell*>::iterator innerIt = (*it).second.begin(); innerIt != (*it).second.end(); innerIt++)
		{
			delete (*innerIt).second;
		}
		(*it).second.clear();
	}
	m_cells.clear();
}

/**
 * Set the state for a cell at a given position, creating it if neccessary.
 */
void LifeMap::SetCellState(int64_t x, int64_t y, bool alive)
{
	Cell *c = m_cells[x][y];
	if (c != nullptr)
	{
		c->SetAlive(alive);
	}
	else
	{
		c = new Cell(x, y, alive);
		m_cells[x][y] = c;
	}

	FindAndSetNeighbors(c);
}

/**
 * Set the state for a cell already in the map, update state as needed. 
 */
void LifeMap::SetCellState(Cell* c, bool alive)
{
	c->SetAlive(alive);

	if (alive)
	{
		FindAndSetNeighbors(c);
	}
	else
	{
		// If no living neighbors, clean up the cell
		if (c->LivingNeighborsCount() == 0)
		{
			// Clean up cell map if portions are empty
			m_cells[c->GetX()].erase(c->GetY());
			if (m_cells[c->GetX()].size() == 0)
			{
				m_cells.erase(c->GetX());
			}
			delete c;
		}
	}
}

/**
 * When a new live cell is added, or an existing cell is made live, we have to start tracking neighbors.
 * All dead cells adjacent to any live cells have the potential to become live, so this can recurse one level.
 * 1. Make sure all neighbors of a live cell are tracked in the map.
 * 2. Make sure all dead cell neighbors of a live cell check if they have other living neighbors.
 */
void LifeMap::FindAndSetNeighbors(Cell* cell)
{
	int64_t x, y;
	CellGrid::iterator xIt;
	std::unordered_map<int64_t, Cell*>::iterator yIt;
	
	// After creating a new cell, make sure it's neighbors exist, attempt to insert each neighbor, if it does not exist
	for (int64_t i = -1; i <= 1; i++)
	{
		for (int64_t j = -1; j <= 1; j++)
		{
			// Bounds checking, don't check the element itself
			if ((i == 0 && j == 0) || (cell->GetX() == INT64_MIN && i < 0) || (cell->GetY() == INT64_MIN && j < 0) || (cell->GetX() == INT64_MAX && i > 0) || (cell->GetY() == INT64_MAX && j > 0))
			{
				continue;
			}
			x = cell->GetX() + i;
			y = cell->GetY() + j;

			// If neighbors are already tracked, exit early
			if (cell->HasNeighborAtPosition(x, y))
			{
				continue;
			}

			xIt = m_cells.find(x);
			if (xIt != m_cells.end())
			{
				yIt = (*xIt).second.find(y);
				// Neighbor already exists, so just track it
				if (yIt != (*xIt).second.end())
				{
					cell->AddNeighbor((*yIt).second);
				}
				// Alive cells should create neighbors that don't already exists
				else if (cell->IsAlive())
				{
					Cell* neighbor = new Cell(x, y, false);
					m_cells[x][y] = neighbor;
					cell->AddNeighbor(neighbor);
					// Recurse so newly created dead cell can check for other live neighbors
					FindAndSetNeighbors(neighbor);
				}
			}
		}
	}
}

/**
 * Run a step of the simualation.
 */
void LifeMap::Step()
{
	std::vector<Cell*> changingCells;
	Cell *current = nullptr;
	int livingCells = 0;

	for (CellGrid::iterator it = m_cells.begin(); it != m_cells.end(); it++)
	{
		for (std::unordered_map<int64_t, Cell*>::iterator innerIt = (*it).second.begin(); innerIt != (*it).second.end(); innerIt++)
		{
			current = (*innerIt).second;
			if (current == nullptr)
			{
				continue;
			}
			int livingNeighbors = current->LivingNeighborsCount();//GetNeighborCount(current);

			if (current->IsAlive())
			{
#ifdef DEBUG_VERBOSE
				std::cout << *current << " is alive" << std::endl;
#endif
				livingCells++;
				// Living cell dies off
				if (livingNeighbors < 2 || livingNeighbors > 3)
				{
#ifdef DEBUG_VERBOSE
				//std::cout << "Change cell " << *current << std::endl;
#endif
					changingCells.push_back(current);
				}
			}
			// Dead cell comes to life
			else if (!current->IsAlive() && livingNeighbors == 3)
			{
#ifdef DEBUG_VERBOSE
				//std::cout << "Change cell " << *current << std::endl;
#endif
				changingCells.push_back(current);
			}
		}
	}

	std::cout << livingCells << " cells alive." << std::endl;

	// Process changing cells queue
	for (int i = 0; i < changingCells.size(); i++)
	{
		SetCellState(changingCells[i], !changingCells[i]->IsAlive());
	}
}