#include "psxsplash.h"


extern mm_sound_effect snd_playstation;

static bool psx_music = false;
static bool psx_fadedin = false;

static int psx_logoFadeColor = 0;	// 255 when faded in

extern int psxLogoTexID;
extern glImage psxLogoImage[(256 / 16) * (64 / 16)];

void psxInit(void) {
	psx_music = false;
	psx_fadedin = false;

	psx_logoFadeColor = 0;	// 255 when faded in
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
	swiWaitForVBlank();
}

void psxGraphicDisplay(void) {
	glColor(RGB15(psx_logoFadeColor/8, psx_logoFadeColor/8, psx_logoFadeColor/8));
	glSprite(85, 25, GL_FLIP_NONE, psxLogoImage);
	glColor(RGB15(31, 31, 31));
}