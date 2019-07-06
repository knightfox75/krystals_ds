#ifndef __RECORDS_H__
#define __RECORDS_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion de los records

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



// Clase CRecords
class CRecords {

	// Segmento privado
	private:

		// Variables
		bool fade_in;		// Control del fade de la pantalla
		bool fade_out;
		s32 bright;

		struct {			// Resumen de la puntuacion
			u32 base;
			u32 level;
			u32 combo;
			u32 clear;
			u32 total;
		} score;

		bool new_record;	// Nuevo record
		u8 record_id;		// ID del nuevo record, si es que existe

		s8 namepos;			// Caracter seleccionado al entrar el nombre del record
		bool blink;			// Control del parpadeo del caracter
		u8 delay;			// Control del tiempo de parpadeo

		// Efecto Fade-in
		bool FadeIn(void);

		// Efecto FadeOut
		bool FadeOut(void);

		// Inicializa el resumen de puntacion
		void Init(void);

		// Genera el report
		void Generate(void);

		// Registra un nuevo record
		bool EnterName(void);



	// Segmento publico
	public:

		// Variables
		s32 max_combo;		// Combo maximo alcanzado en la partida
		bool game_clear;	// Se ha terminado el juego?

		// Constructor
		CRecords(void);

		// Destructor
		~CRecords(void);


		// Informe de la puntuacion
		void Report(void);

};
// Metodos globlales de la clase "CRecords"
extern CRecords* Records;



/*
Metodos principales de los records
*/





#endif
