#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

const int TILE_SIZE = 16;
const int MAP_WIDTH = 19;
const int MAP_HEIGHT = 21;

enum class GameState { MENU, PLAYING, GAME_OVER };

// ==========================================
// UTILIDAD: TEXTO PÍXEL
// ==========================================
class PixelText {
public:
    static void drawText(sf::RenderWindow& window, const std::string& text, float startX, float startY, float pixelSize = 2.0f, sf::Color color = sf::Color::White) {
        float currentX = startX;
        sf::RectangleShape pixel(sf::Vector2f(pixelSize, pixelSize));
        pixel.setFillColor(color);

        for (char c : text) {
            std::vector<std::string> matrix;
            switch (toupper(c)) {
                case 'I': matrix = {"111", " 1 ", " 1 ", " 1 ", "111"}; break;
                case 'N': matrix = {"1 1", "111", "111", "1 1", "1 1"}; break;
                case 'C': matrix = {"111", "1  ", "1  ", "1  ", "111"}; break;
                case 'A': matrix = {"111", "1 1", "111", "1 1", "1 1"}; break;
                case 'R': matrix = {"11 ", "1 1", "11 ", "1 1", "1 1"}; break;
                case 'J': matrix = {" 11", "  1", "  1", "1 1", "111"}; break;
                case 'U': matrix = {"1 1", "1 1", "1 1", "1 1", "111"}; break;
                case 'E': matrix = {"111", "1  ", "11 ", "1  ", "111"}; break;
                case 'G': matrix = {"111", "1  ", "1 1", "1 1", "111"}; break;
                case 'O': matrix = {"111", "1 1", "1 1", "1 1", "111"}; break;
                case 'V': matrix = {"1 1", "1 1", "1 1", "1 1", " 1 "}; break;
                case 'L': matrix = {"1  ", "1  ", "1  ", "1  ", "111"}; break;
                case 'P': matrix = {"111", "1 1", "111", "1  ", "1  "}; break;
                case 'M': matrix = {"1 1", "111", "111", "1 1", "1 1"}; break;
                case 'S': matrix = {"111", "1  ", "111", "  1", "111"}; break;
                case ' ': matrix = {"   ", "   ", "   ", "   ", "   "}; break;
                default:  matrix = {"111", "111", "111", "111", "111"}; break;
            }

            for (size_t r = 0; r < matrix.size(); ++r) {
                for (size_t col = 0; col < matrix[r].size(); ++col) {
                    if (matrix[r][col] == '1') {
                        pixel.setPosition(currentX + col * pixelSize, startY + r * pixelSize);
                        window.draw(pixel);
                    }
                }
            }
            currentX += (matrix[0].size() + 1) * pixelSize;
        }
    }
};

// ==========================================
// CLASE MAPA CON TÚNEL LATERAL
// ==========================================
class Map {
private:
    int grid[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
        {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1},
        {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
        {1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,1,1,0,1,1,0,1,0,1,1,1,1},
        {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0}, // Fila 10: Pasillo con teletransporte
        {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
        {1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1},
        {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    sf::RectangleShape tileShape;

public:
    Map() {
        tileShape.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    }

    bool isWalkable(int x, int y) const {
        // Permitir salir por los extremos en el pasillo de teletransporte
        if (y == 10 && (x < 0 || x >= MAP_WIDTH)) {
            return true;
        }
        if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
            return grid[y][x] == 0;
        }
        return false;
    }

    void render(sf::RenderWindow& window) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                if (grid[y][x] == 1) {
                    tileShape.setFillColor(sf::Color::Blue);
                } else {
                    tileShape.setFillColor(sf::Color::Black);
                }
                tileShape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(tileShape);
            }
        }
    }
};

// ==========================================
// CLASE ALGORITMO A*
// ==========================================
class AStar {
private:
    struct Node {
        int x, y;
        float g, h;
        Node* parent;
        Node(int x, int y, float g = 0, float h = 0, Node* parent = nullptr)
            : x(x), y(y), g(g), h(h), parent(parent) {}
        float f() const { return g + h; }
    };

    struct CompareNode {
        bool operator()(const Node* a, const Node* b) const {
            return a->f() > b->f();
        }
    };

