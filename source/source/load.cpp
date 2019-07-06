/*
-------------------------------------------------

	Krystals DS

	Archivo de carga de archivos

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por Cesar Rincon "NightFox"
	http://www.nightfoxandco.com
	Inicio 08 de Septiembre del 2012

	(c) 2012 - 2013 NightFox & Co.

-------------------------------------------------
*/



/*
-------------------------------------------------
	Includes
-------------------------------------------------
*/

// Includes C
#include <stdio.h>
// Includes propietarios NDS
#include <nds.h>
// Includes librerias propias
#include <nf_lib.h>
// Includes del juego
#include "includes.h"



/*
Metodos de la clase "CLoad"
*/

// Contructor clase CLoad
CLoad::CLoad(void) {
}



// Destructor clase CLoad
CLoad::~CLoad(void) {
}



// Metodos globales de la clase CLoad
CLoad* Load;



// Carga de archivos para el juego (comunes)
void CLoad::GameMainFiles(void) {

	// Variables locales
	char filename[64];
	char name[32];
	u8 n = 0;
	u8 id = 0;

	// Carga los fondos del marco
	NF_LoadTiledBg("bg/game/mainframe_up", "frame_up", 256, 256);
	NF_LoadTiledBg("bg/game/mainframe_down", "frame_down", 256, 256);

	// Carga los fondos del stage
	for (n = 1; n <= BG_NUMBER; n ++) {
		sprintf(filename, "bg/game/bg%02d", n);
		sprintf(name, "bg%02d", n);
		NF_LoadAffineBg(filename, name, 512, 512);
	}
	

	// Carga los sprites de las fichas
	NF_LoadSpriteGfx("sprite/game/gems", GEM_GFX, 16, 16);
	NF_LoadSpritePal("sprite/game/pal00", GEM_PAL);
	// Carga los sprites para el Score
	NF_LoadSpriteGfx("sprite/game/score", SCORE_GFX, 32, 32);
	NF_LoadSpriteGfx("sprite/game/combo", COMBO_GFX, 32, 16);
	NF_LoadSpriteGfx("sprite/game/numbers", NUMBERS_GFX, 16, 16);
	NF_LoadSpritePal("sprite/game/pal01", SCORE_PAL);
	// Carga el sprite para la pausa
	NF_LoadSpriteGfx("sprite/game/pause", PAUSE_GFX, 64, 64);
	NF_LoadSpritePal("sprite/game/pal02", PAUSE_PAL);
	// Carga el sprite para el banner de fin de juego
	NF_LoadSpriteGfx("sprite/game/end1", ENDGAME_GFX, 64, 32);
	NF_LoadSpriteGfx("sprite/game/end2", (ENDGAME_GFX + 1), 64, 32);
	NF_LoadSpritePal("sprite/game/pal03", ENDGAME_PAL);

	// Carga la fuente principal para los textos
	NF_LoadTextFont("font/gamefont", "text_up", 256, 256, 0);
	NF_LoadTextFont("font/gamefont", "text_down", 256, 256, 0);

	// Carga los archivos RAW de sonido (SFX)
	for (n = 0; n < SFX_NUMBER; n ++) {
		sprintf(filename, "snd/sfx/%s", Sound->sfx[n].file);		// Genera la ruta de carga  del archivo seleccionado
		NF_LoadRawSound(filename, id, Sound->sfx[n].freq, 0);		// Carga el archivo
		Sound->sfx[n].id = id;			// Guarda la ID asignada
		id ++;							// Actualiza el marcador de ID
	}

	// Carga los archivos RAW de sonido (VOCES)
	for (n = 0; n < VOICE_NUMBER; n ++) {
		sprintf(filename, "snd/voice/%s", Sound->voice.file[n]);	// Genera la ruta de carga  del archivo seleccionado
		NF_LoadRawSound(filename, id, Sound->voice.freq[n], 0);		// Carga el archivo
		Sound->voice.id[n] = id;		// Guarda la ID asignada
		id ++;							// Actualiza el marcador de ID
	}

}



// Carga los archivos para el menu principal
void CLoad::MenuMainFiles(void) {

	// Carga los fondos del menu principal
	NF_LoadTiledBg("bg/menu/menu_bg_top", "menu_bg_top", 256, 256);
	NF_LoadTiledBg("bg/menu/menu_bg_bottom", "menu_bg_bottom", 256, 256);
	NF_LoadTiledBg("bg/menu/menu_title", "menu_title", 256, 256);

	// Carga los fondos para los records
	NF_LoadTiledBg("bg/menu/records_shadows", "records_shadows", 512, 256);

	// Carga los Sprites
	NF_LoadSpriteGfx("sprite/menu/button_play", 0, 64, 32);
	NF_LoadSpriteGfx("sprite/menu/button_records", 1, 64, 32);
	NF_LoadSpriteGfx("sprite/menu/button_credits", 2, 64, 32);
	NF_LoadSpritePal("sprite/menu/menu_buttons", 0);

	// Carga la fuente
	NF_LoadTextFont("font/menufont", "records", 512, 256, 0);

}



// Carga los archivos para los splash screens
void CLoad::SplashScreensFiles(void) {

	// Carga los fondos
	NF_LoadTiledBg("bg/splash/nfco", "nfco", 256, 256);
	NF_LoadTiledBg("bg/splash/neoflash", "neoflash", 256, 256);
	NF_LoadTiledBg("bg/splash/nfl", "nfl", 256, 256);
	NF_LoadTiledBg("bg/splash/devkitpro", "devkitpro", 256, 256);

}



// Carga los archivos para el resumen de puntuacion
void CLoad::RecordsFiles(void) {

	// Carga los fondos
	NF_LoadTiledBg("bg/records/rec_up", "bgtop", 256, 256);
	NF_LoadTiledBg("bg/records/rec_down", "bgbottom", 256, 256);

	// Carga las fuentes a usar
	NF_LoadTextFont("font/menufont", "report", 256, 256, 0);
	NF_LoadTextFont("font/menufont", "records", 256, 256, 0);

}
