#ifndef SCREEN_H
#define SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

void InitializeScreenUpdate(int i);
void ShutDownScreenUpdate();
void SwapBuffers();
BITMAP* GetBuffer();
BITMAP* GetScreen();
void EnableVSync();
void DisableVSync();
bool VSyncEnabled();
int GetScreenUpdateMethod();

constexpr int TRIPLEBUFFER = 1;
constexpr int PAGEFLIP = 2;
constexpr int SYSTEMBUFFER = 3;
constexpr int DOUBLEBUFFER = 4;

#ifdef __cplusplus
}
#endif

// screen.c

static int updatemethod = 0;
static bool waitforvsync = 0;
static BITMAP *pages[3] = {NULL, NULL, NULL}, *active_page, *current_page;

// helper function
static BITMAP* erase_bitmap(BITMAP* bmp)
{
    destroy_bitmap(bmp);
    return NULL;
}

void InitializeScreenUpdate(int i)
{
    if (i < TRIPLEBUFFER || i > DOUBLEBUFFER)
    {
        i = TRIPLEBUFFER;
    }

    switch (i)
    {
    case TRIPLEBUFFER:
        if (!(gfx_capabilities & GFX_CAN_TRIPLE_BUFFER))
        {
            enable_triple_buffer();
        }

        if ((gfx_capabilities & GFX_CAN_TRIPLE_BUFFER))
        {
            pages[0] = create_video_bitmap(SCREEN_W, SCREEN_H);
            pages[1] = create_video_bitmap(SCREEN_W, SCREEN_H);
            pages[2] = create_video_bitmap(SCREEN_W, SCREEN_H);
            if (pages[0] && pages[1] && pages[2])
            {
                clear_bitmap(pages[0]);
                clear_bitmap(pages[1]);
                clear_bitmap(pages[2]);
                active_page = pages[0];
                current_page = pages[2];
                show_video_bitmap(current_page);
                updatemethod = TRIPLEBUFFER;
                return;
            }
            else
            {
                if (pages[0])
                {
                    pages[0] = erase_bitmap(pages[0]);
                }
                if (pages[1])
                {
                    pages[1] = erase_bitmap(pages[1]);
                }
                if (pages[2])
                {
                    pages[2] = erase_bitmap(pages[2]);
                }
            }
        }
        // fall through if triple buffering isn't supported

    case PAGEFLIP:
        pages[0] = create_video_bitmap(SCREEN_W, SCREEN_H);
        if (!pages[0])
        {
            return;
        }
        pages[1] = create_video_bitmap(SCREEN_W, SCREEN_H);
        if (!pages[1])
        {
            destroy_bitmap(pages[0]);
            return;
        }
        if (pages[0] && pages[1])
        {
            clear_bitmap(pages[0]);
            clear_bitmap(pages[1]);
            active_page = pages[0];
            current_page = pages[1];
            show_video_bitmap(current_page);
            updatemethod = PAGEFLIP;
            return;
        }
        else
        {
            if (pages[0])
            {
                pages[0] = erase_bitmap(pages[0]);
            }
            if (pages[1])
            {
                pages[1] = erase_bitmap(pages[1]);
            }
        }
        // fall through if page flipping isn't supported

    case SYSTEMBUFFER:
        active_page = create_system_bitmap(SCREEN_W, SCREEN_H);
        if (!active_page)
        {
            return;
        }
        current_page = create_video_bitmap(SCREEN_W, SCREEN_H);
        if (!current_page)
        {
            destroy_bitmap(active_page);
            return;
        }
        clear_bitmap(active_page);
        clear_bitmap(current_page);
        show_video_bitmap(current_page);
        updatemethod = SYSTEMBUFFER;
        return;

    case DOUBLEBUFFER:
        active_page = create_bitmap(SCREEN_W, SCREEN_H);
        if (!active_page)
        {
            return;
        }
        current_page = create_video_bitmap(SCREEN_W, SCREEN_H);
        if (!current_page)
        {
            destroy_bitmap(active_page);
            return;
        }
        clear_bitmap(active_page);
        clear_bitmap(current_page);
        show_video_bitmap(current_page);
        updatemethod = DOUBLEBUFFER;
        return;
    }
}

void SwapBuffers()
{
    switch (updatemethod)
    {
    case TRIPLEBUFFER:
        do
        {
        } while (poll_scroll());
        current_page = active_page;
        request_video_bitmap(current_page);

        if (active_page == pages[0])
        {
            active_page = pages[1];
        }
        else if (active_page == pages[1])
        {
            active_page = pages[2];
        }
        else
        {
            active_page = pages[0];
        }
        return;

    case PAGEFLIP:
        current_page = active_page;
        show_video_bitmap(current_page);
        if (active_page == pages[0])
        {
            active_page = pages[1];
        }
        else
        {
            active_page = pages[0];
        }
        return;

    case SYSTEMBUFFER:
    case DOUBLEBUFFER:
        if (waitforvsync)
        {
            vsync();
        }
        blit(active_page, current_page, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        return;
    }
}

void ShutDownScreenUpdate()
{
    if (pages[0])
    {
        pages[0] = erase_bitmap(pages[0]);
    }
    if (pages[1])
    {
        pages[1] = erase_bitmap(pages[0]);
    }
    if (pages[2])
    {
        pages[2] = erase_bitmap(pages[0]);
    }

    if (updatemethod == DOUBLEBUFFER || updatemethod == SYSTEMBUFFER)
    {
        active_page = erase_bitmap(active_page);
        current_page = erase_bitmap(current_page);
    }
}

BITMAP* GetBuffer()
{
    return active_page;
}

BITMAP* GetScreen()
{
    return current_page;
}

void EnableVSync()
{
    waitforvsync = true;
}

void DisableVSync()
{
    waitforvsync = false;
}

bool VSyncEnabled()
{
    return waitforvsync;
}

int GetScreenUpdateMethod()
{
    return updatemethod;
}

#endif