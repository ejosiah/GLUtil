#pragma once

#include <vector>
#include <array>
#include <set>
#include <random>
#include <functional>
#include <tuple>
#include <utility>
#include <stack>
#include <iostream>
#include <array>
#include <functional>

static std::random_device rnd;

inline std::function<int()> rng(int size) {
    std::default_random_engine eng(rnd());
    std::uniform_int_distribution<int> dist{ 0, size - 1 };

    return std::bind(dist, eng);
}

enum class Location { Top, Right, Bottom, Left, Null };

inline Location flip(Location loc) {
    switch (loc) {
    case Location::Top:
        return Location::Bottom;
    case Location::Bottom:
        return Location::Top;
    case Location::Left:
        return Location::Right;
    case Location::Right:
        return Location::Left;
    }
}


struct Id {
    int row, col;
};

struct Wall;
struct Cell;

template<size_t rows, size_t cols>
struct Maze;

using Picker = std::function<Cell* (std::vector<Cell*>)>;

struct Cell {

    Id id;
    std::vector<Cell*> neighbours;
    std::list<Wall*> walls;

    Cell() = default;

    Cell(int i, int j) : id{ i, j } {}

    ~Cell() {
        auto x = id;
    }

    template<size_t rows, size_t cols>
    void addNeighbours(Maze<rows, cols>& maze);

    bool hasWallBetween(const Cell* neighbour) const {
        return wallBetween(neighbour) != nullptr;
    }

    Cell* neighbourAt(Location loc) {
        return nullptr;
    }

    Wall* wallBetween(const Cell* neighbour) const;

    Wall* wallAt(Location loc) const;

    bool operator==(const Cell& other) {
        return id.col == other.id.col && id.row == other.id.row;
    }

    Location locationOf(const Wall& wall) const;
};

inline bool operator<(const Cell& a, const Cell& b) {
    return a.id.col < b.id.col && a.id.row < b.id.row;
}

struct Wall {

    Wall() = default;

    Wall(Cell* left, Cell* right, Location loc) :
        left{ left }, right{ right }, location{ loc } {
        if (left) {
            left->walls.push_back(this);
        }
        if (right) {
            right->walls.push_back(this);
        }
    }

    Wall(Cell* cell, Location loc) :Wall(cell, nullptr, loc) {

    }

    ~Wall() {
        if (left) {
            left->walls.remove(this);
        }
        if (right) {
            right->walls.remove(this);
        }
    }

    bool borders(Cell* cell) {
        
        return (left == cell || right == cell) 
            || (*left == *cell || *right == *cell);
    }

    Cell* left;
    Cell* right;
    Location location;
};

inline bool operator==(const Wall& a, const Wall& b) {
    return ((a.left == b.left && a.right == b.right) 
            || (*(a.left) == *(b.left) && *(a.right) == *(b.right))) 
            && a.location == b.location;
}

template<size_t rows, size_t cols>
void Cell::addNeighbours(Maze<rows, cols>& maze) {
    int i = id.row;
    int j = id.col;
    int k = i + 1;
    if (k < rows) {
        Cell* topNeighbour = &maze.grid[k][j];
        if (!this->hasWallBetween(topNeighbour)) {
            neighbours.push_back(topNeighbour);
            topNeighbour->neighbours.push_back(this);
            new Wall(this, topNeighbour, Location::Top);
        }
    }
    else {
        new Wall(this, Location::Top);
    }

    k = j + 1;
    if (k < cols) {
        Cell* rightNeighbour = &maze.grid[i][k];
        if (!this->hasWallBetween(rightNeighbour)) {
            neighbours.push_back(rightNeighbour);
            rightNeighbour->neighbours.push_back(this);
            new Wall(this, rightNeighbour, Location::Right);
        }
    }
    else {
        new Wall(this, Location::Right);
    }

    k = i - 1;
    if (k > -1) {
        Cell* bottomNeighbour = &maze.grid[k][j];
        if (!this->hasWallBetween(bottomNeighbour)) {
            neighbours.push_back(bottomNeighbour);
            bottomNeighbour->neighbours.push_back(this);
            new Wall(this, bottomNeighbour, Location::Bottom);
        }
    }
    else {
        new Wall(this, Location::Bottom);
    }

    k = j - 1;
    if (k > -1) {
        Cell* leftNeighbour = &maze.grid[i][k];
        if (!this->hasWallBetween(leftNeighbour)) {
            neighbours.push_back(leftNeighbour);
            leftNeighbour->neighbours.push_back(this);
            new Wall(this, leftNeighbour, Location::Left);
        }
    }
    else {
        new Wall(this, Location::Left);
    }
}

