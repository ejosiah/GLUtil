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

static std::random_device rnd;

std::function<int()> rng(int size) {
    std::default_random_engine eng(rnd());
    std::uniform_int_distribution<int> dist{ 0, size - 1 };

    return std::bind(dist, eng);
}

class Wall;

class Cell {
public:
    std::vector<Cell*> neighbours;
    friend class Wall;  // TODO find out why protected inheritance not working
    unsigned i, j = -1;
public:
    virtual ~Cell() = default;

    virtual void add(Cell* neighbour, bool withWall = false);

    Wall* wallBetween(Cell* neighbour);

    bool hasWallBetween(Cell* neighbour) {
        return wallBetween(neighbour) != nullptr;
    }

    void remove(Wall* wall);

    std::vector<Cell*> getNeighbours();

    friend std::ostream& operator<<(std::ostream& out, const Cell& cell) {
        out << "cell[" << cell.i << ", " << cell.j << "]";
        return out;
    }

};

class Wall : public Cell {
public:
    Cell* left;
    Cell* right;

    Wall(Cell* l, Cell* r) :left(l), right(r) {
        i = -1;
        j = -1;
    }

    virtual ~Wall() {
        Wall* wall = this;
        left->remove(wall);
        right->remove(wall);

        left->add(right);
        right->add(left);


    }

};

void Cell::add(Cell* neighbour, bool withWall) {
    if (withWall) {
        Wall* wall = wallBetween(neighbour);
        if (wall) {
            neighbours.push_back(wall);
        }
        else {
            wall = new Wall(this, neighbour);
            neighbours.push_back(wall);
        }
    }
    else {
        neighbours.push_back(neighbour);
    }
}

Wall* Cell::wallBetween(Cell* neighbour) {
    auto itr = std::find_if(neighbour->neighbours.begin(), neighbour->neighbours.end(), [&](Cell* wallOrCell) {
        Wall* wall = dynamic_cast<Wall*>(wallOrCell);
        return wall && (wall->left == this || wall->right == this);
        });
    if (itr != neighbour->neighbours.end()) {
        return dynamic_cast<Wall*>((*itr));
    }
    return nullptr;
}


void Cell::remove(Wall* wall) {
    auto itr = std::find_if(neighbours.begin(), neighbours.end(), [&](Cell* cell) {
        return wall == cell;
        });
    if (itr != neighbours.end()) {
        neighbours.erase(itr);
    }
}

std::vector<Cell*> Cell::getNeighbours() {
    std::vector<Cell*> result;
    for (Cell* neighbour : neighbours) {
        Wall* wall = dynamic_cast<Wall*>(neighbour);
        if (wall) {
            if (wall->left == this) {
                result.push_back(wall->right);
            }
            else {
                result.push_back(wall->left);
            }
        }
        else {
            result.push_back(neighbour);
        }
    }
    return result;
}

template<size_t rows, size_t cols>
class Maze {
private:
    Cell grid[rows][cols];
    std::set<Cell*> unvisited;
    std::stack<Cell*> stack;

    void init() {
        bool withWall = true;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Cell& current = grid[i][j];
                current.i = i;
                current.j = j;
                unvisited.insert(&current);
                int k = j - 1;
                if (k > -1) {
                    Cell& leftNeighbour = grid[i][k];
                    current.add(&leftNeighbour, withWall);
                }
                k = j + 1;
                if (k < cols) {
                    Cell& rightNeighbour = grid[i][k];
                    current.add(&rightNeighbour, withWall);
                }
                k = i - 1;
                if (k > -1) {
                    Cell& topNeighbour = grid[k][j];
                    current.add(&topNeighbour, withWall);
                }
                k = i + 1;
                if (k < rows) {
                    Cell& bottomNeighbour = grid[k][j];
                    current.add(&bottomNeighbour, withWall);
                }
            }
        }
    }

public:
    Maze() {
        init();
    }
    void generate() {
        using namespace std;
        Cell* current = &grid[0][0];
        while (!unvisited.empty()) {
            unvisited.erase(current);
            auto neighbours = unvisitedNeighbours(current);
            if (!neighbours.empty()) {
                Cell* neighbour = pickRandom(neighbours);
                Wall* wall = current->wallBetween(neighbour);
                delete wall;
                stack.push(current);
                current = neighbour;
            }
            else if (!stack.empty()) {
                current = stack.top();
                stack.pop();
            }
        }
    }

    std::vector<Cell*> unvisitedNeighbours(Cell* cell) {
        std::vector<Cell*> neighbours = cell->getNeighbours();
        std::vector<Cell*> result;
        for (Cell* neighbour : neighbours) {
            if (unvisited.find(neighbour) != unvisited.end()) {
                result.push_back(neighbour);
            }
        }
        return result;
    }


    Cell* pickRandom(std::vector<Cell*> cells) {
        size_t n = cells.size();
        int i = rng(n)();
        return cells[i];
    }

    Cell* operator[](const unsigned i) {
        return grid[i];
    }

    std::tuple<unsigned, unsigned> location(const Cell* cell) const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (&grid[i][j] == cell) {
                    return std::make_tuple(i, j);
                }
            }
        }
        return std::make_tuple(-1, -1);
    }

};