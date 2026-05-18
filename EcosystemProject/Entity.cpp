#include "Entity.h"

// Dinh nghia Constructor (Khoi tao gia tri mac dinh)
Entity::Entity() {
    isDead = false;
    animationTimer = 0.0f;
    frameDuration = 0.1f;
    currentFrame = 0;
    totalFrames = 1;
    frameWidth = 0;
    frameHeight = 0;
}

// Dinh nghia Destructor
Entity::~Entity() {
    // Khong co gi can don dep dac biet
}

// Dinh nghia ham animate
void Entity::animate(float deltaTime) {
    animationTimer += deltaTime;

    // Khi du thoi gian thi chuyen sang hinh tiep theo
    if (animationTimer >= frameDuration) {
        animationTimer -= frameDuration;
        currentFrame++;

        // Neu chay het dai hinh thi quay lai hinh dau (0)
        if (currentFrame >= totalFrames) {
            currentFrame = 0;
        }

        // Doi khung cat sang vi tri moi
        currentRect.left = currentFrame * frameWidth;
        currentRect.top = 0; // Gia su sprite sheet chi co 1 hang ngang
        sprite.setTextureRect(currentRect);
    }
}

// Dinh nghia ham updateFacing
void Entity::updateFacing(float directionX) {
    if (directionX > 0) {
        sprite.setScale(std::abs(sprite.getScale().x), sprite.getScale().y); // Quay phai
    }
    else if (directionX < 0) {
        sprite.setScale(-std::abs(sprite.getScale().x), sprite.getScale().y); // Quay trai (Lat nguoc)
    }
}

// Dinh nghia ham draw
void Entity::draw(sf::RenderWindow& window) {
    sprite.setPosition(position);
    window.draw(sprite);
}

// Dinh nghia ham getPosition
sf::Vector2f Entity::getPosition() {
    return position;
}