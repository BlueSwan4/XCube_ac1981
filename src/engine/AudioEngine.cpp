#include "AudioEngine.h"
#include <math.h>

AudioEngine::AudioEngine() : soundOn(true), volume(MIX_MAX_VOLUME) {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0)
		throw EngineException("Failed to init SDL_mixer:", Mix_GetError());
	Mix_AllocateChannels(16);
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

void AudioEngine::playSoundChannel(Mix_Chunk* sound, int channel) {
	if (soundOn) {
		Mix_VolumeChunk(sound, volume);
		Mix_PlayChannel(channel, sound, 0);
	}
}

void AudioEngine::playMP3(Mix_Music * mp3, const int & times) {
	Mix_PlayMusic(mp3, times);
}

void AudioEngine::groupChannel(int channel, std::string groupTag) {
	int group = tags.at(groupTag);
	Mix_GroupChannel(channel, group);
}

void AudioEngine::addTag(std::string newTag) {
	int newNumber = tags.size();
	tags[newTag] = newNumber;
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

void AudioEngine::calculateDistanceEffect(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos) {
	float distanceSound = sqrt(pow(playerPos.x - soundPos.x, 2) + pow(playerPos.y - soundPos.y, 2) + pow(playerPos.z - soundPos.z, 2));
	// Could replace 1 with the power of sound at some point to be more accurate
	float newSoundVolume = (1 / pow(distanceSound, 2) * MIX_MAX_VOLUME);
	if (newSoundVolume < 1) {
		newSoundVolume = 0;
	}
	std::cout << "This is new Sound: " << newSoundVolume << "\n";
	// Doesn't work if set sound, Why? Could be lack of mix_volume_chunk
	//setSoundVolume(newSoundVolume);
	setSoundVolume(newSoundVolume);
	playSound(sound);
}

void AudioEngine::calculateDistanceEffect(Mix_Chunk* sound, Vector2f playerPos, Vector2f soundPos) {
	float distanceSound = sqrt(pow(playerPos.x - soundPos.x, 2) + pow(playerPos.y - soundPos.y, 2));
	// Could replace 1 with the power of sound at some point to be more accurate
	float newSoundVolume = (1 / pow(distanceSound, 2) * MIX_MAX_VOLUME);
	if (newSoundVolume < 1) {
		newSoundVolume = 0;
	}
	std::cout << "This is new Sound: " << newSoundVolume << "\n";
	// Doesn't work if set sound, Why? Could be lack of mix_volume_chunk
	setSoundVolume(newSoundVolume);
	playSound(sound);
}

void AudioEngine::calculateBehindSound(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, int playerRotation, int soundRotation) {
	/*
	int anglePlayerToSoundRad = acos(((playerPos.x * soundPos.z) + (playerPos.z + soundPos.x) /
		((sqrt(pow(playerPos.x, 2) + pow(playerPos.z, 2) * sqrt(pow(soundPos.x, 2) + pow(soundPos.z, 2)))))));
	int anglePlayerToSoundDeg = anglePlayerToSoundRad * 180 / M_PI;
	std::cout << "Angle Beforeeeee: " << anglePlayerToSoundRad;
	std::cout << "Angle Before: " << anglePlayerToSoundDeg;

	float angleDiff = ((playerRotation - anglePlayerToSoundDeg + 180 + 360) % 360) - 180;
	std::cout << "Angle After: " << angleDiff;

	bool playerFacing = (angleDiff <= 90 && angleDiff >= -90);
	
	if (playerFacing) {
		return;
	}
	else {
		float newSoundVolume = getSoundVolume() * 0.7;
		playSound(sound, newSoundVolume);
	}
	*/
	if (playerPos.z > soundPos.z && playerPos.x > soundPos.x) {
		if ((playerRotation >= 135 && playerRotation <= 315) && !(soundRotation > 135 && soundRotation < 315)) {
			return;
		}
		else {
			float newSoundVolume = getSoundVolume() * 0.7;
			Mix_VolumeChunk(sound, newSoundVolume);
		}
	}
	else if (playerPos.z > soundPos.z && playerPos.x < soundPos.x) {
		if ((playerRotation >= 45 && playerRotation <= 225) && !(soundRotation > 45 && soundRotation < 225)) {
			return;
		}
		else {
			float newSoundVolume = getSoundVolume() * 0.7;
			Mix_VolumeChunk(sound, newSoundVolume);
		}
	}
	else if (playerPos.z < soundPos.z && playerPos.x > soundPos.x) {
		if ((soundRotation >= 135 && soundRotation <= 315) && !(playerRotation > 135 && playerRotation < 315)) {
			return;
		}
		else {
			float newSoundVolume = getSoundVolume() * 0.7;
			Mix_VolumeChunk(sound, newSoundVolume);
		}
	}
	else if (playerPos.z < soundPos.z && playerPos.x < soundPos.x) {
		if ((soundRotation >= 45 && soundRotation <= 225) && !(playerRotation > 45 && playerRotation < 225)) {
			return;
		}
		else {
			float newSoundVolume = getSoundVolume() * 0.7;
			Mix_VolumeChunk(sound, newSoundVolume);
		}
	}
}

void AudioEngine::fadeOut(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd) {
	// Got to get from max volume to 0
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = (timeLeft / totalTime) * 128;
		Mix_VolumeChunk(sound, newVolume);
	}
}

