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

void AudioEngine::calculateDistanceEffect(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, float maxDistance) {
	float distanceSound = sqrt(pow(playerPos.x - soundPos.x, 2) + pow(playerPos.y - soundPos.y, 2) + pow(playerPos.z - soundPos.z, 2));
	float percentage = 1 - (distanceSound / maxDistance);
	std::cout << "Percentage : " << percentage << "\n";
	float newSoundVolume;
	if (percentage < 0) {
		newSoundVolume = 0;
	}
	else {
		if (percentage > 1) {
			percentage = 1;
		}
		newSoundVolume = percentage * MIX_MAX_VOLUME;
	}
	std::cout << "Sound Volume : " << newSoundVolume << "\n";
	Mix_VolumeChunk(sound, newSoundVolume);
}

void AudioEngine::calculateDistanceEffect(Mix_Chunk* sound, Vector2f playerPos, Vector2f soundPos, float maxDistance) {
	float distanceSound = sqrt(pow(playerPos.x - soundPos.x, 2) + pow(playerPos.y - soundPos.y, 2));
	float percentage = 1 - (distanceSound / maxDistance);
	float newSoundVolume;
	if (percentage < 0) {
		newSoundVolume = 0;
	}
	else {
		if (percentage > 1) {
			percentage = 1;
		}
		newSoundVolume = percentage * MIX_MAX_VOLUME;
	}
	Mix_VolumeChunk(sound, newSoundVolume);
}

void AudioEngine::calculateBehindSound(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, int playerRotation, 
	int soundRotation, int playerSoundDetectAngle, int audioSoundDetectAngle) {
	bool playerFacing = false;
	bool soundFacing = false;
	
	float targetAnglePlayer = 0;
	float targetAngleSound = 0;
	float upperTargetPlayer = 0;
	float lowerTargetPlayer = 0;
	float upperTargetSound = 0;
	float lowerTargetSound = 0;
	float checker = 0;

	float zDistance = fabsf(soundPos.z - playerPos.z);
	float xDistance = fabsf(soundPos.x - playerPos.x);
	float angleRad = atan(zDistance / xDistance);
	int angleDeg = angleRad * 180 / M_PI;

	if (playerPos.z > soundPos.z && playerPos.x > soundPos.x) {
		targetAnglePlayer = 270 - angleDeg;
		targetAngleSound = angleDeg;
		upperTargetPlayer = targetAnglePlayer + playerSoundDetectAngle;
		lowerTargetPlayer = targetAnglePlayer - playerSoundDetectAngle;
		checker = targetAngleSound - audioSoundDetectAngle;
		upperTargetSound = targetAngleSound + audioSoundDetectAngle;
		lowerTargetSound;
		if (checker < 0) {
			lowerTargetSound = 360 + checker;
			if (soundRotation >= lowerTargetSound || soundRotation <= upperTargetSound) {
				soundFacing = true;
			}
		}
		else {
			if (soundRotation >= lowerTargetSound && soundRotation <= upperTargetSound) {
				soundFacing = true;
			}
		}
		if (playerRotation >= lowerTargetPlayer && playerRotation <= upperTargetPlayer) {
			playerFacing = true;
		}
	}
	else if (playerPos.z > soundPos.z && playerPos.x < soundPos.x) {
		targetAnglePlayer = 90 + angleDeg;
		targetAngleSound = 360 - angleDeg;
		upperTargetPlayer = targetAnglePlayer + playerSoundDetectAngle;
		lowerTargetPlayer = targetAnglePlayer - playerSoundDetectAngle;
		checker = targetAngleSound + audioSoundDetectAngle;
		lowerTargetSound = targetAngleSound - audioSoundDetectAngle;
		upperTargetSound;
		if (checker > 360) {
			upperTargetSound = checker - 360;
			if (soundRotation >= lowerTargetSound || soundRotation <= upperTargetSound) {
				soundFacing = true;
			}
		}
		else {
			if (soundRotation >= lowerTargetSound && soundRotation <= upperTargetSound) {
				soundFacing = true;
			}
		}
		if (playerRotation >= lowerTargetPlayer && playerRotation <= upperTargetPlayer) {
			playerFacing = true;
		}
	}
	else if (playerPos.z < soundPos.z && playerPos.x > soundPos.x) {
		targetAnglePlayer = 270 + angleDeg;
		targetAngleSound = 180 - angleDeg;
		upperTargetSound = targetAngleSound + audioSoundDetectAngle;
		checker = targetAnglePlayer + playerSoundDetectAngle;
		lowerTargetSound = targetAngleSound - audioSoundDetectAngle;
		lowerTargetPlayer = targetAnglePlayer - playerSoundDetectAngle;
		upperTargetPlayer;
		if (checker > 360) {
			upperTargetPlayer = checker - 360;
			if (playerRotation >= lowerTargetPlayer || playerRotation <= upperTargetPlayer) {
				playerFacing = true;
			}
		}
		else {
			if (playerRotation >= lowerTargetPlayer && playerRotation <= upperTargetPlayer) {
				playerFacing = true;
			}
		}
		if (soundRotation >= lowerTargetSound && soundRotation <= upperTargetSound) {
			soundFacing = true;
		}
	}
	else if (playerPos.z < soundPos.z && playerPos.x < soundPos.x) {
		targetAnglePlayer = 90 - angleDeg;
		targetAngleSound = 180 + angleDeg;
		upperTargetSound = targetAngleSound + audioSoundDetectAngle;
		upperTargetPlayer = targetAnglePlayer + playerSoundDetectAngle;
		checker = targetAnglePlayer - playerSoundDetectAngle;
		lowerTargetSound = targetAngleSound - audioSoundDetectAngle;
		lowerTargetPlayer;
		if (checker < 0) {
			lowerTargetPlayer = 360 + checker;
			if (playerRotation >= lowerTargetPlayer || playerRotation <= upperTargetPlayer) {
				playerFacing = true;
			}
		}
		else {
			if (playerRotation >= lowerTargetPlayer && playerRotation <= upperTargetPlayer) {
				playerFacing = true;
			}
		}
		if (soundRotation >= lowerTargetSound && soundRotation <= upperTargetSound) {
			soundFacing = true;
		}
	}

	if (playerFacing && soundFacing) {
		float newSoundVolume = getSoundVolume() * 0.7;
		Mix_VolumeChunk(sound, newSoundVolume);
	}

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
}

