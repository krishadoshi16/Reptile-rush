// === food.hpp (Header for Food & Obstacles) ===
// === Created by Member 3 ===
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

void initFood(sf::CircleShape& food, int gridSize, int blockSize);
void initBonusFood(sf::CircleShape& bonusFood);
void initObstacles(std::vector<sf::RectangleShape>& obstacles, int gridSize, int blockSize);
