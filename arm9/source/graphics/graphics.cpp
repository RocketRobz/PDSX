/*-----------------------------------------------------------------
 Copyright (C) 2015
	Matthew Scholefield

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
#include <gl2d.h>
#include "bios_decompress_callback.h"
#include "FontGraphic.h"

#include "logo_sce.h"
#include "logo_scetm.h"
#include "logo_ps.h"
#include "logo_playstation.h"
#include "text_scea.h"
#include "bg_blueballs.h"
#include "text_mainmenu.h"
#include "button_inked.h"
#include "text_memandcd.h"
#include "button_memandcd.h"
#include "icon_memcard.h"
#include "icon_cd.h"
#include "cursor_psx.h"

#include "graphics.h"
#include "fontHandler.h"

#include "../scesplash.h"
#include "../psxsplash.h"
#include "../psxmenu.h"
#include "../psonemenu.h"

#define CONSOLE_SCREEN_WIDTH 32
#define CONSOLE_SCREEN_HEIGHT 24

//extern bool fadeType;
//int screenBrightness = 31;

extern int gameMode;
extern int psConsoleModel;

int sceLogoTexID, sceTmTexID, psxLogoTexID, playstationLogoTexID, sceaTexID;
int blueBgTexID, blueBallsTexID, mainMenuTexID, inkedButtonTexID, memandcdTexID, menuCursorTexID;
int gridBgTexID, memandcdButtonTexID, memCardTexID, cdTexID;
glImage sceLogoImage[(128 / 16) * (96 / 24)];
glImage sceTmImage[(16 / 16) * (16 / 16)];
glImage psxLogoImage[(128 / 16) * (80 / 16)];
glImage playstationLogoImage[(128 / 16) * (32 / 16)];
glImage sceaImage[(256 / 16) * (64 / 16)];
glImage blueBgImage[(128 / 16) * (192 / 16)];
glImage gridBgImage[(256 / 16) * (192 / 16)];
glImage blueBallsImage[(64 / 16) * (128 / 64)];
glImage mainMenuImage[(128 / 16) * (32 / 16)];
glImage inkedButtonImage[(128 / 16) * (32 / 16)];
glImage memandcdImage[(128 / 16) * (64 / 16)];
glImage memandcdButtonImage[(128 / 16) * (64 / 24)];
glImage memCardImage[(64 / 16) * (64 / 16)];
glImage cdImage[(64 / 16) * (64 / 16)];
glImage menuCursorImage[(16 / 16) * (32 / 16)];

void vramcpy_ui (void* dest, const void* src, int size) 
{
	u16* destination = (u16*)dest;
	u16* source = (u16*)src;
	while (size > 0) {
		*destination++ = *source++;
		size-=2;
	}
}

// Ported from PAlib (obsolete)
void SetBrightness(u8 screen, s8 bright) {
	u16 mode = 1 << 14;

	if (bright < 0) {
		mode = 2 << 14;
		bright = -bright;
	}
	if (bright > 31) bright = 31;
	*(u16*)(0x0400006C + (0x1000 * screen)) = bright + mode;
}

//-------------------------------------------------------
// set up a 2D layer construced of bitmap sprites
// this holds the image when rendering to the top screen
//-------------------------------------------------------

void initSubSprites(void)
{

	oamInit(&oamSub, SpriteMapping_Bmp_2D_256, false);
	int id = 0;

	//set up a 4x3 grid of 64x64 sprites to cover the screen
	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 4; x++)
		{
			oamSub.oamMemory[id].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
			oamSub.oamMemory[id].attribute[1] = ATTR1_SIZE_64 | (64 * x);
			oamSub.oamMemory[id].attribute[2] = ATTR2_ALPHA(1) | (8 * 32 * y) | (8 * x);
			++id;
		}

	swiWaitForVBlank();

	oamUpdate(&oamSub);
}

void drawBG(glImage *images)
{
	for (int y = 0; y < 256 / 16; y++)
	{
		for (int x = 0; x < 256 / 16; x++)
		{
			int i = y * 16 + x;
			glSprite(x * 16, y * 16, GL_FLIP_NONE, &images[i & 255]);
		}
	}
}

void vBlankHandler()
{
	/* if(fadeType == true) {
		screenBrightness--;
		if (screenBrightness < 0) screenBrightness = 0;
	} else {
		screenBrightness++;
		if (screenBrightness > 31) screenBrightness = 31;
	}
	SetBrightness(0, screenBrightness);
	if (flashcardUsed) {
		if (screenmode == 1) SetBrightness(1, screenBrightness);
	} else {
		SetBrightness(1, screenBrightness);
	} */

	glBegin2D();
	{
		if (gameMode == 1) {
			psxGraphicDisplay();
		} else if (gameMode == 2) {
			if (psConsoleModel == 1) {
				psoneMenuGraphicDisplay();
			} else {
				psxMenuGraphicDisplay();
			}
		} else {
			sceGraphicDisplay();
		}
	}
	glEnd2D();
	GFX_FLUSH = 0;
}

