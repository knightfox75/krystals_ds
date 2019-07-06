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
Metodos de la clase "CSavegame"
*/

// Contructor clase CSavegame
CSavegame::CSavegame(void) {

	// Inicializa las variables (Privadas)
	is_emulator = false;
	memset(savefilepath, 0, sizeof(savefilepath));
	memset((void*)&checksum, 0, sizeof(checksum));

	// Inicializa las variables (Publicas)
	memset((void*)&data, 0, sizeof(data));

	// Escribe el cache en la ram
	DC_FlushRange(savefilepath, sizeof(savefilepath));
	DC_FlushRange((void*)&checksum, sizeof(checksum));
	DC_FlushRange((void*)&data, sizeof(data));

}


// Destructor clase CSavegame
CSavegame::~CSavegame(void) {
}



// Metodos globales de la clase "CSavegame"
CSavegame* Savegame;



// Funcion CheckSavegame();
void CSavegame::CheckSavegame(const char* path) {

	// Verifica si es un emulador o el hardware real
	if (CheckFATWrite(path)) {
		// Operaciones en el hardware real
		sprintf(savefilepath, path);
		is_emulator = false;
		CheckSavegameFile();
	} else {
		// Operaciones en el emulador
		is_emulator = true;
		CheckSavegameSRAM();
	}

}


// Funcion WriteSavegame();
void CSavegame::WriteSavegame(void) {

	// Guardalos donde tocan
	if (is_emulator) {
		if (USE_SRAM && !REG_DSIMODE) WriteSRAM();
	} else {
		WriteSavegameFile();
	}

}



// Funcion ReadSavegame();
void CSavegame::ReadSavegame(void) {

	// Guardalos donde tocan
	if (is_emulator) {
		if (USE_SRAM && !REG_DSIMODE) ReadSRAM();
	} else {
		ReadSavegameFile();
	}

}




// Funcion CheckSavegameFile();
void CSavegame::CheckSavegameFile(void) {

	// Variables
	bool filefound = false;		// Existe el fichero
	u16 l = 0;					// Longitud del path al archivo
	FILE* savefile;				// Puntero de archivo
	u32 chk = 0;			// Checksum del savedata

	// Añade la extension .SAV al archivo de guardado
	l = strlen(savefilepath);
	savefilepath[(l - 3)] = 's';
	savefilepath[(l - 2)] = 'a';
	savefilepath[(l - 1)] = 'v';

	// Verifica si existe el archivo de guardado
	savefile = fopen(savefilepath, "rb");
	if (savefile) {
		filefound = true;
	} else {
		filefound = false;
	}
	fclose(savefile);

	// Si el archivo de guardado existe...
	if (filefound) {
		// Lee el archivo
		ReadSavegameFile();
		// Realiza el chk de los datos cargados
		chk = SavedataChecksum();
		// Verifica si hay discrepancias
		if (
			(chk != checksum[0].value)
			||
			(checksum[0].version != SAVEGAMEVERSION)
			||
			(strcmp(checksum[0].magic, MAGIC_STRING) != 0)
			) {
			// Si las hay, inicializa los datos del juego
			InitGameData();
			// Graba el archivo
			WriteSavegameFile();
		}
	} else {	// Si no existe, crea los datos de guardado y crealo
		// Inicializa los datos del juego
		InitGameData();
		// Graba el archivo
		WriteSavegameFile();
	}

}



// Funcion WriteSavegameFile();
void CSavegame::WriteSavegameFile(void) {

	// Variables
	FILE* savefile;				// Puntero de archivo

	// Alamacena el checksum
	checksum[0].value = SavedataChecksum();

	// Crea el Archivo
	savefile = fopen(savefilepath, "wb");

	// Graba los datos solo en caso de poder escribir el archivo
	if (savefile) {
		fwrite((void*)&checksum, sizeof(checksum), 1, savefile);
		fwrite((void*)&data, sizeof(data), 1, savefile);
	}

	// Cierra el archivo
	fclose(savefile);

}



// Funcion ReadSavegameFile();
void CSavegame::ReadSavegameFile(void) {

	// Variables
	FILE* savefile;				// Puntero de archivo

	// Abre el Archivo
	savefile = fopen(savefilepath, "rb");

	// Si el archivo se a abierto con exito
	if (savefile) {
		// Borra el contenido previo
		memset((void*)&checksum, 0, sizeof(checksum));
		memset((void*)&data, 0, sizeof(data));
		DC_FlushRange((void*)&checksum, sizeof(checksum));
		DC_FlushRange((void*)&data, sizeof(data));
		// Lee los datos
		fread((void*)&checksum, sizeof(checksum), 1, savefile);
		fread((void*)&data, sizeof(data), 1, savefile);
	}

	// Cierra el archivo
	fclose(savefile);

}



// Funcion CheckSavegameSRAM();
void CSavegame::CheckSavegameSRAM(void) {

	// Variables
	u32 chk = 0;		// Checksum de los datos del juego

	// Carga los datos de la SRAM
	if (USE_SRAM && !REG_DSIMODE) ReadSRAM();

	// Calcula el Checksum
	chk = SavedataChecksum();

	// Si hay discrepancias
	if (
		(chk != checksum[0].value)
		||
		(checksum[0].version != SAVEGAMEVERSION)
		||
		(strcmp(checksum[0].magic, MAGIC_STRING) != 0)
		) {
		// Inicializa los datos del juego
		InitGameData();
		// Guarda los datos
		if (USE_SRAM && !REG_DSIMODE) WriteSRAM();
	}

}



