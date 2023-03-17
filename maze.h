//
// Created by samue on 17/03/2023.
//

#ifndef CASSIOPEA_MAZE_H
#define CASSIOPEA_MAZE_H

/*
 * This class represents a maze.
 * The maze is a 2D array of characters.
 * index:
 * 0: north/front
 * 1: east/right
 * 2: south/back
 * 3: west/left
 * The maze is a square with a size of 200x200.
 *
 * legend:
 * ' ': empty space
 * '#': wall
 * 'X': visited
 * 'S': start
 * 'V': victim
 * 'B': black tile
 * '?' : unknown
 * 'W': white tile
 *
 */

class maze {
public:
    maze();
    ~maze();
    void set_wall(int direction, bool wall);
    vector<int> get_moves();
private:
    vector<vector<char>> field;
    int x, y;
    int direction;

    static int ix(int direction);
    static int iy(int direction);
    static int absolute_dir_to_relative(int abs);
    static int relative_dir_to_absolute(int rel);

    char get_cell(int direction, int steps);
};


#endif //CASSIOPEA_MAZE_H
