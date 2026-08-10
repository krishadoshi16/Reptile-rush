#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

struct ScorePopup {
    sf::Text text;
    sf::Vector2f position;
    float lifetime;
};

int main() {
    srand((unsigned int)time(0));

    const int blockSize = 20;
    const int gridSize = 30; // 600x600 grid
    const int numPlayers = 2;
    int score[numPlayers] = {0, 0};
    int loser = -1;
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

    // ---------- Snake ----------
    std::vector<sf::RectangleShape> snake[numPlayers];
    for (int p = 0; p < numPlayers; p++)
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape seg(sf::Vector2f(blockSize, blockSize));
        seg.setFillColor(sf::Color(0, 180 - i*40, 0));
        seg.setPosition(blockSize * (gridSize / 2 + (p ? i : -i)), blockSize * (gridSize / 2 + (p ? 5 : -5)));
        snake[p].push_back(seg);
    }

    // ---------- Food ----------
    sf::CircleShape food(blockSize / 2.f);
    food.setFillColor(sf::Color::Red);
    food.setPosition((rand() % gridSize) * blockSize, (rand() % gridSize) * blockSize);

    // ---------- Bonus Food ----------
    sf::CircleShape bonusFood(blockSize / 2.f);
    bonusFood.setPointCount(6); // hexagon
    bonusFood.setFillColor(sf::Color::Magenta);
    bool bonusActive = false;
    sf::Clock bonusClock;

    // ---------- Obstacles ----------
    std::vector<sf::RectangleShape> obstacles;
    for (int i = 0; i < 10; i++) {
        sf::RectangleShape obs(sf::Vector2f(blockSize, blockSize));
        obs.setFillColor(sf::Color(139, 69, 19));
        obs.setPosition((rand() % gridSize) * blockSize, (rand() % gridSize) * blockSize);
        obstacles.push_back(obs);
    }

    // ---------- Font & Score ----------
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;
    sf::Text scoreText("", font, 24);
    scoreText.setFillColor(sf::Color::White);

    std::vector<ScorePopup> popups;

    // ---------- Buttons ----------
    sf::RectangleShape pauseButton(sf::Vector2f(80, 30)), playButton(sf::Vector2f(80,30)), restartButton(sf::Vector2f(80,30));
    pauseButton.setFillColor(sf::Color::Yellow);
    playButton.setFillColor(sf::Color::Blue);
    restartButton.setFillColor(sf::Color::Green);

    sf::Text pauseText("Pause", font, 16), playText("Play", font, 16), restartText("Restart", font, 16);
    pauseText.setFillColor(sf::Color::Black);
    playText.setFillColor(sf::Color::White);
    restartText.setFillColor(sf::Color::Black);

    // ---------- Direction ----------
    enum Direction { Up, Down, Left, Right };
    Direction dir[numPlayers] = { Right, Left };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, 600, 600);
                view = sf::View(visibleArea);
                window.setView(view);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (pauseButton.getGlobalBounds().contains(mPos)) isPaused = true;
                if (playButton.getGlobalBounds().contains(mPos)) isPaused = false;
                if (restartButton.getGlobalBounds().contains(mPos)) {
                    // Reset everything
                    for (int p = 0; p < numPlayers; p++)
                    snake[p].clear();
                    for (int p = 0; p < numPlayers; p++)
                    for (int i = 0; i < 3; i++) {
                        sf::RectangleShape seg(sf::Vector2f(blockSize, blockSize));
                        seg.setFillColor(sf::Color(0, 180 - i*40, 0));
                        seg.setPosition(blockSize * (gridSize / 2 + (p ? i : -i)), blockSize * (gridSize / 2 + (p ? 5 : -5)));
                        snake[p].push_back(seg);
                    }
                    food.setPosition((rand() % gridSize) * blockSize, (rand() % gridSize) * blockSize);
                    bonusActive = false;
                    obstacles.clear();
                    for (int i = 0; i < 10; i++) {
                        sf::RectangleShape obs(sf::Vector2f(blockSize, blockSize));
                        obs.setFillColor(sf::Color(139, 69, 19));
                        obs.setPosition((rand() % gridSize) * blockSize, (rand() % gridSize) * blockSize);
                        obstacles.push_back(obs);
                    }
                    score[0] = score[1] = 0;
                    loser = -1;
                    foodsEaten = 0;
                    speed = 0.15f; // reset initial speed
                    dir[0] = Right; dir[1] = Left;
                    isPaused = false;
                    isGameOver = false;
                    popups.clear();
                }
            }
        }

        // Update button positions
        pauseButton.setPosition(600 - 270, 10);
        playButton.setPosition(600 - 180, 10);
        restartButton.setPosition(600 - 90, 10);
        pauseText.setPosition(pauseButton.getPosition().x + 10, 15);
        playText.setPosition(playButton.getPosition().x + 15, 15);
        restartText.setPosition(restartButton.getPosition().x + 5, 15);

        // Direction input
        if (!isPaused && !isGameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && dir[0] != Down) dir[0] = Up;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && dir[0] != Up) dir[0] = Down;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && dir[0] != Right) dir[0] = Left;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && dir[0] != Left) dir[0] = Right;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && dir[1] != Down) dir[1] = Up;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && dir[1] != Up) dir[1] = Down;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && dir[1] != Right) dir[1] = Left;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && dir[1] != Left) dir[1] = Right;
        }

        // Movement timer
        moveTimer += clock.restart().asSeconds();
        if (!isPaused && !isGameOver && moveTimer >= speed) {
            moveTimer = 0;

            for (int p = 0; p < numPlayers; p++) {
            for (int i = snake[p].size()-1; i > 0; i--)
                snake[p][i].setPosition(snake[p][i-1].getPosition());

            sf::Vector2f headPos = snake[p][0].getPosition();
            switch(dir[p]) {
                case Up: headPos.y -= blockSize; break;
                case Down: headPos.y += blockSize; break;
                case Left: headPos.x -= blockSize; break;
                case Right: headPos.x += blockSize; break;
            }
            snake[p][0].setPosition(headPos);

            // Collision with food
            if (snake[p][0].getGlobalBounds().intersects(food.getGlobalBounds())) {
                sf::RectangleShape newSeg(sf::Vector2f(blockSize, blockSize));
                newSeg.setFillColor(sf::Color(0, 120, 0));
                newSeg.setPosition(snake[p][snake[p].size()-1].getPosition());
                snake[p].push_back(newSeg);
                score[p] += 10;
                foodsEaten++;

                // Score popup
                ScorePopup popup;
                popup.text.setFont(font);
                popup.text.setCharacterSize(18);
                popup.text.setFillColor(sf::Color::Yellow);
                popup.text.setString("+10");
                popup.position = headPos;
                popup.lifetime = 1.0f;
                popups.push_back(popup);

                // Bonus food every 5 foods eaten
                if (foodsEaten % 5 == 0) {
                    bonusActive = true;
                    bonusClock.restart(); // start 5-second timer
                    bonusFood.setPointCount(6); // hexagon
                    bonusFood.setRadius(blockSize / 2.f);
                    bonusFood.setFillColor(sf::Color::Magenta);
                    bonusFood.setPosition((rand() % gridSize) * blockSize, (rand() % gridSize) * blockSize);
                }

                // Spawn new regular food
                food.setPosition((rand() % gridSize) * blockSize, (rand() % gridSize) * blockSize);

                // Gradually increase speed
                if (speed > 0.05f) speed -= 0.005f;
            }

            // Collision with bonus food
            if (bonusActive && snake[p][0].getGlobalBounds().intersects(bonusFood.getGlobalBounds())) {
                score[p] += 50;
                bonusActive = false;

                ScorePopup popup;
                popup.text.setFont(font);
                popup.text.setCharacterSize(18);
                popup.text.setFillColor(sf::Color::Cyan);
                popup.text.setString("+50 Bonus Point");
                popup.position = headPos;
                popup.lifetime = 1.0f;
                popups.push_back(popup);
            }

            // Bonus disappear after 5 seconds
            if (bonusActive && bonusClock.getElapsedTime().asSeconds() >= 5.0f) {
                bonusActive = false;
            }

            // Collision with tail
            for (size_t i = 1; i < snake[p].size(); i++)
                if (snake[p][0].getPosition() == snake[p][i].getPosition())
                    { isGameOver = true; loser = p; }

            // Collision with the other snake
            for (size_t i = 0; i < snake[1-p].size(); i++)
                if (snake[p][0].getPosition() == snake[1-p][i].getPosition())
                    { isGameOver = true; loser = p; }

            // Collision with walls
            if (headPos.x < 0 || headPos.x >= gridSize*blockSize || headPos.y < 0 || headPos.y >= gridSize*blockSize)
                { isGameOver = true; loser = p; }

            // Collision with obstacles
            for (size_t i = 0; i < obstacles.size(); i++)
                if (snake[p][0].getGlobalBounds().intersects(obstacles[i].getGlobalBounds()))
                    { isGameOver = true; loser = p; }
            }
        }

        // ---------- Draw ----------
        window.clear(sf::Color(50,50,50));

        // Grid
        for (int i = 0; i <= gridSize*blockSize; i += blockSize) {
            sf::Vertex line1[] = { sf::Vertex(sf::Vector2f((float)i,0), sf::Color(80,80,80)),
                                   sf::Vertex(sf::Vector2f((float)i,(float)gridSize*blockSize), sf::Color(80,80,80)) };
            window.draw(line1,2,sf::Lines);
            sf::Vertex line2[] = { sf::Vertex(sf::Vector2f(0,(float)i), sf::Color(80,80,80)),
                                   sf::Vertex(sf::Vector2f((float)gridSize*blockSize,(float)i), sf::Color(80,80,80)) };
            window.draw(line2,2,sf::Lines);
        }

        // Obstacles
        for (size_t i =0;i<obstacles.size();i++) window.draw(obstacles[i]);

        // Snake with gradient
        for (int p = 0; p < numPlayers; p++)
        for (size_t i=0;i<snake[p].size();i++){
            int greenValue = 180 - (int)i*30;
            if (greenValue<50) greenValue = 50;
            snake[p][i].setFillColor(p ? sf::Color(greenValue, 0, 0) : sf::Color(0, greenValue, 0));
            window.draw(snake[p][i]);
        }

        // Snake eyes
        for (int p = 0; p < numPlayers; p++) {
        sf::Vector2f headPos = snake[p][0].getPosition();
        sf::CircleShape eye1(3), eye2(3), pupil1(1), pupil2(1);
        eye1.setFillColor(sf::Color::White); eye2.setFillColor(sf::Color::White);
        pupil1.setFillColor(sf::Color::Black); pupil2.setFillColor(sf::Color::Black);
        eye1.setPosition(headPos.x+4, headPos.y+4); eye2.setPosition(headPos.x+blockSize-8, headPos.y+4);
        pupil1.setPosition(eye1.getPosition().x+1, eye1.getPosition().y+1); pupil2.setPosition(eye2.getPosition().x+1, eye2.getPosition().y+1);
        window.draw(eye1); window.draw(eye2); window.draw(pupil1); window.draw(pupil2);
        }

        // Food
        window.draw(food);
        if (bonusActive) window.draw(bonusFood);

        // Buttons
        window.draw(pauseButton); window.draw(pauseText);
        window.draw(playButton); window.draw(playText);
        window.draw(restartButton); window.draw(restartText);

        // Score
        std::stringstream ss;
        ss << "P1: " << score[0] << "    P2: " << score[1];
        scoreText.setString(ss.str());
        window.draw(scoreText);

        // Score popups
        for (size_t i=0;i<popups.size();i++){
            popups[i].text.setPosition(popups[i].position);
            window.draw(popups[i].text);
            popups[i].position.y -= 0.5f;
            popups[i].lifetime -= 0.02f;
        }
        // Remove expired popups
        for (size_t i=0;i<popups.size();){
            if (popups[i].lifetime <=0) popups.erase(popups.begin()+i);
            else i++;
        }

        // Game Over
        if (isGameOver) {
            sf::Text goText("", font, 40);
            goText.setFillColor(sf::Color::Red);
            std::stringstream ss2;
            ss2 << "Game Over!\nPlayer " << (loser+1) << " lost\nP1: " << score[0] << "  P2: " << score[1];
            goText.setString(ss2.str());
            goText.setPosition(600/4, 600/2 - 50);
            window.draw(goText);
        }

        window.display();
    }

    return 0;
}

