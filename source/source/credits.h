#ifndef __CREDITS_H__
#define __CREDITS_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion de los creditos

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



// Clase CCredits
class CCredits {

	// Segmento privado
	private:

		// Variables
		char* buffer;			// Buffer para almacenar el texto
		u32 size;				// Tamaño del archivo cargado

	// Segmento publico
	public:

		// Variables
		s32 credits_endpoint;		// Guarda el punto donde termina la capa de los creditos
		s32 blending[192];			// Tabla de transparencia para los creditos

		// Constructor
		CCredits(void);

		// Destructor
		~CCredits(void);

		// Carga el archivo TXT con los creditos
		void Load(const char* file);

		// Crea las capas de texto con los creditos
		void Parse(void);

		// Crea la tabla de blending
		void MakeBlendingTable(void);

};
// Metodos globlales de la clase CMenu
extern CCredits* Credits;



/*
Metodos principales de los creditos
*/





#endif
