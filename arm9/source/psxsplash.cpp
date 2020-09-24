#include "psxsplash.h"


extern int psConsoleModel;					// 0 = Playstation -> PS, 1 = PSone

extern mm_sound_effect snd_playstation;

static bool psx_music = false;
static bool psx_fadedin = false;

static int psx_logoFadeColor = 0;	// 255 when faded in
static int psx_textFadeColor = 0;	// 255 when faded in

extern int psxLogoTexID;
extern glImage psxLogoImage[(128 / 16) * (80 / 16)];
extern glImage playstationLogoImage[(128 / 16) * (32 / 16)];
extern glImage sceaImage[(256 / 16) * (64 / 16)];

void psxInit(void) {
	psx_music = false;
	psx_fadedin = false;

	psx_logoFadeColor = 0;	// 255 when faded in
	psx_textFadeColor = 0;	// 255 when faded in
}

void psxSplash(void) {
	if (!psx_music) {
		mmEffectEx(&snd_playstation);
		psx_music = true;
	}

	psx_logoFadeColor += 8;
	if (psx_logoFadeColor >= 255) {
		psx_logoFadeColor = 255;
	}
	
	if (psx_logoFadeColor == 255) {
		psx_textFadeColor += 8;
		if (psx_textFadeColor >= 255) {
			psx_textFadeColor = 255;
		}
	}
}

void psxGraphicDisplay(void) {
	glColor(RGB15(psx_logoFadeColor/8, psx_logoFadeColor/8, psx_logoFadeColor/8));
	glSprite(85, 25, GL_FLIP_NONE, psxLogoImage);
	if (psx_logoFadeColor == 255) {
		glColor(RGB15(psx_textFadeColor/8, psx_textFadeColor/8, psx_textFadeColor/8));
		if (psConsoleModel == 1) {
			glSprite(91, 108, GL_FLIP_NONE, &playstationLogoImage[1]);
		} else {
			glSprite(91, 109, GL_FLIP_NONE, &playstationLogoImage[0]);
		}
		glColor(RGB15(31, 31, 31));
		glSprite(0, 128, GL_FLIP_NONE, sceaImage);
	}
	glColor(RGB15(31, 31, 31));
}