#include "helper.h"

// Return the number of items inside a datafile
int items_in_datafile(const DATAFILE* data)
{
    int len = 0;
    while (data[len].type != DAT_END)
    {
        len++;
    }

    return len - 1;
}

void Timer()
{
    game_time++;
}
END_OF_FUNCTION(Timer);
