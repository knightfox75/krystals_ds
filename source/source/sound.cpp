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
// Includes de MAXMOD
#include <maxmod9.h>
// Includes librerias propias
#include <nf_lib.h>
// Includes del juego
#include "includes.h"



/*
Metodos de la clase "CSound"
*/

// Contructor clase CSound
CSound::CSound(void) {
}



// Destructor clase CSound
CSound::~CSound(void) {
}



// Metodos globlales de la clase CSound
CSound* Sound;



/*
Inicializaciones
*/

// Inicializa el motor de sonido (Maxmod & NFLib)
void CSound::Init(void) {

	// Inicializa MAXMOD con en banco de sonido seleccionado
	char sbfname[] = "snd/sbf/soundbank.sbf";
	mmInitDefault(sbfname);

	// Modo Maxmod (30 canales interpolados por soft)
	mmSelectMode(MM_MODE_C);

	// Reserva los canales 0, 1, 2 para libnds sound system
	mmLockChannels(BIT(0) | BIT(1) | BIT(2));

	// Inicializa los buffers de sonido de NFlib
	NF_InitRawSoundBuffers();

	// Inicializa los niveles de volumen
	volume.main = 100;			// Volumen general
	volume.bgm = 100;			// Volumen de la musica
	volume.sfx = 100;			// Volumen de los efectos de sonido
	volume.voice = 100;			// Volumen de las voces

	// Inicializa las variables de control de la Musica
	bgm.play = false;			// No esta sonando
	bgm.pause = false;			// No esta en pausa
	bgm.stop = false;			// Comando STOP deshabilitado
	bgm.id = -1;				// Sin ID valido al incio
	bgm.volume = 1024;			// Volumen inicial de FADE (1024)
	bgm.current_track = -1;		// ID del mod en reproduccion
	bgm.next_track = -1;		// ID del siguiente mod a reproducir

}



// Carga un archivo MOD para su posterior reproduccion
void CSound::BgmLoad(u16 id) {

	// Guarda la ID del MOD cargado
	bgm.id = id;

	// Y cargalo en RAM
	mmLoad(id);

}



// Inicia la reproduccion del archivo actual cargado en RAM
void CSound::BgmPlay(void) {

	// Reproduce el MOD cargado en modo "loop" si no esta sonando nada
	if (!bgm.play) {
		mmStart(bgm.id, MM_PLAY_LOOP);
		BgmVolume(bgm.volume);
	}

	// Y marca que se esta reproduciendo un MOD
	bgm.play = true;

}



// Deten la reproduccion del MOD actual
void CSound::BgmStop(void) {

	// Si esta sonando
	if (bgm.play) {

		// Deten el MOD actual
		mmStop();

		// Descargalo de la memoria
		mmUnload(bgm.id);

		// Y marca que no esta sonando ningun MOD
		bgm.play = false;
		bgm.pause = false;
		bgm.id = -1;

	}

}


// Pausa la reproduccion del MOD actual
void CSound::BgmPause(void) {

	// Si hay algun MOD sonando y no esta ya en pausa
	if (bgm.play && !bgm.pause) {
		// Deten el MOD
		mmStop();
		// Y deja constancia
		bgm.pause = true;
	}

}



// Reanuda la reproduccion del MOD actual
void CSound::BgmResume(void) {

	// Si hay algun MOD sonando, y esta en pausa
	if (bgm.play && bgm.pause) {
		// Reanuda la reproduccion desde el principio
		mmStart(bgm.id, MM_PLAY_LOOP);
		// E indicalo
		bgm.pause = false;
	}

}



// Cambia el volumen de reproduccion del MOD
void CSound::BgmVolume(s16 vol) {

	// Variables
	s16 t_vol = vol;
	s16 level;

	// Fix del volumen
	if (t_vol < 0) t_vol = 0;
	if (t_vol > 1024) t_vol = 1024;

	// Calcula el nivel REAL de volumen segun las opciones
	level = (int)((t_vol * volume.bgm) / 100);		// Segun el control especifico
	level = (int)((level * volume.main) / 100);		// Segun el control maestro

	// Aplica el volumen
	mmSetModuleVolume(level);

	// Guarda el volumen actual
	bgm.volume = t_vol;

}



// Deten inmediatamente el MOD que este sonando
void CSound::BgmCleanUp(void) {

	// Si hay algun MOD en reproduccion
	if (bgm.play) {
		// Deten el MOD
		BgmStop();
	}
	// Resete los flags de control
	bgm.stop = false;			// Comando STOP off
	bgm.current_track = -1;		// ID del mod en reproduccion
	bgm.next_track = -1;		// ID del siguiente mod a reproducir
	bgm.volume = 1024;			// Restaura el volumen

}



