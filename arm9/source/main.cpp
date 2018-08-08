/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2013
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy
	Claudio "sverx"

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>

#include <string.h>
#include <unistd.h>
#include <maxmod9.h>
#include <gl2d.h>

#include "scesplash.h"
#include "psxsplash.h"

#include "graphics/graphics.h"

#include "graphics/fontHandler.h"

#include "soundbank.h"
#include "soundbank_bin.h"

mm_sound_effect snd_sce;
mm_sound_effect snd_playstation;

bool screenSwap = false;
bool fadeType = true;		// false = out, true = in

int gameMode = 0;
bool simulationRunning = false;
bool textPrinted = false;

void InitSound() {
	mmInitDefaultMem((mm_addr)soundbank_bin);
	
	mmLoadEffect( SFX_SCE );
	mmLoadEffect( SFX_PLAYSTATION );

	snd_sce = {
		{ SFX_SCE } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		128,	// panning
	};
	snd_playstation = {
		{ SFX_PLAYSTATION } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		128,	// panning
	};
}

//---------------------------------------------------------------------------------
void stop (void) {
//---------------------------------------------------------------------------------
	while (1) {
		swiWaitForVBlank();
	}
}

char filePath[PATH_MAX];

//---------------------------------------------------------------------------------
void doPause(int x, int y) {
//---------------------------------------------------------------------------------
	// iprintf("Press start...\n");
	printSmall(false, x, y, "Press start...");
	while(1) {
		scanKeys();
		if(keysDown() & KEY_START)
			break;
	}
	scanKeys();
}

void vCountHandler(void) {
	scanKeys();
	int pressed = keysDownRepeat();

	if (pressed & KEY_START) {
		simulationRunning = !simulationRunning;
		if (simulationRunning) {
			if (screenSwap) {
				powerOff(PM_BACKLIGHT_TOP);
			} else {
				powerOff(PM_BACKLIGHT_BOTTOM);
			}
			sceInit();
			psxInit();
			consoleClear();
			gameMode = 0;
		} else {
			mmEffectCancelAll();
			if (screenSwap) {
				powerOn(PM_BACKLIGHT_TOP);
			} else {
				powerOn(PM_BACKLIGHT_BOTTOM);
			}
		}
	}
	if (pressed & KEY_SELECT) {
		screenSwap = !screenSwap;
		if (screenSwap) {
			if (simulationRunning) {
				powerOn(PM_BACKLIGHT_BOTTOM);
				powerOff(PM_BACKLIGHT_TOP);
			}
			lcdMainOnBottom();
		} else {
			if (simulationRunning) {
				powerOn(PM_BACKLIGHT_TOP);
				powerOff(PM_BACKLIGHT_BOTTOM);
			}
			lcdMainOnTop();
		}
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	powerOn(PM_BACKLIGHT_TOP);
	powerOn(PM_BACKLIGHT_BOTTOM);

	irqSet(IRQ_VCOUNT, vCountHandler);
	irqEnable(IRQ_VCOUNT);

	InitSound();	
	graphicsInit();
	//fontInit();

	videoSetModeSub(MODE_0_2D);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);
	consoleClear();

	while (1) {
		if (simulationRunning) {
			textPrinted = false;
			switch (gameMode) {
				case 0:
				default:
					sceSplash();
					break;
				case 1:
					psxSplash();
					break;
			}
		} else {
			if (!textPrinted) {
				printf("PDSX v0.1.0 by Robz8\n");
				printf("\n");
				printf("START: Start/Pause simulation\n");
				printf("SELECT: Swap screens\n");
				textPrinted = true;
			}
		}
	}

	return 0;
}