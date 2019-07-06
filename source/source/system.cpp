/*
-------------------------------------------------

	Krystals DS

	Archivo de funciones de sistema

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
Metodos de la clase "CSystem"
*/

// Contructor clase CSystem
CSystem::CSystem(void) {

	// Crea todos los objetos de gestion de entrada (Input)
	Keypress = new CKeyInput();		// Crea los objetos para la entrada de teclado
	Keyheld = new CKeyInput();
	Keyup = new CKeyInput();		// Crea los objetos para la entrada del touchpad
	Touchpad = new CTouchPad();

}



// Destructor clase CSystem
CSystem::~CSystem(void) {

	// Elimina todos los objetos de gestion de entrada (Input)
	delete Touchpad;
	Touchpad = NULL;
	delete Keyup;
	Keyup = NULL;
	delete Keyheld;
	Keyheld = NULL;
	delete Keypress;
	Keypress = NULL;

}


// Metodos globales de la clase CSystem
CSystem* System;


// Metodo Readkeys();
void CSystem::ReadKeys(void) {

	// Variables
	u16 keys = 0;

	// Lee el teclado via Libnds
	scanKeys();

	// Teclas "HELD"
	keys = keysHeld();
	if (keys & KEY_UP) {Keyheld->up = true;} else {Keyheld->up = false;}
	if (keys & KEY_DOWN) {Keyheld->down = true;} else {Keyheld->down = false;}
	if (keys & KEY_LEFT) {Keyheld->left = true;} else {Keyheld->left = false;}
	if (keys & KEY_RIGHT) {Keyheld->right = true;} else {Keyheld->right = false;}
	if (keys & KEY_A) {Keyheld->a = true;} else {Keyheld->a = false;}
	if (keys & KEY_B) {Keyheld->b = true;} else {Keyheld->b = false;}
	if (keys & KEY_X) {Keyheld->x = true;} else {Keyheld->x = false;}
	if (keys & KEY_Y) {Keyheld->y = true;} else {Keyheld->y = false;}
	if (keys & KEY_L) {Keyheld->l = true;} else {Keyheld->l = false;}
	if (keys & KEY_R) {Keyheld->r = true;} else {Keyheld->r = false;}
	if (keys & KEY_START) {Keyheld->start = true;} else {Keyheld->start = false;}
	if (keys & KEY_SELECT) {Keyheld->select = true;} else {Keyheld->select = false;}
	if (keys & KEY_LID) {Keyheld->lid = true;} else {Keyheld->lid = false;}

	// Teclas "PRESS"
	keys = keysDown();
	if (keys & KEY_UP) {Keypress->up = true;} else {Keypress->up = false;}
	if (keys & KEY_DOWN) {Keypress->down = true;} else {Keypress->down = false;}
	if (keys & KEY_LEFT) {Keypress->left = true;} else {Keypress->left = false;}
	if (keys & KEY_RIGHT) {Keypress->right = true;} else {Keypress->right = false;}
	if (keys & KEY_A) {Keypress->a = true;} else {Keypress->a = false;}
	if (keys & KEY_B) {Keypress->b = true;} else {Keypress->b = false;}
	if (keys & KEY_X) {Keypress->x = true;} else {Keypress->x = false;}
	if (keys & KEY_Y) {Keypress->y = true;} else {Keypress->y = false;}
	if (keys & KEY_L) {Keypress->l = true;} else {Keypress->l = false;}
	if (keys & KEY_R) {Keypress->r = true;} else {Keypress->r = false;}
	if (keys & KEY_START) {Keypress->start = true;} else {Keypress->start = false;}
	if (keys & KEY_SELECT) {Keypress->select = true;} else {Keypress->select = false;}
	if (keys & KEY_LID) {Keypress->lid = true;} else {Keypress->lid = false;}

	// Teclas "UP"
	keys = keysUp();
	if (keys & KEY_UP) {Keyup->up = true;} else {Keyup->up = false;}
	if (keys & KEY_DOWN) {Keyup->down = true;} else {Keyup->down = false;}
	if (keys & KEY_LEFT) {Keyup->left = true;} else {Keyup->left = false;}
	if (keys & KEY_RIGHT) {Keyup->right = true;} else {Keyup->right = false;}
	if (keys & KEY_A) {Keyup->a = true;} else {Keyup->a = false;}
	if (keys & KEY_B) {Keyup->b = true;} else {Keyup->b = false;}
	if (keys & KEY_X) {Keyup->x = true;} else {Keyup->x = false;}
	if (keys & KEY_Y) {Keyup->y = true;} else {Keyup->y = false;}
	if (keys & KEY_L) {Keyup->l = true;} else {Keyup->l = false;}
	if (keys & KEY_R) {Keyup->r = true;} else {Keyup->r = false;}
	if (keys & KEY_START) {Keyup->start = true;} else {Keyup->start = false;}
	if (keys & KEY_SELECT) {Keyup->select = true;} else {Keyup->select = false;}
	if (keys & KEY_LID) {Keyup->lid = true;} else {Keyup->lid = false;}

}



