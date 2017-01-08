/*
 * Player.h
 *
 *  Created on: Dec 26, 2016
 *      Author: anrus
 */

#ifndef SRC_PLAYER_H_
#define SRC_PLAYER_H_

class Player {
private:
    vec2i pos;
    vec2i dir;
    rect bounds;
    char disp_char;
    char ship_type;
    bool moving;
    int energy;
    int score;
    int remainingDistance;
    int speed;
    int level;
    int enemySpawnRate;
    void checkMissileCollision(SpaceObject missle, size_t missle_i, ObjectField& asteroids);

public:
	Player();
	virtual ~Player();

	void move(int in_char, rect game_area, ObjectField& asteroids);

	void checkCollision(ObjectField& asteroids);

	bool gameOver();

	void draw(int tick, WindowManager* wm, ObjectField& asteroids);

	void playerStatus(WINDOW* main_wnd, int tick);

	void reset();

	int getPlayerSpeed();

	bool levelCompleted();

	void addToScore(int);
};

#endif /* SRC_PLAYER_H_ */
