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

inline std::function<int()> rng(int size) {
    std::default_random_engine eng(rnd());
    std::uniform_int_distribution<int> dist{ 0, size - 1 };

    return std::bind(dist, eng);
}

enum class Location{ Top, Right, Bottom, Left, Null};

class Wall;

class Cell {
public:
    std::vector<Cell*> neighbours;
    friend class Wall;  // TODO find out why protected inheritance not working
    unsigned i, j = -1;
	const static Cell NullCell;
public:
    virtual ~Cell() = default;

    virtual void add(Cell* neighbour, Location location = Location::Null, bool withWall = false);

    Wall* wallBetween(Cell* neighbour);

    bool hasWallBetween(Cell* neighbour) {
        return wallBetween(neighbour) != nullptr;
    }

	void remove(Wall* wall);

    std::vector<Cell*> getNeighbours();

	std::vector<Wall*> walls();

	Location location(const Wall* wal) const;

    friend std::ostream& operator<<(std::ostream& out, const Cell& cell) {
        out << "cell[" << cell.i << ", " << cell.j << "]";
        return out;
    }

};

class Wall : public Cell {
public:
    Cell* left;
    Cell* right;
	Location location;

    Wall(Cell* l, Cell* r, Location loc) :left(l), right(r), location(loc) {
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

	Cell* otherSideOf(Cell* cell) {
		if (cell == left) {
			return right;
		}
		else {
			return left;
		}
	}
};

void Cell::add(Cell* neighbour, Location location, bool withWall) {
    if (withWall) {
        Wall* wall = wallBetween(neighbour);
        if (wall) {
            neighbours.push_back(wall);
        }
        else {
            wall = new Wall(this, neighbour, location);
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

std::vector<Wall*> Cell::walls() {
	std::vector<Wall*> rtVal;
	for (Cell* neighbour : neighbours) {
		Wall* wall = dynamic_cast<Wall*>(neighbour);
		if (wall) {
			rtVal.push_back(wall);
		}
	}
	return rtVal;
}

Location Cell::location(const Wall* wal) const {
	return Location::Null;
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
                    current.add(&leftNeighbour, Location::Left withWall);
				}
				else {
					current.add(new Wall(nullptr, current, Location::Left));
				}
                k = j + 1;
                if (k < cols) {
                    Cell& rightNeighbour = grid[i][k];
                    current.add(&rightNeighbour, Location::Right, withWall);
				}
				else {
					current.add(new Wall(current, nullptr, Location::Right));
				}
                k = i + 1;
                if (k < rows) {
                    Cell& topNeighbour = grid[k][j];
                    current.add(&topNeighbour, Location::Top withWall);
				}
				else {
					current.add(new Wall(nullptr, current, Location::Top))
				}
                k = i - 1;
                if (k > -1) {
                    Cell& bottomNeighbour = grid[k][j];
                    current.add(&bottomNeighbour, Location::Bottom withWall);
				}
				else {
					current.add(new Wall(current, nullptr, Location::Bottom));
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