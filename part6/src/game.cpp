#include <unistd.h>
#include <ncurses.h>

#include <cstdint>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

#include "game.h"
#include "ObjectField.h"
#include "WindowManager.h"
#include "Player.h"


void checkGameOver(Player& player1, bool& exit_requested);
int calcModuloVal(int baseVal, Player player1, int multiplier);
void checkLevelCompleted(Player& player1, bool& exit_requested);


ObjectField asteroids;
ObjectField stars;
WindowManager wm;

int init() {

	srand(time(0));

	return 0;
}

void run() {

	int tick = 0;
	Player player1;

	asteroids.setBounds(wm.getGameArea());
	stars.setBounds(wm.getGameArea());

	int in_char = 0;
	bool exit_requested = false;

	const std::vector<std::string> story_text = {
			"Just another Monday, and you're on your way to work...",
			"When suddenly...", "You realize you left the oven on!",
			"Take a shortcut through that asteroid field!",
			"Get back to the house before your planet explodes!" };
	exit_requested = wm.cutScene(story_text, stars);

	tick = 0;
	while (!exit_requested) {

		wm.clearGameWindow();

		// read in input key, if any (non-blocking as defined earlier)
		in_char = wm.readKey();

		switch (in_char) {
		case 'q':
			exit_requested = true;
			break;
		default:
			player1.move(in_char, wm.getGameArea(), asteroids);
			break;
		}

		if (tick % calcModuloVal(7, player1, 1) == 0)
			stars.update('.');

		if (tick > 100 && tick % calcModuloVal(20, player1, 2) == 0)
			asteroids.update('*');

		player1.checkCollision(asteroids);

		stars.draw(wm.getGameWnd(), '.', false);
		player1.draw(tick, &wm, asteroids);
		asteroids.draw(wm.getGameWnd(), '*', true);

		// draw UI elements
		player1.playerStatus(wm.getMainWnd(), tick);

		wm.refreshAllWnd();

		checkLevelCompleted(player1, exit_requested);

		checkGameOver(player1, exit_requested);

		tick++;

		usleep(10000); // 1 ms
	};

}

int calcModuloVal(int baseVal, Player player1, int multiplier) {
	int moduloVal = (baseVal - (multiplier * player1.getPlayerSpeed()) / 2);
	return std::max(moduloVal, 1);
}

void checkLevelCompleted(Player& player1, bool& exit_requested) {
	if (player1.levelCompleted()) {

		const std::vector<std::string> story_text = {
				"Level 1 Complete!",
				"Prepare for level 2" };
		exit_requested = wm.cutScene(story_text, stars);

		player1.reset();
		stars.resetAll();
		asteroids.resetAll();
	}
}

void checkGameOver(Player& player1, bool& exit_requested) {

	if (player1.gameOver()) {

		// store an approx location where text will be centered
		const int xpos = wm.getGameArea().width() / 2 - 6;
		const int ypos = wm.getGameArea().height() / 2 - 2;

		// erase current game content on window and redraw a clean window
		wm.clearMainWnd();
		wattron(wm.getMainWnd(), A_BOLD);
		box(wm.getMainWnd(), 0, 0);
		wattroff(wm.getMainWnd(), A_BOLD);

		wmove(wm.getMainWnd(), (int) wm.getGameArea().bot() + 3, 1);
		whline(wm.getMainWnd(), '-', (int) wm.getScreenArea().width() - 2);

		wm.refreshAllWnd();

		// print game over prompt
		mvwprintw(wm.getGameWnd(), ypos, xpos, "GAME OVER");
		mvwprintw(wm.getGameWnd(), ypos + 2, xpos - 7,
				"Press SPACE to play again");
		mvwprintw(wm.getGameWnd(), ypos + 4, xpos - 7,
				"Press 'q' to quit the game");

		// loop until player either quits or restarts game
		while (1) {
			int in_char = wgetch(wm.getMainWnd());

			if (in_char == ' ') { // reset all variables and restart game
				player1.reset();
				stars.resetAll();
				asteroids.resetAll();
				in_char = 0;
				exit_requested = false;
				break;
			}

			else if (in_char == 'q') {
				exit_requested = true;
				break;
			}

			wm.refreshGameWnd();

			usleep(10000); // 1 ms
		}
	}
}

