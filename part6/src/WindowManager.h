/*
 * WindowManager.h
 *
 *  Created on: Dec 26, 2016
 *      Author: anrus
 */

#ifndef SRC_WINDOWMANAGER_H_
#define SRC_WINDOWMANAGER_H_

class WindowManager {

private:
	WINDOW* main_wnd;
	WINDOW* game_wnd;

	rect game_area;
	rect screen_area;

	vec2ui cur_size;
	void applyColorscheme(short fg, short bg);
	void setFrame();
	void winResize(int &orig_width, int &orig_height);

public:
	WindowManager();
	virtual ~WindowManager();
	void clearGameWindow();
	int readKey();
	WINDOW* getGameWnd();
	WINDOW* getMainWnd();
	rect getGameArea();
	rect getScreenArea();
	void clearMainWnd();
	void refreshMainWnd();
	void refreshGameWnd();
	void refreshAllWnd();
	bool cutScene(std::vector<std::string> story_text, ObjectField stars);

};

#endif /* SRC_WINDOWMANAGER_H_ */
