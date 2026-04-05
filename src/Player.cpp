#include <cmath>
#include <algorithm>

#include "Player.hpp"
#include "Walls.hpp"

    Player::Player(const WorldConfig& worldConfig) : m_worldConfig(&worldConfig) {
        const sf::Color shadowColor(0, 0, 0, 100);
        m_shape.setRadius(WorldConfig::tilePixels * 0.4f);
        m_shape.setFillColor(shadowColor);
        m_shape.setOrigin(m_shape.getGeometricCenter());
    }

    void Player::setTilePosition(const sf::Vector2i& tilePos) {
        const sf::Vector2f tileTopLeft = m_worldConfig->tileToPixelPosition(tilePos);
        m_positionPx = tileTopLeft + sf::Vector2f(WorldConfig::tilePixels * 0.5f, WorldConfig::tilePixels * 0.5f);
        m_shape.setPosition(m_positionPx);
    }

    void Player::update(float deltaClock, const Walls& walls) {
        sf::Vector2f move{0.f, 0.f};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) move.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) move.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) move.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) move.x += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
            m_speedPxPerSec = 400.f;
        }
        else {
            m_speedPxPerSec = 200.f;
        };

        // Normalize diagonal movement by using pythagoras
        const float lengthSquared = move.x * move.x + move.y * move.y;
        if (lengthSquared > 0.f) {
            const float invertedLength = 1.f / std::sqrt(lengthSquared);
            move.x *= invertedLength;
            move.y *= invertedLength;
        }

        // Calculate Speed based on time, not on fps to ensure consistent speed on various devices
        const sf::Vector2f delta = {move.x * m_speedPxPerSec * deltaClock, move.y * m_speedPxPerSec * deltaClock};

        {
            const sf::Vector2f old = m_positionPx;

            m_positionPx = {old.x + delta.x, old.y};
            m_shape.setPosition(m_positionPx);
            if (walls.collidesPixelRect(m_shape.getGlobalBounds())) {
                m_positionPx = {old.x, m_positionPx.y};
                m_shape.setPosition(m_positionPx);
            }

            const sf::Vector2f afterX = m_positionPx;
            m_positionPx = {afterX.x, afterX.y + delta.y};
            m_shape.setPosition(m_positionPx);
            if (walls.collidesPixelRect(m_shape.getGlobalBounds())) {
                m_positionPx = {m_positionPx.x, afterX.y};
                m_shape.setPosition(m_positionPx);
            }
        }

        {
            const sf::Vector2f worldPx = m_worldConfig->worldPixelSize();
            const sf::Vector2f half = m_shape.getGeometricCenter();
            m_positionPx = {
                std::clamp(m_positionPx.x, half.x, worldPx.x - half.x),
                std::clamp(m_positionPx.y, half.y, worldPx.y - half.y)
            };
            m_shape.setPosition(m_positionPx);
        }
    }

    void Player::draw(sf::RenderTarget& target, const sf::RenderStates& states) const { target.draw(m_shape, states); }

    sf::Vector2f Player::positionPx() const { return m_positionPx; }