    static float heuristic(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

public:
    static std::vector<sf::Vector2i> findPath(sf::Vector2i start, sf::Vector2i target, const Map& map) {
        std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;
        std::vector<std::vector<bool>> closedSet(MAP_HEIGHT, std::vector<bool>(MAP_WIDTH, false));

        Node* startNode = new Node(start.x, start.y, 0, heuristic(start.x, start.y, target.x, target.y));
        openSet.push(startNode);

        std::vector<sf::Vector2i> path;
        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};

        while (!openSet.empty()) {
            Node* current = openSet.top();
            openSet.pop();

            if (current->x == target.x && current->y == target.y) {
                Node* temp = current;
                while (temp) {
                    path.push_back(sf::Vector2i(temp->x, temp->y));
                    temp = temp->parent;
                }
                std::reverse(path.begin(), path.end());
                break;
            }

            if (current->x >= 0 && current->x < MAP_WIDTH && current->y >= 0 && current->y < MAP_HEIGHT) {
                closedSet[current->y][current->x] = true;
            }

            for (int i = 0; i < 4; ++i) {
                int nx = current->x + dx[i];
                int ny = current->y + dy[i];

                if (map.isWalkable(nx, ny)) {
                    int validNx = (nx + MAP_WIDTH) % MAP_WIDTH;
                    int validNy = std::max(0, std::min(ny, MAP_HEIGHT - 1));

                    if (!closedSet[validNy][validNx]) {
                        float newG = current->g + 1;
                        float newH = heuristic(validNx, validNy, target.x, target.y);
                        openSet.push(new Node(validNx, validNy, newG, newH, current));
                    }
                }
            }
        }
        return path;
    }
};

// ==========================================
// CLASE BASE PERSONAJE
// ==========================================
class Character {
protected:
    sf::Vector2i gridPos;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Character(int startX, int startY, const std::string& texturePath) : gridPos(startX, startY) {
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Error al cargar " << texturePath << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setScale(
            static_cast<float>(TILE_SIZE) / texture.getSize().x,
            static_cast<float>(TILE_SIZE) / texture.getSize().y
        );
    }

    virtual ~Character() = default;

    sf::Vector2i getGridPos() const { return gridPos; }
    void setGridPos(int x, int y) { gridPos = sf::Vector2i(x, y); }

    virtual void render(sf::RenderWindow& window) {
        sprite.setPosition(gridPos.x * TILE_SIZE, gridPos.y * TILE_SIZE);
        window.draw(sprite);
    }
};

// ==========================================
// CLASE PACMAN
// ==========================================
class Pacman : public Character {
private:
    sf::Clock moveClock;
    float moveInterval = 0.10f;

public:
    Pacman(int startX, int startY) 
        : Character(startX, startY, "images/pacman.png") {}

    void handleInput(sf::Keyboard::Key key, const Map& map) {
        if (moveClock.getElapsedTime().asSeconds() >= moveInterval) {
            sf::Vector2i nextPos = gridPos;
            if (key == sf::Keyboard::Up) nextPos.y--;
            if (key == sf::Keyboard::Down) nextPos.y++;
            if (key == sf::Keyboard::Left) nextPos.x--;
            if (key == sf::Keyboard::Right) nextPos.x++;

            if (map.isWalkable(nextPos.x, nextPos.y)) {
                // Teletransportación lateral (Wrap-around)
                if (nextPos.x < 0) {
                    nextPos.x = MAP_WIDTH - 1;
                } else if (nextPos.x >= MAP_WIDTH) {
                    nextPos.x = 0;
                }

                gridPos = nextPos;
                moveClock.restart();
            }
        }
    }
};

// ==========================================
// CLASE FANTASMA
// ==========================================
class Ghost : public Character {
private:
    sf::Clock moveClock;
    float moveInterval = 0.45f;

public:
    Ghost(int startX, int startY) 
        : Character(startX, startY, "images/ghost.png") {}

    void update(sf::Vector2i targetPos, const Map& map) {
        if (moveClock.getElapsedTime().asSeconds() >= moveInterval) {
            std::vector<sf::Vector2i> path = AStar::findPath(gridPos, targetPos, map);
            if (path.size() > 1) {
                gridPos = path[1];
            }
            moveClock.restart();
        }
    }
};

// ==========================================
// BOTÓN INTERACTIVO
// ==========================================
class Button {
private:
    sf::RectangleShape shape;
    std::string text;
    sf::Color defaultColor;
    sf::Color hoverColor;

public:
    Button(float x, float y, float width, float height, const std::string& label, sf::Color baseColor)
        : text(label), defaultColor(baseColor) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x - width / 2.0f, y - height / 2.0f);
        shape.setFillColor(baseColor);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::White);

        hoverColor = sf::Color(
            std::min(baseColor.r + 60, 255),
            std::min(baseColor.g + 60, 255),
            std::min(baseColor.b + 60, 255)
        );
    }

    void updateHover(const sf::Vector2f& mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) {
            shape.setFillColor(hoverColor);
        } else {
            shape.setFillColor(defaultColor);
        }
    }

    bool isClicked(const sf::Vector2f& mousePos) const {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void render(sf::RenderWindow& window) {
        window.draw(shape);
        sf::FloatRect bounds = shape.getGlobalBounds();
        float textWidth = text.length() * 4 * 2.0f;
        float textX = bounds.left + (bounds.width - textWidth) / 2.0f;
        float textY = bounds.top + (bounds.height - 10.0f) / 2.0f;
        PixelText::drawText(window, text, textX, textY, 2.0f, sf::Color::White);
    }
};

