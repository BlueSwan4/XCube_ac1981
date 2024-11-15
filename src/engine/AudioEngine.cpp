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

void AudioEngine::pauseChannel(int channel) {
	Mix_Pause(channel);
}

void AudioEngine::resumeChannel(int channel) {
	Mix_Pause(channel);
}

void AudioEngine::groupChannel(int channel, std::string groupTag) {
	int group = tags.at(groupTag);
	Mix_GroupChannel(channel, group);
}

void AudioEngine::addTag(std::string newTag) {
	int newNumber = tags.size();
	tags[newTag] = newNumber;
}

void AudioEngine::resetSound(Mix_Chunk* sound, int channel) {
	Mix_VolumeChunk(sound, MIX_MAX_VOLUME);
	Mix_UnregisterAllEffects(channel);
}

void AudioEngine::emptyChunk(Mix_Chunk* sound) {
	Mix_FreeChunk(sound);
}

void AudioEngine::soundPanning(Vector3f playerPos, Vector3f soundPos, int playerRotation, int affectedChannel) {

	if (soundPos.z == playerPos.z && soundPos.x == playerPos.x) {return;}

	float targetAnglePlayer = 0;
	float relativeRotation;
	float panPosition = -1;
	float zDistance = fabsf(soundPos.z - playerPos.z);
	float xDistance = fabsf(soundPos.x - playerPos.x);
	float angleRad = atan(zDistance / xDistance);
	int angleDeg = angleRad * 180 / M_PI;

	if (soundPos.z == playerPos.z && soundPos.x == playerPos.x) {
		return;
	}

	if (playerPos.z >= soundPos.z && playerPos.x >= soundPos.x) {
		targetAnglePlayer = 270 - angleDeg;
	} else if (playerPos.z >= soundPos.z && playerPos.x <= soundPos.x) {
		targetAnglePlayer = 90 + angleDeg;
	} else if (playerPos.z <= soundPos.z && playerPos.x >= soundPos.x) {
		targetAnglePlayer = 270 + angleDeg;
	} else if (playerPos.z <= soundPos.z && playerPos.x <= soundPos.x) {
		targetAnglePlayer = 90 - angleDeg;
	}
	relativeRotation = playerRotation - targetAnglePlayer;
	std::cout << "Target Angle: " << targetAnglePlayer;

	if (relativeRotation < 0) {
		relativeRotation = 360 + relativeRotation;
	}

	if (relativeRotation >= 0 && relativeRotation < 90) {
		panPosition = relativeRotation / 90;
		panPosition = -panPosition;
	} else if (relativeRotation >= 90 && relativeRotation <= 270) {
		relativeRotation -= 180;
		panPosition = relativeRotation / 90;
	} else if (relativeRotation > 270 && relativeRotation <= 360) {
		relativeRotation -= 90;
		panPosition = relativeRotation / 90;
	}

	float p = ((M_PI * (panPosition + 1)) / 4);
	int leftSound = cos(p) * 255;
	int rightSound = sin(p) * 255;
	Mix_SetPanning(affectedChannel, leftSound, rightSound);
}

void AudioEngine::calculateDistanceEffect3D(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, float maxDistance, int currentVolume) {
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
		newSoundVolume = percentage * currentVolume;
	}
	std::cout << "Sound Volume : " << newSoundVolume << "\n";
	Mix_VolumeChunk(sound, newSoundVolume);
}

