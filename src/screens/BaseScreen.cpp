#include "BaseScreen.h"

BaseScreen::BaseScreen(LGFX* display, ScreenID id) 
    : tft(display), screenId(id), needsRedraw(true) {
}