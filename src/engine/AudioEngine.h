#ifndef __AUDIO_ENGINE_H__
#define __AUDIO_ENGINE_H__

#include <SDL_mixer.h>

#include "EngineCommon.h"
#include "GameMath.h"

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
		* Frees audio chunks at the end of a session
		*
		* @param sound - the sound to play
		*
		*/
		void emptyChunk(Mix_Chunk* sound);

		/**
		* Plays WAV files while taking into account panning (Uses constant power panning)
		*
		* @param sound - the sound to play
		* @param xPosPlayer - the x axis position of the player
		* @param xPosSound - the x axis position of the sound
		* 
		*/
		void playSoundPanning(Mix_Chunk* sound, float xPosPlayer, float xPosSound);

		/**
		* calculate how distance effects volume of sound in 3D (Inverse Square Law)
		*
		* @param sound - the sound to play
		* @param playerPos - the Vector3 position of the player
		* @param soundPos - the Vector3 position of the sound
		*
		*/
		void calculateDistanceEffect(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos);

		/**
		* calculate how distance effects volume of sound in 2D (Inverse Square Law)
		*
		* @param sound - the sound to play
		* @param playerPos - the Vector2 position of the player
		* @param soundPos - the Vector2 position of the sound
		*
		*/
		void calculateDistanceEffect(Mix_Chunk* sound, Vector2f playerPos, Vector2f soundPos);

		/**
		* Change the volume if the sound is behind the player
		*
		* @param sound - the sound to play
		*
		*/
		void calculateBehindSound(Mix_Chunk* sound, float zPosPlayer, float zPosSound);

};

class AudioElement {
private:
	Mix_Chunk* sound;
	Vector3f soundPosition;
public:
	// Getters and Setters
	AudioElement(Mix_Chunk* passedSound, Vector3f passedSoundPosition);

	void setSoundPosition(Vector3f passedSoundPosition);
	void setSound(Mix_Chunk* passedSound);

	Vector3f getSoundPosition();
	Mix_Chunk* getSound();
};

#endif