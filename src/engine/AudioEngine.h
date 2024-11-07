#ifndef __AUDIO_ENGINE_H__
#define __AUDIO_ENGINE_H__

#include <SDL_mixer.h>
#include <SDL.h>
#include <string>
#include <map>

#include "EngineCommon.h"
#include "GameMath.h"

class AudioEngine {
	friend class XCube2Engine;
	private:
		AudioEngine();
		bool soundOn;
		int volume;
		std::map<std::string, int> tags;
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
		* Call this to manually specify the volume of the sound
		*
		* @param sound - the sound to play
		* @param channel - the channel to play the sound through
		*/
		void playSoundChannel(Mix_Chunk* sound, int channel);

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
		void playSoundPanning(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, int playerRotation);

		/**
		* calculate how distance effects volume of sound in 3D (Inverse Square Law)
		*
		* @param sound - the sound to play
		* @param playerPos - the Vector3 position of the player
		* @param soundPos - the Vector3 position of the sound
		* @param currentVolume - the current volume of the sound passed
		* @param maxDistance - the int value of the maximum distance you can hear the sound from
		*
		*/
		void calculateDistanceEffect3D(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, float maxDisatance, int currentVolume);

		/**
		* calculate how distance effects volume of sound in 2D (Inverse Square Law)
		*
		* @param sound - the sound to play
		* @param playerPos - the Vector2 position of the player
		* @param currentVolume - the current volume of the sound passed
		* @param soundPos - the Vector2 position of the sound
		* @param maxDistance - the int value of the maximum distance you can hear the sound from
		*
		*/
		void calculateDistanceEffect2D(Mix_Chunk* sound, Vector2f playerPos, Vector2f soundPos, float maxDistance, int currentVolume);

		/**
		* Change the volume if the sound is behind the player
		*
		* @param sound - the sound to play
		* @param playerPos - position of the player
		* @param soundPos - position of the sound
		* @param playerRotation - rotation of the player
		* @param soundRotation - rotation of the sound
		* @param soundAngleDetect - the angle to which the player can see
		* @param soundAngleDetect - the angle to which the audio source can see
		*
		*/
		void calculateBehindSound(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, 
			int playerRotation, int soundRotation, int playerSoundAngleDetect, int audioSoundAngleDetect);

		/**
		* Fade out sound chunk
		* 
		* @param sound - the sound to fade out
		* @param fadeTimeStart - When the fade out started (used to calculate volume)
		* @param fadeTimeEnd - When the fade out is ending (used to calculate volume)
		* @param currentVolume - The current volume of the sound passed
		* @param func - A function to chnagethe fading graph from linear to another funtion
		*
		*/
		void fadeOut(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, int currentVolume);
		void fadeOut(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, int currentVolume, float(*func)(float));

		/**
		* Fade in sound chunk
		*
		* @param sound - the sound to fade in
		* @param fadeTimeStart - When the fade out started (used to calculate volume)
		* @param fadeTimeEnd - When the fade out is ending (used to calculate volume)
		* @param currentVolume - The current volume of the sound passed
		* @param func - A function to chnagethe fading graph from linear to another funtion
		*
		*/
		void fadeIn(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, int currentVolume);
		void fadeIn(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, int currentVolume, float(*func)(float));

		/**
		* Assigns a channel to a group
		*
		* @param channel - the channel to add to a group
		* @param group - the group to add the channel to
		*
		*/

		void groupChannel(int channel, std::string groupTag);

		/**
		* Adds a tag to the tag map and a corresponding number
		*
		* @param newTag - the name of the new tag
		*
		*/

		void addTag(std::string newTag);

};

class AudioElement {
protected:
	Mix_Chunk* sound;
	int channel;
	float fadeTimeStart;
	float fadeTimeEnd;
	std::string groupTag;
	float maxDistance;
public:
	// Constructor
	AudioElement();
	AudioElement(Mix_Chunk* passedSound, int passedChannel);

	/**
	* Set up the paramaters for fading out a sound
	*
	* @param passedFadeTime - duration of time to fade out over
	*
	*/
	void startFadingOut(float passedFadeTime);

	// Getters and Setters
	void setSound(Mix_Chunk* passedSound);
	void setChannel(int passedChannel);
	void setFadeTimeStart(float passedFadeTimeStart);
	void setFadeTimeEnd(float passedFadeTimeEnd);
	void setGroupTag(std::string passedGroupTag);
	void setMaxDistance(float passedMaxDistance);

	Mix_Chunk* getSound();
	int getChannel();
	float getFadeTimeStart();
	float getFadeTimeEnd();
	std::string getGroupTag();
	float getMaxDistance();

	/**
	* Call this to manually to find the volue of a sound
	*
	* @param sound - the sound to find the volume of
	*/
	int getChunkVolume();
};

class AudioElement2D : public AudioElement{
private:
	Vector2f soundPosition2D;
public:
	// Constructor
	AudioElement2D(Mix_Chunk* passedSound, Vector2f passedSoundPosition, int passedChannel, float passedMaxDistance);

	// Getters and Setters
	void setSoundPosition2D(Vector2f passedSoundPosition);
	Vector2f getSoundPosition2D();
};

class AudioElement3D : public AudioElement {
private:
	Vector3f soundPosition3D;
	int soundRotation;
	int detectArc;
public:
	// Constructor
	AudioElement3D(Mix_Chunk* passedSound, Vector3f passedSoundPosition, int passedChannel, float passedMaxDistance,
		int passedDetectArc, int passedSoundRotation);

	// Getters and Setters
	void setSoundPosition3D(Vector3f passedSoundPosition);
	void setSoundRotation(int passedSoundRotation);
	void setDetectArc(int passeddetectArc);

	Vector3f getSoundPosition3D();
	int getSoundRotation();
	int getDetectArc();
};

#endif