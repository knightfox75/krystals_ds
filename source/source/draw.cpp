/*
-------------------------------------------------

	Krystals DS

	Archivo de funciones de dibujado del juego

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
// Includes C++
#include <vector>
// Includes propietarios NDS
#include <nds.h>
// Includes librerias propias
#include <nf_lib.h>
// Includes del juego
#include "includes.h"



/*
Metodos de la clase "CDraw"
*/

// Contructor clase CDraw
CDraw::CDraw(void) {

	// Control del fade
	bright = 0;
	fade_in = true;
	fade_out = false;

	// Control de Erase
	erase_frame = 0;
	erase_delay = 0;

	// Modo 7
	mode7_scroll = 0;
	mode7_angle = 0;
	memset(shading_table, 0, sizeof(shading_table));
	memset(shading, 0, sizeof(shading));

	// Inicializaciones del cambio de fondo
	background.update = false;
	background.thread = BGCHANGE_START;
	background.next_thread = BGCHANGE_START;
	background.bright = 0;
	background.num = 0;
	memset((void*)&background.list, 0, sizeof(background.list));

	// Flags de control de actualizacion de las gemas en pantalla
	update.board = true;
	update.next_gem = true;
	update.my_gem = true;
	update.gem_pos = true;
	update.text = true;

	// Control de la animacion de destello
	memset((void*)&sparkle, 0, sizeof(sparkle));

	// Manda el cache a la ram
	DC_FlushRange(shading_table, sizeof(shading_table));
	DC_FlushRange(shading, sizeof(shading));
	DC_FlushRange((void*)&background.list, sizeof(background.list));
	DC_FlushRange((void*)&sparkle, sizeof(sparkle));

}



// Destructor clase CGame
CDraw::~CDraw(void) {

}



// Metodos globales de la clase CDraw
CDraw* Draw;



/*
Efectos de FADE
*/

// Efecto fade-in
bool CDraw::FadeIn(void) {

	bool done = false;

	bright += 64;
	if (bright >= (16 << 8)) bright = (16 << 8);
	setBrightness(3, ((bright >> 8) - 16));
	if (bright == (16 << 8)) {
		fade_in = false;
		bright = 0;
		done = true;
	}

	return done;

}



// Efecto fade-out
bool CDraw::FadeOut(void) {

	bool done = false;

	bright += 64;
	if (bright >= (16 << 8)) bright = (16 << 8);
	setBrightness(3, -(bright >> 8));
	if (bright == (16 << 8)) {
		fade_out = false;
		bright = 0;
		done = true;
	}

	return done;

}



/*
Dibujado y actualizacion de graficos en pantalla
*/

// Actualiza la pantalla
void CDraw::UpdateScreen(void) {

	// Si no esta en pausa, animacion del destello
	if (!Engine->pause) SparkleAnimation();
	// Actualizacion del tablero
	if (update.board) {
		UpdateBoard();
		update.board = false;
	}

	// Actualiza el marcador NEXT GEM
	if (update.next_gem) {
		UpdateNextGem();
		update.next_gem = false;
	}
	// Actualiza la gema en movimiento
	if (update.my_gem) {
		UpdateMyGem();
		update.my_gem = false;
	}
	// Actualiza la posicion de la gema
	if (update.gem_pos) {
		UpdateGemPos();
		update.gem_pos = false;
	}

	// Actualiza el "Score" si es necesario
	if (Score->update.score) {
		Score->UpdateScore();
		Score->update.score = false;
	}
	// Actualiza el "Level" si es necesario
	if (Score->update.level) {
		Score->UpdateLevel();
		Score->update.level = false;
	}
	// Actualiza el "Left" si es necesario
	if (Score->update.left) {
		Score->UpdateLeft();
		Score->update.left = false;
	}

	// Actualiza las capas de texto si es necesario
	if (update.text) {
		NF_UpdateTextLayers();
		update.text = false;
	}

	// Control de visibilidad de los banners
	if (Score->banner_combo > 0) {
		Score->banner_combo --;
		if (Score->banner_combo == 0) {
			Score->DeleteComboBanner();
			Score->DeleteScoreBanner();
		}
	}

	// Si es necesario, cambia de fondo
	if (background.update) ChangeBg();

	// Actualiza el OAM de NFlib
	NF_SpriteOamSet(0);
	NF_SpriteOamSet(1);

	// Sincronismo
	swiWaitForVBlank();

	// Actualiza el OAM de Libnds
	oamUpdate(&oamMain);
	oamUpdate(&oamSub);

	// Si no esta en pausa, animacion del fondo
	if (!Engine->pause) Mode7Animation();

}