void AudioEngine::fadeOut(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, float (*func)(float)) {
	// Got to get from max volume to 0
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = func((timeLeft / totalTime) * 128);
		Mix_VolumeChunk(sound, newVolume);
	}
}

void AudioEngine::fadeIn(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd) {
	// Got to get from max volume to 0
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = ((totalTime - timeLeft) / totalTime) * 128;
		Mix_VolumeChunk(sound, newVolume);
	}
}

void AudioEngine::fadeIn(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, float (*func)(float)) {
	// Got to get from max volume to 0
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = func(((totalTime - timeLeft) / totalTime) * 128);
		Mix_VolumeChunk(sound, newVolume);
	}
}

AudioElement::AudioElement() {
	sound = NULL;
	channel = -1;
	fadeTimeStart = 0;
	fadeTimeEnd = 0;
	groupTag = "";
}

AudioElement::AudioElement(Mix_Chunk* passedSound, int passedChannel) {
	sound = passedSound;
	channel = passedChannel;
	groupTag = "";
	fadeTimeStart = false;
	fadeTimeEnd = 0;
}

void AudioElement::startFadingOut(float passedFadeTime) {
	setFadeTimeStart(SDL_GetTicks());
	setFadeTimeEnd(passedFadeTime + SDL_GetTicks());

}

void AudioElement::setSound(Mix_Chunk* passedSound) {
	sound = passedSound;
}

void AudioElement::setChannel(int passedChannel) {
	channel = passedChannel;
}

void AudioElement::setFadeTimeStart(float passedFadeTimeStart) {
	fadeTimeStart = passedFadeTimeStart;
}

void AudioElement::setFadeTimeEnd(float passedFadeTimeEnd) {
	fadeTimeEnd = passedFadeTimeEnd;
}

void AudioElement::setGroupTag(std::string passedGroupTag) {
	groupTag = passedGroupTag;
}

Mix_Chunk* AudioElement::getSound() {
	return sound;
}

int AudioElement::getChannel() {
	return channel;
}

float AudioElement::getFadeTimeStart() {
	return fadeTimeStart;
}

float AudioElement::getFadeTimeEnd() {
	return fadeTimeEnd;
}

std::string AudioElement::getGroupTag() {
	return groupTag;
}

AudioElement3D::AudioElement3D(Mix_Chunk* passedSound, Vector3f passedSoundPosition, int passedChannel) {
	sound = passedSound;
	soundPosition3D = passedSoundPosition;
	channel = passedChannel;
	groupTag = "";
	fadeTimeStart = false;
	fadeTimeEnd = 0;
}

void AudioElement3D::setSoundPosition3D(Vector3f passedSoundPosition) {
	soundPosition3D = passedSoundPosition;
}

void AudioElement3D::setSoundRotation(int passedSoundRotation) {
	soundRotation = passedSoundRotation;
}

Vector3f AudioElement3D::getSoundPosition3D() {
	return soundPosition3D;
}

int AudioElement3D::getSoundRotation() {
	return soundRotation;
}

AudioElement2D::AudioElement2D(Mix_Chunk* passedSound, Vector2f passedSoundPosition, int passedChannel) {
	sound = passedSound;
	soundPosition2D = passedSoundPosition;
	channel = passedChannel;
	groupTag = "";
	fadeTimeStart = false;
	fadeTimeEnd = 0;
}

void AudioElement2D::setSoundPosition2D(Vector2f passedSoundPosition) {
	soundPosition2D = passedSoundPosition;
}

Vector2f AudioElement2D::getSoundPosition2D() {
	return soundPosition2D;
}