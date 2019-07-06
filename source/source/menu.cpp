/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion del Menu

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
#include <time.h>
// Includes propietarios NDS
#include <nds.h>
// Includes librerias propias
#include <nf_lib.h>
// Includes del juego
#include "includes.h"



/*
Metodos de la clase "CMenu"
*/

// Contructor clase CMenu
CMenu::CMenu(void) {

	// Inicializa las variables
	option = MMOPT_FIRST;

	// Control del fade
	bright = 0;
	fade_in = true;
	fade_out = false;

	// Inicializa la maquina de estos del menu
	thread = MMST_MENU;
	next_thread = MMST_MENU;
	state = 0;
	next_state = 0;

	// Control de fondos y posicion de items
	menu_px = 0;
	bg_x[0] = 0, bg_x[1] = 0;
	bg_y[0] = 0, bg_y[1] = 0;

	// Crea los objetos de clases necesarios
	Engine3d = new CEngine3d();		// Motor de dibujado 3D en OpenGL
	Credits = new CCredits();		// Creditos

}



// Destructor clase CMenu
CMenu::~CMenu(void) {

	// Destruye los objetos creados en esta clase
	delete Engine3d;
	Engine3d = NULL;
	delete Credits;
	Credits = NULL;

}



// Metodos globales de la clase CMenu
CMenu* Menu;