// Dibujado inicial del decorado
void CDraw::DrawStage(void) {

	// Variables
	u8 scr = 0;
	u8 board_x, board_y;	// Indice de la tabla
	s32 pos_x, pos_y;		// Posicion de los sprites
	u8 spr;					// Indice del Sprite
	u8 n = 0;				// Uso general

	// Cambio de modo
	NF_Set2D(0, 2);		// Modo 2 en ambas pantallas (modo affine)
	NF_Set2D(1, 2);

	// Reinicializa los sistemas de fondos y sprites despues de cambiar de modo Grafico
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);
	NF_InitSpriteSys(0);
	NF_InitSpriteSys(1);

	// Crea el decorado inicial
	char bgname[8];
	sprintf(bgname, "bg%02d", background.list[background.num]);
	NF_CreateAffineBg(0, LAYER_BG, bgname, 1);		// Crea los fondos Affine
	NF_CreateAffineBg(1, LAYER_BG, bgname, 1);
	NF_AffineBgCenter(0, LAYER_BG, 128, 512);		// Cambiales el centro de referencia
	NF_AffineBgCenter(1, LAYER_BG, 128, 512);
	mode7_scroll = 0;
	mode7_angle = 0;

	// Crea los fondos del marco
	NF_CreateTiledBg(0, LAYER_FRAME, "frame_up");
	NF_CreateTiledBg(1, LAYER_FRAME, "frame_down");

	// Habilita el efecto sombreado en el fondo
	REG_BLDCNT = BLEND_FADE_BLACK | BLEND_SRC_BG3;
	REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG3;
	// Crea la tabla de shading
	MakeShadeTable();

	// Prepara el tablero de fichas (Carga de Gfx)
	for (scr = 0; scr < 2; scr ++) {
		NF_VramSpriteGfx(scr, GEM_GFX, GEM_GFX, false);
		NF_VramSpritePal(scr, GEM_PAL, GEM_PAL);
	}

	// Crea los sprites de las gemas del tablero en ambas pantallas (LAYER_BOARD)
	board_x = 0, board_y = 0;
	for (scr = 0; scr < 2; scr ++) {
		// Coordenadas iniciales
		pos_x = 32;
		if (scr == 0) {
			pos_y = 32;
		} else {
			pos_y = 0;
		}
		// Crea los sprites
		spr = GEM_SPR;
		for (board_y = 0; board_y < 10; board_y ++) {
			for (board_x = 0; board_x < 8; board_x ++) {
				NF_CreateSprite(scr, spr, GEM_GFX, GEM_PAL, pos_x, pos_y);
				NF_SpriteLayer(scr, spr, LAYER_BOARD);
				NF_SpriteFrame(scr, spr, 0);
				NF_ShowSprite(scr, spr, false);
				spr ++;
				pos_x += 16;
			}
			pos_x = 32;
			pos_y += 16;
		}

	}

	// Crea los sprites del marcador "next gem" (LAYER_GUI)
	for (n = 0; n < 3; n ++) {
		NF_CreateSprite(0, (GEM_NEXT_SPR + n), GEM_GFX, GEM_PAL, GEM_NEXT_X, (GEM_NEXT_Y + (n << 4)));
		NF_SpriteLayer(0, (GEM_NEXT_SPR + n), LAYER_GUI);
		NF_SpriteFrame(0, (GEM_NEXT_SPR + n), 0);
	}

	// Crea los sprites de la gema mobil (LAYER_BOARD)
	for (scr = 0; scr < 2; scr ++) {
		for (n = 0; n < 3; n ++) {
			NF_CreateSprite(scr, (GEM_MOV_SPR + n), GEM_GFX, GEM_PAL, -32, -32);
			NF_SpriteLayer(scr, (GEM_MOV_SPR + n), LAYER_BOARD);
			NF_SpriteFrame(scr, (GEM_MOV_SPR + n), 0);
		}
	}

	// Manda a la VRAM los GFX y las Paletas para el Score
	NF_VramSpriteGfx(1, SCORE_GFX, SCORE_GFX, false);
	NF_VramSpriteGfx(0, COMBO_GFX, COMBO_GFX, false);
	NF_VramSpriteGfx(0, NUMBERS_GFX, NUMBERS_GFX, false);
	NF_VramSpriteGfx(1, NUMBERS_GFX, NUMBERS_GFX, false);
	NF_VramSpritePal(0, SCORE_PAL, SCORE_PAL);
	NF_VramSpritePal(1, SCORE_PAL, SCORE_PAL);
	// Crea los Sprites de "Combo" (LAYER_GUI)
	NF_CreateSprite(0, COMBO_SPR, COMBO_GFX, SCORE_PAL, 63, 79);
	NF_SpriteLayer(0, COMBO_SPR, LAYER_GUI);
	NF_SpriteFrame(0, COMBO_SPR, 0);
	NF_ShowSprite(0, COMBO_SPR, false);
	NF_CreateSprite(0, (COMBO_SPR + 1), COMBO_GFX, SCORE_PAL, 95, 79);
	NF_SpriteLayer(0, (COMBO_SPR + 1), LAYER_GUI);
	NF_SpriteFrame(0, (COMBO_SPR + 1), 1);
	NF_ShowSprite(0, (COMBO_SPR + 1), false);
	for (n = 0; n < COMBO_NUM; n ++) {
		NF_CreateSprite(0, (NUMBERS_SPR + n), NUMBERS_GFX, SCORE_PAL, -32, -32);
		NF_SpriteLayer(0, (NUMBERS_SPR + n), LAYER_GUI);
		NF_SpriteFrame(0, (NUMBERS_SPR + n), 0);
		NF_ShowSprite(0, (NUMBERS_SPR + n), false);
	}
	// Crea los Sprites de "Score" (LAYER_GUI)
	NF_CreateSprite(1, SCORE_SPR, SCORE_GFX, SCORE_PAL, 63, 47);
	NF_SpriteLayer(1, SCORE_SPR, LAYER_GUI);
	NF_SpriteFrame(1, SCORE_SPR, 0);
	NF_ShowSprite(1, SCORE_SPR, false);
	NF_CreateSprite(1, (SCORE_SPR + 1), SCORE_GFX, SCORE_PAL, 95, 47);
	NF_SpriteLayer(1, (SCORE_SPR + 1), LAYER_GUI);
	NF_SpriteFrame(1, (SCORE_SPR + 1), 1);
	NF_ShowSprite(1, (SCORE_SPR + 1), false);
	for (n = 0; n < SCORE_NUM; n ++) {
		NF_CreateSprite(1, (NUMBERS_SPR + n), NUMBERS_GFX, SCORE_PAL, -32, -32);
		NF_SpriteLayer(1, (NUMBERS_SPR + n), LAYER_GUI);
		NF_SpriteFrame(1, (NUMBERS_SPR + n), 0);
		NF_ShowSprite(1, (NUMBERS_SPR + n), false);
	}

	// Crea los sprites de la pausa
	for (n = 0; n < 2; n ++) {
		// Manda los graficos a la VRAM
		NF_VramSpriteGfx(n, PAUSE_GFX, PAUSE_GFX, false);
		NF_VramSpritePal(n, PAUSE_PAL, PAUSE_PAL);
		// Crea los sprites en la pantalla
		NF_CreateSprite(n, PAUSE_SPR, PAUSE_GFX, PAUSE_PAL, -64, -64);
		NF_SpriteLayer(n, PAUSE_SPR, LAYER_GUI);
		NF_ShowSprite(n, PAUSE_SPR, false);
	}


	// Crea las capas de texto (LAYER_GUI)
	NF_CreateTextLayer(0, LAYER_GUI, 0, "text_up");
	NF_CreateTextLayer(1, LAYER_GUI, 0, "text_down");

}



