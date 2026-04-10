#include <unordered_set>

#include "Config.hpp"

class Walls {
public:
    bool load(const WorldConfig& worldConfig,
        const std::string& defaultWallTexturePath,
        const std::vector<BiomeData>& biomes);

    void addWallTiles(const TileRect& rect);

    void drawBehindPlayer(sf::RenderTarget& target, const sf::RenderStates& worldStates, sf::Vector2f playerScreenY);

    void drawInFrontOfPlayer(sf::RenderTarget& target, const sf::RenderStates& worldStates, sf::Vector2f playerScreenY);

    bool collidesPixelRect(const sf::FloatRect& playerBounds) const;

private:
    void drawSplitByPlayer(sf::RenderTarget& target,
                           const sf::RenderStates& worldStates,
                           sf::Vector2f playerScreenPos,
                           bool drawFrontGroup);

    bool inBounds(const sf::Vector2i& tile) const;

    bool isWallTile(const sf::Vector2i& tile) const;

    const sf::Texture* pickTextureForTile(const sf::Vector2i& tile) const;

    const WorldConfig* m_worldConfig = nullptr;

    sf::Texture m_defaultTexture;

    std::vector<BiomeData> m_biomes;
    std::vector<sf::Texture> m_biomeTextures;

    std::vector<TileRect> m_wallRects;
    std::vector<sf::Vector2i> m_wallTiles;

    std::unordered_set<sf::Vector2i, Vec2iHash, Vec2iEq> m_wallTileSet;

    sf::RectangleShape m_tileShape;
};