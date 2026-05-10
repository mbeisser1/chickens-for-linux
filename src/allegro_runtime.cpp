#include "allegro_runtime.h"

#include <allegro.h>

#include "helper.h"
#include "settings.h"

void AllegroRuntime::install_core(const Settings& settings)
{
    allegro_init();
    install_mouse();
    install_keyboard();
    install_sound(DIGI_AUTODETECT, MIDI_NONE, nullptr);
    install_timer();

    LOCK_VARIABLE(game_time);
    LOCK_FUNCTION(Timer);

    install_int_ex(Timer, BPS_TO_TIMER(60 + settings.GAME_SPEED_OFFSET));
}
