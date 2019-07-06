#ifndef __SAVEGAME_H__
#define __SAVEGAME_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion del Savegame

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



// Clase CSavegame
class CSavegame {

	// Segmento privado
	private:

		// Variables

		bool is_emulator;			// Deteccion de si es un emulador o una consola
		char savefilepath[256];		// Ubicacion del archivo de datos de guardado

		struct {
			u32 value;				// Valor Checksum
			u32 version;			// Version del savegame
			char magic[12];			// ID del juego (KRYSTALSDS)
		} checksum[1];


		// Funcion CheckSavegameFile();
		void CheckSavegameFile(void);
		// Verifica si el archivo de guardado.
		// Si existe, cargalo, si no, crea uno de nuevo.


		// Funcion WriteSavegame();
		void WriteSavegameFile(void);
		// Guarda los datos en un archivo


		// Funcion ReadSavegame();
		void ReadSavegameFile(void);
		// Carga los datos desde un archivo


		// Funcion CheckSavegameSRAM();
		void CheckSavegameSRAM(void);
		// EMULADOR - SRAM
		// Verifica si el archivo de guardado.
		// Si existe, cargalo, si no, crea uno de nuevo.


		// Funcion WriteSRAM();
		void WriteSRAM(void);
		// Guarda los datos en SRAM (Emulador)


		// Funcion WriteSRAM();
		void ReadSRAM(void);
		// Lee los datos en SRAM (Emulador)


		// Funcion CheckFATWrite();
		bool CheckFATWrite(const char* path);
		// Verifica si puedes escribir en FAT


		// Funcion SavedataChecksum();
		u32 SavedataChecksum(void);
		// Verifica la integridad de los datos


		// Funcion InitGameData();
		void InitGameData(void);
		// Inicializa todos los datos del juego



	// Segmento publico
	public:

		// Variables

		struct {
			char name[5];		// Siglas del jugador
			u32 score;			// Puntuacion alcanzada
			u32 level;			// Nivel
			u32 combo;			// Combo
			bool cleared;		// Juego terminado
		} data[3];


		// Constructor
		CSavegame(void);

		// Destructor
		~CSavegame(void);


		// Funcion CheckSavegame();
		void CheckSavegame(const char* path);
		// Verifica el archivo de guardado y crealo si no existe


		// Funcion WriteSavegame();
		void WriteSavegame(void);
		// Guarda los datos del juego


		// Funcion ReadSavegame();
		void ReadSavegame(void);
		// Carga los datos del juego

};
// Metodos globlales de la clase "CSavegame"
extern CSavegame* Savegame;



/*
Metodos principales del Savegame
*/





#endif