// Controlador de la musica
void CSound::BgmController(void) {

	// Deten la musica si has recivido el comando STOP
	if (bgm.stop) {

		// Si esta sonando algo
		if (bgm.play) {
			bgm.volume -= 16;
			if (bgm.volume < 0) bgm.volume = 0;
			BgmVolume(bgm.volume);
			// Si ya has bajado todo el volumen, deten la musica
			if (bgm.volume == 0) {
				BgmStop();
				bgm.stop = false;
				bgm.current_track = -1;
				bgm.next_track = -1;
			}
		} else {
			// Si no, anula el comando
			bgm.stop = false;
		}

	} else {	// Si no has recivido orden de detener la musica

		// Cambio de musica o inicio de musica?
		if (bgm.next_track != bgm.current_track) {
			// Si actualmente no hay ninguna musica sonando, cargala y reproducela
			if (!bgm.play) {
				BgmLoad(bgm.next_track);
				BgmPlay();
				bgm.current_track = bgm.next_track;
			} else {
				// Si hay una cancion en curso, baja el volumen
				bgm.volume -= 16;
				if (bgm.volume < 0) bgm.volume = 0;
				BgmVolume(bgm.volume);
				// Si ya has bajado todo el volumen, deten la musica
				if (bgm.volume == 0) {
					BgmStop();
				}
			}
		} else {	// Si es necesario, realiza el FADE-IN de la musica
			if (bgm.volume < 1024) {
				// Incrementa el volumen
				bgm.volume += 16;
				if (bgm.volume > 1024) bgm.volume = 1024;
				BgmVolume(bgm.volume);
			}
		}

	}

}



// Crea una lista random de las musicas disponibles
void CSound::MakeBgmList(void) {

	// Variables internas
	u8 n = 0, i = 0;

	// Inicializa el vector de cola de musicas (BGM_NUMBER elementos)
	bgm.cue.reserve(BGM_NUMBER);

	// Añade todos los elementos a la cola
	for (n = MOD_BGM01; n <= MOD_BGM10; n ++) {
		bgm.cue.push_back(n);	// Añade un nuevo elemento a la cola
	}

	// Ahora, extrae aleatoriamente todos los elementos de la cola, para crear una lista random de musicas
	n = 0;
	while (bgm.cue.size() > 0) {	// Mientras quede algun elemento en cola
		// Genera que elemento aleatorio extraeras
		i = (int)(rand() % bgm.cue.size());
		// Copia el elemento de la cola al array de lista de fondos
		bgm.list[n] = bgm.cue[i];
		n ++;
		// Ahora elimina este elemento de la lista
		bgm.cue.erase(bgm.cue.begin() + i);
	}

	// Elimina el vector de la memoria
	bgm.cue.clear();
	
}



// Reproduce un sonido
void CSound::SfxPlay(u16 id) {

	// Si este SFX se esta reproduciendo, eliminalo antes
	if (sfx[id].active) soundKill(sfx[id].handler);

	// Calcula el volumen de este SFX
	u16 t_vol = (int)((sfx[id].volume * volume.sfx) / 100);
	t_vol = (int)((t_vol * volume.main) / 100);

	// Inicia la reproduccion de este sonido y guarda los datos del mismo
	sfx[id].handler = NF_PlayRawSound(sfx[id].id, t_vol, sfx[id].pan, false, 0);
	sfx[id].active = true;
	sfx[id].position = 0;

}


// Deten un sonido
void CSound::SfxStop(u16 id) {

	// Si este SFX se esta reproduciendo, eliminalo
	if (sfx[id].active) {
		soundKill(sfx[id].handler);
		sfx[id].active = false;
	}

}



// Controlador de los SFX
void CSound::SfxController(void) {

	// Variables
	u8 n = 0;

	// Analiza todos los SFX antivos
	for (n = 0; n < SFX_NUMBER; n ++) {
		// Si esta activo, actualiza el indicador de posicion
		if (sfx[n].active) {
			sfx[n].position ++;
			// Si has llegado al final del archivo, detenlo
			if (sfx[n].position >= sfx[n].length) {
				SfxStop(n);
			}
		}
	}
	
}



// Reproduce una voz
void CSound::VoicePlay(u16 id) {

	// Si esta voz se esta reproduciendo, eliminala antes
	if (voice.active) soundKill(voice.handler);

	// Calcula el volumen de esta voz
	u16 t_vol = (int)((voice.volume[id] * volume.voice) / 100);
	t_vol = (int)((t_vol * volume.main) / 100);

	// Inicia la reproduccion de esta voz y guarda los datos de la misma
	voice.handler = NF_PlayRawSound(voice.id[id], t_vol, voice.pan[id], false, 0);
	voice.active = true;
	voice.position = 0;
	voice.size = voice.length[id];

}