void AudioEngine::fadeOut(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd) {
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = (timeLeft / totalTime) * 128;
		Mix_VolumeChunk(sound, newVolume);
	}
}

void AudioEngine::fadeOut(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, float (*func)(float)) {
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = func((timeLeft / totalTime) * 128);
		Mix_VolumeChunk(sound, newVolume);
	}
}

void AudioEngine::fadeIn(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd) {
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = ((totalTime - timeLeft) / totalTime) * 128;
		Mix_VolumeChunk(sound, newVolume);
	}
}

void AudioEngine::fadeIn(Mix_Chunk* sound, float fadeTimeStart, float fadeTimeEnd, float (*func)(float)) {
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
	maxDistance = 0;
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

void AudioElement::setMaxDistance(float passedMaxDistance) {
	maxDistance = passedMaxDistance;
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

float AudioElement::getMaxDistance() {
	return maxDistance;
}

AudioElement3D::AudioElement3D(Mix_Chunk* passedSound, Vector3f passedSoundPosition, int passedChannel, float passedMaxDistance) {
	sound = passedSound;
	soundPosition3D = passedSoundPosition;
	channel = passedChannel;
	maxDistance = passedMaxDistance;
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

AudioElement2D::AudioElement2D(Mix_Chunk* passedSound, Vector2f passedSoundPosition, int passedChannel, float passedMaxDistance) {
	sound = passedSound;
	soundPosition2D = passedSoundPosition;
	channel = passedChannel;
	maxDistance = passedMaxDistance;
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