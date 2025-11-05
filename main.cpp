#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include "snake.hpp"
#include "food.hpp"
#include "ui.hpp"

struct ScorePopup {
    sf::Text text;
    sf::Vector2f position;
    float lifetime;
};

int main() {
    srand((unsigned int)time(0));

    const int blockSize = 20;
    const int gridSize = 30; // 600x600 grid
    int score = 0;
    int foodsEaten = 0;
    float speed = 0.15f; // faster initial speed
    bool isPaused = false;
    bool isGameOver = false;

    // ---------- Window ----------
    sf::RenderWindow window(sf::VideoMode(800, 800), "Snake Game");
    sf::Clock clock;
    float moveTimer = 0;

    sf::View view(sf::FloatRect(0, 0, 600, 600));
    window.setView(view);

    // Initialize components
    std::vector<sf::RectangleShape> snake;
    std::vector<sf::RectangleShape> obstacles;
    sf::CircleShape food(blockSize / 2.f);
    sf::CircleShape bonusFood(blockSize / 2.f);
    sf::Font font;
    std::vector<ScorePopup> popups;

    // Load font
    if (!font.loadFromFile("arial.ttf")) return -1;

    // Create snake, food, obstacles, and UI
    initSnake(snake, gridSize, blockSize);
    initFood(food, gridSize, blockSize);
    initBonusFood(bonusFood);
    initObstacles(obstacles, gridSize, blockSize);

    // Create UI buttons
    UIElements ui = initUI(font);

    enum Direction { Up, Down, Left, Right };
    Direction dir = Right;

    sf::Clock bonusClock;
    bool bonusActive = false;

    sf::Text scoreText("", font, 24);
    scoreText.setFillColor(sf::Color::White);