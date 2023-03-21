//
// Created by samue on 17/03/2023.
//

#include "maze.h"
#include <vector>
#include <list>
#include <tuple>
#include <set>

maze::maze() {
    int size = 200;
    field = vector<vector<char>>(size, vector<char>(size, '?'));
    x = size / 2;
    y = size / 2;
    direction = 0;

    checkpoint_x = x;
    checkpoint_y = y;

    // set all the corners
    for (int i = 1; i < size; i+=2) {
        for (int j = 1; j < size; j+=2) {
            field[i][j] = '#';
        }
    }
}

maze::~maze() = default;

int maze::ix(int direction) {
    switch (direction) {
        case 1:
            return 1;
        case 3:
            return -1;
        default:
            return 0;
    }
}

int maze::iy(int direction) {
    switch (direction) {
        case 0:
            return -1;
        case 2:
            return 1;
        default:
            return 0;
    }
}

int maze::absolute_dir_to_relative(int abs) {
    return (abs - direction + 4) % 4;
}

int maze::relative_dir_to_absolute(int rel) {
    return (rel + direction) % 4;
}

void maze::set_wall(int direction, bool wall) {
    int abs = relative_dir_to_absolute(direction);
    int dx = ix(abs);
    int dy = iy(abs);
    field[y + dy][x + dx] = wall ? '#' : 'W';
}

vector<int> maze::get_moves() {
    // bfs to find the shortest path to the nearest victim
    list<tuple<int, int, vector<int>> bfs_queue;
    set<pair<int, int>> visited;

    bfs_queue.push_back(make_tuple(x, y, vector<int>()));
    visited.insert(make_pair(x, y));

    vector<int> finalPath;

    while(!bfs_queue.empty()) {
        auto [tmpX, tmpY, path] = bfs_queue.front();
        bfs_queue.pop_front();

        if (field[tmpY][tmpX] == '?') {
            finalPath = path;
            break;
        }

        for (int i = 0; i < 4; i++) {
            if (is_cell_visitable(tmpX, tmpY, i, 2) &&
                is_cell_visitable(tmpX, tmpY, i, 1) &&
                visited.find(make_pair(x + dx, y + dy)) == visited.end() // cell not visited
                ) {
                visited.insert(make_pair(x + dx, y + dy));
                vector<int> new_path = path;
                new_path.push_back(i);
                bfs_queue.push_back(make_tuple(x + dx, y + dy, new_path));
            }
        }
    }

    // adesso ho la lista di direzioni assolute da seguire
    // devo convertirle in relative

    vector<int> relativePath(finalPath.size());
    int tmpDir = direction;
    for (int path : finalPath) {
        tmpDir = (tmpDir + path) % 4;
        relativePath.push_back(tmpDir);
        if(tmpDir != 0)
            relativePath.push_back(0);
    }

    return relativePath;
}

char maze::get_cell(int direction, int steps) {
    int dx = ix(abs);
    int dy = iy(abs);
    return field[y + dy * steps][x + dx * steps];
}

bool maze::is_cell_visitable(int x, int y, int direction, int steps) {
    int dx = ix(direction);
    int dy = iy(direction);
    char cell = field[y + dy * steps][x + dx * steps];
    return cell == ' ' || cell == 'V' || cell == 'S' || cell == 'W';
}

void maze::set_black(int direction) {
    field[y yp(direction)*2][x ix(direction)*2] = 'B';
}

void maze::set_checkpoint() {
    checkpoint_x = x;
    checkpoint_y = y;
}

void maze::goto_checkpoint() {
    x = checkpoint_x;
    y = checkpoint_y;
    direction = 0;
}

void maze::goto_next_cell() {
    x += ix(direction)*2;
    y += iy(direction)*2;
    field[y][x] = 'W';
}

void maze::rotate(int direction) {
    this->direction = (this->direction + direction + 4) % 4;
}

String maze::to_string() {
    String s = "";
    for (int i = 0; i < field.size(); i++) {
        for (int j = 0; j < field[i].size(); j++) {
            s += field[i][j];
        }
        s += '\n';
    }
    return s;
}