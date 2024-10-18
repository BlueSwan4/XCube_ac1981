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

void AudioEngine::emptyChunk(Mix_Chunk* sound) {
	Mix_FreeChunk(sound);
}

void AudioEngine::playSoundPanning(Mix_Chunk* sound, float xPosPlayer, float xPosSound) {
	// Assuming sound source is at x position 400 (Change later)
	float panPosition = NULL;
	float panDistance = 400;
	float relativeXPosPlayer = xPosPlayer - xPosSound;
	std::cout << "Player Position: " << relativeXPosPlayer << "\n";


	// If above 800 or below 0 calculation won't work, change 
	// later to be width of screen as variable
	if (relativeXPosPlayer > panDistance) {
		relativeXPosPlayer = panDistance;
	}
	else if(relativeXPosPlayer < -panDistance){
		relativeXPosPlayer = -panDistance;
	}

	// Converting xPosPlayer into a percentage left or right
	// of the sound source.
	if (relativeXPosPlayer > 0) {
		panPosition = relativeXPosPlayer / 400;
	}
	else if (relativeXPosPlayer < 0) {
		panPosition = (relativeXPosPlayer / 400);
	}
	else if(relativeXPosPlayer == 0){
		panPosition = 0;
	}

	// Reverse sign to make sound come out of correct side
	panPosition = -panPosition;

	std::cout << "Power Pan : " << panPosition << "\n";

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

void AudioEngine::calculateBehindSound(Mix_Chunk* sound, float zPosPlayer, float zPosSound) {
	if (zPosPlayer > zPosSound) {
		float newSoundVolume = getSoundVolume() * 0.7;
		playSound(sound, newSoundVolume);
	}
}

AudioElement::AudioElement(Mix_Chunk* passedSound, Vector3f passedSoundPosition) {
	sound = passedSound;
	soundPosition = passedSoundPosition;
}

void AudioElement::setSound(Mix_Chunk* passedSound) {
	sound = passedSound;
}

void AudioElement::setSoundPosition(Vector3f passedSoundPosition) {
	soundPosition = passedSoundPosition;
}

Mix_Chunk* AudioElement::getSound() {
	return sound;
}

Vector3f AudioElement::getSoundPosition() {
	return soundPosition;
}