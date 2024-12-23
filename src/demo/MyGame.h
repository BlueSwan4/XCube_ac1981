#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"
#include "../engine/custom/MyEngineSystem.h"
#include "../engine/AudioEngine.h"
#include "../engine/XCube2d.h"

struct GameKey {
	Point2 pos;
	bool isAlive;
};

class MyGame : public AbstractGame {
	private:
		//Rect box;
		Vector3f cube;

		Vector3i velocity;
		AudioElement3D* shootSound;
		AudioElement3D* jumpSound;
		AudioElement3D* backgroundSound;

		std::vector<std::shared_ptr<GameKey>> gameKeys;

		/* UI */
		const int WINDOW_WIDTH = 800;
		const int WINDOW_HEIGHT = 600;

		/* GAMEPLAY */
		int score, numKeys, lives;
		int playerRotation = 0;
		bool gameWon;

		void handleKeyEvents();
		void update();
		void render();
		void renderUI();
	public:
        MyGame();
		~MyGame();
};

#endif