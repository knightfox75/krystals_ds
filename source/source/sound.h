#ifndef __SOUND_H__
#define __SOUND_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion del sonido

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
#include <vector>



// Clase CSound
class CSound {

	// Segmento privado
	private:

	// Segmento publico
	public:

		// Variables

		// Musica
		struct {
			s16 volume;					// Volumen de la musica para los FADE
			s16 id;						// ID del track que esta cargado en RAM actualmente
			s16 current_track;			// Guarda el track que se esta reproduciendo actualmente
			s16 next_track;				// Indica que track sera el siguiente en reproducirse
			std::vector<u8> cue;		// Vector para generar la cola aleatoria de reproduccion
			u8 list[BGM_NUMBER];		// Lista de canciones a reproducir
			u8 num;						// nº track seleccionado en la cola de reproduccion
			bool play;					// Flag: La musica se esta reproduciendo?
			bool pause;					// Flag: La musica esta en pausa
			bool stop;					// Flag: comando STOP
		} bgm;

		// Efectos de sonido
		struct {
			u16 length;			// Duracion del sonido
			u16 position;		// Tiempo actual de reproduccion
			u16 id;				// ID del sonido (referencia al cargado en RAM)
			u16 handler;		// ID de instanca devuelto al reproducir el sonido
			s16 volume;			// Volumen de este sonido
			s16 pan;			// Balance de este sonido
			s16 freq;			// Frecuencia al que esta codificado
			bool active;		// Esta activo?
			char file[16];		// Nombre del archivo
		} sfx[SFX_NUMBER];

		// Gestion de las voces
		struct {
			u16 length[VOICE_NUMBER];		// Duracion del sonido 
			u16 id[VOICE_NUMBER];			// ID del sonido (referencia al cargado en RAM)
			s16 volume[VOICE_NUMBER];		// Volumen de este sonido
			s16 pan[VOICE_NUMBER];			// Balance de este sonido
			s16 freq[VOICE_NUMBER];			// Frecuencia al que esta codificado
			char file[VOICE_NUMBER][16];	// Nombre del archivo
			u16 handler;					// ID de instanca devuelto al reproducir el sonido
			u16 position;					// Tiempo actual de reproduccion
			u16 size;						// Tiempo total de la voz cargada
			s8 last;						// Ultima voz reproducida
			bool active;					// Esta activo?
		} voice;


		// Control del volumen
		struct {
			s16 main;			// Volumen maestro
			s16 bgm;			// Volumen musica
			s16 sfx;			// Volumen de los efectos de sonido
			s16 voice;			// Volumen de las voces
		} volume;

		// Constructor
		CSound(void);

		// Destructor
		~CSound(void);

		// Inicializa el motor de sonido (Maxmod & NFLib)
		void Init(void);

		// Carga un archivo MOD para su posterior reproduccion
		void BgmLoad(u16 id);

		// Inicia la reproduccion del archivo actual cargado en RAM
		void BgmPlay(void);

		// Deten la reproduccion del MOD actual
		void BgmStop(void);

		// Pausa el MOD actual
		void BgmPause(void);

		// Reanuda el MOD actual
		void BgmResume(void);

		// Cambia el volumen de reproduccion del MOD
		void BgmVolume(s16 vol);

		// Deten inmediatamente el MOD que este sonando
		void BgmCleanUp(void);

		// Controlador de la musica
		void BgmController(void);

		// Crea una lista random de las musicas disponibles
		void MakeBgmList(void);

		// Reproduce un sonido
		void SfxPlay(u16 id);

		// Deten un sonido
		void SfxStop(u16 id);

		// Controlador de los SFX
		void SfxController(void);

		// Reproduce una voz
		void VoicePlay(u16 id);

		// Deten una voz
		void VoiceStop(void);

		// Controlador de las voces
		void VoiceController(void);

		// Carga la info de los SFX del juego
		void GameSfxInfo(void);

		// Reinicia el sistema de sonido
		void Reset(void);

		// Voces (si es necesario) al obtener un combo
		void ComboVoice(u16 gems);

};
// Metodos globlales de la clase CSound
extern CSound* Sound;



/*
Metodos para la gestion del sonido
*/



#endif