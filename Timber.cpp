// Timber.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>



using namespace sf;

// function prototype
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch?
// left or right?
enum class side {LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];

int main() {

	// create a video mode object
	VideoMode vm(1920, 1080);
	
	// create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	// create a texture to hold a graphic on the GPU
	Texture textureBackground;

	// load the graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	// create a sprite
	Sprite spriteBackground;

	// attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	//prepare the bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);

	// is the be currently moving?
	bool beeActive = false;

	// how fast can the bee fly?
	float beeSpeed = 0.0f;

	// setup clouds 
	Texture  textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);

	// position the clouds at the left of the screen at different heights
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 150);
	spriteCloud3.setPosition(0, 300);

	// are clouds on the screen at this point?
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;

	// how fast are the clouds
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	// control time
	Clock clock;

	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// track whether the game is paused or running
	bool paused = true;

	// Draw some text
	int score = 0;

	Text messageText;
	Text scoreText;

	// we need to chose a font
	Font font;
	font.loadFromFile("fonts/Komikap_.ttf");

	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);

	// Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");

	// Make it big enough to be readable
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(50);

	// chose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// Position the text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	// prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	// set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);

		// set the sprite origin to dead center
		// we can then spin it around without changing its position
		branches[i].setOrigin(220, 20);
	}

	// prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer; 
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	// The player starts on the left
	side playerSide = side::LEFT;

	// prepare the gravestone
	Texture textureRip;
	textureRip.loadFromFile("graphics/rip.png");
	Sprite spriteRip;
	spriteRip.setTexture(textureRip);
	spriteRip.setPosition(600, 860);

	//prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	// useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// control the players input
	bool acceptInput = false;

	// Prepare the sounds
	// player chopping sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sounds/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	// player death sound
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sounds/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	// out of time sound
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sounds/out_of_time.wav");
	Sound oot;
	oot.setBuffer(ootBuffer);

	// Game Loop starts here
	while (window.isOpen()) {
		// handle the players input
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				// listen for key presses again
				acceptInput = true;

				// hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// reset the game time and score
			score = 0;
			timeRemaining = 6;

			// make all the branches disappear - starting in the second position
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			// make sure the gravestone is hidden
			spriteRip.setPosition(675, 2000);

			// move the player into position
			spritePlayer.setPosition(580, 720);
			spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);

			acceptInput = true;
		}

		// wrap the player controls to make sure we are accepting input
		if (acceptInput) {
			// first handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				// make sure the player is on the right side
				playerSide = side::RIGHT;
				score++;

				// add to the amount of time remaining
				timeRemaining += (2 / score) + 0.15f;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);

				// update the branches
				updateBranches(score);

				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				// play chop sound
				chop.play();
			}
			// handle pressing left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				// make sure the player is on the left side
				playerSide = side::LEFT;
				score++;

				// add to the amount of time remaining
				timeRemaining += (2 / score) + 0.15f;

				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);

				// update branches
				updateBranches(score);

				// set the log flying
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				// play chop sound
				chop.play();
			}
		}

		// update the scene
		if (!paused) {
			// measure time
			Time dt = clock.restart();

			// subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// resize the timebar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {
				// Pause the game
				paused = true;

				// play out of time sound
				oot.play();

				//Change the message shown to the player to indicate game over
				messageText.setString("Out of time!!\nPress Enter to play again!");

				// reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
			}

			// setup the bee
			if (!beeActive) {
				// how fast is the bee?
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				// how high is the bee?
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);

				// activate bee
				beeActive = true;
			}
			else {
				// bee is active so lets move it instead
				spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);

				// if the bee has gone off left side of screen reset it
				if (spriteBee.getPosition().x < -100) {
					beeActive = false;
				}
			}

			// manage the clouds
			// Cloud1
			if (!cloud1Active) {
				// how fast is the cloud
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else {
				// cloud is active so lets move it
				spriteCloud1.setPosition(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y);

				// has the cloud reached the right hand edge of the screen?
				if (spriteCloud1.getPosition().x > 1920) {
					// reset cloud with new height and speed
					cloud1Active = false;
				}
			}

			// Cloud2
			if (!cloud2Active) {
				// how fast is the cloud
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 150);
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else {
				// cloud is active so lets move it
				spriteCloud2.setPosition(spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y);

				// has the cloud reached the right hand edge of the screen?
				if (spriteCloud2.getPosition().x > 1920) {
					// reset cloud with new height and speed
					cloud2Active = false;
				}
			}

			// Cloud3
			if (!cloud3Active) {
				// how fast is the cloud
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);

				// how high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 150);
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else {
				// cloud is active so lets move it
				spriteCloud3.setPosition(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y);

				// has the cloud reached the right hand edge of the screen?
				if (spriteCloud3.getPosition().x > 1920) {
					// reset cloud with new height and speed
					cloud3Active = false;
				}
			}

			// update score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;

				if (branchPositions[i] == side::LEFT)
				{
					// move the sprite left 
					branches[i].setPosition(610, height);

					// flip the sprite around
					branches[i].setRotation(180);
				}
				else if(branchPositions[i] == side::RIGHT) {
					// move the sprite right
					branches[i].setPosition(1330, height);

					//set the sprite rotation to normal
					branches[i].setRotation(0);
				} 
				else {
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			// animate the flying log
			if (logActive) {
				spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()), spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

				// has the log reached the screen edge?
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
					// set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// has the player been squished by a branch?
			if (branchPositions[5] == playerSide) {
				// death
				paused = true;
				acceptInput = false;

				// draw the gravestone
				spriteRip.setPosition(525, 760);

				// hide the player
				spritePlayer.setPosition(2000, 660);

				// play death sound
				death.play();
				
				// change the text of the message
				messageText.setString("SQUISHED!! Press ENTER to try again");



				// center the message on the screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
			}
		}

		// draw the scene

		// clear everything from the last frame
		window.clear();

		// draw our game scene here
		window.draw(spriteBackground);

		// draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		// draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		// draw the tree
		window.draw(spriteTree);

		// draw the player
		window.draw(spritePlayer);

		// draw the axe
		window.draw(spriteAxe);

		// draw the flying log
		window.draw(spriteLog);

		// draw the gravestone
		window.draw(spriteRip);

		// draw the bee
		window.draw(spriteBee);

		// Draw the score
		window.draw(scoreText);

		// draw unpause message if paused
		// or the game over message
		if (paused) {
			window.draw(messageText);
		}

		// draw the time bar
		window.draw(timeBar);

		//show everything we just drew
		window.display();
	}

	return 0;
}

// function definition
void updateBranches(int seed) {
	// move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}

	// spawn a new branch at position 0
	// LEFT, RIGHT, or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}