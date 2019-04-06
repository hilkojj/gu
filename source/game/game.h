#include "screen.h"

#ifndef GAME_H
#define GAME_H

namespace Game
{

void setScreen(Screen *screen);
void render(double deltaTime);
void end();

}; // namespace Game

#endif
