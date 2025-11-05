#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

void initSnake(std::vector<sf::RectangleShape>& snake, int gridSize, int blockSize);
void moveSnake(std::vector<sf::RectangleShape>& snake, int blockSize, int dir);
void drawSnake(sf::RenderWindow& window, std::vector<sf::RectangleShape>& snake);
bool checkSelfCollision(const std::vector<sf::RectangleShape>& snake);