Wall* Cell::wallBetween(const Cell* neighbour) const {
    if (this == neighbour) return nullptr;
    for (auto myWall : walls) {
        for (auto theirWall : neighbour->walls) {
            if (theirWall == myWall) return myWall;
        }
    }
    return nullptr;
}

Wall* Cell::wallAt(Location loc) const{
    auto itr = std::find_if(walls.begin(), walls.end(), [&](Wall* wall) {
        bool res =  (wall->location == loc && *(wall->left) == *this) || (flip(wall->location) == loc && *(wall->right) == *this);
        return res;
    });


    if (itr != walls.end()) {
        auto wall = *itr;
        return *itr;
    }
    return nullptr;
}

Location Cell::locationOf(const Wall& wall) const {
    if (wall.left == this) return wall.location;
    else return flip(wall.location);
}

template<size_t rows, size_t cols>
struct Maze {
    friend struct Cell;

    ~Maze() {
        // delete all wals
    }

    void init() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Cell* cell = &grid[i][j];
                cell->id = { i, j };
                cell->addNeighbours(*this);
            }
        }
    }

    const Cell* cellAt(const Id& id) const {
        assert(id.row >= 0 && id.row < rows);
        assert(id.col >= 0 && id.col < cols);
        return &grid[id.row][id.col];
    }

    const Cell* operator[](const Id& id) const {
        return cellAt(id);
    }

    std::set<Wall*> walls() {
        std::set<Wall*> rtVal;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Cell* cell = &grid[i][j];
                cell->id = { i, j };
                rtVal.insert(cell->walls.begin(), cell->walls.end());
            }
        }
    }

    Cell grid[rows][cols];
};



auto pickRandom = [](std::vector<Cell*> cells) {
    auto loc = rng(cells.size());
    return cells.at(loc());
};


class RecursiveBackTrackingMazeGenerator {
public:
    RecursiveBackTrackingMazeGenerator(Picker picker = pickRandom) :pick{ picker } {

    }

    virtual ~RecursiveBackTrackingMazeGenerator() = default;

    template<size_t rows, size_t cols>
    void generate(Maze<rows, cols>& maze) {
        maze.init();

        auto grid = maze.grid;
        visted.insert(&grid[0][0]);
        next.push(&grid[0][0]);

        while (!next.empty()) {
            auto current = next.top();
            next.pop();

            auto neighbours = unvisted(current->neighbours);
            if (!neighbours.empty()) {
                next.push(current);
                auto neighbour = pick(neighbours);
                auto wall = current->wallBetween(neighbour);
                delete wall;
                visted.insert(neighbour);
                next.push(neighbour);
            }
        }
   }

    std::vector<Cell*> unvisted(std::vector<Cell*> cells) {
        std::vector<Cell*> result{ cells.size() };
        auto itr = std::copy_if(cells.begin(), cells.end(), result.begin(), [&](Cell* cell) {
            return visted.find(cell) == visted.end();
        });

        result.resize(std::distance(result.begin(), itr));
        return result;
    }

private:
    std::set<Cell*> visted;
    std::stack<Cell*> next;
    Picker pick;
};