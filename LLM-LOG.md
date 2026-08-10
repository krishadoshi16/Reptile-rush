# LLM-LOG.md

## Setup

| Item | Detail |
|---|---|
| Model | Claude Opus 4.5 |
| Tool / harness | Claude Code (CLI) |
| IDE / editor | VS Code |
| Assignment doc pasted in? | No — I described the four requirements in my own words |
| Code given to the LLM | Whole file (`Snake_game_final_code.cpp`), plus the output of `git diff --stat` when I was checking for noise |
| Prompts to working code | 6 |
| Total prompts in session | 8 (6 for the change, 2 for build/toolchain setup) |
| External dependency | SFML 2.x (2.6.1/2.6.2). Not vendored, not optional — see "Build environment" below |

---

## Session

### Prompt 1

> I have a C++/SFML snake game in a single file. I want to add a second snake controlled by WASD, while the first snake keeps the arrow keys. Both snakes should eat the same fruit and whoever gets there first grows. Each player needs their own score on screen, and the game should end when either snake hits a wall, itself, or the other snake — and it should say which player lost. Change the code so that the number of lines of change is minimal and a new snake could be added. Don't add menus or extra features, don't rename anything, don't reformat.

Pasted the whole of `Snake_game_final_code.cpp` after this.

The reply's core idea was to stop treating the snake as one variable and index everything by player:

```cpp
const int numPlayers = 2;
int score[numPlayers] = {0, 0};
int loser = -1;
...
std::vector<sf::RectangleShape> snake[numPlayers];
Direction dir[numPlayers] = { Right, Left };
```

and then wrap the existing per-snake blocks in `for (int p = 0; p < numPlayers; p++)`, rewriting `snake` → `snake[p]`, `dir` → `dir[p]`, `score` → `score[p]`.

This was the right shape, but the version it gave me re-indented the whole movement block by one level, which made the diff look enormous.

### Prompt 2

> The logic is right but you re-indented everything inside the movement loop, so my diff shows about 90 lines changed when only a handful are real changes. Redo it so the existing lines keep their current indentation — just add the `for` line and the closing brace around them.

This dropped the diff a lot. The block ended up looking like this — deliberately not re-indented:

```cpp
            for (int p = 0; p < numPlayers; p++) {
            for (int i = snake[p].size()-1; i > 0; i--)
                snake[p][i].setPosition(snake[p][i-1].getPosition());

            sf::Vector2f headPos = snake[p][0].getPosition();
            switch(dir[p]) {
```

Ugly, but it is the honest minimum and it respects "no reformatting".

### Prompt 3

> Both snakes now spawn on top of each other in the middle and instantly collide. Give them separate starting rows and have player 2 start moving the other way.

```cpp
seg.setPosition(blockSize * (gridSize / 2 + (p ? i : -i)),
                blockSize * (gridSize / 2 + (p ? 5 : -5)));
```

Player 1 sits five rows above centre heading right, player 2 five rows below heading left.

### Prompt 4

> Game over works but it just says "Game Over" — it doesn't say who lost. Also when I hit Restart, player 2's snake doesn't reset and the old score stays.

Two fixes. The collision checks had to record *who* died, not just *that* someone died:

```cpp
if (headPos.x < 0 || headPos.x >= gridSize*blockSize || headPos.y < 0 || headPos.y >= gridSize*blockSize)
    { isGameOver = true; loser = p; }
```

and the restart handler had a second copy of the spawn code that I had missed entirely:

```cpp
score[0] = score[1] = 0;
loser = -1;
...
dir[0] = Right; dir[1] = Left;
```

This was the one that cost me the most time. The spawn logic exists in two places in the file and I only patched one.

### Prompt 5

> I still need snake-vs-snake collision — right now they pass straight through each other.

```cpp
// Collision with the other snake
for (size_t i = 0; i < snake[1-p].size(); i++)
    if (snake[p][0].getPosition() == snake[1-p][i].getPosition())
        { isGameOver = true; loser = p; }
```

`snake[1-p]` only works for exactly two players, which I accepted as a deliberate trade for a small diff.

### Prompt 6

> I don't have SFML installed on this machine so I can't link. Is there a way to at least confirm this compiles?

Suggested writing a stub `SFML/Graphics.hpp` with just the classes and methods this file touches, then running:

```bash
g++ -std=c++17 -fsyntax-only -Wall -I<stubdir> Snake_game_final_code.cpp
```

