/****************************************************************************
 * Copyright (C) 2017,2018 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <wups.h>

#include <vector>
#include <string>

#include <vpad/input.h>
#include <coreinit/screen.h>
#include <coreinit/thread.h>
#include <common/c_retain_vars.h>
#include <nsysnet/socket.h>
#include <utils/logger.h>

WUPS_PLUGIN_NAME("SwipSwapMirror");
WUPS_PLUGIN_DESCRIPTION("Fork of SwipSwapMe, Swaps the gamepad and tv screen when pressing a certain button (TV is default)");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("KrispyRice9, original by Maschell");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_FS_ACCESS()

uint32_t SplashScreen(int32_t time,int32_t combotime);

/* Entry point */
ON_APPLICATION_START(args) {
    memset(gVoiceInfos,0,sizeof(gVoiceInfos));
    socket_lib_init();
    log_init();
}

ON_APP_STATUS_CHANGED(status) {
    gAppStatus = status;
}

INITIALIZE_PLUGIN() {
    uint32_t res = SplashScreen(10,2);
    gButtonCombo = res;
}

#define FPS 60
uint32_t SplashScreen(int32_t time,int32_t combotime) {
    uint32_t result = VPAD_BUTTON_TV;

    // Init screen
    OSScreenInit();

    uint32_t screen_buf0_size = OSScreenGetBufferSizeEx(SCREEN_TV);
    uint32_t screen_buf1_size = OSScreenGetBufferSizeEx(SCREEN_DRC);

    uint32_t * screenbuffer0 = (uint32_t*)memalign(0x100, screen_buf0_size);
    uint32_t * screenbuffer1 = (uint32_t*)memalign(0x100, screen_buf1_size);

    if(screenbuffer0 == NULL || screenbuffer1 == NULL) {
        if(screenbuffer0 != NULL) {
            free(screenbuffer0);
        }
        if(screenbuffer1 != NULL) {
            free(screenbuffer1);
        }
        return result;
    }

    OSScreenSetBufferEx(SCREEN_TV, (void *)screenbuffer0);
    OSScreenSetBufferEx(SCREEN_DRC, (void *)screenbuffer1);

    OSScreenEnableEx(SCREEN_TV, 1);
    OSScreenEnableEx(SCREEN_DRC, 1);

    // Clear screens
    OSScreenClearBufferEx(SCREEN_TV, 0);
    OSScreenClearBufferEx(SCREEN_DRC, 0);

    // Flip buffers
    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);

    OSScreenClearBufferEx(SCREEN_TV, 0);
    OSScreenClearBufferEx(SCREEN_DRC, 0);

    std::vector<std::string> strings;
    strings.push_back("SwipSwapMe 0.2 - by Maschell.");
    strings.push_back("");
    strings.push_back("");
    strings.push_back("Press the combo you want to use for swapping now for 2 seconds.");
    strings.push_back("Pressing the TV button will return directly.");
    strings.push_back("");
    strings.push_back("Otherwise the default combo (TV button) will be used in 10 seconds.");
    uint8_t pos = 0;
    for (std::vector<std::string>::iterator it = strings.begin() ; it != strings.end(); ++it) {
        OSScreenPutFontEx(SCREEN_TV, 0, pos, (*it).c_str());
        OSScreenPutFontEx(SCREEN_DRC, 0, pos, (*it).c_str());
        pos++;
    }

    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);

    int32_t tickswait = time * FPS * 16;

    int32_t sleepingtime = 16;
    int32_t times = tickswait/16;
    int32_t i=0;

    VPADStatus vpad_data;
    VPADReadError error;
    uint32_t last = 0xFFFFFFFF;
    int32_t timer = 0;
    while(i<times) {
        VPADRead(VPAD_CHAN_0, &vpad_data, 1, &error);
        if(vpad_data.trigger == VPAD_BUTTON_TV)
            break;
        if(last == vpad_data.hold && last != 0) {
            timer++;
        } else {
            last = vpad_data.hold;
            timer = 0;
        }
        if(timer >= combotime*FPS) {
            result = vpad_data.hold;
            break;
        }
        i++;
        OSSleepTicks(OSMicrosecondsToTicks(sleepingtime*1000));
    }

    if(screenbuffer0 != NULL) {
        free(screenbuffer0);
    }
    if(screenbuffer1 != NULL) {
        free(screenbuffer1);
    }

    return result;
}
