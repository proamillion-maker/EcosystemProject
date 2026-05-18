#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

inline float getDistance(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
}

class Entity {
public:
    sf::Vector2f position;
    sf::Sprite sprite;
    bool isDead;

    sf::IntRect currentRect;
    float animationTimer;
    float frameDuration;
    int currentFrame;
    int totalFrames;
    int frameWidth;
    int frameHeight;

    Entity();
    virtual ~Entity();

    virtual void update(float deltaTime) = 0;

    void animate(float deltaTime);
    void updateFacing(float directionX);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition();
};