// Funcion WriteSRAM();
void CSavegame::WriteSRAM(void) {

	// Variables
	u16 adress = 0;			// Posicion en la SRAM
	u16 i = 0;				// Indice
	bool slot2 = false;		// Propietario actual del SLOT2

	// Alamacena el checksum
	checksum[0].value = SavedataChecksum();

	// Si no lo es, haz al ARM9 propietario del SLOT2 y guarda el propietario actual
	slot2 = (REG_EXMEMCNT & ARM7_OWNS_ROM) == 0;
	sysSetCartOwner(true);

	// Copia los datos del bloque checksum
	for (i = 0; i < sizeof(checksum); i ++) {
		((u8*)SRAM)[adress] = ((u8*)checksum)[i];
		adress ++;
	}

	// Copia los datos del bloque data
	for (i = 0; i < sizeof(data); i ++) {
		((u8*)SRAM)[adress] = ((u8*)data)[i];
		adress ++;
	}

	// Devuelve el propietario del SLOT2 al propietario anterior
	sysSetCartOwner(slot2);

}



// Funcion WriteSRAM();
void CSavegame::ReadSRAM(void) {

	// Variables
	u16 adress = 0;			// Posicion en la SRAM
	u16 i = 0;				// Indice
	bool slot2 = false;		// Propietario actual del SLOT2

	// Borra el contenido previo
	memset((void*)&checksum, 0, sizeof(checksum));
	memset((void*)&data, 0, sizeof(data));
	DC_FlushRange((void*)&checksum, sizeof(checksum));
	DC_FlushRange((void*)&data, sizeof(data));

	// Si no lo es, haz al ARM9 propietario del SLOT2 y guarda el propietario actual
	slot2 = (REG_EXMEMCNT & ARM7_OWNS_ROM) == 0;
	sysSetCartOwner(true);

	// Copia los datos del bloque checksum
	for (i = 0; i < sizeof(checksum); i ++) {
		((u8*)checksum)[i] = ((u8*)SRAM)[adress];
		adress ++;
	}

	// Copia los datos del bloque data
	for (i = 0; i < sizeof(data); i ++) {
		((u8*)data)[i] = ((u8*)SRAM)[adress];
		adress ++;
	}

	// Devuelve el propietario del SLOT2 al propietario anterior
	sysSetCartOwner(slot2);

}



// Verifica si puedes escribir en FAT
bool CSavegame::CheckFATWrite(const char* path) {

	// Variables
	bool filefound = false;		// Existe el fichero
	u16 l = 0;					// Longitud del path al archivo
	FILE* savefile;				// Puntero de archivo
	char testfile[256];			// Nombre de archivo

	// Almacena el path
	sprintf(testfile, path);

	// Añade la extension .CHK al archivo de verificacion
	l = strlen(testfile);
	testfile[(l - 3)] = 'c';
	testfile[(l - 2)] = 'h';
	testfile[(l - 1)] = 'k';

	// Verifica si puedes abrir en el archivo de verificacion
	savefile = fopen(testfile, "rb");
	if (savefile) {
		filefound = true;
	} else {
		filefound = false;
	}
	fclose(savefile);

	// Si no puedes, crea uno e intentalo abrir de nuevo
	if (!filefound) {
		savefile = fopen(testfile, "wb");
		fclose(savefile);
		savefile = fopen(testfile, "rb");
		if (savefile) filefound = true;
		fclose(savefile);
	}

	return filefound;

}



// Funcion SavedataChecksum();
u32 CSavegame::SavedataChecksum(void) {

	// Variables
	u16 i = 0;
	u32 chk = 0;

	// Checksum de los datos del bloque data
	for (i = 0; i < sizeof(data); i ++) {
		chk += ((u8*)data)[i];
	}

	// Variables
	return chk;

}



// Funcion InitGameData();
void CSavegame::InitGameData(void) {
	
	// Magic String del chk
	sprintf(checksum[0].magic, MAGIC_STRING);
	checksum[0].magic[10] = '\0';
	// Identificador de la version
	checksum[0].version = SAVEGAMEVERSION;

	// Inicializa los records
	memset((void*)&data, 0, sizeof(data));
	DC_FlushRange((void*)&data, sizeof(data));
	data[0].name[0] = 'F', data[0].name[1] = 'O', data[0].name[2] = 'X', data[0].name[3] = '\0';
	data[0].score = 300;
	data[0].level = 3;
	data[0].combo = 15;
	data[0].cleared = false;
	data[1].name[0] = 'A', data[1].name[1] = 'N', data[1].name[2] = 'D', data[1].name[3] = '\0';
	data[1].score = 200;
	data[1].level = 2;
	data[1].combo = 10;
	data[1].cleared = false;
	data[2].name[0] = 'J', data[2].name[1] = 'D', data[2].name[2] = 'C', data[2].name[3] = '\0';
	data[2].score = 100;
	data[2].level = 1;
	data[2].combo = 5;
	data[2].cleared = false;

}

