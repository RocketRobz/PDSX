#include "lodepng.h"
#include "psxmenu.h"

#include "bg_blue_png_bin.h"

static bool menu_bgLoaded = false;
static bool musicStopped = false;
static bool displayMenuGraphics = false;
static int menu_cursor = 0;
static bool menu_textFade = false;

static int menu_textFadeColor = 255;	// 0 when faded out

extern int psConsoleModel;					// 0 = Playstation -> PS, 1 = PSone

extern int pressed;

extern int gridBgTexID;
extern glImage blueBallsImage[(64 / 16) * (128 / 64)];
extern glImage mainMenuImage[(128 / 16) * (32 / 16)];
extern glImage inkedButtonImage[(128 / 16) * (32 / 16)];
extern glImage memandcdImage[(128 / 16) * (64 / 16)];
extern glImage menuCursorImage[(16 / 16) * (32 / 16)];
extern u16 blueBg[2][256*192];

void psxMenuInit(void) {
	if (!menu_bgLoaded) {
		std::vector<unsigned char> image;
		unsigned width, height;
		lodepng::decode(image, width, height, bg_blue_png_bin, bg_blue_png_bin_size);
		int x = 0;
		int x2 = 255;
		int y = 0;
		bool alternatePixel = false;
		for(unsigned i=0;i<image.size()/4;i++) {
			image[(i*4)+3] = 0;
			if (alternatePixel) {
				if (image[(i*4)] >= 0x4) {
					image[(i*4)] -= 0x4;
					image[(i*4)+3] |= BIT(0);
				}
				if (image[(i*4)+1] >= 0x4) {
					image[(i*4)+1] -= 0x4;
					image[(i*4)+3] |= BIT(1);
				}
				if (image[(i*4)+2] >= 0x4) {
					image[(i*4)+2] -= 0x4;
					image[(i*4)+3] |= BIT(2);
				}
			}
			blueBg[0][(y*256)+x] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
			blueBg[0][(y*256)+x2] = blueBg[0][(y*256)+x];
			if (alternatePixel) {
				if (image[(i*4)+3] & BIT(0)) {
					image[(i*4)] += 0x4;
				}
				if (image[(i*4)+3] & BIT(1)) {
					image[(i*4)+1] += 0x4;
				}
				if (image[(i*4)+3] & BIT(2)) {
					image[(i*4)+2] += 0x4;
				}
			} else {
				if (image[(i*4)] >= 0x4) {
					image[(i*4)] -= 0x4;
				}
				if (image[(i*4)+1] >= 0x4) {
					image[(i*4)+1] -= 0x4;
				}
				if (image[(i*4)+2] >= 0x4) {
					image[(i*4)+2] -= 0x4;
				}
			}
			blueBg[1][(y*256)+x] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
			blueBg[1][(y*256)+x2] = blueBg[1][(y*256)+x];
			if ((i % 128) == 127) alternatePixel = !alternatePixel;
			alternatePixel = !alternatePixel;
			x++;
			x2--;
			if (x==128) {
				x=0;
				x2=255;
				y++;
			}
		}
		menu_bgLoaded = true;
	}

	musicStopped = false;
	displayMenuGraphics = false;
	menu_cursor = 0;
	menu_textFade = false;

	menu_textFadeColor = 255;
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

	if (menu_textFade) {
		menu_textFadeColor -= 10;
		if (menu_textFadeColor < 0) {
			menu_textFadeColor = 255;
			menu_textFade = false;
		}
	} else {
		if (pressed & KEY_UP) {
			menu_cursor--;
			if (menu_cursor < 0) menu_cursor = 0;
		}

		if (pressed & KEY_DOWN) {
			menu_cursor++;
			if (menu_cursor > 1) menu_cursor = 1;
		}

		if (pressed & KEY_A) {
			menu_textFade = true;
		}
	}
}

void psxMenuGraphicDisplay(void) {
	if (displayMenuGraphics) {
		glSprite(39, 6, GL_FLIP_NONE, &blueBallsImage[0]);
		glSprite(83, 128, GL_FLIP_NONE, &blueBallsImage[1]);
		glSprite(181, 17, GL_FLIP_NONE, mainMenuImage);
		glSprite(22, 79, GL_FLIP_NONE, inkedButtonImage);
		glSprite(22, 119, GL_FLIP_NONE, inkedButtonImage);
		if (menu_cursor == 0) {
			glColor(RGB15(menu_textFadeColor/8, menu_textFadeColor/8, menu_textFadeColor/8));
			glSprite(10, 87, GL_FLIP_NONE, &memandcdImage[0]);			// Zoomed-in "Memory Card"
			glColor(RGB15(31, 31, 31));
			glSprite(26, 127, GL_FLIP_NONE, &memandcdImage[3]);		// Zoomed-out "CD Player"
			if (!menu_textFade) glSprite(66, 94, GL_FLIP_NONE, &menuCursorImage[0]);
		} else {
			glSprite(10, 87, GL_FLIP_NONE, &memandcdImage[1]);			// Zoomed-out "Memory Card"
			glColor(RGB15(menu_textFadeColor/8, menu_textFadeColor/8, menu_textFadeColor/8));
			glSprite(26, 127, GL_FLIP_NONE, &memandcdImage[2]);		// Zoomed-in "CD Player"
			glColor(RGB15(31, 31, 31));
			if (!menu_textFade) glSprite(66, 135, GL_FLIP_NONE, &menuCursorImage[0]);
		}
	}
	glColor(RGB15(31, 31, 31));
}