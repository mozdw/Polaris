#include<bangtal.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <math.h>

SceneID gameScene;
SceneID startScene;
ObjectID startButton;
ObjectID helpButton;
ObjectID player;
ObjectID tile[100];
ObjectID meter[5];
ObjectID m;
ObjectID fadeImage;
ObjectID item;
ObjectID barrier;
ObjectID itemEff, boostEff;
ObjectID title;
TimerID tick;
TimerID fadeTimer;
TimerID start;
TimerID itemEffTimer;
TimerID boostEffTimer;
TimerID overTimer;
SoundID BGM;
SoundID jumpSound;
SoundID barrierSound;
SoundID boostSound;
SoundID buttonSound;
SoundID gameoverSound;

float tickDelay = 0.01f;

bool gameStarted = false;
bool isMoving = false;
bool isJumping = false;
bool isBarrier = false;
bool waitForOver = false;

float jumpForce = 0;


int dir = 1;
int charSprite = 0;


float cameraY = 0;
int playerX = 608;
int playerY = 250;

int itemX = 0;
int itemY = 0;

int itemEffX = 0;
int itemEffY = 0;

int tileX[50];
int tileY[50];
int tileMoving[50];
int tileSpeed[50];
int passedTile = 0;

int fadeNum = 0;
int nextScene = 1;

int itemEffNum = 0;
int boostEffNum = 15;

ObjectID createObject(
	const char* image,
	SceneID scene, int x, int y,
	bool shown = true)
{
	ObjectID object = createObject(image);
	locateObject(object, scene, x, y);
	if (shown) {
		showObject(object);
	}

	return object;
}

bool isOnBlock() {
	for (int index = 0; index < 50; index++)
	{
		if (playerX+32 >= tileX[index])
		{
			if (playerX + 32 <= tileX[index] + 96)
			{
				if (playerY >= tileY[index]+80) {
					if (playerY <= tileY[index] + 100) {
						return true;
					}
				}
			}
		}

		

	}
	return false;
}

bool isGetItem() {

	if (playerX + 64 >= itemX)
	{
		if (playerX <= itemX + 48)
		{
			if (playerY + 64 >= itemY) {
				if (playerY <= itemY + 48) {
					return true;
				}
			}
		}
	}

	return false;

}

void showMeter() {
	int meterNum = cameraY/ 45;
	if (meterNum >= 99999) {
		for (int index = 0; index < 5; index++) {
			setObjectImage(meter[index], "UI/num_9,png");
		}
	}
	else
	{
		int meterDig[5];
		for (int i = 0; i < 5; i++) {
			meterDig[i] = meterNum / (int)pow(10, 4 - i);
			meterNum %= (int)pow(10, 4 - i);
		}
		for (int index = 0; index < 5; index++) {

			std::string meterStr = "UI/num_" + std::to_string(meterDig[index]) + ".png";
			const char* meterFile = meterStr.c_str();
			setObjectImage(meter[index], meterFile);
		}

	}
	
	
}

