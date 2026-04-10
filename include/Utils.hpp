#pragma once
#include <SFML/Graphics.hpp>
#include "Config.hpp"

inline bool intersects(const sf::FloatRect& entity, const sf::FloatRect& solid) {
    return (entity.position.x < solid.position.x + solid.size.x) &&
           (entity.position.x + entity.size.x > solid.position.x) &&
           (entity.position.y < solid.position.y + solid.size.y) &&
           (entity.position.y + entity.size.y > solid.position.y);
}

inline bool containsTile(const TileRect& regionRect, const sf::Vector2i& tile) {
    return  tile.x >= regionRect.x && tile.y >= regionRect.y &&
            tile.x < (regionRect.x + regionRect.w) && tile.y < (regionRect.y + regionRect.h);
}

inline sf::Transform makeTiltTransform(const sf::Vector2f& pivot) {
    constexpr float rotationDeg = -18.f;
    constexpr float shearX = 0.45f;
    constexpr float scaleY = 0.70f;

    const sf::Transform shear(
        1.f, shearX, 0.f,
        0.f, 1.f,    0.f,
        0.f, 0.f,    1.f
    );

    sf::Transform t = sf::Transform::Identity;
    t.translate(pivot);
    t.rotate(sf::degrees(rotationDeg));
    t.combine(shear);
    t.scale(sf::Vector2f{1.f, scaleY});
    t.translate(-pivot);
    return t;
}