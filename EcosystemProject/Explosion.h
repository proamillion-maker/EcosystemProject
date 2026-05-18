#pragma once
#include <SFML/Graphics.hpp>

class Explosion {
public:
    sf::Sprite sprite;
    sf::IntRect currentRect;

    float animationTimer;
    float frameDuration;
    int currentFrame;
    int totalFrames;
    int frameWidth;
    int frameHeight;

    bool isFinished;

    Explosion(sf::Vector2f pos, const sf::Texture& tex);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

private:
    sf::Vector2f position;
};