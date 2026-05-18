#include "wolf.h" 
#include <cmath>
#include <cstdlib>

// Định nghĩa Constructor
Wolf::Wolf(sf::Vector2f pos, const sf::Texture& tex) {
    position = pos;
    speed = 2.0f;
    energy = 200.0f;

    // Khởi tạo lang thang
    wanderTimer = 0.0f;
    wanderDir = { 0.0f, 0.0f };

    // --- CẤU HÌNh ANIMATION CHO SÓI ---
    totalFrames = 4;
    frameWidth = 253;
    frameHeight = 283; // Lưu ý: Biến này là int, nên để số nguyên (bỏ .8 đi)
    frameDuration = 0.1f;

    sprite.setTexture(tex);
    currentRect = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentRect);

    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
    sprite.setScale(0.5f, 0.5f);
}

// Định nghĩa hàm update
void Wolf::update(float deltaTime) {
    energy -= deltaTime * 10.0f;
    if (energy <= 0) isDead = true;

    animate(deltaTime);
}

// Định nghĩa hàm hunt
void Wolf::hunt(std::vector<Rabbit>& rabbits, float deltaTime) {
    int closestIndex = -1;
    float minDist = 300.0f;

    for (int i = 0; i < rabbits.size(); i++) {
        // Hàm getDistance nằm trong Entity.h, đã được include qua wolf.h
        float d = getDistance(position, rabbits[i].position);
        if (d < minDist) { minDist = d; closestIndex = i; }
    }

    if (closestIndex != -1) {
        // --- TRƯỜNG HỢP 1: THẤY THỎ -> ĐUỔI THEO ---
        sf::Vector2f target = rabbits[closestIndex].position;
        sf::Vector2f dir = target - position;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) dir /= len;

        position += dir * speed * deltaTime * 60.0f;
        updateFacing(dir.x);

        // Ăn thỏ
        if (minDist < 10.0f) {
            rabbits[closestIndex].isDead = true;
            energy += 100.0f;
        }
    }
    else {
        // --- TRƯỜNG HỢP 2: KHÔNG THẤY THỎ -> ĐI LANG THANG TÌM ---
        wanderTimer -= deltaTime;
        if (wanderTimer <= 0) {
            wanderTimer = 1.5f + (rand() % 100) / 100.0f;

            float randX = (rand() % 200 - 100) / 100.0f;
            float randY = (rand() % 200 - 100) / 100.0f;
            wanderDir = { randX, randY };

            float len = std::sqrt(wanderDir.x * wanderDir.x + wanderDir.y * wanderDir.y);
            if (len > 0) wanderDir /= len;
        }

        position += wanderDir * (speed * 0.6f) * deltaTime * 60.0f;
        if (std::abs(wanderDir.x) > 0.1f) updateFacing(wanderDir.x);
    }
}