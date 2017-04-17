#pragma once
#include "LifeMap.h"

/**
 * Displays and handles stepping through of Life Game
 */
class LifeGame
{
public:
	LifeGame(LifeMap* life);
	virtual ~LifeGame();

	bool Exit() { return m_exit; }
	void Draw();
	void Update();
	

private:
	bool m_exit;

	void *m_window;
	void *m_renderer;
	LifeMap* m_life;
};