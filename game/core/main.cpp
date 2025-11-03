#define SDL_MAIN_HANDLED
#include"game.h"
#include<windows.h>

int main()
{
	SetConsoleOutputCP(65001);
	game::core::Game game;
	game.run();
	return 0;
}