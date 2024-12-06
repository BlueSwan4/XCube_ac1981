#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), cube(0, 300 ,0) {
	TTF_Font * font = ResourceManager::loadFont("res/fonts/arial.ttf", 72);
	gfx->useFont(font);
	gfx->setVerticalSync(true);
	gfx->setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	Mix_Chunk* soundOne = ResourceManager::loadSound("res/audio/shoot.wav");
	Mix_Chunk* soundThree = ResourceManager::loadSound("res/audio/background_Music.wav");
	shootSound = new AudioElement3D(soundOne, Vector3f(-10, 0, 0), -1, 1000, 90, 0);
	backgroundSound = new AudioElement3D(soundThree, Vector3f(WINDOW_WIDTH / 2, 0, WINDOW_HEIGHT / 2), 3, 1000, 90, 0);
	playerRotation = 0;

	Mix_PlayChannel(3, backgroundSound->getSound(), -1);
}

MyGame::~MyGame() {

}

void MyGame::handleKeyEvents() {
	int speed = 3;

	if (eventSystem->isPressed(Key::W)) {
		//velocity.y = -speed;
		playerRotation += 1;
		if (playerRotation >= 360) {
			playerRotation = 0;
		}
	}

	if (eventSystem->isPressed(Key::S)) {
		//velocity.y = speed;
		playerRotation -= 1;
		if (playerRotation < 0) {
			playerRotation = 259;
		}
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
	if (eventSystem->isPressed(Key::LEFT)) {
		sfx->muteSound(backgroundSound->getSound());
	}
	if (eventSystem->isPressed(Key::ONE)) {
		sfx->soundPanning(cube, backgroundSound->getSoundPosition3D(), playerRotation, 3);
	}
	if (eventSystem->isPressed(Key::TWO)) {
		sfx->calculateDistanceEffect3D(backgroundSound->getSound(), cube, backgroundSound->getSoundPosition3D(), backgroundSound->getMaxDistance(), 128);
	}
	if (eventSystem->isPressed(Key::THREE)) {
		sfx->calculateBehindSound(backgroundSound->getSound(), cube, backgroundSound->getSoundPosition3D(), playerRotation, 
			backgroundSound->getSoundRotation(), 90, 128);
	}
	if (eventSystem->isPressed(Key::FOUR)) {
		backgroundSound->startFadingIn(5);
	}
	if (eventSystem->isPressed(Key::FIVE)) {
		backgroundSound->startFadingOut(5);
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
	std::string xSound = "X Sound " + std::to_string((int)backgroundSound->getSoundPosition3D().x);
	std::string zSound = "Z Sound " + std::to_string((int)backgroundSound->getSoundPosition3D().z);
	std::string rotSound = "Rot Sound " + std::to_string((int)backgroundSound->getSoundRotation());
	std::string xCube = "X Cube " + std::to_string((int)cube.x);
	std::string zCube = "Z Cube " + std::to_string((int)cube.z);
	std::string rotCube = "Rot Cube " + std::to_string((int)playerRotation);
	gfx->drawText(xSound, WINDOW_WIDTH - xSound.length() * 40, 25);
	gfx->drawText(zSound, WINDOW_WIDTH - zSound.length() * 40, 100);
	gfx->drawText(rotSound, WINDOW_WIDTH - rotSound.length() * 40, 175);
	gfx->drawText(xCube, WINDOW_WIDTH - xCube.length() * 40, 250);
	gfx->drawText(zCube, WINDOW_WIDTH - zCube.length() * 40, 325);
	gfx->drawText(rotCube, WINDOW_WIDTH - rotCube.length() * 40, 400);
	if (eventSystem->isPressed(Key::SPACE)) {
		std::string easterEgg = "Violet_Cleak-CI517-2024";
		gfx->drawText(easterEgg, WINDOW_WIDTH - easterEgg.length() * 35, 475);
	}
}