// Actualizacion del de las gemas del tablero
void CDraw::UpdateBoard(void) {

	// Variables
	u8 scr = 0;
	u8 board_x, board_y;	// Indice de la tabla
	u8 idy;					// Indices del array
	u8 spr;					// Indice del Sprite
	u8 gem;					// Gema en el tablero
	u8 frame;				// Frame
	s8 spark;				// Control de animacion del destello

	// Actualiza los sprites de las fichas en ambas pantallas
	for (scr = 0; scr < 2; scr ++) {
		// Actualiza los frames de los sprites
		spr = GEM_SPR;
		// Dependiendo si hay que realizar la animacion de destello o no...
		if (sparkle.active) {	// Animacion de destello
			for (board_y = 0; board_y < 10; board_y ++) {
				for (board_x = 0; board_x < 8; board_x ++) {
					// Calcula los indices
					idy = (board_y + (scr * 10));
					// Si la ficha no esta pendiente de eliminacion...
					if (Board->erase[idy][board_x] == 0) {
						// Calcula el frame de animacion...
						if ((sparkle.frame[board_x] >= 0) && (sparkle.frame[board_x] <= 4)) {
							spark = (12 + sparkle.frame[board_x]);
						} else {
							spark = 0;
						}
						// Segun el valor....
						gem = Board->board[idy][board_x];
						// Actualiza el sprite
						if (gem == EMPTY) {
							// No hay ficha en esta casilla
							NF_SpriteFrame(scr, spr, GEM_GRAY);
							NF_ShowSprite(scr, spr, false);
						} else {
							// Hay una ficha
							frame = GetGemColorFrame(gem);
							// Si es una ficha de color...
							if ((gem >= BLUE) && (gem <= LILAC)) frame += spark;
							// Aplica la animacion
							NF_SpriteFrame(scr, spr, frame);
							NF_ShowSprite(scr, spr, true);
						}
					}
					// Siguiente sprite
					spr ++;
				}
			}
		} else {	// Actualizacion normal
			for (board_y = 0; board_y < 10; board_y ++) {
				for (board_x = 0; board_x < 8; board_x ++) {
					// Calcula los indices
					idy = (board_y + (scr * 10));
					// Si la ficha no esta pendiente de eliminacion...
					if (Board->erase[idy][board_x] == 0) {
						// Segun el valor....
						gem = Board->board[idy][board_x];
						// Actualiza el sprite
						if (gem == EMPTY) {
							// No hay ficha en esta casilla
							NF_SpriteFrame(scr, spr, GEM_GRAY);
							NF_ShowSprite(scr, spr, false);
						} else {
							// Hay una ficha
							frame = GetGemColorFrame(gem);
							NF_SpriteFrame(scr, spr, frame);
							NF_ShowSprite(scr, spr, true);
						}
					}
					// Siguiente sprite
					spr ++;
				}
			}
		}
	}

}



