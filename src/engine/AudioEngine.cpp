#include "AudioEngine.h"
#include <math.h>

AudioEngine::AudioEngine() : soundOn(true), volume(MIX_MAX_VOLUME) {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0)
		throw EngineException("Failed to init SDL_mixer:", Mix_GetError());
}

AudioEngine::~AudioEngine() {}

void AudioEngine::toggleSound() {
	soundOn = !soundOn;
}

void AudioEngine::setSoundVolume(const int & _volume) {
	volume = _volume;
}

int AudioEngine::getSoundVolume() {
	return volume;
}

void AudioEngine::playSound(Mix_Chunk * sound) {
	playSound(sound, volume);
}

void AudioEngine::playSound(Mix_Chunk * sound, const int & _volume) {
	if (soundOn) {
		Mix_VolumeChunk(sound, _volume);
		Mix_PlayChannel(-1, sound, 0);
	}
}

void AudioEngine::playMP3(Mix_Music * mp3, const int & times) {
	Mix_PlayMusic(mp3, times);
}

void AudioEngine::playSoundPanning(Mix_Chunk* sound, float xPosPlayer) {
	// Assuming sound source is at x position 400 (Change later)
	float panPosition = NULL;

	// If above 800 or below 0 calculation won't work, change 
	// later to be width of screen as variable
	if (xPosPlayer > 800) {
		xPosPlayer = 800;
	}
	else if(xPosPlayer < 0){
		xPosPlayer = 0;
	}

	// Converting xPosPlayer into a percentage left or right
	// of the sound source.
	if (xPosPlayer > 400) {
		xPosPlayer -= 400;
		panPosition = xPosPlayer / 400;
	}
	else if (xPosPlayer < 400) {
		panPosition = -(xPosPlayer / 400);
	}
	else if(xPosPlayer == 400){
		panPosition = 0;
	}

	//std::cout << "Power Pan : " << panPosition << "\n";

	// Calculates how much panning
	float p = ((M_PI * (panPosition + 1))/4);
	int leftSound = cos(p) * 255;
	int rightSound = sin(p) * 255;
	Mix_SetPanning(1, leftSound, rightSound);
	Mix_PlayChannel(1, sound, 0);
}

void AudioEngine::calculateDistanceEffect(Mix_Chunk* sound) {
	float distanceSound = 10;
	// Could replace 1 with the power of sound at some point to be more accurate
	float newSoundVolume = (1 / (distanceSound * distanceSound)) * MIX_MAX_VOLUME;
	if (newSoundVolume < 1) {
		newSoundVolume = 0;
	}
	std::cout << "This is new Sound: " << newSoundVolume << "\n";
	// Doesn't work if set sound, Why? Could be lack of mix_volume_chunk
	playSound(sound, newSoundVolume);
}