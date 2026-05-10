#ifndef ALLEGRO_RUNTIME_H
#define ALLEGRO_RUNTIME_H

struct Settings;

// Allegro core: library init, drivers, timers, locked timer IRQ hook (see helper.cpp Timer).
class AllegroRuntime
{
  public:
    static void install_core(const Settings& settings);
};

#endif
