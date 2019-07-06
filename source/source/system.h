#ifndef __SYSTEM_H__
#define __SYSTEM_H__



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


// Includes
#include <nds.h>



// Clase CSystem
class CSystem {

	// Segmento publico
	public:

		// Constructor
		CSystem(void);

		// Destructor
		~CSystem(void);

		// Lee todos los botones de la DS
		void ReadKeys(void);

		// Lee la pantalla tactil de la DS
		void ReadTouchpad(void);

		// Inicializa el hard de la DS y NFLIB
		void Boot(void);

		// Reinicia los buffers de datos y hardware al modo 2d por defecto
		void Reset(void);

};
// Metodos globales de la clase CSystem
extern CSystem* System;

/*
Metodos del sistema
*/



// Clase CKeyInput
class CKeyInput {

	// Metodos publicos
	public:

		// Botones NDS
		bool up;
		bool down;
		bool left;
		bool right;
		bool a;
		bool b;
		bool x;
		bool y;
		bool r;
		bool l;
		bool select;
		bool start;
		bool lid;		// Tapa de la DS

		// Constructor
		CKeyInput(void);

		// Destructor
		~CKeyInput(void);

};
// Metodos globales de la clase CKeyInput
extern CKeyInput* Keypress;		// Teclas presionadas
extern CKeyInput* Keyheld;		// Teclas presionadas y mantenidas
extern CKeyInput* Keyup;		// Teclas levantadas



// Clase CTouchPad
class CTouchPad {

	// Metodos publicos
	public:

		// Valores del Touchpad
		bool touch;		// Se esta tocando el toucpad?
		s32 x;			// Coordenadas del puntero (pixeles)
		s32 y;
		s32 raw_x;		// Coordenadas del puntero (RAW)
		s32 raw_y;
		s32 press_a;	// Nivel de presion en el tactil
		s32 press_b;

		// Constructor
		CTouchPad(void);

		// Destructor
		~CTouchPad(void);

};
// Metodos globales de la clase CTouchPad
extern CTouchPad *Touchpad;		// Valores del touchpad

/*
Metodos de entrada de botones y tactil
*/



#endif