// Actualizacion de los elementos de la gema "NEXT" en la pantalla
void CDraw::UpdateNextGem(void) {

	// Variables
	u8 n = 0;
	u8 frame = 0;

	// Crea los sprites del marcador "next gem"
	for (n = 0; n < 3; n ++) {
		// Frame para mostrar la ficha correcta
		frame = GetGemColorFrame(Engine->next_gem[n]);
		// Si es una ficha especial
		if (Engine->next_gem_type == GEM_MORPH) frame += 11;
		// Actualiza el sprite
		NF_SpriteFrame(0, (GEM_NEXT_SPR + n), frame);
	}

}



// Actualizion de los elementos de la gema en movimiento
void CDraw::UpdateMyGem(void) {

	// Variables
	u8 n = 0;
	u8 scr = 0;
	u8 frame = 0;

	// Crea los sprites del marcador "next gem"
	for (n = 0; n < 3; n ++) {
		// Frame para mostrar la ficha correcta
		frame = GetGemColorFrame(Engine->gem[n]);
		// Si es una ficha especial
		if (Engine->gem_type == GEM_MORPH) frame += 11;
		// Actualiza el sprite
		for (scr = 0; scr < 2; scr ++) NF_SpriteFrame(scr, (GEM_MOV_SPR + n), frame);
	}

}



// Posiciona la gema en pantalla
void CDraw::UpdateGemPos(void) {

	// Variables
	u8 n = 0;
	s32 x, y;

	// Posiciona la gema en pantalla
	x = (32 + (Engine->gem_x << 4));
	for (n = 0; n < 3; n ++) {
		if ((Engine->gem_y + n) <= 9) {
			// Oculta la ficha de la pantalla inferior
			NF_MoveSprite(1, (GEM_MOV_SPR + n), -32, -32);
			// Coloca la ficha en la pantalla superior en su posicion
			y = (32 + ((Engine->gem_y + n) << 4));
			NF_MoveSprite(0, (GEM_MOV_SPR + n), x, y);
		} else {
			// Oculta la ficha de la pantalla superior
			NF_MoveSprite(0, (GEM_MOV_SPR + n), -32, -32);
			// Coloca la ficha en la pantalla inferior en su posicion
			y = ((Engine->gem_y + (n - 10)) << 4);
			NF_MoveSprite(1, (GEM_MOV_SPR + n), x, y);
		}
	}

}



