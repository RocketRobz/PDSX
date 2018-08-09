#include "psxmenu.h"


static bool musicStopped = false;
static bool displayMenuGraphics = false;
static int menu_cursor = 0;

extern int pressed;

extern int blueBgTexID, mainMenuTexID;
extern glImage blueBgImage[(128 / 16) * (192 / 16)];
extern glImage mainMenuImage[(128 / 16) * (32 / 16)];
extern glImage menuCursorImage[(16 / 16) * (16 / 16)];
extern glImage memandcdImage[(128 / 16) * (64 / 16)];

void psxMenuInit(void) {
	musicStopped = false;
	displayMenuGraphics = false;
	menu_cursor = 0;
}

void psxMenu(void) {
	if (!musicStopped) {
		for (int i = 0; i < 30; i++) {
			swiWaitForVBlank();
			if (i == 29) {
				mmEffectCancelAll();	// Stop SPE logo music
			}
		}
		musicStopped = true;
	}

	displayMenuGraphics = true;
	
	if (pressed & KEY_UP) {
		menu_cursor--;
		if (menu_cursor < 0) menu_cursor = 0;
	}

	if (pressed & KEY_DOWN) {
		menu_cursor++;
		if (menu_cursor > 1) menu_cursor = 1;
	}

	swiWaitForVBlank();
}

void psxMenuGraphicDisplay(void) {
	glSprite(0, 0, GL_FLIP_NONE, blueBgImage);
	glSprite(128, 0, GL_FLIP_H, blueBgImage);
	if (displayMenuGraphics) {
		glSprite(181, 17, GL_FLIP_NONE, mainMenuImage);
		if (menu_cursor == 0) {
			glSprite(10, 87, GL_FLIP_NONE, &memandcdImage[0]);			// Zoomed-in "Memory Card"
			glSprite(26, 127, GL_FLIP_NONE, &memandcdImage[3]);		// Zoomed-out "CD Player"
			glSprite(66, 94, GL_FLIP_NONE, menuCursorImage);
		} else {
			glSprite(10, 87, GL_FLIP_NONE, &memandcdImage[1]);			// Zoomed-out "Memory Card"
			glSprite(26, 127, GL_FLIP_NONE, &memandcdImage[2]);		// Zoomed-in "CD Player"
			glSprite(66, 135, GL_FLIP_NONE, menuCursorImage);
		}
	}
	glColor(RGB15(31, 31, 31));
}