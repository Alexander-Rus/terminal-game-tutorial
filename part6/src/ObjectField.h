#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H

class SpaceObject {

public:
    SpaceObject(int, int);
    SpaceObject(int, int, char type, float x_velocity, float y_velocity);
    void update();
    vec2i getPos() const;
    char type;
    vec2i pos;

private:
    float x_velocity, y_velocity;

};



class ObjectField {

public:
    void update(char type);
    void erase(size_t);
    std::vector<SpaceObject> getData() const;

    void setBounds(rect);

    void draw(WINDOW* game_wnd, char drawChar, bool boldFlag);
    void resetAll();
    void addObject(SpaceObject);
    rect getBounds();

private:
    rect field_bounds;
    std::vector<SpaceObject> object_set;
    void checkMissileCollision(SpaceObject missle,  size_t i);

};

#endif
