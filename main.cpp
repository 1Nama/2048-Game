#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define SIZE 4
#define TILE_SIZE 100
#define WINDOW_SIZE (SIZE * TILE_SIZE)

int grid[SIZE][SIZE];

void initGrid() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = 0;

    srand(static_cast<unsigned>(time(nullptr)));
    int x = rand() % SIZE;
    int y = rand() % SIZE;
    grid[x][y] = (rand() % 2 + 1) * 2;
}

void drawGrid(sf::RenderWindow &window) {
    window.clear(sf::Color::Black);
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE - 5, TILE_SIZE - 5));
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            tile.setPosition(j * TILE_SIZE + 5, i * TILE_SIZE + 5);
            if (grid[i][j] == 0) {
                tile.setFillColor(sf::Color(200, 200, 200)); // Gray for empty tiles
            } else {
                tile.setFillColor(sf::Color(100, 100 + grid[i][j], 100)); // Color based on value
                sf::Font font;
                if (!font.loadFromFile("arial.ttf")) {
                    std::cerr << "Could not load font\n";
                    return;
                }
                sf::Text text(std::to_string(grid[i][j]), font, 30);
                text.setFillColor(sf::Color::White);
                text.setPosition(j * TILE_SIZE + 35, i * TILE_SIZE + 25);
                window.draw(tile);
                window.draw(text);
            }
        }
    }
    window.display();
}

bool canMove() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == 0) return true;
            if (i != 0 && grid[i][j] == grid[i - 1][j]) return true;
            if (i != SIZE - 1 && grid[i][j] == grid[i + 1][j]) return true;
            if (j != 0 && grid[i][j] == grid[i][j - 1]) return true;
            if (j != SIZE - 1 && grid[i][j] == grid[i][j + 1]) return true;
        }
    }
    return false;
}

void addRandomTile() {
    int emptyTiles[SIZE * SIZE][2], emptyCount = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == 0) {
                emptyTiles[emptyCount][0] = i;
                emptyTiles[emptyCount][1] = j;
                emptyCount++;
            }
        }
    }

    if (emptyCount > 0) {
        int randomTile = rand() % emptyCount;
        int x = emptyTiles[randomTile][0];
        int y = emptyTiles[randomTile][1];
        grid[x][y] = (rand() % 2 + 1) * 2;
    }
}

void rotateGrid() {
    int temp[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            temp[i][j] = grid[i][j];

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = temp[SIZE - j - 1][i];
}

bool slideLeft() {
    bool moved = false;
    for (int i = 0; i < SIZE; i++) {
        int target = 0;
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] != 0) {
                if (target != j) {
                    grid[i][target] = grid[i][j];
                    grid[i][j] = 0;
                    moved = true;
                }
                target++;
            }
        }
    }
    return moved;
}

bool mergeLeft() {
    bool moved = false;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 1; j++) {
            if (grid[i][j] != 0 && grid[i][j] == grid[i][j + 1]) {
                grid[i][j] *= 2;
                grid[i][j + 1] = 0;
                moved = true;
            }
        }
    }
    return moved;
}

bool moveLeft() {
    bool moved = slideLeft();
    moved |= mergeLeft();
    moved |= slideLeft();
    return moved;
}

void moveGrid(char direction) {
    bool moved = false;
    for (int i = 0; i < direction - 'A'; i++)
        rotateGrid();
    moved = moveLeft();
    for (int i = 0; i < (4 - (direction - 'A')) % 4; i++)
        rotateGrid();

    if (moved)
        addRandomTile();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "2048 Game");
    initGrid();
    drawGrid(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) moveGrid('D');
                else if (event.key.code == sf::Keyboard::D) moveGrid('C');
                else if (event.key.code == sf::Keyboard::S) moveGrid('B');
                else if (event.key.code == sf::Keyboard::A) moveGrid('A');
                drawGrid(window);
            }
        }

        if (!canMove()) {
            sf::Font font;
            if (!font.loadFromFile("arial.ttf")) {
                std::cerr << "Could not load font\n";
                return -1;
            }
            sf::Text gameOverText("Game Over!", font, 50);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(100, 200);
            window.clear();
            window.draw(gameOverText);
            window.display();
            sf::sleep(sf::seconds(2));
            window.close();
        }
    }

    return 0;
}
