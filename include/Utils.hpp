bool intersects(const sf::FloatRect& entity, const sf::FloatRect& solid) {
    return (entity.position.x < solid.position.x + solid.size.x) &&
           (entity.position.x + entity.size.x > solid.position.x) &&
           (entity.position.y < solid.position.y + solid.size.y) &&
           (entity.position.y + entity.size.y > solid.position.y);
}

bool containsTile(const TileRect& regionRect, const sf::Vector2i& tile) {
    return  tile.x >= regionRect.x && tile.y >= regionRect.y &&
            tile.x < (regionRect.x + regionRect.w) && tile.y < (regionRect.y + regionRect.h);
}