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
Metodos de la clase "CCredits"
*/

// Contructor clase CCredits
CCredits::CCredits(void) {

	// Inicializa las variables
	buffer = NULL;								// Buffer donde esta almacenado el texto
	size = 0;									// Tamaño del buffer de datos
	credits_endpoint = 0;						// Punto donde se acaban los creditos
	memset(blending, 0, sizeof(blending));		// Tabla de transparencias
	DC_FlushRange(blending, sizeof(blending));	// Manda el cache a la ram

}



// Destructor clase CCredits
CCredits::~CCredits(void) {

	// Borra el buffer de texto
	free(buffer);
	buffer = NULL;

}



// Metodos globales de la clase CCredits
CCredits* Credits;




// Carga el archivo TXT con los creditos
void CCredits::Load(const char *file) {

	// Variables
	FILE* file_id;			// Declara los punteros a los ficheros
	char filename[256];		// Variable para almacenar el path al archivo
	u32 lines = 0;			// Numero de lineas en el texto
	u32 text_height = 0;	// Altura de la capa de texto (en pixeles, bloques de 256)
	u32 n = 0;				// Uso general

	sprintf(filename, "%s/%s", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		size = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		buffer = (char*) calloc (size, sizeof(char));
		if (buffer == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(buffer, 1, size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Cuenta lineas
	for (n = 0; n < size; n ++) {
		if (buffer[n] == 0x0a) lines ++;
	}

	// Calcula el tamaño de la capa de texto
	n = (192 + (lines << 4) + 192);
	text_height = (((n >> 8) + 1) << 8);

	// Carga las fuentes y crea las capas de texto correspondientes
	NF_LoadTextFont16("font/menufont", "credits", 512, text_height, 0);

	// Guarda la altura de los creditos (en pixeles)
	credits_endpoint = ((lines + 12) << 4);

}



// Crea las capas de texto con los creditos
void CCredits::Parse(void) {

	// Variables
	char text[64];			// Buffer de la linea de texto
	u16 len = 0;			// Longitud de la linea de texto
	s16 x = 0;				// Coordenadas para escribir el texto
	s16 y = 0;
	bool newline = false;	// Nueva linea
	u32 chr = 0;			// Caracter dentro de la linea
	u32 n = 0;				// Uso general

	// Crea las capas de texto
	NF_CreateTextLayer16(1, 0, 0, "credits");
	NF_DefineTextColor(1, 0, 0, 31, 31, 31);

	// Prepara la escritura de texto
	memset(text, 0, sizeof(text));
	DC_FlushRange(text, sizeof(text));
	y = 12;

	// Escribe el texto en las capas
	for (n = 0; n < size; n++) {

		// Si es una nueva linea
		if (newline) {
			// Escribe la linea actual
			text[chr] = '\0';
			len = strlen(text);
			x = (16 - (len >> 1));
			// Escribela
			NF_WriteText16(1, 0, x, y, text);
			// Preparate para una nueva linea
			memset(text, 0, sizeof(text));
			DC_FlushRange(text, sizeof(text));
			chr = 0;
			y ++;
			// Linea completada
			newline = false;
		}

		// Copia los caracteres del buffer a la linea
		if ((buffer[n] != 0x0d) && (buffer[n] != 0x0a)) {
			text[chr] = buffer[n];
			chr ++;
		} else {
			// Si es final de linea, marcalo
			if (buffer[n] == 0x0a) newline = true;
		}

	}

	// Actualiza las capas de texto
	NF_UpdateTextLayers();

}



// Crea la tabla de blending
void CCredits::MakeBlendingTable(void) {

	// Variables
	u8 n = 0;

	// Crea la tabla de transparencias
	for (n = 0; n < 192; n ++) {
		if ((n >= 0) && (n <= 31)) {			// Parte superior
			blending[n] = (n >> 1);
		} else if ((n >= 128) && (n <= 159)) {	// Parte inferior
			blending[n] = ((159 - n) >> 1);
		} else if (n > 159) {					// Parte invisible inferior
			blending[n] = 0x00;
		} else {								// Parte central
			blending[n] = 0x10;
		}
	}

}