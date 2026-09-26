#include "food_position.hpp"
#include <cstdlib>

FoodCell RandomFoodPositionSource::nextPosition(int gridSize) {
    return FoodCell{rand() % gridSize, rand() % gridSize};
}

FoodCell selectFoodCell(FoodPositionSource& positionSource, int gridSize) {
    return positionSource.nextPosition(gridSize);
}
