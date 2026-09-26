#pragma once

struct FoodCell {
    int x;
    int y;
};

class FoodPositionSource {
public:
    virtual ~FoodPositionSource() = default;
    virtual FoodCell nextPosition(int gridSize) = 0;
};

class RandomFoodPositionSource : public FoodPositionSource {
public:
    FoodCell nextPosition(int gridSize) override;
};

FoodCell selectFoodCell(FoodPositionSource& positionSource, int gridSize);
