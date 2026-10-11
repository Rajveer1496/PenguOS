// Draw Desktop
// clickable apps
// cursor

// combine planes and draw, with depth

#include "gui.h"
#include "graphics.h"

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;

void gui_main(){
    //draw background
    struct plane * background = create_plane(0,0,VBE_WIDTH,VBE_HEIGHT);

    //draw taskbar

    //draw apps
}
