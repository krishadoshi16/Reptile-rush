#include "food.hpp"
#include <cstdlib>

void initFood(sf::CircleShape& food, int gridSize, int blockSize) {
    RandomFoodPositionSource positionSource;
    initFood(food, gridSize, blockSize, positionSource);
}

void initFood(sf::CircleShape& food, int gridSize, int blockSize, FoodPositionSource& positionSource) {
    food.setFillColor(sf::Color::Red);
    const FoodCell cell = selectFoodCell(positionSource, gridSize);
    food.setPosition(cell.x * blockSize, cell.y * blockSize);
}

void initBonusFood(sf::CircleShape& bonusFood) {
    bonusFood.setPointCount(6);
    bonusFood.setFillColor(sf::Color::Magenta);
}

void initObstacles(std::vector<sf::RectangleShape>& obstacles, int gridSize, int blockSize) {
    obstacles.clear();
    for (int i = 0; i < 10; i++) {
        sf::RectangleShape obs(sf::Vector2f(blockSize, blockSize));
        obs.setFillColor(sf::Color(139, 69, 19));
        obs.setPosition((rand() % gridSize) * blockSize, (rand() % gridSize) * blockSize);
        obstacles.push_back(obs);
    }
}
