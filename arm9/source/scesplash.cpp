#include "scesplash.h"

extern int gameMode;
extern int modeOrder;

extern mm_sound_effect snd_sce;

static bool sce_music = false;
static int sce_fadeTime[2] = {0};
static bool sce_fadedin = false;

static int sce_bgColor = 0;	// 185 when faded in

static int sce_triangle1_x[2] = {77, 127};
static int sce_triangle1_y[3] = {95, 45, 145};
static int sce_triangle2_x[2] = {177, 127};
static int sce_triangle2_y[3] = {95, 45, 145};

static int sce_triangle_zoomdelay[5] = {0};
static bool sce_triangle_zoomdelay3AddDelay = false;

static bool sce_trianglesFormed = false;

static int sce_loopOnLogo = 0;

extern int sceLogoTexID;
extern glImage sceLogoImage[(128 / 16) * (48 / 24)];

void sceInit(void) {
	sce_music = false;
	sce_fadeTime[0] = 0;
	sce_fadeTime[1] = 0;
	sce_fadedin = false;

	sce_bgColor = 0;	// 185 when faded in

	sce_triangle1_x[0] = 77;
	sce_triangle1_x[1] = 127;
	sce_triangle1_y[0] = 95;
	sce_triangle1_y[1] = 45;
	sce_triangle1_y[2] = 145;
	sce_triangle2_x[0] = 177;
	sce_triangle2_x[1] = 127;
	sce_triangle2_y[0] = 95;
	sce_triangle2_y[1] = 45;
	sce_triangle2_y[2] = 145;

	sce_triangle_zoomdelay[0] = 0;
	sce_triangle_zoomdelay[1] = 0;
	sce_triangle_zoomdelay[2] = 0;
	sce_triangle_zoomdelay[3] = 0;
	sce_triangle_zoomdelay[4] = 0;
	sce_triangle_zoomdelay3AddDelay = false;

	sce_trianglesFormed = false;

	sce_loopOnLogo = 0;
}

void sceSplash(void) {
	if (!sce_music) {
		mmEffectEx(&snd_sce);
		sce_music = true;
	}

	if (!sce_fadedin) {
		if (sce_fadeTime[0] == 30) {
			if (sce_fadeTime[1] == 60) {
				sce_fadedin = true;
			} else {
				sce_fadeTime[1]++;
				sce_bgColor += 3;
			}
		} else {
			sce_fadeTime[0]++;
		}
	} else {
		sce_bgColor = 185;

		sce_triangle_zoomdelay[0]++;
		if (sce_triangle_zoomdelay[0] == 2) {
			sce_triangle_zoomdelay[0] = 0;

			if (sce_triangle1_x[0] != 109) {
				sce_triangle1_x[0]++;
			}

			if (sce_triangle2_x[0] != 146) {
				sce_triangle2_x[0]--;
			}

		}

		sce_triangle_zoomdelay[1]++;
		if (sce_triangle_zoomdelay[1] == 10) {
			sce_triangle_zoomdelay[1] = 0;

			if (sce_triangle1_x[1] != 131) {
				sce_triangle1_x[1]++;
			}

			if (sce_triangle2_x[1] != 123) {
				sce_triangle2_x[1]--;
			}
		}

		sce_triangle_zoomdelay[2]++;
		if (sce_triangle_zoomdelay[2] == 3) {
			sce_triangle_zoomdelay[2] = 0;

			if (sce_triangle1_y[0] != 76) {
				sce_triangle1_y[0]--;
			}
			if (sce_triangle2_y[0] != 113) {
				sce_triangle2_y[0]++;
			}
		}

		// Bottom pointy edge of left triangle, and top pointy edge of right triengle
		sce_triangle_zoomdelay[3]++;
		if (sce_triangle_zoomdelay[3] == 1+sce_triangle_zoomdelay3AddDelay) {
			sce_triangle_zoomdelay[3] = 0;
			sce_triangle_zoomdelay3AddDelay = !sce_triangle_zoomdelay3AddDelay;

			if (sce_triangle1_y[2] != 98) {
				sce_triangle1_y[2]--;
			}
			if (sce_triangle2_y[1] != 90) {
				sce_triangle2_y[1]++;
			} else {
				sce_trianglesFormed = true;
			}
		}

		// Top pointy edge of left triangle, and bottom pointy edge of right triengle
		sce_triangle_zoomdelay[4]++;
		if (sce_triangle_zoomdelay[4] == 5) {
			sce_triangle_zoomdelay[4] = 0;

			if (sce_triangle1_y[1] != 54) {
				sce_triangle1_y[1]++;
			}
			if (sce_triangle2_y[2] != 136) {
				sce_triangle2_y[2]--;
			}
		}
	}

	sce_loopOnLogo++;
	swiWaitForVBlank();

	if (sce_loopOnLogo == 60*7) {
		sce_fadedin = false;
		if (modeOrder == 1) {
			gameMode = 2;	// Go to Main Menu
		} else {
			for (int i = 185; i >= 0; i -= 10) {
				sce_bgColor = i;
				swiWaitForVBlank();
			}
			sce_bgColor = 0;
			gameMode = 1;	// Go to PSX splash
		}
	}
}

void sceGraphicDisplay(void) {
	glBoxFilled(0, 0, 256, 192, RGB15(sce_bgColor/8, sce_bgColor/8, sce_bgColor/8));
	if (sce_fadedin) {
		// Draw left half of back triangle
		glTriangleFilledGradient(77, 95,
								127, 45,
								127, 145,
								RGB15(184/8, 14/8, 9/8), RGB15(189/8, 144/8, 0/8), RGB15(189/8, 144/8, 0/8));
		// Draw right half of back triangle
		glTriangleFilledGradient(177, 95,
								127, 45,
								127, 145,
								RGB15(184/8, 14/8, 9/8), RGB15(189/8, 144/8, 0/8), RGB15(189/8, 144/8, 0/8));
		// Draw right half of front triangle
		glTriangleFilledGradient(sce_triangle2_x[0], sce_triangle2_y[0],
								sce_triangle2_x[1], sce_triangle2_y[1],
								sce_triangle2_x[1], sce_triangle2_y[2],
								RGB15(184/8, 14/8, 9/8), RGB15(189/8, 144/8, 0/8), RGB15(189/8, 144/8, 0/8));
		// Draw left half of front triangle
		glTriangleFilledGradient(sce_triangle1_x[0], sce_triangle1_y[0],
								sce_triangle1_x[1], sce_triangle1_y[1],
								sce_triangle1_x[1], sce_triangle1_y[2],
								RGB15(184/8, 14/8, 9/8), RGB15(189/8, 144/8, 0/8), RGB15(189/8, 144/8, 0/8));
		if (sce_trianglesFormed) {
			glSprite(80, 23, GL_FLIP_NONE, &sceLogoImage[0]);
			glSprite(80, 154, GL_FLIP_NONE, &sceLogoImage[1]);
		}
	}
	glColor(RGB15(31, 31, 31));
}