// Efecto fade-in
bool CMenu::FadeIn(void) {

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
bool CMenu::FadeOut(void) {

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



// Menu principal del juego
u8 CMenu::MainMenu(void) {

	// Variables
	bool loop = true;		// Control de ejecucion

	// Inicializa el menu
	Init();


	// Bucle de ejecucion del menu
	while (loop) {

		// Lee el teclado
		System->ReadKeys();

		// Efectos Fade IN/OUT
		if (fade_in) FadeIn();
		if (fade_out) loop = !FadeOut();


		/* Maquina de estados del menu */
		switch (thread) {

			/*  Menu principal */
			case MMST_MENU:
				// Si no hay eventos pendientes, ni vas a salir...
				if (!fade_in && !fade_out && loop) StateMainMenu();
				break;
			
			/* Records */
			case MMST_RECORDS:
				StateRecords();
				break;

			/* Credits */
			case MMST_CREDITS:
				StateCredits();
				break;

			/* Error */
			default:
				break;

		}


		// Render de la escena 3D
		Engine3d->Render();

		// Gestion del sonido
		Sound->BgmController();

		// Actualiza la pantalla (Incluye VSYNC)
		Render2d();

		// Cambio de thread si es necesario
		if (thread != next_thread) thread = next_thread;

	}

	// Segun la opcion del menu, deten todos los sonidos antes de salir
	if (option == MMOPT_RUNGAME) {
		Sound->Reset();
	}

	// Devuelve el resultado
	return option;

}



// Inicializaciones del menu
void CMenu::Init(void) {

	// Borra todo el contenido de la memoria
	System->Reset();

	// Pantalla en negro
	setBrightness(3, -16);
	swiWaitForVBlank();

	// Carga los archivos necesarios
	Load->MenuMainFiles();
	Credits->Load("text/credits.txt");

	// Inicializa el random
	srand(time(NULL));

	// Inicializa el entorno 3D
	Engine3d->Init3D();
	Engine3d->Setup3dWorld();
	Engine3d->Render();

	// Crea los elementos 2D
	CreateMainMenu();
	Credits->MakeBlendingTable();

	// Actualiza los botones del menu
	ChangeOption(0);

	// Manda todos los elementos a la pantalla y dejalos listos para el Fade In
	NF_SpriteOamSet(1);			// Actualiza el OAM de NFL
	swiWaitForVBlank();			// Espera al sincronismo
	oamUpdate(&oamSub);			// Actualiza el OAM de Libnds

	// Prepara el Fade IN
	fade_in = true;

	// E indica la musica que sonara
	Sound->bgm.next_track = MOD_TITLE;

	// Inicializa los niveles de volumen
	Sound->volume.main = 100;			// Volumen general
	Sound->volume.bgm = 100;			// Volumen de la musica
	Sound->volume.sfx = 100;			// Volumen de los efectos de sonido
	Sound->volume.voice = 100;			// Volumen de las voces

}



// Crea los elementos 2D del menu
void CMenu::CreateMainMenu(void) {

	// Variables
	u8 n = 0, id = 0;
	s32 x = 0, y = 0;

	// Crea los fondos
	NF_CreateTiledBg(0, 3, "menu_bg_top");
	NF_CreateTiledBg(0, 1, "menu_title");
	NF_CreateTiledBg(1, 3, "menu_bg_bottom");

	// Ajusta el alpha de el titulo
	REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG1 | (BLEND_DST_BG0 | BLEND_DST_BG3);
	REG_BLDALPHA = 0x0A | (0x0A << 8);

	// Manda los sprites a la VRAM
	NF_VramSpriteGfx(1, 0, 0, false);
	NF_VramSpriteGfx(1, 1, 1, false);
	NF_VramSpriteGfx(1, 2, 2, false);
	NF_VramSpritePal(1, 0, 0);

	// Crea los Sprites
	for (n = 0; n < 3; n ++) {
		// Calcula la posicion
		x = ((n * 48) + 16);
		y = ((n * 48) + 16);
		// Dibuja los Sprites en pantalla (Parte izquierda)
		NF_CreateSprite(1, id, n, 0, x, y);
		NF_SpriteFrame(1, id, 0);
		id ++;
		// Dibuja los Sprites en pantalla (Parte derecha)
		x += 64;
		NF_CreateSprite(1, id, n, 0, x, y);
		NF_SpriteFrame(1, id, 1);
		id ++;
	}

}



// Rutina cuando esta en modo Menu Principal
void CMenu::StateMainMenu(void) {

	// Si pulsas arriba...
	if (Keypress->up) ChangeOption(-1);

	// Si pulsas abajo
	if (Keypress->down) ChangeOption(1);

	// Si pulsas A, B o START
	if (Keypress->a || Keypress->b || Keypress->start) {
		// Si estas en PLAY
		if (option == MMOPT_RUNGAME) {
			fade_out = true;			// Ordena el fade out de pantalla		
			Sound->bgm.stop = true;		// Y detener la musica con fade out
		} else {
			// Si no, cambia el estado del menu a Records o Credits
			if (option == MMOPT_RECORDS) {
				next_thread = MMST_RECORDS;
			} else if (option == MMOPT_CREDITS) {
				next_thread = MMST_CREDITS;
			}
			// Reinicializa la submaquina de estados
			state = 0, next_state = 0;
		}
	}

}



// Rutina cuando esta seleccionada la opcion "RECORDS"
void CMenu::StateRecords(void) {

	// Variables
	s32 speed = 0;
	char text[32];

	// Maquina de estados
	switch (state) {

		// Crea los elementos necesarios
		case 0:
			// Crea los fondos
			NF_CreateTiledBg(1, 1, "records_shadows");
			// Crea la capa de texto...
			NF_CreateTextLayer(1, 0, 0, "records");
			// Escribe los textos a mostrar
			NF_WriteText16(1, 0, 37, 2, "       SCORE   LV  MAX");
			sprintf(text, "%s   %07d  %02d  %03d", Savegame->data[0].name, (int)Savegame->data[0].score, (int)Savegame->data[0].level, (int)Savegame->data[0].combo);
			NF_WriteText16(1, 0, 37, 4, text);
			sprintf(text, "%s   %07d  %02d  %03d", Savegame->data[1].name, (int)Savegame->data[1].score, (int)Savegame->data[1].level, (int)Savegame->data[1].combo);
			NF_WriteText16(1, 0, 37, 6, text);
			sprintf(text, "%s   %07d  %02d  %03d", Savegame->data[2].name, (int)Savegame->data[2].score, (int)Savegame->data[2].level, (int)Savegame->data[2].combo);
			NF_WriteText16(1, 0, 37, 8, text);
			NF_UpdateTextLayers();
			// Ajusta el alpha de la capa de sombreado
			REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BG3;
			REG_BLDALPHA_SUB = 0x08 | (0x08 << 8);
			// Posicion inicial de los fondos
			bg_x[0] = 0, bg_y[0] = 0;
			bg_x[1] = 0, bg_y[1] = 1;
			menu_px = 0;
			// E indica el siguiente thread
			next_state = 1;
			break;

		// Desplaza el menu (Entrada)
		case 1:
			speed = (((256 << 8) - menu_px) >> 3);
			if (speed < 256) speed = 256;
			menu_px += speed;
			if (menu_px > (256 << 8)) {
				menu_px = (256 << 8);
				next_state = 2;
			}
			break;

		// Espera a que se pulse alguna tecla
		case 2:
			// Cambia de estado
			if (Keypress->a || Keypress->b || Keypress->start) next_state = 3;
			break;

		// Desplaza el menu (Salida)
		case 3:
			speed = (menu_px >> 3);
			if (speed < 256) speed = 256;
			menu_px -= speed;
			if (menu_px <= 0) {
				menu_px = 0;
				// Elimina la capa de texto y sombras
				NF_DeleteTiledBg(1, 1);
				NF_DeleteTextLayer(1, 0);
				// Reinicializa la submaquina de estados
				state = 0, next_state = 0;
				// Reinicia el alpha
				REG_BLDCNT_SUB = BLEND_NONE;
				REG_BLDALPHA_SUB = 0x0F | (0x0F << 8);
				// Y vuelve al menu principal
				next_thread = MMST_MENU;
			}
			break;

		// Error trap
		default:
			break;

	}

	// Cambio de estado, si es necesario
	if (state != next_state) state = next_state;

}



// Rutina cuando esta seleccionada la opcion "CREDITS"
void CMenu::StateCredits(void) {

	// Variables
	s32 speed = 0;

	// Maquina de estados
	switch (state) {

		// Crea los elementos necesarios
		case 0:
			// Escribe los textos a mostrar
			Credits->Parse();
			// Posicion inicial de los fondos
			bg_x[0] = 256, bg_y[0] = 32;
			menu_px = 0;
			// Define el efecto blending
			REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG0 | BLEND_DST_BG3;
			// Habilita la ejecucion del efecto "blending" en el fondo
			irqSet(IRQ_HBLANK, Mode7Credits);
			irqEnable(IRQ_HBLANK);
			// E indica el siguiente thread
			next_state = 1;
			break;

		// Desplaza el menu (Entrada)
		case 1:
			menu_px = -menu_px;
			speed = (((256 << 8) - menu_px) >> 3);
			if (speed < 256) speed = 256;
			menu_px += speed;
			if (menu_px > (256 << 8)) {
				menu_px = (256 << 8);
				next_state = 2;
			}
			menu_px = -menu_px;
			break;

		// Espera a que se pulse alguna tecla
		case 2:
			// Scroll de los creditos
			bg_y[0] += 1;
			// Cambia de estado
			if (Keypress->a || Keypress->b || Keypress->start || (bg_y[0] >= Credits->credits_endpoint)) next_state = 3;
			break;

		// Desplaza el menu (Salida)
		case 3:
			menu_px = -menu_px;
			speed = (menu_px >> 3);
			if (speed < 256) speed = 256;
			menu_px -= speed;
			if (menu_px <= 0) {
				menu_px = 0;
				// Deshabilita la interrupcion
				irqClear(IRQ_HBLANK);
				irqDisable(IRQ_HBLANK);
				// Elimina la capa de texto y sombras
				NF_DeleteTextLayer(1, 0);
				// Reinicia el alpha
				REG_BLDCNT_SUB = BLEND_NONE;
				REG_BLDALPHA_SUB = 0x0F | (0x0F << 8);
				// Reinicializa la submaquina de estados
				state = 0, next_state = 0;
				// Y vuelve al menu principal
				next_thread = MMST_MENU;
			}
			menu_px = -menu_px;
			break;

		// Error trap
		default:
			break;

	}

	// Cambio de estado, si es necesario
	if (state != next_state) state = next_state;

}



// Renderiza la escena 2D
void CMenu::Render2d(void) {

	// Variables
	u8 n = 0;						// Contador generico
	u8 id = 0;						// ID de sprite
	s32 x = 0, y = 0, temp_x;		// Coordenadas de posicion

	// Reposiciona los sprites
	for (n = 0; n < 3; n ++) {
		// Calcula la posicion
		x = ((n * 48) + 16);
		y = ((n * 48) + 16);
		// Mueve los Sprites en pantalla (Parte izquierda)
		temp_x = (x - (menu_px >> 8));
		if (temp_x < -64) temp_x = -64;
		if (temp_x > 256) temp_x = 256;
		NF_MoveSprite(1, id, temp_x, y);
		id ++;
		// Mueve los Sprites en pantalla (Parte derecha)
		x += 64;
		temp_x = (x - (menu_px >> 8));
		if (temp_x < -64) temp_x = -64;
		if (temp_x > 256) temp_x = 256;
		NF_MoveSprite(1, id, temp_x, y);
		id ++;
	}

	// Nucleo de actualizacion de la pantalla
	NF_SpriteOamSet(1);			// Actualiza el OAM de NFL
	swiWaitForVBlank();			// Espera al sincronismo
	oamUpdate(&oamSub);			// Actualiza el OAM de Libnds

	// Reposiciona las capas de texto/sombreados
	NF_ScrollBg(1, 1, (bg_x[1] + (menu_px >> 8)), bg_y[1]);
	NF_ScrollBg(1, 0, (bg_x[0] + (menu_px >> 8)), bg_y[0]);

}



// Cambia de opcion del menu
void CMenu::ChangeOption(s8 change) {

	// Variables
	u8 n = 0;

	// Si se has de subir
	if (change > 0) {
		option ++;
		if (option > MMOPT_LAST) {
			option = MMOPT_LAST;
		}
	}

	// Si se has de subir
	if (change < 0) {
		option --;
		if (option < MMOPT_FIRST) {
			option = MMOPT_FIRST;
		}
	}

	// Actualiza los indicadores del menu
	for (n = 0; n < 3; n ++) {
		// Si es la opcion seleccionada actualmente
		if (n == option) {
			NF_SpriteFrame(1, (n << 1), 2);
			NF_SpriteFrame(1, ((n << 1) + 1), 3);
		} else {
			// Si no lo es
			NF_SpriteFrame(1, (n << 1), 0);
			NF_SpriteFrame(1, ((n << 1) + 1), 1);
		}
	}


}