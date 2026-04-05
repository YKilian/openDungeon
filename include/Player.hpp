#pragma once
#include <SFML/Graphics.hpp>
#include "Config.hpp" // Für WorldConfig

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
    sf::CircleShape m_shape;
    sf::Vector2f m_positionPx{0.f, 0.f};
    float m_speedPxPerSec = 200.f;
};