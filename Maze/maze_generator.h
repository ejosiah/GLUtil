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
public:
    virtual ~Cell() = default;

    virtual void add(Cell* neighbour, Location location = Location::Null, bool withWall = false);

    virtual Wall* wallBetween(Cell* neighbour);

    virtual bool hasWallBetween(Cell* neighbour) {
        return wallBetween(neighbour) != nullptr;
    }

    virtual void remove(Wall* wall);

    virtual std::set<Cell*> getNeighbours();

    virtual std::set<Wall*> walls();

    virtual Location location(const Wall* wal) const;

    friend std::ostream& operator<<(std::ostream& out, const Cell& cell) {
        out << "cell[" << cell.i << ", " << cell.j << "]";
        return out;
    }
};

class NullCell : public Cell {
public:
    NullCell() {
        i = j = -2;
    };

    virtual ~NullCell() = default;

    void add(Cell* neighbour, Location location, bool withWall) override {

    }

    Wall* wallBetween(Cell* neighbour) override {
        return nullptr;
    }

    bool hasWallBetween(Cell* neighbour) override {
        return false;
    }

    void remove(Wall* wall) override {

    }

    std::set<Cell*> getNeighbours() override {
        return {};
    }

    std::set<Wall*> walls() override {
        return {};
    }

    Location location(const Wall* wal) const override {
        return Location::Null;
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
        left->neighbours.push_back(this);
        right->neighbours.push_back(this);
    }

    virtual ~Wall() {
        left->remove(this);
        right->remove(this);

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
        if (!hasWallBetween(neighbour)) {
            new Wall(this, neighbour, location);
        }
    }
    else {
        neighbours.push_back(neighbour);
    }
}

//Wall* Cell::wallBetween(Cell* neighbour) {
//    auto itr = std::find_if(neighbour->neighbours.begin(), neighbour->neighbours.end(), [&](Cell* wallOrCell) {
//        Wall* wall = dynamic_cast<Wall*>(wallOrCell);
//        return wall && (wall->left == this || wall->right == this);
//    });
//    if (itr != neighbour->neighbours.end()) {
//        return dynamic_cast<Wall*>((*itr));
//    }
//    return nullptr;
//}

Wall* Cell::wallBetween(Cell* neighbour) {
    auto itr = std::find_if(neighbours.begin(), neighbours.end(), [&](Cell* cell) {
        return cell == neighbour;
    });
    if (itr != neighbours.end()) {
        return nullptr;
    }
    auto walls = this->walls();
    auto itr2 = std::find_if(walls.begin(), walls.end(), [&](Wall* wall) {
        return wall->left == neighbour || wall->right == neighbour;
    });
    if (itr2 != walls.end()) {
        return *itr2;
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

std::set<Cell*> Cell::getNeighbours() {
    std::set<Cell*> result;
    for (Cell* neighbour : neighbours) {
        Wall* wall = dynamic_cast<Wall*>(neighbour);
        if (wall) {
            if (wall->left == this) {
                result.insert(wall->right);
            }
            else if(wall->right == this){
                result.insert(wall->left);
            }
        }
        else{
            if (!dynamic_cast<NullCell*>(neighbour)) {
                result.insert(neighbour);
            }
        }
    }
    return result;
}

std::set<Wall*> Cell::walls() {
	std::set<Wall*> rtVal;
	for (Cell* neighbour : neighbours) {
		Wall* wall = dynamic_cast<Wall*>(neighbour);
		if (wall) {
			rtVal.insert(wall);
		}
	}
	return rtVal;
}

Location Cell::location(const Wall* wal) const {
	return Location::Null;
}


template<size_t rows, size_t cols>
class Maze {
public:
    Cell grid[rows][cols];
    std::set<Cell*> unvisited;
    std::stack<Cell*> stack;
    NullCell* nullCell = new NullCell;
    std::stringstream ss;

    void init() {
        bool withWall = true;
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Cell* current = &grid[i][j];
                Cell* cNull = nullCell;
                current->i = i;
                current->j = j;
                unvisited.insert(current);
                int k = j - 1;
                if (k > -1) {
                    Cell* leftNeighbour = &grid[i][k];
                    current->add(leftNeighbour, Location::Left, withWall);
				}
				else {
					current->add(new Wall(cNull, current, Location::Left));
				}
                k = j + 1;
                if (k < cols) {
                    Cell* rightNeighbour = &grid[i][k];
                    current->add(rightNeighbour, Location::Right, withWall);
				}
				else {
					current->add(new Wall(current, cNull, Location::Right));
				}
                k = i + 1;
                if (k < rows) {
                    Cell* topNeighbour = &grid[k][j];
                    current->add(topNeighbour, Location::Top, withWall);
				}
				else {
                    current->add(new Wall(cNull, current, Location::Top));
				}
                k = i - 1;
                if (k > -1) {
                    Cell* bottomNeighbour = &grid[k][j];
                    current->add(bottomNeighbour, Location::Bottom, withWall);
				}
				else {
					current->add(new Wall(current, cNull, Location::Bottom));
				}
                ss << *current << " ";
            }
            ss << "\n";
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
            auto neighbours = unvisitedNeighbours(current); // remove duplicate neighbours
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
        std::set<Cell*> neighbours = cell->getNeighbours();
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