#include "Rabbit.h"

// Dinh nghia Constructor
Rabbit::Rabbit(sf::Vector2f pos, float geneSpeed, const sf::Texture& tex) {
    position = pos;
    speed = geneSpeed;
    energy = 100.0f;
    senseRadius = 150.0f;
    texturePtr = &tex;

    wanderTimer = 0.0f;
    wanderDir = { 0.0f, 0.0f };

    // --- CAU HINH ANIMATION ---
    totalFrames = 4;
    frameWidth = 292;
    frameHeight = 213;
    frameDuration = 0.1f;

    sprite.setTexture(tex);
    currentRect = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentRect);
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
    sprite.setScale(0.1f, 0.1f);
}

// Dinh nghia ham update
void Rabbit::update(float deltaTime) {
    energy -= deltaTime * (5.0f + speed * 0.5f);
    if (energy <= 0) isDead = true;
    animate(deltaTime);
}

// Dinh nghia ham runAway (Chay tron)
bool Rabbit::runAway(const std::vector<sf::Vector2f>& wolfPositions, float deltaTime) {
    int closestIndex = -1;
    float minDist = senseRadius;

    for (int i = 0; i < wolfPositions.size(); i++) {
        float d = getDistance(position, wolfPositions[i]);
        if (d < minDist) {
            minDist = d;
            closestIndex = i;
        }
    }

    if (closestIndex != -1) {
        sf::Vector2f threatPos = wolfPositions[closestIndex];
        sf::Vector2f runDir = position - threatPos;

        float len = std::sqrt(runDir.x * runDir.x + runDir.y * runDir.y);
        if (len > 0) runDir /= len;

        // Tho hoang loan chay cham hon toc do toi da (speed * 0.8f)
        position += runDir * (speed * 0.8f) * deltaTime * 60.0f;

        if (std::abs(runDir.x) > 0.1f) updateFacing(runDir.x);

        energy -= 10.0f * deltaTime; // Chay tron ton nhieu nang luong
        return true;
    }
    return false;
}

// Dinh nghia ham findFood (Tim an)
bool Rabbit::findFood(std::vector<sf::Vector2f>& plants, float deltaTime) {
    int closestIndex = -1;
    float minDist = senseRadius;

    for (int i = 0; i < plants.size(); i++) {
        float d = getDistance(position, plants[i]);
        if (d < minDist) { minDist = d; closestIndex = i; }
    }

    if (closestIndex != -1) {
        sf::Vector2f target = plants[closestIndex];
        sf::Vector2f dir = target - position;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) dir /= len;

        // Di tim an voi toc do vua phai
        position += dir * (speed * 0.8f) * deltaTime * 60.0f;

        if (std::abs(dir.x) > 0.1f) updateFacing(dir.x);

        if (minDist < 10.0f) {
            plants.erase(plants.begin() + closestIndex);
            energy += 40.0f;
            return true;
        }
    }
    else {
        // --- DI LANG THANG ---
        wanderTimer -= deltaTime;
        if (wanderTimer <= 0) {
            wanderTimer = 2.0f + (rand() % 100) / 100.0f;
            float randX = (rand() % 200 - 100) / 100.0f;
            float randY = (rand() % 200 - 100) / 100.0f;
            wanderDir = { randX, randY };
            float len = std::sqrt(wanderDir.x * wanderDir.x + wanderDir.y * wanderDir.y);
            if (len > 0) wanderDir /= len;
        }

        // Di lang thang rat cham
        position += wanderDir * (speed * 0.5f) * deltaTime * 60.0f;
        if (std::abs(wanderDir.x) > 0.1f) updateFacing(wanderDir.x);
    }
    return false;
}

// Dinh nghia ham reproduce (Sinh san)
Rabbit Rabbit::reproduce() {
    float mutation = (rand() % 100 - 50) / 100.0f; // Dot bien gen toc do
    float childSpeed = speed + mutation;

    // Gioi han toc do con
    if (childSpeed > 3.0f) childSpeed = 3.0f;
    if (childSpeed < 0.5f) childSpeed = 0.5f;

    energy -= 50;
    return Rabbit(position, childSpeed, *texturePtr);
}