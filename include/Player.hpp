#pragma once
#include <SFML/Graphics.hpp>
#include "Config.hpp"

class Walls;

class Player {
public:
    explicit Player(const WorldConfig& worldConfig);
    void setTilePosition(const sf::Vector2i& tilePos);
    void update(float deltaClock, const Walls& walls);
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;
    sf::Vector2f positionPx() const;

private:
    const WorldConfig* m_worldConfig;
    sf::VertexArray m_shadow;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    sf::Vector2f m_positionPixel{0.f, 0.f};
    float m_speedPxPerSec = 150.f;

    float m_animationTime = 0.f;
    int m_animationFrame = 0;
    int m_direction = 0;

    const unsigned int m_spriteWidth = 20;
    const unsigned int m_spriteHeight = 20;
    const unsigned int m_frameCount = 5;
    float m_playerScale = 2.f;
};