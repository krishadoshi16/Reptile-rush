// === food.hpp (Header for Food & Obstacles) ===
// === Created by Member 3 ===
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "food_position.hpp"

void initFood(sf::CircleShape& food, int gridSize, int blockSize);
void initFood(sf::CircleShape& food, int gridSize, int blockSize, FoodPositionSource& positionSource);
void initBonusFood(sf::CircleShape& bonusFood);
void initObstacles(std::vector<sf::RectangleShape>& obstacles, int gridSize, int blockSize);