// Metodo ReadTouchpad();
void CSystem::ReadTouchpad(void) {

	// Variables
	u16 keys = 0;
	touchPosition touchscreen;

	// Lee el teclado via Libnds
	scanKeys();

	// Verifica el estado del touchscreen
	keys = keysHeld();
	if (keys & KEY_TOUCH) {Touchpad->touch = true;} else {Touchpad->touch = false;}

	// Lee el TOUCHPAD via Libnds
	touchRead(&touchscreen); 

	// Guarda el estado del touchscreen
	Touchpad->x = touchscreen.px;
	Touchpad->y = touchscreen.py;
	Touchpad->raw_x = touchscreen.rawx;
	Touchpad->raw_y = touchscreen.rawy;
	Touchpad->press_a = touchscreen.z1;
	Touchpad->press_b = touchscreen.z2;

}



// Metodo Boot();
void CSystem::Boot(void) {

	// Pantalla de espera inicializando NitroFS
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);	
	consoleDemoInit();
	if (NF_GetLanguage() == 5) {
		iprintf("\n Iniciando NitroFS,\n por favor, espere.\n");
	} else {
		iprintf("\n NitroFS init. Please wait.\n");
	}
	swiWaitForVBlank();

	// Define el ROOT e inicializa el sistema de archivos
	NF_SetRootFolder("NITROFS");	// Define la carpeta ROOT para usar NitroFS

	// Inicializa el motor 2D
	NF_Set2D(0, 0);				// Modo 2D_0 en ambas pantallas
	NF_Set2D(1, 0);	

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		// Inicializa los fondos Tileados para la pantalla superior
	NF_InitTiledBgSys(1);		// Iniciliaza los fondos Tileados para la pantalla inferior

	// Inicializa los Sprites
	NF_InitSpriteBuffers();			// Inicializa los buffers para almacenar sprites y paletas
	NF_InitSpriteSys(0, 128);		// Inicializa los sprites para la pantalla superior
	NF_InitSpriteSys(1, 128);		// Inicializa los sprites para la pantalla inferior

	// Inicializa el engine de texto
	NF_InitTextSys(0);
	NF_InitTextSys(1);

	// Define el color del backdrop a negro en ambas pantallas
	setBackdropColor(0);
	setBackdropColorSub(0);

	// Inicializa el sonido (MAXMOD & NFlib)
	Sound->Init();

}



// Metodo Reset();
void CSystem::Reset(void) {

	// Variables locales
	u8 screen = 0;
	u8 layer = 0;

	// La pantalla principal es la superior
	lcdMainOnTop();

	// Resetea el modo grafico a 0 en ambas pantallas
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);

	// Borra los buffers de Fondos
	NF_ResetTiledBgBuffers();

	// Borra los buffers de Sprites
	NF_ResetSpriteBuffers();

	// Reinicializa los fondos
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);

	// Reinicializa los Sprites
	NF_InitSpriteSys(0, 128);
	NF_InitSpriteSys(1, 128);

	// Reinicializa las capas de texto
	NF_InitTextSys(0);
	NF_InitTextSys(1);

	// Elimina los posibles efectos de Alpha Blending
	REG_BLDCNT = BLEND_NONE;
	REG_BLDCNT_SUB = BLEND_NONE;
	REG_BLDALPHA = 0x0F | (0x0F << 8);
	REG_BLDALPHA_SUB = 0x0F | (0x0F << 8);

	// Restaura el color del backdrop a negro en ambas pantallas
	setBackdropColor(0);
	setBackdropColorSub(0);

	// Resetea la posicion de todas las capas de fondo
	for (screen = 0; screen < 2; screen ++) {
		for (layer = 0; layer < 4; layer ++) {
			NF_ScrollBg(screen, layer, 0, 0);
		}
	}

}



/*
Metodos de la clase "CKeyInput"
*/

// Contructor clase CKeyInput
CKeyInput::CKeyInput(void) {

	// Todos los flags abajo
	up = false;
	down = false;
	left = false;
	right = false;
	a = false;
	b = false;
	x = false;
	y = false;
	r = false;
	l = false;
	select = false;
	start = false;
	lid = false;

}

// Destructor clase CKeyInput
CKeyInput::~CKeyInput(void) {
}

CKeyInput* Keypress;		// Teclas presionadas
CKeyInput* Keyheld;			// Teclas presionadas y mantenidas
CKeyInput* Keyup;			// Teclas levantadas



/*
Metodos de la clase "CTouchPad"
*/

// Contructor clase CTouchPad
CTouchPad::CTouchPad(void) {

	touch = false;		// Se esta tocando el toucpad?
	x = 0;				// Coordenadas del puntero (pixeles)
	y = 0;
	raw_x = 0;			// Coordenadas del puntero (RAW)
	raw_y = 0;
	press_a = 0;		// Nivel de presion en el tactil
	press_b = 0;

}

// Destructor clase CTouchPad
CTouchPad::~CTouchPad(void) {
}

CTouchPad* Touchpad;		// Valores del touchpad