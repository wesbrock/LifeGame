#include <iostream>
#include <vector>
#include "LifeMap.h"
#include "LifeGame.h"

/**
 * Wrapper for running and testing game of life.
 */
int main()
{
	std::cout << "Welcome to the Game of Life!" << std::endl;

	std::vector<std::pair<int64_t, int64_t>> testCoords;

	for (int64_t i = 0; i < 1000; i+= 2)
	{
		for (int64_t j = 0; j < 1000; j += 10)
		{
			testCoords.push_back(std::pair<int64_t, int64_t>(i, j));
		}
	}
	testCoords.push_back(std::pair<int64_t, int64_t>(INT64_MIN, INT64_MIN));
	testCoords.push_back(std::pair<int64_t, int64_t>(INT64_MIN, INT64_MAX));
	testCoords.push_back(std::pair<int64_t, int64_t>(INT64_MAX, INT64_MIN));
	testCoords.push_back(std::pair<int64_t, int64_t>(INT64_MAX, INT64_MAX));

	//LifeMap life(testCoords);
	LifeMap life("Test2.txt");
	LifeGame game(&life);

	while (!game.Exit())
	{
		game.Update();
		game.Draw();
	}

	return 0;
}