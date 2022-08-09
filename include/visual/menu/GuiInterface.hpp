/*
  Created on 29.07.18.
*/

#pragma once
#include "settings/Registered.hpp"
union SDL_Event;

extern settings::Button open_gui_button;

namespace gui
{

void init();
void shutdown();
void draw();
bool handleSdlEvent(SDL_Event *event);

void onLevelLoad();
} // namespace gui
