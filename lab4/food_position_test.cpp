#include "../food_position.hpp"
#include <cassert>

class FixedFoodPositionSource : public FoodPositionSource {
public:
    explicit FixedFoodPositionSource(FoodCell value)
        : position(value), calls(0), lastGridSize(0) {}

    FoodCell nextPosition(int gridSize) override {
        ++calls;
        lastGridSize = gridSize;
        return position;
    }

    FoodCell position;
    int calls;
    int lastGridSize;
};

int main() {
    FixedFoodPositionSource source(FoodCell{4, 7});

    const FoodCell chosen = selectFoodCell(source, 30);

    assert(source.calls == 1);
    assert(source.lastGridSize == 30);
    assert(chosen.x == 4);
    assert(chosen.y == 7);

    RandomFoodPositionSource randomSource;
    const FoodCell randomCell = randomSource.nextPosition(30);
    (void)randomCell;
}
