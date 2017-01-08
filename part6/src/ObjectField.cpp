#include <unistd.h>
#include <ncurses.h>
#include <cmath>

#include <cstdint>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

void log2(std::string val) {
	std::ofstream outfile;
	outfile.open("/home/anrus/log.txt", std::ios_base::app);

	outfile << val << std::endl;
	outfile.close();
}

#include "game.h"
#include "ObjectField.h"
#include "WindowManager.h"
#include "Player.h"

void ObjectField::update(char type) {
	// update existing objects
	//for (size_t i = object_set.size()-1; (i <= 0 && object_set.size() > 0);  i--) {
	for (size_t i = 0; i < object_set.size(); i++) {
		if (object_set.at(i).getPos().y > field_bounds.bot()
				|| object_set.at(i).getPos().y < field_bounds.top()
				|| object_set.at(i).getPos().x > field_bounds.right()
				|| object_set.at(i).getPos().x < field_bounds.left()) {
			//     	if (object_set.at(i).type == '*') {
			//   	    ostringstream os;
			// 	    os << "X: " << object_set.at(i).getPos().x << " Y: " << std::abs(object_set.at(i).getPos().y);
			//	    log2(os.str());
			//	}

			object_set.erase(object_set.begin() + i);
		}
		object_set.at(i).update();
	}

	// spawn a new object
	SpaceObject s(rand() % field_bounds.width(), 0, type, 0, 1);
	object_set.push_back(s);
}

rect ObjectField::getBounds() {
	return field_bounds;
}

void ObjectField::checkMissileCollision(SpaceObject missle, size_t missle_i) {
	rect bounds = { { missle.pos.x - 1, missle.pos.y }, { 3, 2 } };

	// collision detection
	ostringstream os;

	for (size_t i = 0; i < object_set.size(); i++) {
		if (i == missle_i)
			continue;
		if (bounds.contains(object_set.at(i).getPos())) {
			if (object_set.at(i).type == 'V') {
				object_set.erase(object_set.begin() + i);
			}
			object_set.erase(object_set.begin() + missle_i);
		}
	}
}

void ObjectField::erase(size_t i) {
	object_set.erase(object_set.begin() + i);
}

std::vector<SpaceObject> ObjectField::getData() const {
	return object_set;
}

void ObjectField::setBounds(rect a) {
	field_bounds = a;
}

SpaceObject::SpaceObject(int nx, int ny) {
	pos.x = nx;
	pos.y = ny;
	type = '*';
	x_velocity = 0;
	y_velocity = 1;
}

void SpaceObject::update() {
	pos.y += y_velocity;
	pos.x += x_velocity;
}

vec2i SpaceObject::getPos() const {
	return pos;
}

void ObjectField::draw(WINDOW* game_wnd, char drawChar, bool boldFlag) {
	for (auto o : getData()) {
		if (boldFlag)
			wattron(game_wnd, A_BOLD);
		mvwaddch(game_wnd, (int )o.getPos().y, (int )o.getPos().x, o.type);
		if (boldFlag)
			wattroff(game_wnd, A_BOLD);
	}
}

void ObjectField::resetAll() {
	object_set.clear();
}

SpaceObject::SpaceObject(int x, int y, char type, float x_velocity,
		float y_velocity) {
	pos.x = x;
	pos.y = y;
	this->type = type;
	this->x_velocity = x_velocity;
	this->y_velocity = y_velocity;
}

void ObjectField::addObject(SpaceObject spaceObject) {
	object_set.push_back(spaceObject);
}
