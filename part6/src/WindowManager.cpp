/*
 * WindowManager.cpp
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
#include "Player.h"
#include "WindowManager.h"

WindowManager::WindowManager() {

	initscr();

	cbreak();
	noecho();
	clear();
	refresh();

	curs_set(0);

	start_color();

	// read in window size
	cur_size = {0, 0};
	getmaxyx(main_wnd, cur_size.x, cur_size.y);

	// define area for screen (default terminal size)
	screen_area = { {0, 0}, {80, 24}};

	// set screen size accordingly
	wresize(main_wnd, (int) screen_area.height(), (int) screen_area.width());

	// initialize window areas
	int infopanel_height = 4;
	game_wnd = newwin((int) screen_area.height() - infopanel_height - 2,
			(int) screen_area.width() - 2, (int) screen_area.top() + 1,
			(int) screen_area.left() + 1);
	main_wnd = newwin((int) screen_area.height(), (int) screen_area.width(), 0,
			0);

	// define area for movement
	game_area = { {0, 0}, {screen_area.width() - 2, screen_area.height() - infopanel_height - 4}};

	applyColorscheme(COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);

	// enable function keys
	keypad(main_wnd, true);
	keypad(game_wnd, true);

	// disable input blocking
	nodelay(main_wnd, true);
	nodelay(game_wnd, true);

	// enable color
	if (!has_colors()) {
		endwin();
		printf("ERROR: Terminal does not support color.\n");
		exit(1);
	}

	// draw frame around whole screen
	wattron(main_wnd, A_BOLD);
	box(main_wnd, 0, 0);
	wattroff(main_wnd, A_BOLD);

	// draw dividing line between game and stats
	wmove(main_wnd, (int) game_area.bot() + 3, 1);
	whline(main_wnd, '-', (int) screen_area.width() - 2);

	// initial draw
	wrefresh(main_wnd);
	wrefresh(game_wnd);
}

WindowManager::~WindowManager() {
	delwin(main_wnd);
	delwin(game_wnd);

	endwin();
}

void WindowManager::clearGameWindow() {
	werase(game_wnd);
}

int WindowManager::readKey() {
	int in_char = wgetch(main_wnd);
	in_char = tolower(in_char);

	return in_char;
}

void WindowManager::applyColorscheme(short fg, short bg) {
	init_pair(1, fg, bg);
	wbkgd(main_wnd, COLOR_PAIR(1));
	wbkgd(game_wnd, COLOR_PAIR(1));
}

void WindowManager::setFrame() {
	// creates simple frame around window composed of vertical and horizontal lines
	attron(A_BOLD);
	box(main_wnd, 0, 0);
	attroff(A_BOLD);

	// border characters can be set manually using the border function
	// border( wnd, leftside, rightside, topside, bottom side, tlcorner,
	//                                      trcorner, blcorner, brcorner);
}

void WindowManager::winResize(int &orig_width, int &orig_height) {
	int new_width, new_height;

	getmaxyx(main_wnd, new_width, new_height);

	// if window dimensions have changed, update border
	if (new_width != orig_width || new_height != orig_height) {
		orig_width = new_width;
		orig_height = new_height;

		wresize(main_wnd, new_height, 0);
		mvwin(main_wnd, new_height, 0);

		wclear(main_wnd);
		setFrame();
	}

}

WINDOW* WindowManager::getGameWnd() {
	return game_wnd;
}

WINDOW* WindowManager::getMainWnd() {
	return main_wnd;
}

rect WindowManager::getGameArea() {
	return game_area;
}

rect WindowManager::getScreenArea() {
	return screen_area;
}

void WindowManager::clearMainWnd() {
	werase(main_wnd);
}

void WindowManager::refreshMainWnd() {
	wrefresh(main_wnd);
}

void WindowManager::refreshGameWnd() {
	wrefresh(game_wnd);
}

void WindowManager::refreshAllWnd() {
	wrefresh(main_wnd);
	wrefresh(game_wnd);
}
#if 1
bool WindowManager::cutScene(std::vector<std::string> story_text, ObjectField stars) {
	bool exit_requested = false;

	mvwprintw(main_wnd, 22, 57, "press SPACE to skip...");

	// story mode demo
	int tick = 1;
	size_t story_part = 0;
	size_t story_position = 0;

	while (1) {
		clearGameWindow();
		int in_char = wgetch(main_wnd);

		if (tick % 10 == 0)
			stars.update('.');

		stars.draw(game_wnd, '.', false);

		if (story_position < story_text[story_part].length()) {
			wattron(main_wnd, A_BOLD);
			mvwaddch(main_wnd, 20, 5 + story_position,
					story_text[story_part][story_position]);
			wattroff(main_wnd, A_BOLD);
			story_position++;
		}

		if (tick % 300 == 0) {
			story_part++;
			story_position = 0;
			mvwhline(main_wnd, 20, 1, ' ',
					(int)getScreenArea().width() - 2);
		}

		if (in_char == 'q') {
			exit_requested = true;
			break;
		}

		if (story_part >= story_text.size() || in_char == ' ')
			break;

		refreshGameWnd();

		tick++;
		usleep(10000); // 1 ms
	}

	// white-out
	mvwhline(main_wnd, 22, 57, ' ', 22);
	mvwhline(main_wnd, 20, 1, ' ', 60);

	return exit_requested;
}
#else


bool WindowManager::cutScene(std::vector<std::string> story_text, ObjectField stars) {
	bool exit_requested = false;

	mvwprintw(main_wnd, 22, 57, "press SPACE to skip...");

	// store an approx location where text will be centered
	//const int xpos = game_area.width() / 2 - 6;
	//const int ypos = game_area.height() / 2 - 2;

	// erase current game content on window and redraw a clean window
	clearMainWnd();
	wattron(main_wnd, A_BOLD);
	box(main_wnd, 0, 0);
	wattroff(main_wnd, A_BOLD);

	wmove(main_wnd, (int) game_area.bot() + 3, 1);
	whline(main_wnd, '-', (int) screen_area.width() - 2);

	refreshAllWnd();

	// story mode demo
	int tick = 1;
	size_t story_part = 0;
	size_t story_position = 0;

	while (1) {
		clearGameWindow();
		int in_char = wgetch(main_wnd);

		if (tick % 10 == 0)
			stars.update();

		//stars.draw(game_wnd, '.', false);

		if (story_position < story_text[story_part].length()) {
			wattron(game_wnd, A_BOLD);
			mvwaddch(game_wnd, 10, 5 + story_position,
					story_text[story_part][story_position]);
			wattroff(game_wnd, A_BOLD);
			story_position++;
		}

		if (tick % 300 == 0) {
			story_part++;
			story_position = 0;
			mvwhline(game_wnd, 10, 1, ' ',
					(int)getScreenArea().width() - 2);
		}

		if (in_char == 'q') {
			exit_requested = true;
			break;
		}

		if (story_part >= story_text.size() || in_char == ' ')
			break;

		refreshGameWnd();

		tick++;
		usleep(10000); // 1 ms
	}

	// white-out
	mvwhline(main_wnd, 22, 57, ' ', 22);
	mvwhline(game_wnd, 20, 1, ' ', 60);

	return exit_requested;
}
#endif
