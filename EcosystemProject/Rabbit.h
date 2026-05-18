#pragma once
#include "Entity.h"
#include <vector>

class Rabbit : public Entity {
public:
    float speed;
    float energy;
    float senseRadius;
    const sf::Texture* texturePtr;

    sf::Vector2f wanderDir;
    float wanderTimer;

    Rabbit(sf::Vector2f pos, float geneSpeed, const sf::Texture& tex);

    void update(float deltaTime) override;

    bool runAway(const std::vector<sf::Vector2f>& wolfPositions, float deltaTime);
    bool findFood(std::vector<sf::Vector2f>& plants, float deltaTime);
    Rabbit reproduce();
};