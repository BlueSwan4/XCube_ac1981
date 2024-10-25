#include "MyGame.h"
#include "../engine/custom/MyEngineSystem.h"
#include "../engine/AudioEngine.h"
#include "../engine/XCube2d.h"

// Audio
// box(5, 5, 30, 30)

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), cube(0, 0 ,0) {
	TTF_Font * font = ResourceManager::loadFont("res/fonts/arial.ttf", 72);
	gfx->useFont(font);
	gfx->setVerticalSync(true);
	gfx->setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    for (int i = 0; i < numKeys; i++) {
        std::shared_ptr<GameKey> k = std::make_shared<GameKey>();
        k->isAlive = true;
        k->pos = Point2(getRandom(0, 750), getRandom(0, 550));
        gameKeys.push_back(k);
    }

	Mix_Chunk* soundOne = ResourceManager::loadSound("res/audio/shoot.wav");
	Mix_Chunk* soundTwo = ResourceManager::loadSound("res/audio/jump.wav");
	Mix_Chunk* soundThree = ResourceManager::loadSound("res/audio/background_Music.wav");
	Mix_Music* background_music = ResourceManager::loadMP3("res/audio/DDLoop1.wav");
	shootSound = new AudioElement(soundOne, Vector3f(0, 0, 2), -1);
	jumpSound = new AudioElement(soundTwo, Vector3f(0, 0, 0), -1);
	backgroundSound = new AudioElement(soundThree, Vector3f(0, 0, 0), 3);

	//sfx->calculateDistanceEffect(shootSound->getSound(), cube, shootSound->getSoundPosition());
	//sfx->playMP3(background_music, -1);

	// Extra added code
	/*
	if (shootSound.getSound() == nullptr) {
		std::cout << "Null Pointer";
	}
	else {
		sfx->playSoundPanning(shootSound, 5);
		//sfx->calculateDistanceEffect(shootSound);
	}
	*/
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
		backgroundSound->startFadingOut(5000);
	}
	if (eventSystem->isPressed(Key::LEFT)) {
		sfx->playSound(backgroundSound->getSound());
	}
}

float testFunction(float testFloat) {
	std::cout << "Working" << testFloat;
	return testFloat;
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

	for (auto key : gameKeys) {
		if (key->isAlive && Rect(cube.x, cube.y, 30, 30).contains(key->pos)) {
			score += 200;
			key->isAlive = false;
			numKeys--;
		}
	}


	velocity.x = 0;
    velocity.y = 0;
	velocity.z = 0;

	if (numKeys == 0) {
		gameWon = true;
	}

	// Audio Fading
	sfx->fadeOut(backgroundSound->getSound(), backgroundSound->getChannel(), backgroundSound->getFadeTimeStart(), backgroundSound->getFadeTimeEnd(), &testFunction);
}

void MyGame::render() {
	gfx->setDrawColor(SDL_COLOR_RED);
	gfx->drawRect(Rect(cube.x, cube.y, 30, 30));

	gfx->setDrawColor(SDL_COLOR_YELLOW);
	for (auto key : gameKeys)
        if (key->isAlive)
		    gfx->drawCircle(key->pos, 5);
}

void MyGame::renderUI() {
	gfx->setDrawColor(SDL_COLOR_AQUA);
	std::string scoreStr = std::to_string(score);
	gfx->drawText(scoreStr, 780 - scoreStr.length() * 50, 25);

	if (gameWon)
		gfx->drawText("YOU WON", 250, 500);
}