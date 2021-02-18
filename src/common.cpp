//
// Created by aogun on 2021/2/18.
//

#include <cstdio>
#include "windows.h"
#include "version.h"

extern HWND g_hwnd;

void display_about()
{
    char sz[256];
    snprintf(sz, 256, "MP4 viewer\nversion %d.%d.%d\n"
                      "author: angus(aogun)\ncontact:angus_ag@msn.com",
                      MP4_VIEWER_VERSION_MAJOR,
                      MP4_VIEWER_VERSION_MINOR,
                      MP4_VIEWER_VERSION_PATCH);
    int msgboxID = MessageBox(
            g_hwnd,
            sz,
            "About",
            MB_OK
    );
}