void fadeScene() {

	if (fadeNum < 12) {
		std::string fadeStr = "BG/fade" + std::to_string(fadeNum) + ".png";
		const char* fadeFile = fadeStr.c_str();

		setObjectImage(fadeImage, fadeFile);
		setTimer(fadeTimer, 0.07f);
		startTimer(fadeTimer);

		fadeNum++;

		if (fadeNum == 8) {
			if (nextScene == 1)
			{
				locateObject(fadeImage, gameScene, 0, 0);
				enterScene(gameScene);
				
			}
			if (nextScene == 2)
			{
				locateObject(fadeImage, startScene, 0, 0);
				enterScene(startScene);

			}

		}

		if (fadeNum == 12) {

			if (nextScene == 2) {
				std::string scoreStr = "이번 기록은 " + std::to_string((int)cameraY / 45) + "m입니다.\n최고 기록에 도전해 보세요!";
				const char* scoreFile = scoreStr.c_str();
				showMessage(scoreFile);
				cameraY = 0;
			}

			stopTimer(fadeTimer);
			hideObject(fadeImage);
			fadeNum = 0;
		}
			

	}
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {

	if (object == helpButton) {
		playSound(buttonSound);
		showMessage("키보드로 조작하는 플랫폼 게임입니다.\n발판을 밟고 최대한 높이 올라가 보세요!");
	}

	if (object == startButton) {
		playSound(buttonSound);
		itemX = rand() % 4 * 250 + rand() % 100 + 150;
		itemY = rand() % 1000 + 2000;
		showObject(item);

		for (int index = 1; index < 50; index++)
		{
			std::string tileStr = "Tile/tile_" + std::to_string(rand() % 4) + ".png";
			const char* tileFile = tileStr.c_str();
			tileX[index] = rand() % 4 * 250 + rand() % 100 + 150;
			tileY[index] = index * 90 - index % 3 * 45 * (rand() % 4) + 300;
			if (rand() % 150 < index - 15) {
				if (rand() % 2 == 0) {
					tileMoving[index] = 1;
				}
				else {
					tileMoving[index] = -1;
				}
			}
			else {
				tileMoving[index] = 0;
			}

			tileSpeed[index] = rand() % 3 + 5;

			setObjectImage(tile[index], tileFile);
			locateObject(tile[index], gameScene, tileX[index] , tileY[index]);

		}

		tileX[0] = 592;
		tileY[0] = 150;
		std::string tileStr = "Tile/tile_" + std::to_string(rand() % 4) + ".png";
		const char* tileFile = tileStr.c_str();
		setObjectImage(tile[0], tileFile);
		locateObject(tile[0], gameScene, tileX[0], tileY[0]);

		locateObject(player, gameScene, 608, 250);


		


		
		nextScene = 1;
		showObject(fadeImage);
		fadeScene();
		showMeter();
		setTimer(start, 1.5f);
		startTimer(start);
	}
}



void keyboardCallback(KeyCode code, KeyState state) {

	if (gameStarted) {
		if (code == KeyCode::KEY_RIGHT_ARROW && state == KeyState::KEY_PRESSED) {
			isMoving = true;
			dir = 1;
		}
		else if (code == KeyCode::KEY_LEFT_ARROW && state == KeyState::KEY_PRESSED) {
			isMoving = true;
			dir = -1;
		}
		else if (code == KeyCode::KEY_RIGHT_ARROW && state == KeyState::KEY_RELEASED && dir == 1) {
			isMoving = false;
			setObjectImage(player, "Char/player_move_right_0.png");
		}
		else if (code == KeyCode::KEY_LEFT_ARROW && state == KeyState::KEY_RELEASED && dir == -1) {
			isMoving = false;
			setObjectImage(player, "Char/player_move_left_0.png");
		}
		else if (code == KeyCode::KEY_UP_ARROW && state == KeyState::KEY_PRESSED && isJumping == false) {
			playSound(jumpSound);
			isJumping = true;
			jumpForce = 18;
		}
	}
	
}

void timerCallback(TimerID timer) {

	if (timer == start) {
		playSound(BGM, true);
		gameStarted = true;
		setTimer(tick, tickDelay);
		startTimer(tick);
	}

	if (timer == tick) {

		if (isMoving == true) {
			if (playerX + 8 * dir>0)
			{
				if (playerX + 8 * dir < 1216)
				{
					playerX += 8 * dir;
				}
				else
				{
					playerX = 1216;
				}
			}
			else
			{
				playerX = 0;
			}
			if (dir == 1) {
				std::string charStr = "Char/player_move_right_" + std::to_string(charSprite/8) + ".png";
				const char* charFile = charStr.c_str();
				setObjectImage(player, charFile);
				charSprite++;
				if (charSprite == 23) {
					charSprite = 0;
				}
			}
			else if (dir == -1) {
				std::string charStr = "Char/player_move_left_" + std::to_string(charSprite / 8) + ".png";
				const char* charFile = charStr.c_str();
				setObjectImage(player, charFile);
				charSprite++;
				if (charSprite == 23) {
					charSprite = 0;
				}
			}

		}

		if (playerY - cameraY + jumpForce >= 650) {
			cameraY += playerY - cameraY + jumpForce - 650;
			playerY = cameraY + 650;
		}
		else
		{
			playerY += jumpForce;
		}


		

		if (isGetItem() == true) {
			itemEffX = itemX - 96;
			itemEffY = itemY - 96;
			locateObject(itemEff, gameScene, itemEffX, itemEffY - cameraY);
			showObject(itemEff);
			setTimer(itemEffTimer, 0.05f);
			startTimer(itemEffTimer);

			itemX = rand() % 4 * 250 + rand() % 100 + 150;
			itemY = rand() % 2000 + 5000 + cameraY;
			
			
			
			if (isBarrier == true) {
				locateObject(boostEff, gameScene, playerX - 160 + 32, playerY - cameraY - 220);
				showObject(boostEff);
				setTimer(boostEffTimer, 0.05f);
				startTimer(boostEffTimer);
				isJumping = true;
				jumpForce = 35;
				playSound(boostSound);
			}
			else {
				isBarrier = true;
				locateObject(barrier, gameScene, playerX - 16, playerY - 16 - cameraY);
				showObject(barrier);
				isJumping = true;
				jumpForce = 20;
				playSound(barrierSound);
			}
			
		}

		cameraY += 0.8 + 0.18 * sqrt(passedTile);

		locateObject(player, gameScene, playerX, playerY-cameraY);
		if (isBarrier == true) {
			locateObject(barrier, gameScene, playerX - 16, playerY - 16 - cameraY);
		}
		locateObject(item, gameScene, itemX , itemY - cameraY);
		locateObject(itemEff, gameScene, itemEffX, itemEffY - cameraY);
		locateObject(boostEff, gameScene, playerX - 160 + 32, playerY - cameraY - 220);

		for (int index = 0; index < 50; index++) {

			if (tileMoving[index] != 0) {
				if (tileX[index] + tileMoving[index] * tileSpeed[index] > 1184) {
					tileX[index] = 1184;
					tileMoving[index] *= -1;
				}
				else if (tileX[index] + tileMoving[index] * tileSpeed[index] < 0) {
					tileX[index] = 0;
					tileMoving[index] *= -1;

				}
				else {
					tileX[index] += tileMoving[index] * tileSpeed[index];
				}
				
			}

			if (tileY[index] - cameraY < -100) {
				passedTile++;
				std::string tileStr = "Tile/tile_" + std::to_string(rand() % 4) + ".png";
				const char* tileFile = tileStr.c_str();
				setObjectImage(tile[index], tileFile);
				tileX[index] = rand() % 4 * 250 + rand() % 100 + 150;
				tileY[index] = (passedTile + 50) * 90 - (passedTile + 50) % 3 * 45 * (rand() % 4);
				if (rand() % (80 + passedTile) < passedTile) {
					if (rand() % 2 == 0) {
						tileMoving[index] = 1;
					}
					else {
						tileMoving[index] = -1;
					}
					tileSpeed[index] = rand() % 3 + 5;
				}
				else {
					tileMoving[index] = 0;
				}
			}

			locateObject(tile[index], gameScene, tileX[index], tileY[index] - cameraY);

		}

		if (isOnBlock() == true)
		{
			if (jumpForce <= 0) {
				jumpForce = 0;
				isJumping = false;
			}

		}
		else
		{
			jumpForce -= 0.5;
		}

		setTimer(tick, tickDelay);
		startTimer(tick);


		if (playerY - cameraY <= -90)
		{
			if (isBarrier == true) {
				stopTimer(tick);
			}
			else {
				isMoving = false;
				isJumping = false;
				isBarrier = false;

				jumpForce = 0;


				dir = 1;
				charSprite = 0;

				playerX = 608;
				playerY = 250;

				passedTile = 0;
				stopTimer(tick);
				stopSound(BGM);
				playSound(gameoverSound);
				gameStarted = false;
			}

			setTimer(overTimer, 1.8f);
			startTimer(overTimer);
			

		}
			
		showMeter();
	}

	if (timer == overTimer) {

		if (isBarrier == true) {
			setTimer(tick, tickDelay);
			startTimer(tick);
			locateObject(boostEff, gameScene, playerX - 160 + 32, playerY - cameraY - 180);
			showObject(boostEff);
			setTimer(boostEffTimer, 0.03f);
			startTimer(boostEffTimer);
			isJumping = true;
			jumpForce = 35;
			hideObject(barrier);
			isBarrier = false;
			playSound(barrierSound);
			playSound(boostSound);
		}
		else {
			nextScene = 2;
			showObject(fadeImage);
			fadeScene();
		}

		
	}

	if (timer == fadeTimer) {
		fadeScene();
	}
	if (timer == itemEffTimer) {
		if (itemEffNum < 8) {
			std::string effStr = "Eff/getItem_" + std::to_string(itemEffNum) + ".png";
			const char* effFile = effStr.c_str();
			setObjectImage(itemEff, effFile);
			itemEffNum++;
			setTimer(itemEffTimer, 0.05f);
			startTimer(itemEffTimer);
		}
		else {
			itemEffNum = 0;
			hideObject(itemEff);
		}
		
	}
	if (timer == boostEffTimer) {
		if (boostEffNum < 26) {
			std::string effStr = "Eff/boost_" + std::to_string(boostEffNum) + ".png";
			const char* effFile = effStr.c_str();
			setObjectImage(boostEff, effFile);
			boostEffNum++;
			setTimer(boostEffTimer, 0.05f);
			startTimer(boostEffTimer);
		}
		else {
			boostEffNum = 15;
			hideObject(boostEff);
		}

	}
}



int main() {

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	gameScene = createScene("", "BG/BG_1.png");
	startScene = createScene("", "BG/BG_1.png");

	setMouseCallback(mouseCallback);
	setKeyboardCallback(keyboardCallback);
	setTimerCallback(timerCallback);

	srand(time(NULL));
	
	title = createObject("UI/Title.png", startScene, 290, 230, true);


	for (int index = 1; index < 50; index++)
	{
		std::string tileStr = "Tile/tile_" + std::to_string(rand() % 4) + ".png";
		const char* tileFile = tileStr.c_str();
		tileX[index] = rand() % 4 * 250 + rand() % 100 + 150;
		tileY[index] = index * 90 - index % 3 * 45 * (rand() % 4) ;
		if (rand() % 120 < index - 15) {
			if (rand() % 2 == 0) {
				tileMoving[index] = 1;
			}
			else {
				tileMoving[index] = -1;
			}
		}
		else {
			tileMoving[index] = 0;
		}

		tileSpeed[index] = rand() % 3 + 5;

		tile[index] = createObject(tileFile, gameScene, tileX[index], tileY[index], true);

	}
	
	tileX[0] = 592;
	tileY[0] = 150;
	std::string tileStr = "Tile/tile_" + std::to_string(rand() % 4) + ".png";
	const char* tileFile = tileStr.c_str();
	tile[0] = createObject(tileFile, gameScene, tileX[0], tileY[0], true);

	item = createObject("Item/light.png");
	scaleObject(item, 2.0f);
	locateObject(item, gameScene, 0, 1000);

	boostEff = createObject("Eff/boost_0.png");
	locateObject(boostEff, gameScene, 0, -500);

	player = createObject("Char/player_move_right_0.png", gameScene, 608, 250, true);
	scaleObject(player, 2.0f);
	startButton = createObject("UI/Start.png", startScene, 508, 150, true);
	helpButton = createObject("UI/Help.png", startScene, 508, 30, true);

	barrier = createObject("Eff/barrier.png");
	scaleObject(barrier, 0.5f);

	itemEff = createObject("Eff/getItem_0.png");

	m = createObject("UI/m.png", gameScene, 720, 650, true);
	for (int index = 0; index < 5; index++) {
		meter[index] = createObject("UI/num_0.png", gameScene, 520 + index * 40, 650, true);
	}


	fadeImage = createObject("BG/fade0.png", startScene, 0, 0, false);

	tick = createTimer(tickDelay);
	start = createTimer(3.0f);
	overTimer = createTimer(1.5f);
	fadeTimer = createTimer(0.07);
	itemEffTimer = createTimer(0.05);
	boostEffTimer = createTimer(0.05);
	

	BGM = createSound("BGM/demodasip.mp3");
	jumpSound = createSound("SE/jump.mp3");
	barrierSound = createSound("SE/barrier.mp3");
	boostSound = createSound("SE/boost.mp3");
	buttonSound = createSound("SE/button.ogg");
	gameoverSound = createSound("SE/gameover.mp3");

	
	startGame(startScene);


}