Clean, no warnings. The stub is a throwaway and is not committed.

---

## Build environment (after the code was already working)

These prompts were about getting a runnable binary, not about the change itself. Counting them separately because the assignment asks for prompts *to working code*.

### Prompt 7

> Give me the command to build and play the game.

The reply refused to just hand over the README's command and checked the machine first, which turned up two things the README does not mention:

- SFML is not installed anywhere on this machine — no headers, no import libraries, no DLLs on either drive.
- `arial.ttf` is not in the repo, but line 64 is `if (!font.loadFromFile("arial.ttf")) return -1;`. Without that file the game exits immediately with no error message. This is true on `main` as well; it is not something my change introduced. Fix is to copy it in before running:

```bash
cp /c/Windows/Fonts/arial.ttf .
```

It also flagged that the README's build line targets `main.cpp`, which is truncated at line 61 and does not compile. The correct target is the single-file game:

```bash
g++ -std=c++17 Snake_game_final_code.cpp -IC:/SFML/include -LC:/SFML/lib \
    -lsfml-graphics -lsfml-window -lsfml-system -o reptile-rush.exe
```

### Prompt 8

> I found SFML 3.1.0 on the download page — is that the right one?

No, and this was the most useful answer of the session. Two independent problems:

1. **Wrong major version.** This game is written against SFML 2.x. SFML 3 broke the API extensively — `pollEvent` now returns `std::optional<sf::Event>`, so the `while (window.pollEvent(event))` loop is gone; `event.type` is replaced by `event->is<sf::Event::Closed>()`; keys became scoped enums (`sf::Keyboard::Key::Up`); `sf::Text` can no longer be default-constructed; `FloatRect::intersects()` was replaced by `findIntersection()`; `setPosition(x, y)` now requires a vector; `Font::loadFromFile` became `openFromFile`. Adopting SFML 3 would mean rewriting essentially the whole file — which is a port, and the assignment explicitly forbids porting the game or swapping libraries because it destroys the diff.

2. **Compiler mismatch.** The SFML download page warns that compiler versions must match 100%, and the MinGW builds offered are GCC 14.2.0. My toolchain is GCC 15.2.0 UCRT, so the prebuilt binaries are not ABI-compatible with it.

The suggested route was to build **SFML 2.6.2** from source with my own compiler, which fixes both problems at once (I have `cmake 3.30.4` and `mingw32-make 4.4.1`):

```bash
git clone --depth 1 --branch 2.6.2 https://github.com/SFML/SFML.git /c/SFML-src
cmake -S C:/SFML-src -B C:/SFML-build -G "MinGW Makefiles" \
      -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=C:/SFML
cmake --build C:/SFML-build --target install -j
```

**Dependency, stated plainly: this build requires the SFML 2.x library (2.6.1/2.6.2). It is not vendored in the repo and it is not optional — without it the compile fails at `#include <SFML/Graphics.hpp>`. SFML 3.x will not work without porting the game.**

**Outcome:** the from-source route worked. SFML 2.6.2 built and installed to `C:\SFML`, the game compiled with no warnings under `-Wall`, and `reptile-rush.exe` launches. Verified afterwards that the binary imports `sfml-graphics-2.dll`, `sfml-window-2.dll` and `sfml-system-2.dll` — i.e. it really did link against SFML 2, not against the 3.1.0 DLLs that were also sitting in `C:\SFML\bin` from an earlier extract. Worth checking, because that folder ended up holding both versions and the wrong one linking would have failed in confusing ways.

---

## What failed, and what worked

- **Attempt 1** — correct logic, but re-indented the movement block. Rejected: it inflated the diff to roughly double its real size for zero behavioural gain.
- **Attempt 2** — correct and minimal, but both snakes spawned at the same cell and died on frame one.
- **Attempt 3** — playable, but the restart button only reset player 1, and the game-over screen never named the loser.
- **Attempt 4 onwards** — added `loser`, patched the second spawn site, added snake-vs-snake collision. This is what is in the PR.

**Total prompts to working code: 6.** A further 2 prompts (7 and 8) went on build environment and the SFML dependency, after the code was already correct.

**Code provided to the LLM:** the whole of `Snake_game_final_code.cpp` (311 lines). Not the whole repo — the other files (`main.cpp`, `snake.cpp`, `food.cpp`, `ui.cpp`) are a separate, incomplete modular version that does not build, so they were never in scope.

**Assignment document pasted in:** no.
