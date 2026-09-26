# Lab 4 — Reptile Rush

| | |
|---|---|
| Repository | [krishadoshi16/Reptile-rush](https://github.com/krishadoshi16/Reptile-rush) |
| Base tag | `lab4-base` at commit `021cd1d` |
| Pull request | This PR (`lab4` → `main`) |

---

## 1. Five rules — [5]

| # | Rule |
|---|---|
| 1 | When the snake head reaches one of its body cells, the game detects a self-collision. |
| 2 | When the snake eats regular food, the score increases by 10. |
| 3 | Eating regular food adds one segment to the snake. |
| 4 | Hitting a wall ends the game. |
| 5 | Regular food is placed within the board grid. |

The repository contains multiple versions. These rules are reconstructed from my understanding and the checked-in game variants; I cannot claim they were recorded before I opened the source during this work.

---

## 2. What you could test, and what stopped you — [10]

No source was changed while assessing these blockers. All cited lines below exist in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | Head/body overlap is detected | No | `snake.cpp:27-32` exposes the predicate, but constructing and linking SFML shapes requires SFML graphics libraries; the installed libraries are 32-bit and the available compiler is 64-bit. |
| 2 | Regular food adds 10 points | No | `Snake_game_final_code.cpp:163-170` performs collision detection, score update, and other game state changes in one loop; there is no callable scoring operation. |
| 3 | Regular food grows the snake | No | `Snake_game_final_code.cpp:164-170` creates and appends the segment inside the same game-loop branch. |
| 4 | Wall collision ends the game | No | `Snake_game_final_code.cpp:224-226` checks the boundary and changes loop-owned state inline. |
| 5 | Regular food is placed within the board grid | No | `food.cpp:4-5` calls `rand()` inside food initialization, with no injectable position source in the base. |

> **Rules testable without modifying the source: 1 / 5**

The self-collision predicate is already a separate function, although the local SFML/compiler architecture mismatch prevented linking a runnable test here. The other rules are embedded in the game loop.

---

## 3. Coverage, and what it missed — [6]

| | |
|---|---|
| Line coverage | 100% of executable lines in `food_position.cpp` (4/4) |
| Branch coverage | N/A — gcov found no branches in `food_position.cpp` |
| Command used | `g++ --coverage -O0 -g -std=c++17 lab4/food_position_test.cpp food_position.cpp -o lab4/coverage_run.exe`; run `./lab4/coverage_run.exe`; then `gcov -b lab4/coverage_run-food_position.gcno` |

These percentages cover only the seam unit, not the full game.

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | Regular food is placed within the board grid. |
| Line that runs | `food_position.cpp:4-5` |
| The assertion that is missing | The test calls the random provider at `lab4/food_position_test.cpp:31` but discards its result; it never asserts that either coordinate is between 0 and 29. |

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | Food-cell selection can be supplied deterministically and checked without running the SFML window. |
| Commit 1 (seam) | `58650fe` — Add injectable food position source |
| Commit 2 (test) | This commit — Test injectable food position selection |
| Seam kind | object |
| Enabling point | `FoodPositionSource` in `food_position.hpp`; `initFood` delegates selection through `selectFoodCell` in `food.cpp`. |
| What production code gave up | `initFood` no longer calls `rand()` directly. Its existing overload creates the random provider, preserving normal play; the added overload accepts a provider for deterministic callers. |

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | spy |
| The method under test | `selectFoodCell(FoodPositionSource&, int)` |

The test passes a spy that records the requested grid size and returns a fixed cell. Production asks the collaborator for a position, so the returned value and the recorded call can both be checked.

---

## 6. Two smells in your own tests — [5]

| # | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | Assertion roulette: several bare positional assertions | `lab4/food_position_test.cpp:25-28` | Add assertion messages or compare a named expected/actual value. |
| 2 | The random-provider call has no assertion, so that test act passes for any coordinates | `lab4/food_position_test.cpp:31-32` | Assert both coordinates are within `[0, 30)`. |