// Deten una voz
void CSound::VoiceStop() {

	// Si esta voz se esta reproduciendo, eliminala
	if (voice.active) {
		soundKill(voice.handler);
		voice.active = false;
	}

}



// Controlador de las voces
void CSound::VoiceController(void) {

	// Si esta activo, actualiza el indicador de posicion
	if (voice.active) {
		voice.position ++;
		// Si has llegado al final del archivo, detenlo
		if (voice.position >= voice.size) {
			VoiceStop();
		}
	}

}



// Carga la info de los SFX del juego
void CSound::GameSfxInfo(void) {

	/*
	Efectos de sonido
	*/

	// SFX de la Explosion
	sprintf(sfx[SFX_ID_EXPLOSION].file, "explosion");
	sfx[SFX_ID_EXPLOSION].active = false;
	sfx[SFX_ID_EXPLOSION].freq = 22050;
	sfx[SFX_ID_EXPLOSION].volume = 127;
	sfx[SFX_ID_EXPLOSION].pan = 64;
	sfx[SFX_ID_EXPLOSION].length = 86;
	sfx[SFX_ID_EXPLOSION].position = 0;


	/*
	Voces
	*/

	// Voz "Nice"
	sprintf(voice.file[VOICE_ID_NICE], "nice");
	voice.freq[VOICE_ID_NICE] = 22050;
	voice.volume[VOICE_ID_NICE] = 127;
	voice.pan[VOICE_ID_NICE] = 64;
	voice.length[VOICE_ID_NICE] = 43;

	// Voz "Great"
	sprintf(voice.file[VOICE_ID_GREAT], "great");
	voice.freq[VOICE_ID_GREAT] = 22050;
	voice.volume[VOICE_ID_GREAT] = 127;
	voice.pan[VOICE_ID_GREAT] = 64;
	voice.length[VOICE_ID_GREAT] = 39;

	// Voz "Fantastic"
	sprintf(voice.file[VOICE_ID_FANTASTIC], "fantastic");
	voice.freq[VOICE_ID_FANTASTIC] = 22050;
	voice.volume[VOICE_ID_FANTASTIC] = 127;
	voice.pan[VOICE_ID_FANTASTIC] = 64;
	voice.length[VOICE_ID_FANTASTIC] = 62;

	// Voz "Wonderfull"
	sprintf(voice.file[VOICE_ID_WONDREFULL], "wonderfull");
	voice.freq[VOICE_ID_WONDREFULL] = 22050;
	voice.volume[VOICE_ID_WONDREFULL] = 127;
	voice.pan[VOICE_ID_WONDREFULL] = 64;
	voice.length[VOICE_ID_WONDREFULL] = 51;

	// Voz "Game Over"
	sprintf(voice.file[VOICE_ID_GAMEOVER], "game_over");
	voice.freq[VOICE_ID_GAMEOVER] = 22050;
	voice.volume[VOICE_ID_GAMEOVER] = 127;
	voice.pan[VOICE_ID_GAMEOVER] = 64;
	voice.length[VOICE_ID_GAMEOVER] = 93;

	// Parametros comunes de las voces
	voice.position = 0;		// Reseteo de posicion
	voice.size = 0;			// Duracion de la voz seleccionada
	voice.last = -1;		// Ultima voz reproducida
	voice.active = false;	// Ninguna voz activa

}



// Reinicia el sistema de sonido
void CSound::Reset(void) {

	// Deten si esta sonando alguna musica
	BgmCleanUp();

	// Reinicia los buffers de sonido (NFlib)
	NF_ResetRawSoundBuffers();

}



// Voces (si es necesario) al obtener un combo
void CSound::ComboVoice(u16 gems) {

	// Dependiendo del numero de gemas del combo
	if (gems >= 20) {
		if (voice.last != VOICE_ID_WONDREFULL) {
			VoicePlay(VOICE_ID_WONDREFULL);
			voice.last = VOICE_ID_WONDREFULL;
		}
	} else if (gems >= 15) {
		if (voice.last != VOICE_ID_FANTASTIC) {
			VoicePlay(VOICE_ID_FANTASTIC);
			voice.last = VOICE_ID_FANTASTIC;
		}
	} else if (gems >= 10) {
		if (voice.last != VOICE_ID_GREAT) {
			VoicePlay(VOICE_ID_GREAT);
			voice.last = VOICE_ID_GREAT;
		}
	} else if (gems >= 5) {
		if (voice.last != VOICE_ID_NICE) {
			VoicePlay(VOICE_ID_NICE);
			voice.last = VOICE_ID_NICE;
		}
	} else {
		voice.last = -1;
	}

}