// ==========================================
// CLASE MENÚ PRINCIPAL
// ==========================================
class Menu {
private:
    Button startButton;

public:
    Menu() : startButton((MAP_WIDTH * TILE_SIZE) / 2.0f, 180, 160, 40, "INICIAR JUEGO", sf::Color(0, 150, 0)) {}

    void handleMouseMove(const sf::Vector2f& mousePos) {
        startButton.updateHover(mousePos);
    }

    bool handleMouseClick(const sf::Vector2f& mousePos) {
        return startButton.isClicked(mousePos);
    }

    void render(sf::RenderWindow& window) {
        PixelText::drawText(window, "PACMAN A*", (MAP_WIDTH * TILE_SIZE) / 2.0f - 54, 80, 3.0f, sf::Color::Yellow);
        startButton.render(window);
    }
};

// ==========================================
// CLASE MENÚ GAME OVER
// ==========================================
class GameOverMenu {
private:
    Button restartButton;
    Button exitButton;

public:
    GameOverMenu() 
        : restartButton((MAP_WIDTH * TILE_SIZE) / 2.0f, 160, 170, 40, "VOLVER A JUGAR", sf::Color(0, 100, 200)),
          exitButton((MAP_WIDTH * TILE_SIZE) / 2.0f, 220, 170, 40, "CERRAR PROGRAMA", sf::Color(200, 0, 0)) {}

    void handleMouseMove(const sf::Vector2f& mousePos) {
        restartButton.updateHover(mousePos);
        exitButton.updateHover(mousePos);
    }

    int handleMouseClick(const sf::Vector2f& mousePos) {
        if (restartButton.isClicked(mousePos)) return 1;
        if (exitButton.isClicked(mousePos)) return 2;
        return 0;
    }

    void render(sf::RenderWindow& window) {
        PixelText::drawText(window, "GAME OVER", (MAP_WIDTH * TILE_SIZE) / 2.0f - 54, 80, 3.0f, sf::Color::Red);
        restartButton.render(window);
        exitButton.render(window);
    }
};

// ==========================================
// CLASE JUEGO
// ==========================================
class Game {
private:
    sf::RenderWindow window;
    Map map;
    Pacman pacman;
    Ghost ghost;
    Menu mainMenu;
    GameOverMenu gameOverMenu;
    GameState currentState;

public:
    Game() 
        : window(sf::VideoMode(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE), "Pac-Man POO con A*"),
          pacman(1, 1),
          ghost(17, 19),
          currentState(GameState::MENU) {
        window.setFramerateLimit(60);
    }

    void resetGame() {
        pacman.setGridPos(1, 1);
        ghost.setGridPos(17, 19);
        currentState = GameState::PLAYING;
    }

    void run() {
        while (window.isOpen()) {
            processEvents();
            if (currentState == GameState::PLAYING) {
                update();
            }
            render();
        }
    }

private:
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (currentState == GameState::MENU) {
                mainMenu.handleMouseMove(mousePos);

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (mainMenu.handleMouseClick(mousePos)) {
                        resetGame();
                    }
                }
            } 
            else if (currentState == GameState::GAME_OVER) {
                gameOverMenu.handleMouseMove(mousePos);

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    int action = gameOverMenu.handleMouseClick(mousePos);
                    if (action == 1) {
                        resetGame();
                    } else if (action == 2) {
                        window.close();
                    }
                }
            } 
            else if (currentState == GameState::PLAYING) {
                if (event.type == sf::Event::KeyPressed) {
                    pacman.handleInput(event.key.code, map);
                }
            }
        }
    }

    void update() {
        ghost.update(pacman.getGridPos(), map);

        if (pacman.getGridPos() == ghost.getGridPos()) {
            currentState = GameState::GAME_OVER;
        }
    }

    void render() {
        window.clear(sf::Color::Black);

        if (currentState == GameState::MENU) {
            mainMenu.render(window);
        } else if (currentState == GameState::PLAYING) {
            map.render(window);
            pacman.render(window);
            ghost.render(window);
        } else if (currentState == GameState::GAME_OVER) {
            map.render(window);
            pacman.render(window);
            ghost.render(window);
            gameOverMenu.render(window);
        }

        window.display();
    }
};

// ==========================================
// MAIN
// ==========================================
int main() {
    Game game;
    game.run();
    return 0;
}