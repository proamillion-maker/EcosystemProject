#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <cmath>

// Include các file Header
#include "Explosion.h"
#include "Rabbit.h"
#include "Wolf.h"

// --- THÔNG SỐ MÔI TRƯỜNG ---
const int W_WIDTH = 1200;
const int W_HEIGHT = 800;
const int BORDER_SIZE = 50;

// --- TRẠNG THÁI GAME ---
enum GameState {
    MENU,
    SETTINGS,
    PLAYING,
    PAUSED
};

// Hàm tính khoảng cách
inline float getDistanceMain(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
}

// Hàm phụ trợ tạo nút bấm (Text Button)
void setupButton(sf::Text& text, sf::Font& font, std::string str, int size, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.0f);
    // Căn giữa
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2, bounds.height / 2);
    text.setPosition(x, y);
}

// Hàm kiểm tra chuột có bấm vào nút không
bool isClicked(sf::Text& text, sf::Vector2f mousePos) {
    return text.getGlobalBounds().contains(mousePos);
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "Ecosystem: Simulation Mode");
    window.setFramerateLimit(60);

    // --- SETUP MÔI TRƯỜNG ---
    sf::RectangleShape riverTop(sf::Vector2f(W_WIDTH, BORDER_SIZE));
    riverTop.setFillColor(sf::Color(0, 191, 255, 200)); riverTop.setPosition(0, 0);
    sf::RectangleShape riverBottom(sf::Vector2f(W_WIDTH, BORDER_SIZE));
    riverBottom.setFillColor(sf::Color(0, 191, 255, 200)); riverBottom.setPosition(0, W_HEIGHT - BORDER_SIZE);
    sf::RectangleShape cliffLeft(sf::Vector2f(BORDER_SIZE, W_HEIGHT));
    cliffLeft.setFillColor(sf::Color(105, 105, 105)); cliffLeft.setPosition(0, 0);
    sf::RectangleShape cliffRight(sf::Vector2f(BORDER_SIZE, W_HEIGHT));
    cliffRight.setFillColor(sf::Color(105, 105, 105)); cliffRight.setPosition(W_WIDTH - BORDER_SIZE, 0);

    // --- RESOURCES ---
    sf::Music bgMusic;
    float currentVolume = 50.0f; // Biến lưu âm lượng
    if (!bgMusic.openFromFile("b.ogg")) std::cout << "LOI NHAC\n";
    else { bgMusic.setLoop(true); bgMusic.setVolume(currentVolume); bgMusic.play(); }

    sf::Font font;
    if (!font.loadFromFile("pixel.ttf")) {
        if (!font.loadFromFile("font.ttf")) font.loadFromFile("arial.ttf");
    }

    // --- UI CHO MENU CHÍNH ---
    sf::Text titleText, btnPlay, btnSettings, btnExit;
    setupButton(titleText, font, "WILDLIFE SIMULATOR", 60, W_WIDTH / 2, 150);
    titleText.setFillColor(sf::Color::Yellow);
    setupButton(btnPlay, font, "PLAY SIMULATION", 40, W_WIDTH / 2, 300);
    setupButton(btnSettings, font, "SETTINGS", 40, W_WIDTH / 2, 400);
    setupButton(btnExit, font, "EXIT", 40, W_WIDTH / 2, 500);

    // --- UI CHO SETTINGS ---
    sf::Text settingsTitle, volText, btnVolUp, btnVolDown, btnBack;
    setupButton(settingsTitle, font, "SETTINGS", 50, W_WIDTH / 2, 150);
    setupButton(volText, font, "MUSIC VOLUME: 50", 35, W_WIDTH / 2, 300);
    setupButton(btnVolDown, font, "[-]", 50, W_WIDTH / 2 - 200, 300);
    setupButton(btnVolUp, font, "[+]", 50, W_WIDTH / 2 + 200, 300);
    setupButton(btnBack, font, "BACK TO MENU", 35, W_WIDTH / 2, 500);

    // --- UI CHO PAUSE MENU ---
    sf::RectangleShape pauseOverlay(sf::Vector2f(W_WIDTH, W_HEIGHT));
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    sf::Text pauseTitle, btnResume, btnQuitMenu;
    sf::Text pauseVolText, btnPauseVolDown, btnPauseVolUp;

    setupButton(pauseTitle, font, "SIMULATION PAUSED", 50, W_WIDTH / 2, 150);
    pauseTitle.setFillColor(sf::Color::Cyan);

    setupButton(btnResume, font, "RESUME", 40, W_WIDTH / 2, 250);

    // UI chỉnh nhạc trong Pause
    setupButton(pauseVolText, font, "VOLUME: 50", 30, W_WIDTH / 2, 350);
    setupButton(btnPauseVolDown, font, "[-]", 40, W_WIDTH / 2 - 150, 350);
    setupButton(btnPauseVolUp, font, "[+]", 40, W_WIDTH / 2 + 150, 350);

    setupButton(btnQuitMenu, font, "QUIT TO MENU", 40, W_WIDTH / 2, 450);


    // --- LOAD TEXTURE & GAME ASSETS ---
    sf::Texture rabbitTex, wolfTex, explosionTex, grassTex, backgroundTex;
    if (!rabbitTex.loadFromFile("rabbit.png")) rabbitTex.create(32, 32);
    if (!wolfTex.loadFromFile("shut.png")) wolfTex.create(48, 48);
    if (!explosionTex.loadFromFile("explosion.png")) explosionTex.create(64, 64);
    if (!grassTex.loadFromFile("grass.png")) grassTex.create(16, 16);
    if (!backgroundTex.loadFromFile("ground.png")) backgroundTex.create(W_WIDTH, W_HEIGHT);

    rabbitTex.setSmooth(false); wolfTex.setSmooth(false);
    explosionTex.setSmooth(false); grassTex.setSmooth(false); backgroundTex.setSmooth(false);

    backgroundTex.setRepeated(true);
    sf::Sprite backgroundSprite(backgroundTex);
    backgroundSprite.setTextureRect(sf::IntRect(0, 0, W_WIDTH, W_HEIGHT));

    sf::Sprite plantSprite(grassTex);
    plantSprite.setOrigin(grassTex.getSize().x / 2.0f, grassTex.getSize().y / 2.0f);
    plantSprite.setScale(0.05f, 0.05f);

    sf::Text statsText;
    statsText.setFont(font);
    statsText.setCharacterSize(20);
    statsText.setFillColor(sf::Color::White);
    statsText.setOutlineColor(sf::Color::Black);
    statsText.setOutlineThickness(2.0f);
    statsText.setPosition(BORDER_SIZE + 10, BORDER_SIZE + 10);

    // --- KHỞI TẠO ENTITY ---
    std::vector<Rabbit> rabbits;
    std::vector<Wolf> wolves;
    std::vector<sf::Vector2f> plants;
    std::vector<Explosion> explosions;

    // (ĐÃ XÓA };

    resetGame();

    sf::Clock clock;
    int totalDeadRabbits = 0;
    int totalDeadWolves = 0;

    GameState gameState = MENU;

    // --- MAIN LOOP ---
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // --- XỬ LÝ PHÍM BẤM (KEYBOARD) ---
            if (event.type == sf::Event::KeyPressed) {
                // Phím ESCAPE để Tạm dừng / Tiếp tục
                if (event.key.code == sf::Keyboard::Escape) {
                    if (gameState == PLAYING) {
                        gameState = PAUSED; // Đang chơi -> Pause
                        pauseVolText.setString("VOLUME: " + std::to_string((int)currentVolume));
                    }
                    else if (gameState == PAUSED) {
                        gameState = PLAYING; // Đang Pause -> Chơi tiếp
                    }
                }
            }

            // --- XỬ LÝ CLICK CHUỘT ---
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {

                    // 1. MENU
                    if (gameState == MENU) {
                        if (isClicked(btnPlay, mousePos)) {
                            resetGame();
                            gameState = PLAYING;
                        }
                        if (isClicked(btnSettings, mousePos)) gameState = SETTINGS;
                        if (isClicked(btnExit, mousePos)) window.close();
                    }

                    // 2. SETTINGS
                    else if (gameState == SETTINGS) {
                        if (isClicked(btnBack, mousePos)) gameState = MENU;
                        if (isClicked(btnVolUp, mousePos)) {
                            currentVolume += 10.0f; if (currentVolume > 100.0f) currentVolume = 100.0f;
                            bgMusic.setVolume(currentVolume);
                            volText.setString("MUSIC VOLUME: " + std::to_string((int)currentVolume));
                        }
                        if (isClicked(btnVolDown, mousePos)) {
                            currentVolume -= 10.0f; if (currentVolume < 0.0f) currentVolume = 0.0f;
                            bgMusic.setVolume(currentVolume);
                            volText.setString("MUSIC VOLUME: " + std::to_string((int)currentVolume));
                        }
                    }

                    // 3. PAUSED
                    else if (gameState == PAUSED) {
                        if (isClicked(btnResume, mousePos)) {
                            gameState = PLAYING;
                        }
                        if (isClicked(btnQuitMenu, mousePos)) {
                            gameState = MENU;
                        }
                        if (isClicked(btnPauseVolUp, mousePos)) {
                            currentVolume += 10.0f; if (currentVolume > 100.0f) currentVolume = 100.0f;
                            bgMusic.setVolume(currentVolume);
                            pauseVolText.setString("VOLUME: " + std::to_string((int)currentVolume));
                        }
                        if (isClicked(btnPauseVolDown, mousePos)) {
                            currentVolume -= 10.0f; if (currentVolume < 0.0f) currentVolume = 0.0f;
                            bgMusic.setVolume(currentVolume);
                            pauseVolText.setString("VOLUME: " + std::to_string((int)currentVolume));
                        }
                    }

                    // 4. TRONG GAME (Trồng cỏ)
                    else if (gameState == PLAYING) {
                        if (mousePos.x > BORDER_SIZE && mousePos.x < W_WIDTH - BORDER_SIZE &&
                            mousePos.y > BORDER_SIZE && mousePos.y < W_HEIGHT - BORDER_SIZE) {
                            plants.push_back(mousePos);
                        }
                    }
                }

                // 5.TRONG GAME (Tạo vụ nổ)
                if (gameState == PLAYING) {
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        // CHỨC NĂNG GOD MODE: TẠO VỤ NỔ
                        explosions.push_back(Explosion(mousePos, explosionTex));
                        float blastRadius = 100.0f;
                        for (auto& r : rabbits) if (getDistanceMain(mousePos, r.position) < blastRadius) r.isDead = true;
                        for (auto& w : wolves) if (getDistanceMain(mousePos, w.position) < blastRadius) w.isDead = true;
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        // VẼ GIAO DIỆN

        if (gameState == MENU) {
            window.draw(titleText);
            window.draw(btnPlay);
            window.draw(btnSettings);
            window.draw(btnExit);

            // Hover effect
            btnPlay.setFillColor(isClicked(btnPlay, mousePos) ? sf::Color::Green : sf::Color::White);
            btnSettings.setFillColor(isClicked(btnSettings, mousePos) ? sf::Color::Green : sf::Color::White);
            btnExit.setFillColor(isClicked(btnExit, mousePos) ? sf::Color::Red : sf::Color::White);
        }

        else if (gameState == SETTINGS) {
            window.draw(settingsTitle); window.draw(volText);
            window.draw(btnVolUp); window.draw(btnVolDown); window.draw(btnBack);

            btnBack.setFillColor(isClicked(btnBack, mousePos) ? sf::Color::Yellow : sf::Color::White);
            btnVolUp.setFillColor(isClicked(btnVolUp, mousePos) ? sf::Color::Green : sf::Color::White);
            btnVolDown.setFillColor(isClicked(btnVolDown, mousePos) ? sf::Color::Red : sf::Color::White);
        }

        else if (gameState == PLAYING || gameState == PAUSED) {
            // NẾU ĐANG CHƠI THÌ UPDATE LOGIC
            if (gameState == PLAYING) {
                if (rand() % 100 < 5) plants.push_back({ (float)(rand() % safeWidth + BORDER_SIZE), (float)(rand() % safeHeight + BORDER_SIZE) });
                for (auto& exp : explosions) exp.update(deltaTime);

                std::vector<sf::Vector2f> threatPositions;
                for (const auto& wolf : wolves) threatPositions.push_back(wolf.position);

                for (int i = 0; i < rabbits.size(); i++) {
                    rabbits[i].update(deltaTime);
                    bool isRunning = rabbits[i].runAway(threatPositions, deltaTime);
                    if (!isRunning) rabbits[i].findFood(plants, deltaTime);
                    if (rabbits[i].position.x < BORDER_SIZE) rabbits[i].position.x = BORDER_SIZE;
                    if (rabbits[i].position.x > W_WIDTH - BORDER_SIZE) rabbits[i].position.x = W_WIDTH - BORDER_SIZE;
                    if (rabbits[i].position.y < BORDER_SIZE) rabbits[i].position.y = BORDER_SIZE;
                    if (rabbits[i].position.y > W_HEIGHT - BORDER_SIZE) rabbits[i].position.y = W_HEIGHT - BORDER_SIZE;
                    if (rabbits[i].energy > 150) rabbits.push_back(rabbits[i].reproduce());
                }

                for (auto& wolf : wolves) {
                    wolf.update(deltaTime);
                    wolf.hunt(rabbits, deltaTime);
                    if (wolf.position.x < BORDER_SIZE) wolf.position.x = BORDER_SIZE;
                    if (wolf.position.x > W_WIDTH - BORDER_SIZE) wolf.position.x = W_WIDTH - BORDER_SIZE;
                    if (wolf.position.y < BORDER_SIZE) wolf.position.y = BORDER_SIZE;
                    if (wolf.position.y > W_HEIGHT - BORDER_SIZE) wolf.position.y = W_HEIGHT - BORDER_SIZE;
                }


                for (auto it = rabbits.begin(); it != rabbits.end();) {
                    if (it->isDead) { totalDeadRabbits++; it = rabbits.erase(it); }
                    else ++it;
                }
                for (auto it = wolves.begin(); it != wolves.end();) {
                    if (it->isDead) { totalDeadWolves++; it = wolves.erase(it); }
                    else ++it;
                }
                for (auto it = explosions.begin(); it != explosions.end();) {
                    if (it->isFinished) { it = explosions.erase(it); }
                    else ++it;
                }
            }

            // DÙ ĐANG CHƠI HAY PAUSE THÌ VẪN VẼ GAME

            window.draw(backgroundSprite);
            window.draw(riverTop); window.draw(riverBottom);
            window.draw(cliffLeft); window.draw(cliffRight);
            for (auto& p : plants) { plantSprite.setPosition(p); window.draw(plantSprite); }
            for (auto& r : rabbits) r.draw(window);
            for (auto& w : wolves) w.draw(window);
            for (auto& exp : explosions) exp.draw(window);

            std::string infoString =
                "Rabbits: " + std::to_string(rabbits.size()) + " | Wolves: " + std::to_string(wolves.size()) + "\n" +
                "Dead R:  " + std::to_string(totalDeadRabbits) + " | Dead W: " + std::to_string(totalDeadWolves) + "\n" +
                "[R-Click]: Explosion | [ESC]: Pause";
            statsText.setString(infoString);
            window.draw(statsText);

            // NẾU ĐANG PAUSE -> VẼ THÊM MENU PAUSE ĐÈ LÊN
            if (gameState == PAUSED) {
                window.draw(pauseOverlay); // Lớp phủ mờ
                window.draw(pauseTitle);
                window.draw(btnResume);
                window.draw(btnQuitMenu);

                window.draw(pauseVolText);
                window.draw(btnPauseVolUp);
                window.draw(btnPauseVolDown);

                btnResume.setFillColor(isClicked(btnResume, mousePos) ? sf::Color::Green : sf::Color::White);
                btnQuitMenu.setFillColor(isClicked(btnQuitMenu, mousePos) ? sf::Color::Red : sf::Color::White);
                btnPauseVolUp.setFillColor(isClicked(btnPauseVolUp, mousePos) ? sf::Color::Yellow : sf::Color::White);
                btnPauseVolDown.setFillColor(isClicked(btnPauseVolDown, mousePos) ? sf::Color::Yellow : sf::Color::White);
            }
        }

        window.display();
    }
    return 0;
}