#ifndef __AUDIO_ENGINE_H__
#define __AUDIO_ENGINE_H__

#include <SDL_mixer.h>

#include "EngineCommon.h"

class AudioEngine {
	friend class XCube2Engine;
	private:
		AudioEngine();
		bool soundOn;
		int volume;
	public:
		~AudioEngine();
		void toggleSound();

		/**
		* Controls the volume of all sounds
		* @param volume - in the range [0..128]
		*/
		void setSoundVolume(const int &);
		int getSoundVolume();

		void playSound(Mix_Chunk * sound);

		/**
		* Call this to manually specify the volume of the sound
		*
		* @param sound - the sound to play
		* @param volume - the volume at which to play in range [0..128]
		*/
		void playSound(Mix_Chunk * sound, const int & _volume);

		/**
		* Plays mp3 file given amount of times
		*
		* @param mp3 - the file to play
		* @param times - number of times, -1 will play indefinitely
		*/
		void playMP3(Mix_Music * mp3, const int & times);

		/**
		* Plays WAV files while taking into account panning (Uses constant power panning)
		*
		* @param sound - the sound to play
		* @param xPosPlayer - the x axis position of the player
		*/
		void playSoundPanning(Mix_Chunk* sound, float xPosPlayer);

		/**
		* calculate how distance effects volume of sound (Inverse Square Law)
		*
		* @param sound - the sound to play
		* 
		*/
		void calculateDistanceEffect(Mix_Chunk* sound);

};

#endif