// Opten el primer frame de la gema segun el color
u8 CDraw::GetGemColorFrame(u8 color) {

	// Variables
	u8 frame = GEM_GRAY;

	// Segun el color, devuelve el frame
	switch (color) {
		case EMPTY:		// Sin ficha
			frame = GEM_GRAY;
			break;
		case BLUE:		// Azul
			frame = GEM_BLUE;
			break;
		case RED:		// Roja
			frame = GEM_RED;
			break;
		case GREEN:		// Verde
			frame = GEM_GREEN;
			break;
		case YELLOW:	// Amarilla
			frame = GEM_YELLOW;
			break;
		case LILAC:		// Lila
			frame = GEM_LILAC;
			break;
		case WHITE:		// Blanca
			frame = GEM_WHITE;
			break;
		case GRAY:		// Gris
			frame = GEM_GRAY;
			break;
		default:		// Error, por defecto
			frame = GEM_GRAY;
			break;
	}

	// Devuelve el resultado
	return frame;

}



// Animacion de borrado
bool CDraw::EraseAnimation(void) {

	// Variables
	u8 scr, n;
	u8 x, y;
	bool done = false;


	// Control de animacion
	erase_delay ++;
	if (erase_delay > DELETE_ANIMATION_SPEED) {

		// Actualiza el frame
		erase_delay = 0;
		erase_frame ++;

		// Si es el ultimo frame de la animacion, borra las fichas del tablero y sal
		if (erase_frame > 10) {

			// Marca la animacion como terminada
			erase_frame = 10;
			done = true;

			// Elimina las gemas del tablero
			for (y = 0; y < 20; y ++) {
				for (x = 0; x < 8; x ++) {
					if (Board->erase[y][x] == 1) Board->board[y][x] = EMPTY;
					Board->erase[y][x] = 0;
				}
			}
			update.board = true;

		} else {

			// Animacion
			for (scr = 0; scr < 2; scr ++) {
				if (Board->delete_gem[scr].num > 0) {
					for (n = 0; n < Board->delete_gem[scr].num; n ++) {
						NF_SpriteFrame(scr, Board->delete_gem[scr].spr[n], (Board->delete_gem[scr].frame[n] + erase_frame));
					}
				}
			}

		}

	}

	// Devuelve
	return done;

}



// Animacion de destellos
void CDraw::SparkleAnimation(void) {

	// Variables
	u8 x;

	// Segun el thread
	switch (sparkle.thread) {

		// Temporizador
		case 0:
			sparkle.timer ++;
			if (sparkle.timer > 180) {	// 1 destello cada 3 segundos
				sparkle.timer = 0;
				// Parametros iniciales de la animacion
				for (x = 0; x < 8; x ++) sparkle.frame[x] = -x;
				sparkle.delay = 0;
				// Marcala como activa
				sparkle.active = true;
				// Pasa a ejecucion
				sparkle.thread = 1;
			}
			break;

		// Ejecuta la animacion
		case 1:
			// Control de velocidad de la animacion
			sparkle.delay ++;
			if (sparkle.delay > 4) {
				sparkle.delay = 0;
				// Animacion
				for (x = 0; x < 8; x ++) sparkle.frame[x] ++;
				// Marca para actualizar el panel
				update.board = true;
				// Fin de la animacion
				if (sparkle.frame[7] > 4) {
					sparkle.active = false;
					sparkle.thread = 0;
				}
			}
			break;
		// Error trap
		default:
			break;

	}

}



// Movimiento del fondo
void CDraw::Mode7Animation(void) {

	// Calcula la nueva posicion
	mode7_scroll += (64 + (Engine->total_gems << 1));
	// Limites de la animacion
	if (mode7_scroll > (512 << 8)) mode7_scroll -= (512 << 8);
	// Nuevo angulo
	mode7_angle += (2 + (Engine->total_gems >> 5));
	if (mode7_angle > 512) mode7_angle -= 512;

}



// Crea la tabla de shading
void CDraw::MakeShadeTable(void) {

	// Variables
	u8 line = 0;

	// Crea la tabla de shading
	for (line = 0; line < 192; line ++) {
		// Sombreado parte superior
		if (line <= 95) {
			shading_table[line] = 0x00;								// Sin sombreado
		} else if (line <= 159) {
			shading_table[line] = (0x0f - ((159 - line) >> 2));		// Sombreado degradado
		} else {
			shading_table[line] = 0x0f;								// Sombra total
		}
	}

	// Crea una copia que sera la que se use en el shading
	memcpy(shading, shading_table, sizeof(shading_table));
	DC_FlushRange(shading, sizeof(shading));

}


