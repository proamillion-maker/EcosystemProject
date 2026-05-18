#include "Explosion.h"

// Dinh nghia Constructor
Explosion::Explosion(sf::Vector2f pos, const sf::Texture& tex) {
    sprite.setTexture(tex);
    position = pos;
    isFinished = false;

    // --- CAU HINH ANH VU NO ---
    totalFrames = 15;
    frameWidth = 130;
    frameHeight = 127;
    frameDuration = 0.05f;

    animationTimer = 0.0f;
    currentFrame = 0;

    // Cat khung hinh dau tien
    currentRect = sf::IntRect(0, 0, frameWidth, frameHeight);
    sprite.setTextureRect(currentRect);

    // Dat tam vao giua de no ngay tai diem click
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
    sprite.setPosition(position);

    // Phong to vu no len mot chut cho hoanh trang
    sprite.setScale(1.5f, 1.5f);
}

// Dinh nghia ham update
void Explosion::update(float deltaTime) {
    // Neu da no xong thi khong lam gi nua
    if (isFinished) return;

    animationTimer += deltaTime;

    // Chuyen khung hinh
    if (animationTimer >= frameDuration) {
        animationTimer -= frameDuration;
        currentFrame++;

        // KIEM TRA KET THUC: Neu chay qua so khung hinh thi dung lai
        if (currentFrame >= totalFrames) {
            isFinished = true; // Danh dau de xoa
        }
        else {
            // Doi khung cat sang hinh tiep theo
            currentRect.left = currentFrame * frameWidth;
            sprite.setTextureRect(currentRect);
        }
    }
}

// Dinh nghia ham draw
void Explosion::draw(sf::RenderWindow& window) {
    if (!isFinished) {
        window.draw(sprite);
    }
}