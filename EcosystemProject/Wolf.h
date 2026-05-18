#pragma once
#include "Entity.h"
#include "Rabbit.h"
#include <vector>

class Wolf : public Entity {
public:
    float speed;
    float energy;

    sf::Vector2f wanderDir;
    float wanderTimer;

    Wolf(sf::Vector2f pos, const sf::Texture& tex);

    void update(float deltaTime) override;

    void hunt(std::vector<Rabbit>& rabbits, float deltaTime);
}; // <--- QUAN TRỌNG: PHẢI CÓ DẤU CHẤM PHẨY NÀY