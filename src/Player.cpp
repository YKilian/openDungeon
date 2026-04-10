#include <cmath>
#include <iostream>

#include "Player.hpp"
#include "Walls.hpp"
#include "Utils.hpp"

Player::Player(const WorldConfig& worldConfig)
    : m_worldConfig(&worldConfig),
      m_shadow(sf::PrimitiveType::TriangleFan, 18), // Number of vertices around the center (must be >= 5 to be visible). The first vertex is the center of the shadow, the others are distributed around it in a circle.
      m_texture(),
      m_sprite(m_texture)
{

    bool loaded = m_texture.loadFromFile("../assets/Character-test.png");
    if (!loaded) {
        loaded = m_texture.loadFromFile("assets/Character-test.png");
    }

    if (!loaded) {
        // To avoid a crash due to missing texture, a default texture is declared.
        // This will keep the player visible
        std::cerr << "Log: Player texture not found. Changing to default.\n";
        const sf::Image fallback({m_spriteWidth, m_spriteHeight}, sf::Color::Magenta);
        (void)m_texture.loadFromImage(fallback); // Ignore the return value, since it will allways succeed
    }

    // Player texture
    m_sprite.setTexture(m_texture, true);
    m_sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(m_spriteWidth), static_cast<int>(m_spriteHeight)}));
    m_sprite.setOrigin({static_cast<float>(m_spriteWidth) / 2, static_cast<float>(m_spriteHeight)});
    m_sprite.setScale({m_playerScale, m_playerScale});

    // Shadow
    float shadowRadius = WorldConfig::tilePixels * 0.4f;
    sf::Color shadowColorInner(20, 20, 40, 150); // Dark core
    sf::Color shadowColorOuter(20, 20, 40, 0); // Transparent edge

    // Center of the shadow
    m_shadow[0].position = {0.f, 0.f};
    m_shadow[0].color = shadowColorInner;

    const int vertices = static_cast<int>(m_shadow.getVertexCount());
    for (size_t i = 1; i < vertices; ++i) {
        const float angle = static_cast<float>(i - 1) * 2.f * 3.14159f / static_cast<float>(vertices - 2); // Angle for the current vertex (evenly distributed around the circle). Starts at 0 and goes up to 2*PI.
        m_shadow[i].position = { std::cos(angle) * shadowRadius, std::sin(angle) * shadowRadius }; // Position of the vertex around the center, creating a circle. Calculated using basic trigonometry (cos for x, sin for y).
        m_shadow[i].color = shadowColorOuter;
    }
}

/**
 * Set the player position based on tile coordinates. The player will be centered on the tile.
 * @param tilePos x and y coordinates of the tile to set the player on
 */
void Player::setTilePosition(const sf::Vector2i& tilePos) {
    // Player should span in the middle of the tile, so half a tile is added to the position.
    m_positionPixel = m_worldConfig->tileToPixelPosition(tilePos) +
                   sf::Vector2f(WorldConfig::tilePixels * 0.5f, WorldConfig::tilePixels * 0.5f);
    m_sprite.setPosition(m_positionPixel);
}

void Player::update(float deltaClock, const Walls& walls) {
    sf::Vector2f input{0.f, 0.f};
    float speedMultiplier = 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) input.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) input.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) input.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) input.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) speedMultiplier = 1.5f;

    sf::Vector2f move{0.f, 0.f};
    bool isMoving = (input.x != 0.f || input.y != 0.f);

    if (isMoving) {
        sf::Transform inv = makeTiltTransform({0.f, 0.f}).getInverse(); // Inverse to convert input from screen space to world space, so that movement is consistent with the tilted view of the world.
        move = inv.transformPoint(input);

        // Fix the length of diagonal movement (normalize the move vector). Otherwise, diagonal movement would be faster than horizontal/vertical movement, which can feel unbalanced.
        float len = std::sqrt(move.x * move.x + move.y * move.y);
        if (len > 0.f) move /= len;

        // Animation
        m_animationTime += deltaClock;
        if (m_animationTime >= 0.1f) {
            m_animationTime = 0.f;
            m_animationFrame = (m_animationFrame + 1) % m_frameCount;
        }

        // Choose correct sprite row based on movement direction.
        if (input.y > 0)      m_direction = 0; // Down
        else if (input.y < 0) m_direction = 1; // Up
        else if (input.x < 0) m_direction = 2; // Left
        else if (input.x > 0) m_direction = 3; // Right
    } else {
        m_animationFrame = 0; // Idle-Frame
    }

    m_sprite.setTextureRect(sf::IntRect(
        {m_animationFrame * static_cast<int>(m_spriteWidth), m_direction * static_cast<int>(m_spriteHeight)},
        {static_cast<int>(m_spriteWidth), static_cast<int>(m_spriteHeight)}
    ));

    sf::Vector2f delta = move * m_speedPxPerSec * speedMultiplier * deltaClock;
    sf::Vector2f oldPos = m_positionPixel;

    auto getCollisionRect = [&](const sf::Vector2f& pos) {
        float hitboxMargin = 10.f;
        return sf::FloatRect({pos.x - hitboxMargin, pos.y - hitboxMargin}, {WorldConfig::textureTilePixels + hitboxMargin, WorldConfig::textureTilePixels + hitboxMargin}); // Collision rectangle around the player's feet. The collision rectangle is smaller than the player shadow to allow for better movement around corners and to prevent getting stuck on walls.
    };

    // First check collision, then move
    m_positionPixel.x += delta.x;
    if (walls.collidesPixelRect(getCollisionRect(m_positionPixel))) m_positionPixel.x = oldPos.x;

    m_positionPixel.y += delta.y;
    if (walls.collidesPixelRect(getCollisionRect(m_positionPixel))) m_positionPixel.y = oldPos.y;

    m_sprite.setPosition(m_positionPixel);
}

void Player::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
    // First draw the shadow, then the player sprite on top. This ensures that the shadow is always behind the player.
    sf::RenderStates shadowStates = states;

    // Update shadow position
    shadowStates.transform.translate(m_positionPixel);
    shadowStates.transform.scale({m_playerScale / 2, m_playerScale / 2});
    target.draw(m_shadow, shadowStates);

    // Draw player
    sf::RenderStates billboardStates = sf::RenderStates::Default;
    target.draw(m_sprite, billboardStates);
}

sf::Vector2f Player::positionPx() const { return m_positionPixel; }