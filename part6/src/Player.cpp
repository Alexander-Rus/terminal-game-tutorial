/*
 * Player.cpp
 *
 *  Created on: Dec 26, 2016
 *      Author: anrus
 */

#include <unistd.h>
#include <ncurses.h>

#include <cstdint>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "game.h"
#include "ObjectField.h"
#include "WindowManager.h"
#include "Player.h"


Player::Player() {
	reset();
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

void Player::move(int in_char, rect game_area, ObjectField& asteroids) {
	switch (in_char) {
	case KEY_UP:
	case 'w':
	case 'i':
		if (pos.y > game_area.top()) {
			speed++;
			pos.y -= 1;
		}
		break;
	case KEY_DOWN:
	case 's':
	case 'k':
		if (pos.y < game_area.bot() + 1 && speed >= 2) {
			speed--;
			pos.y += 1;
		}
		break;
	case KEY_LEFT:
	case 'a':
	case 'j':
		if (pos.x > game_area.left() + 1)
			pos.x -= 1;
		break;
	case KEY_RIGHT:
	case 'd':
	case 'l':
		if (pos.x < game_area.right() - 1)
			pos.x += 1;
		break;
	default:
		break;
	}

	if (in_char == ' ') {
		SpaceObject s(pos.x, pos.y - 1, '|', 0, -2);
		asteroids.addObject(s);
	}
}

void Player::checkCollision(ObjectField& asteroids) {
	bounds = { {pos.x - 1, pos.y}, {3, 2}};

	// collision detection
	for (size_t i = 0; i < asteroids.getData().size(); i++) {
		if (bounds.contains(asteroids.getData().at(i).getPos())) {
			asteroids.erase(i);
			energy -= 10;
		}
	}
	for (size_t i = 0; i < asteroids.getData().size(); i++) {
		if (asteroids.getData().at(i).type == '|') {
			 checkMissileCollision(asteroids.getData().at(i), i, asteroids);
		}
	}
}

void Player::checkMissileCollision(SpaceObject missle, size_t missle_i, ObjectField& asteroids) {
	rect bounds = { { missle.pos.x - 1, missle.pos.y }, { 3, 2 } };

	for (size_t i = 0; i < asteroids.getData().size(); i++) {
		if (i == missle_i)
			continue;
		if (bounds.contains(asteroids.getData().at(i).getPos())) {
			//if (asteroids.getData().at(i).type == 'V') {
			//	asteroids.getData().erase(asteroids.getData().begin() + i);
			//	score += 50;
			//}
			asteroids.getData().erase(asteroids.getData().begin() + missle_i);
		}
	}
}

bool Player::gameOver() {
	if (energy <= 0)
		return true;

	return false;
}

void Player::draw(int tick, WindowManager* wm, ObjectField& asteroids) {
	WINDOW* game_wnd = wm->getGameWnd();
	// player ship main body
	wattron(game_wnd, A_BOLD);
	mvwaddch(game_wnd, (int )pos.y, (int )pos.x, disp_char); // (y, x)
	wattroff(game_wnd, A_BOLD);

	// player ship accessories
	wattron(game_wnd, A_ALTCHARSET);
	//mvaddch(pos.y - 1, pos.x, ACS_UARROW);
	mvwaddch(game_wnd, (int )pos.y, (int )pos.x - 1, ACS_LARROW);
	mvwaddch(game_wnd, (int )pos.y, (int )pos.x + 1, ACS_RARROW);

	// animate engine flame :)
	if (tick / 5 % 3) { // 5 ms cycle, 50% duty
		wattron(game_wnd, COLOR_PAIR(tick % 2 ? 3 : 4));
		mvwaddch(game_wnd, (int )pos.y + 1, (int )pos.x, ACS_UARROW);
		wattroff(game_wnd, COLOR_PAIR(tick % 2 ? 3 : 4));
	}

	wattroff(game_wnd, A_ALTCHARSET);

	remainingDistance -= speed;

	if (tick % 100 == 0) {
		score += (speed / 4) + 1;
	}
#if 1
	if (tick % enemySpawnRate == 0) {
		rect bounds = wm->getGameArea();
		float xV, yV;
		int x, y;
		switch (rand() % 4) {
		//entering from the top
		case 0:
			x = rand() % bounds.right();
			y = bounds.top();
			xV = 0; //(rand() % 3) - 1;
			yV = 1;
			break;
		//entering from the left side
		case 1:
			x = bounds.left();
			y = rand() % bounds.bot();
			xV = 2;
			yV = 0; //(rand() % 3) - 1;
			break;
		//entering from the bottom
		case 2:
			x = rand() % bounds.right();
			y = bounds.bot();
			xV = 0; //(rand() % 3) - 1;
			yV = -1;
			break;
		//entering from the right side
		case 3:
			x = bounds.right();
			y = rand() % bounds.bot();
			xV = -2;
			yV = 0; //(rand() % 3) - 1;
			break;
		}
		SpaceObject s(x,y, 'V', xV, yV);
		asteroids.addObject(s);
	}
#endif
}

void Player::playerStatus(WINDOW* main_wnd, int tick) {
	// energy bar
	wmove(main_wnd, 20, 1);
	whline(main_wnd, ' ', 25); // health bar is 25 chars long
	wmove(main_wnd, 20, 1);
	int col_pair = 1;

	for (int i = 0; i < energy; i += 4) {
		if (i > 100)
			col_pair = 5; // blue
		else if (i > 50)
			col_pair = 2; // green
		else if (i > 25)
			col_pair = 3; // yellow
		else
			col_pair = 4; // red

		wattron(main_wnd, COLOR_PAIR(col_pair));
		wattron(main_wnd, A_BOLD);
		waddch(main_wnd, '/');
		wattroff(main_wnd, A_BOLD);
		wattroff(main_wnd, COLOR_PAIR(col_pair));
	}

	// draw static string to hold percentage
	mvwprintw(main_wnd, 21, 1, " - E N E R G Y -      //");

	// draw numeric percentage
	wattron(main_wnd, A_BOLD);
	if (energy <= 25) {
		wattron(main_wnd, COLOR_PAIR(4));
		if (tick % 100 < 50)
			mvwprintw(main_wnd, 21, 18, "%i%%", energy);
		wattroff(main_wnd, COLOR_PAIR(4));
	} else
		mvwprintw(main_wnd, 21, 18, "%i%%", energy);
	wattroff(main_wnd, A_BOLD);

	// show the speed
	mvwprintw(main_wnd, 20, 50, " SPEED %i   ", speed);

	// show the speed
	mvwprintw(main_wnd, 21, 50, " SCORE %i   ", score);

	// show the speed
	mvwprintw(main_wnd, 22, 50, " REMAINING DISTANCE %i    ",
			remainingDistance);
}

void Player::reset() {
	// initialize player
	disp_char = 'o';
	pos = {10, 15};
	bounds = { {pos.x - 1, pos.y}, {3, 2}}; // player is 3 wide, 2 tall
	moving = false;
	energy = 100;
	score = 0;
	remainingDistance = 50000;
	speed = 1;
	level = 1;
	enemySpawnRate = 1000;
}

int Player::getPlayerSpeed() {
	return speed;
}

bool Player::levelCompleted() {
	if (remainingDistance <= 0)
		return true;
	return false;
}

void Player::addToScore(int points) {
	score += points;
}