// Cambio dinamico de fondo
void CDraw::ChangeBg(void) {

	// Variables locales
	u8 n = 0;

	// Gestion de threads en el cambio de fondo
	switch (background.thread) {

		// Prepara el cambio de decorado
		case BGCHANGE_START:
			// Inicializalo todo
			background.bright = 0;		// Brillo
			// Indica un cambio de musica
			Sound->bgm.num ++;
			if (Sound->bgm.num >= BGM_NUMBER) Sound->bgm.num = 0;
			Sound->bgm.next_track = Sound->bgm.list[Sound->bgm.num];
			// Inicia el cambio de fondo
			background.next_thread = BGCHANGE_FADEOUT;
			break;

		// Fade OUT
		case BGCHANGE_FADEOUT:
			// Nuevo valor del fade
			background.bright ++;
			// Si has terminado, cambio de thread
			if (background.bright >= 100) {
				background.bright = 100;
				background.next_thread = BGCHANGE_CHANGE;
			}
			// Aplica a cada valor de la tabla de shade su valor segun el brillo
			for (n = 0; n < 192; n ++) {
				shading[n] = (shading_table[n] + (((0x10 - shading_table[n]) * background.bright) / 100));
			}
			break;

		// Cambio de fondo
		case BGCHANGE_CHANGE:
			// Siguiente fondo en la lista
			background.num ++;
			if (background.num >= BG_NUMBER) background.num = 0;
			// Archivo a mostrar
			char name[8];
			sprintf(name, "bg%02d", background.list[background.num]);
			// Cambia el fondo
			NF_CreateAffineBg(0, LAYER_BG, name, 1);		// Crea los fondos Affine
			NF_CreateAffineBg(1, LAYER_BG, name, 1);
			NF_AffineBgCenter(0, LAYER_BG, 128, 512);		// Cambiales el centro de referencia
			NF_AffineBgCenter(1, LAYER_BG, 128, 512);
			// Siguiente thread
			background.next_thread = BGCHANGE_FADEIN;
			break;

		// Fade IN
		case BGCHANGE_FADEIN:
			// Nuevo valor del fade
			background.bright --;
			// Si has terminado, cambio de thread
			if (background.bright <= 0) {
				background.bright = 0;
				background.next_thread = BGCHANGE_EXIT;
			}
			// Aplica a cada valor de la tabla de shade su valor segun el brillo
			for (n = 0; n < 192; n ++) shading[n] = (shading_table[n] + (((0x10 - shading_table[n]) * background.bright) / 100));
			break;

		// Sal del cambio de fondo
		case BGCHANGE_EXIT:
			// Restura la tabla original de shades
			memcpy(shading, shading_table, sizeof(shading_table));
			DC_FlushRange(shading, sizeof(shading));
			// Reinicia los parametros de la funcion para el siguiente uso
			background.thread = BGCHANGE_START;
			background.next_thread = BGCHANGE_START;
			// Sal de la funcion
			background.update = false;
			break;

		// Nada, error trap
		default:
			break;
	}

	// Cambio de thread si es necesario
	if (background.next_thread != background.thread) background.thread = background.next_thread;

}



// Crea una lista random de los fondos disponibles
void CDraw::MakeBgList(void) {

	// Variables internas
	u8 n = 0, i = 0;

	// Inicializa el vector de cola de fondos (BG_NUMBER elementos)
	Draw->background.cue.reserve(BG_NUMBER);

	// Añade todos los elementos a la cola
	for (n = 1; n <= BG_NUMBER; n ++) {
		Draw->background.cue.push_back(n);	// Añade un nuevo elemento a la cola
	}

	// Ahora, extrae aleatoriamente todos los elementos de la cola, para crear una lista random de fondos
	n = 0;
	while (Draw->background.cue.size() > 0) {	// Mientras quede algun elemento en cola
		// Genera que elemento aleatorio extraeras
		i = (int)(rand() % Draw->background.cue.size());
		// Copia el elemento de la cola al array de lista de fondos
		Draw->background.list[n] = Draw->background.cue[i];
		n ++;
		// Ahora elimina este elemento de la lista
		Draw->background.cue.erase(Draw->background.cue.begin() + i);
	}

	// Elimina el vector de la memoria
	Draw->background.cue.clear();
	
}