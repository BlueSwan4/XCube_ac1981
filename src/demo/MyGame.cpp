#include "MyGame.h"
#include "../engine/custom/MyEngineSystem.h"
#include "../engine/AudioEngine.h"
#include "../engine/XCube2d.h"

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), cube(0, 0 ,0) {
	TTF_Font * font = ResourceManager::loadFont("res/fonts/arial.ttf", 72);
	gfx->useFont(font);
	gfx->setVerticalSync(true);
	gfx->setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	Mix_Chunk* soundOne = ResourceManager::loadSound("res/audio/shoot.wav");
	Mix_Chunk* soundThree = ResourceManager::loadSound("res/audio/background_Music.wav");
	shootSound = new AudioElement3D(soundOne, Vector3f(-10, 0, 0), -1, 1000, 90, 0);
	backgroundSound = new AudioElement3D(soundThree, Vector3f(WINDOW_WIDTH / 2, 0, WINDOW_HEIGHT / 2), 3, 1000, 90, 0);

	//sfx->soundPanning(cube, shootSound->getSoundPosition3D(), 0, 1);
	Mix_PlayChannel(3, backgroundSound->getSound(), -1);
	//sfx->calculateBehindSound(shootSound->getSound(), cube, shootSound->getSoundPosition3D(), 45, 235, 90, 90);
	//sfx->calculateDistanceEffect(shootSound->getSound(), cube, shootSound->getSoundPosition());
	//sfx->playMP3(background_music, -1);

	// Extra added code
	backgroundSound->startFadingIn(5);
}

MyGame::~MyGame() {

}

void MyGame::handleKeyEvents() {
	int speed = 3;

	if (eventSystem->isPressed(Key::W)) {
		velocity.y = -speed;
	}

	if (eventSystem->isPressed(Key::S)) {
		velocity.y = speed;
	}

	if (eventSystem->isPressed(Key::A)) {
		velocity.x = -speed;
	}

	if (eventSystem->isPressed(Key::D)) {
		velocity.x = speed;
	}
	if (eventSystem->isPressed(Key::UP)) {
		velocity.z = speed;
	}
	if (eventSystem->isPressed(Key::DOWN)) {
		velocity.z = -speed;
	}
	if (eventSystem->isPressed(Key::SPACE)) {

	}
	if (eventSystem->isPressed(Key::LEFT)) {

	}
	if (eventSystem->isPressed(Key::ONE)) {
		sfx->soundPanning(cube, backgroundSound->getSoundPosition3D(), 0, 3);
	}
	if (eventSystem->isPressed(Key::TWO)) {
		sfx->calculateDistanceEffect3D(backgroundSound->getSound(), cube, backgroundSound->getSoundPosition3D(), backgroundSound->getMaxDistance(), 128);
	}
	if (eventSystem->isPressed(Key::THREE)) {
		sfx->calculateBehindSound(backgroundSound->getSound(), cube, backgroundSound->getSoundPosition3D(), 0, 
			backgroundSound->getSoundRotation(), 90, 128);
	}
	if (eventSystem->isPressed(Key::FOUR)) {
		backgroundSound->startFadingIn(2);
	}
	if (eventSystem->isPressed(Key::FIVE)) {
		backgroundSound->startFadingOut(2);
	}
	if (eventSystem->isPressed(Key::SIX)) {
		sfx->resetSound(backgroundSound->getSound(), 3);
	}
}

// Use cube.z so you can move forward and backwards, y movement not important for demo
void MyGame::update() {
	cube.x += velocity.x;

	if (cube.x <= 0) {
		cube.x = 0;
	}
	else if (cube.x >= WINDOW_WIDTH) {
		cube.x = WINDOW_WIDTH;
	}

	cube.y += velocity.y;

	cube.y += velocity.y;
	if (cube.y <= 0) {
		cube.y = 0;
	}
	else if (cube.y >= WINDOW_HEIGHT) {
		cube.y = WINDOW_HEIGHT;
	}

	cube.z += velocity.z;

	velocity.x = 0;
    velocity.y = 0;
	velocity.z = 0;

	// Audio Fading
	if (backgroundSound->getCurrentlyFadingOut()) {
		bool isFading = sfx->fadeOut(backgroundSound->getSound(), -1, backgroundSound->getFadeTimeStart(), backgroundSound->getFadeTimeEnd(), 128);
		backgroundSound->setCurrentlyFadingOut(isFading);
	}
	if (backgroundSound->getCurrentlyFadingIn()) {
		bool isFading = sfx->fadeIn(backgroundSound->getSound(), 3, backgroundSound->getFadeTimeStart(), backgroundSound->getFadeTimeEnd(), 128);
		backgroundSound->setCurrentlyFadingIn(isFading);
	}
}

void MyGame::render() {
	gfx->setDrawColor(SDL_COLOR_RED);
	gfx->drawRect(Rect(cube.x, cube.y, 30, 30));
}

void MyGame::renderUI() {
	gfx->setDrawColor(SDL_COLOR_AQUA);
	std::string xSound = std::to_string((int)backgroundSound->getSoundPosition3D().x);
	std::string zSound = std::to_string((int)backgroundSound->getSoundPosition3D().z);
	std::string rotSound = std::to_string((int)backgroundSound->getSoundRotation());
	std::string xCube = std::to_string((int)cube.x);
	std::string zCube = std::to_string((int)cube.z);
	gfx->drawText(xSound, 780 - xSound.length() * 50, 25);
	gfx->drawText(zSound, 780 - zSound.length() * 50, 100);
	gfx->drawText(rotSound, 780 - rotSound.length() * 50, 175);
	gfx->drawText(xCube, 780 - xCube.length() * 50, 250);
	gfx->drawText(zCube, 780 - zCube.length() * 50, 325);
}