void AudioEngine::calculateDistanceEffect2D(Mix_Chunk* sound, Vector2f playerPos, Vector2f soundPos, float maxDistance, int currentVolume) {
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
	int soundRotation, int playerSoundDetectAngle, int audioSoundDetectAngle, int currentVolume) {
	if (soundPos.z == playerPos.z && soundPos.x == playerPos.x) { return; }

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

	if (playerPos.z >= soundPos.z && playerPos.x >= soundPos.x) {
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
	else if (playerPos.z >= soundPos.z && playerPos.x <= soundPos.x) {
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
	else if (playerPos.z <= soundPos.z && playerPos.x >= soundPos.x) {
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
	else if (playerPos.z <= soundPos.z && playerPos.x <= soundPos.x) {
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

	if (!(playerFacing && soundFacing)) {
		float newSoundVolume = currentVolume * 0.6;
		Mix_VolumeChunk(sound, newSoundVolume);
	}
	else if (!playerFacing || !soundFacing) {
		float newSoundVolume = currentVolume * 0.8;
		Mix_VolumeChunk(sound, newSoundVolume);
	}
}

void AudioEngine::calculateBehindSound(Mix_Chunk* sound, Vector3f playerPos, Vector3f soundPos, int playerRotation,
	int soundRotation, int playerSoundDetectAngle, int currentVolume) {
	if (soundPos.z == playerPos.z && soundPos.x == playerPos.x) { return; }

	bool playerFacing = false;
	float targetAnglePlayer = 0;
	float upperTargetPlayer = 0;
	float lowerTargetPlayer = 0;
	float checker = 0;

	float zDistance = fabsf(soundPos.z - playerPos.z);
	float xDistance = fabsf(soundPos.x - playerPos.x);
	float angleRad = atan(zDistance / xDistance);
	int angleDeg = angleRad * 180 / M_PI;

	if (playerPos.z > soundPos.z && playerPos.x > soundPos.x) {
		targetAnglePlayer = 270 - angleDeg;
		upperTargetPlayer = targetAnglePlayer + playerSoundDetectAngle;
		lowerTargetPlayer = targetAnglePlayer - playerSoundDetectAngle;

		if (playerRotation >= lowerTargetPlayer && playerRotation <= upperTargetPlayer) {
			playerFacing = true;
		}
	}
	else if (playerPos.z > soundPos.z && playerPos.x < soundPos.x) {
		targetAnglePlayer = 90 + angleDeg;
		upperTargetPlayer = targetAnglePlayer + playerSoundDetectAngle;
		lowerTargetPlayer = targetAnglePlayer - playerSoundDetectAngle;

		if (playerRotation >= lowerTargetPlayer && playerRotation <= upperTargetPlayer) {
			playerFacing = true;
		}
	}
	else if (playerPos.z < soundPos.z && playerPos.x > soundPos.x) {
		targetAnglePlayer = 270 + angleDeg;
		checker = targetAnglePlayer + playerSoundDetectAngle;
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
	}
	else if (playerPos.z < soundPos.z && playerPos.x < soundPos.x) {
		targetAnglePlayer = 90 - angleDeg;
		upperTargetPlayer = targetAnglePlayer + playerSoundDetectAngle;
		checker = targetAnglePlayer - playerSoundDetectAngle;
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
	}

	if (!playerFacing) {
		float newSoundVolume = currentVolume * 0.7;
		Mix_VolumeChunk(sound, newSoundVolume);
	}
}

bool AudioEngine::fadeOut(Mix_Chunk* sound, int channel, float fadeTimeStart, float fadeTimeEnd, int currentVolume) {
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = (timeLeft / totalTime) * currentVolume;
		Mix_VolumeChunk(sound, newVolume);
		if (timeLeft <= 0.1) {
			Mix_HaltChannel(channel);
			return false;
		}
		else {
			return true;
		}
	}
}

bool AudioEngine::fadeOut(Mix_Chunk* sound, int channel, float fadeTimeStart, float fadeTimeEnd, int currentVolume, float (*func)(float)) {
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = func((timeLeft / totalTime) * currentVolume);
		Mix_VolumeChunk(sound, newVolume);
		if (timeLeft <= 0.1) {
			Mix_HaltChannel(channel);
			return false;
		}
		else {
			return true;
		}
	}
}

bool AudioEngine::fadeIn(Mix_Chunk* sound, int channel, float fadeTimeStart, float fadeTimeEnd, int currentVolume) {
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = ((totalTime - timeLeft) / totalTime) * currentVolume;
		Mix_VolumeChunk(sound, newVolume);
		if (timeLeft <= 0.1) {
			Mix_HaltChannel(channel);
			return false;
		}
		else {
			return true;
		}
	}
}

bool AudioEngine::fadeIn(Mix_Chunk* sound, int channel, float fadeTimeStart, float fadeTimeEnd, int currentVolume, float (*func)(float)) {
	if (fadeTimeEnd > SDL_GetTicks()) {
		float totalTime = fadeTimeEnd - fadeTimeStart;
		float timeLeft = fadeTimeEnd - SDL_GetTicks();
		float newVolume = func(((totalTime - timeLeft) / totalTime) * currentVolume);
		Mix_VolumeChunk(sound, newVolume);
		if (timeLeft <= 0.1) {
			Mix_HaltChannel(channel);
			return false;
		}
		else {
			return true;
		}
	}
}

AudioElement::AudioElement() {
	sound = NULL;
	channel = -1;
	fadeTimeStart = 0;
	fadeTimeEnd = 0;
	groupTag = "";
	maxDistance = 0;
	currentlyFadingOut = false;
	currentlyFadingIn = false;
}

AudioElement::AudioElement(Mix_Chunk* passedSound, int passedChannel) {
	sound = passedSound;
	channel = passedChannel;
	fadeTimeStart = 0;
	fadeTimeEnd = 0;
	maxDistance = 0;
	groupTag = "";
	currentlyFadingIn = false;
	currentlyFadingOut = false;
}

void AudioElement::startFadingOut(float passedFadeTime) {
	if (currentlyFadingOut == false) {
		passedFadeTime = passedFadeTime * 1000;
		currentlyFadingIn = false;
		setFadeTimeStart(SDL_GetTicks());
		setFadeTimeEnd(passedFadeTime + SDL_GetTicks());
		currentlyFadingOut = true;
	}
}

void AudioElement::startFadingIn(float passedFadeTime) {
	if (currentlyFadingIn == false) {
		passedFadeTime = passedFadeTime * 1000;
		currentlyFadingOut = false;
		setFadeTimeStart(SDL_GetTicks());
		setFadeTimeEnd(passedFadeTime + SDL_GetTicks());
		currentlyFadingIn = true;
	}
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

void AudioElement::setCurrentlyFadingOut(bool passedCurrentlyFadingOut) {
	currentlyFadingOut = passedCurrentlyFadingOut;
}

void AudioElement::setCurrentlyFadingIn(bool passedCurrentlyFadingIn) {
	currentlyFadingIn = passedCurrentlyFadingIn;
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

int AudioElement::getChunkVolume() {
	return Mix_VolumeChunk(sound, -1);
}

bool AudioElement::getCurrentlyFadingOut() {
	return currentlyFadingOut;
}

bool AudioElement::getCurrentlyFadingIn() {
	return currentlyFadingIn;
}

AudioElement3D::AudioElement3D(Mix_Chunk* passedSound, Vector3f passedSoundPosition, int passedChannel,
	float passedMaxDistance, int passedDetectArc, int passedSoundRotation) {
	sound = passedSound;
	soundPosition3D = passedSoundPosition;
	soundRotation = passedSoundRotation;
	channel = passedChannel;
	maxDistance = passedMaxDistance;
	groupTag = "";
	fadeTimeStart = 0;
	fadeTimeEnd = 0;
	setDetectArc(passedDetectArc);
	currentlyFadingIn = false;
	currentlyFadingOut = false;
}

void AudioElement3D::setSoundPosition3D(Vector3f passedSoundPosition) {
	soundPosition3D = passedSoundPosition;
}

void AudioElement3D::setSoundRotation(int passedSoundRotation) {
	soundRotation = passedSoundRotation;
}

void AudioElement3D::setDetectArc(int passedDetectArc) {
	if (passedDetectArc > 90) {
		detectArc = 90;
	}
	else if (passedDetectArc < 0) {
		detectArc = 0;
	}
	else {
		detectArc = passedDetectArc;
	}
}

Vector3f AudioElement3D::getSoundPosition3D() {
	return soundPosition3D;
}

int AudioElement3D::getSoundRotation() {
	return soundRotation;
}

int AudioElement3D::getDetectArc() {
	return detectArc;
}

AudioElement2D::AudioElement2D(Mix_Chunk* passedSound, Vector2f passedSoundPosition, 
	int passedChannel, float passedMaxDistance) {
	sound = passedSound;
	soundPosition2D = passedSoundPosition;
	channel = passedChannel;
	maxDistance = passedMaxDistance;
	groupTag = "";
	fadeTimeStart = 0;
	fadeTimeEnd = 0;
	currentlyFadingIn = false;
	currentlyFadingOut = false;
}

void AudioElement2D::setSoundPosition2D(Vector2f passedSoundPosition) {
	soundPosition2D = passedSoundPosition;
}

Vector2f AudioElement2D::getSoundPosition2D() {
	return soundPosition2D;
}