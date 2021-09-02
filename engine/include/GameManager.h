#pragma once
#include <deque>
#include "Enemy.h"

class Player;

enum class GameState
{
	MAIN_MENU,
	GAME,
	GAME_OVER,
	VICTORY,

	COUNT
};

class GameManager
{
private:
	GameState gameState = GameState::MAIN_MENU;

public:
	GameManager() = default;

	GameState getState() { return gameState; }
	void setState(GameState state) { gameState = state; }

};
