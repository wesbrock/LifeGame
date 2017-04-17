
#include "LifeGame.h"
#include <iostream>
#include <stdlib.h>
#include "SDL.h"

LifeGame::LifeGame(LifeMap* life) : m_life(life)
{
	m_exit = false;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, (SDL_Window**)&m_window, (SDL_Renderer**)&m_renderer);
}

LifeGame::~LifeGame()
{
	SDL_DestroyWindow((SDL_Window*)m_window);
	SDL_DestroyRenderer((SDL_Renderer*)m_renderer);
	SDL_Quit();

	m_window = nullptr;
	m_renderer = nullptr;
	m_life = nullptr;
}

/**
 * Check for input - process next phase on space bar, exit on escape
 */
void LifeGame::Update()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			m_exit = true;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_SPACE)
			{
				m_life->Step();
			}
			else if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				m_exit = true;
			}
		}
	}

}

/**
 * Draw the screen - we unfortuantely only render what shows up in the 800-600 range, currently
 */
void LifeGame::Draw()
{
	SDL_Renderer *renderPointer = (SDL_Renderer*)m_renderer;

	SDL_SetRenderDrawColor(renderPointer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderPointer);
	SDL_SetRenderDrawColor(renderPointer, 0xFF, 0xFF, 0xFF, 0xFF);

	Cell* current = nullptr;
	for (LifeMap::CellGrid::const_iterator it = m_life->GetCells().begin(); it != m_life->GetCells().end(); it++)
	{
		for (std::unordered_map<int64_t, Cell*>::const_iterator innerIt = (*it).second.begin(); innerIt != (*it).second.end(); innerIt++)
		{
			current = (*innerIt).second;
			if (current->IsAlive())
			{
				SDL_RenderDrawPoint(renderPointer, current->GetX(), current->GetY());
			}
		}
	}

	SDL_RenderPresent(renderPointer);
}