void graphicsInit()
{
	/* *(u16*)(0x0400006C) |= BIT(14);
	*(u16*)(0x0400006C) &= BIT(15);
	SetBrightness(0, 31);
	SetBrightness(1, 31); */
	
	irqSet(IRQ_VBLANK, vBlankHandler);
	irqEnable(IRQ_VBLANK);
	////////////////////////////////////////////////////////////
	videoSetMode(MODE_5_3D);
	//videoSetModeSub(MODE_5_2D);

	// Initialize GL in 3D mode
	glScreen2D();

	// Set up enough texture memory for our textures
	// Bank A is just 128kb and we are using 194 kb of
	// sprites
	// vramSetBankA(VRAM_A_TEXTURE);
	vramSetBankB(VRAM_B_TEXTURE);
	vramSetBankF(VRAM_F_TEX_PALETTE); // Allocate VRAM bank for all the palettes
	vramSetBankE(VRAM_E_MAIN_BG);

	// Sony Computer Entertainment
	sceLogoTexID = glLoadTileSet(sceLogoImage, // pointer to glImage array
							128, // sprite width
							24, // sprite height
							128, // bitmap width
							96, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) logo_scePal, // Load our 16 color tiles palette
							(u8*) logo_sceBitmap // image data generated by GRIT
							);

	sceTmTexID = glLoadTileSet(sceTmImage, // pointer to glImage array
							16, // sprite width
							16, // sprite height
							16, // bitmap width
							16, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_16, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_16, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) logo_scetmPal, // Load our 16 color tiles palette
							(u8*) logo_scetmBitmap // image data generated by GRIT
							);

	// Playstation
	psxLogoTexID = glLoadTileSet(psxLogoImage, // pointer to glImage array
							128, // sprite width
							80, // sprite height
							128, // bitmap width
							80, // bitmap height
							GL_RGB256, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF, // param for glTexImage2D() in videoGL.h
							256, // Length of the palette to use (256 colors)
							(u16*) logo_psPal, // Load our 256 color tiles palette
							(u8*) logo_psBitmap // image data generated by GRIT
							);

	playstationLogoTexID = glLoadTileSet(playstationLogoImage, // pointer to glImage array
							128, // sprite width
							16, // sprite height
							128, // bitmap width
							32, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_32, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) logo_playstationPal, // Load our 16 color tiles palette
							(u8*) logo_playstationBitmap // image data generated by GRIT
							);

	sceaTexID = glLoadTileSet(sceaImage, // pointer to glImage array
							256, // sprite width
							64, // sprite height
							256, // bitmap width
							64, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_256, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) text_sceaPal, // Load our 16 color tiles palette
							(u8*) text_sceaBitmap // image data generated by GRIT
							);

	// Main menu (Playstation)
	blueBallsTexID = glLoadTileSet(blueBallsImage, // pointer to glImage array
							64, // sprite width
							64, // sprite height
							64, // bitmap width
							128, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) bg_blueballsPal, // Load our 16 color tiles palette
							(u8*) bg_blueballsBitmap // image data generated by GRIT
							);

	inkedButtonTexID = glLoadTileSet(inkedButtonImage, // pointer to glImage array
							128, // sprite width
							32, // sprite height
							128, // bitmap width
							32, // bitmap height
							GL_RGB256, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_32, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							64, // Length of the palette to use (64 colors)
							(u16*) button_inkedPal, // Load our 64 color tiles palette
							(u8*) button_inkedBitmap // image data generated by GRIT
							);

	mainMenuTexID = glLoadTileSet(mainMenuImage, // pointer to glImage array
							128, // sprite width
							32, // sprite height
							128, // bitmap width
							32, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_32, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (16 colors)
							(u16*) text_mainmenuPal, // Load our 16 color tiles palette
							(u8*) text_mainmenuBitmap // image data generated by GRIT
							);

	memandcdTexID = glLoadTileSet(memandcdImage, // pointer to glImage array
							128, // sprite width
							16, // sprite height
							128, // bitmap width
							64, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							3, // Length of the palette to use (3 colors)
							(u16*) text_memandcdPal, // Load our 3 color tiles palette
							(u8*) text_memandcdBitmap // image data generated by GRIT
							);

	menuCursorTexID = glLoadTileSet(menuCursorImage, // pointer to glImage array
							16, // sprite width
							16, // sprite height
							16, // bitmap width
							32, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_16, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_32, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							13, // Length of the palette to use (13 colors)
							(u16*) cursor_psxPal, // Load our 13 color tiles palette
							(u8*) cursor_psxBitmap // image data generated by GRIT
							);

	// Main menu (PSone)
	memandcdButtonTexID = glLoadTileSet(memandcdButtonImage, // pointer to glImage array
							128, // sprite width
							24, // sprite height
							128, // bitmap width
							48, // bitmap height
							GL_RGB256, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_128, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							64, // Length of the palette to use (256 colors)
							(u16*) button_memandcdPal, // Load our 256 color tiles palette
							(u8*) button_memandcdBitmap // image data generated by GRIT
							);

	memCardTexID = glLoadTileSet(memCardImage, // pointer to glImage array
							64, // sprite width
							64, // sprite height
							64, // bitmap width
							64, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (13 colors)
							(u16*) icon_memcardPal, // Load our 13 color tiles palette
							(u8*) icon_memcardBitmap // image data generated by GRIT
							);

	cdTexID = glLoadTileSet(cdImage, // pointer to glImage array
							64, // sprite width
							64, // sprite height
							64, // bitmap width
							64, // bitmap height
							GL_RGB16, // texture type for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeX for glTexImage2D() in videoGL.h
							TEXTURE_SIZE_64, // sizeY for glTexImage2D() in videoGL.h
							GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
							16, // Length of the palette to use (13 colors)
							(u16*) icon_cdPal, // Load our 13 color tiles palette
							(u8*) icon_cdBitmap // image data generated by GRIT
							);
}
