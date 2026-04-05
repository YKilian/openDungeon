#include "Walls.hpp"
#include "Utils.hpp"

    bool Walls::load(const WorldConfig& worldConfig,
              const std::string& defaultWallTexturePath,
              const std::vector<BiomeData>& biomes) {
        m_worldConfig = &worldConfig;

        if (!m_defaultTexture.loadFromFile(defaultWallTexturePath)) return false;
        m_defaultTexture.setRepeated(true);

        m_biomes = biomes;

        m_biomeTextures.clear();
        m_biomeTextures.reserve(m_biomes.size());
        for (const auto& biome : m_biomes) {
            sf::Texture texture;
            if (!texture.loadFromFile(biome.wallTexture)) return false;
            texture.setRepeated(true);
            m_biomeTextures.push_back(std::move(texture));
        }

        m_tileShape.setSize({WorldConfig::tilePixels, WorldConfig::tilePixels});
        m_tileShape.setTextureRect(sf::IntRect({0, 0}, {WorldConfig::textureTilePixels, WorldConfig::textureTilePixels}));

        m_wallTileSet.clear();
        m_wallTileSet.reserve(256);

        return true;
    }

    void Walls::addWallTiles(const TileRect& rect) {
        m_wallRects.push_back(rect);

        for (int yCounter = rect.y; yCounter < rect.y + rect.h; ++yCounter) {
            for (int xCounter = rect.x; xCounter < rect.x + rect.w; ++xCounter) {
                const sf::Vector2i t{xCounter, yCounter};
                m_wallTiles.push_back(t);
                m_wallTileSet.insert(t);
            }
        }
    }

    void Walls::drawBehindPlayer(sf::RenderTarget& target, const sf::RenderStates& worldStates, float playerScreenY) {
        drawSplitByPlayer(target, worldStates, false);
    }

    void Walls::drawInFrontOfPlayer(sf::RenderTarget& target, const sf::RenderStates& worldStates, float playerScreenY) {
        drawSplitByPlayer(target, worldStates, true);
    }

    bool Walls::collidesPixelRect(const sf::FloatRect& playerBounds) const {
        for (const auto& wall : m_wallRects) {
            if (intersects(playerBounds, m_worldConfig->tileRectToPixelRect(wall))) return true;
        }
        return false;
    }

    void Walls::drawSplitByPlayer(sf::RenderTarget& target,
                           const sf::RenderStates& worldStates,
                           bool drawFrontGroup) {

        constexpr float wallHeightPx = 40.f;
        const sf::Vector2f up = {0.f, -wallHeightPx};
        const sf::Vector2f shadowVector = {-15.f, wallHeightPx*.4f};

        const sf::Color leftSideColor(100, 100, 100, 255);
        const sf::Color lowerSideColor(70, 70, 70, 255);
        const sf::Color topColor(150, 150, 150, 255);
        const sf::Color shadowColor(0, 0, 0, 100);

        auto drawQuad = [&](const sf::Texture* texture,
                            const sf::Vector2f& a, const sf::Vector2f& b,
                            const sf::Vector2f& c, const sf::Vector2f& d,
                            const sf::Color& color,
                            bool textured) {
            sf::VertexArray quad(sf::PrimitiveType::TriangleStrip, 4);
            quad[0].position = a; quad[1].position = b; quad[2].position = d; quad[3].position = c;

            quad[0].color = color; quad[1].color = color; quad[2].color = color; quad[3].color = color;

            sf::RenderStates rs = sf::RenderStates::Default;
            if (textured) {
                rs.texture = texture;
                const float textureTilePixels = static_cast<float>(WorldConfig::textureTilePixels);
                quad[0].texCoords = {0.f, 0.f};
                quad[1].texCoords = {textureTilePixels,   0.f};
                quad[2].texCoords = {0.f, textureTilePixels};
                quad[3].texCoords = {textureTilePixels,   textureTilePixels};
            }

            target.draw(quad, rs);
        };

        std::vector<sf::Vector2i> tiles = m_wallTiles;
        std::sort(tiles.begin(), tiles.end(), [](const sf::Vector2i& a, const sf::Vector2i& b) {
            if (a.y != b.y) return a.y < b.y;
            return a.x < b.x;
        });

        // Render shadows first, so they will get covered by walls
        for (const auto& tile : tiles) {
            const sf::Vector2f base = m_worldConfig->tileToPixelPosition(tile);
            const float tilePixels = WorldConfig::tilePixels;

            const sf::Vector2f pointUpperLeftCorner = base + sf::Vector2f(0.f, 0.f);
            const sf::Vector2f pointUpperRightCorner = base + sf::Vector2f(tilePixels,   0.f);
            const sf::Vector2f pointLowerRightCorner = base + sf::Vector2f(tilePixels,   tilePixels);
            const sf::Vector2f pointLowerLEftCorner = base + sf::Vector2f(0.f, tilePixels);

            const sf::Vector2f bottomUpperLeftCorner = worldStates.transform.transformPoint(pointUpperLeftCorner);
            const sf::Vector2f bottomUpperRightCorner = worldStates.transform.transformPoint(pointUpperRightCorner);
            const sf::Vector2f bottomLowerRightCorner = worldStates.transform.transformPoint(pointLowerRightCorner);
            const sf::Vector2f bottomLowerLeftCorner = worldStates.transform.transformPoint(pointLowerLEftCorner);

            const bool isFront = true;
            if (isFront != drawFrontGroup) continue;

            // Just render shadows, if there is no neighbor
            const bool hasLeftNeighbor   = isWallTile({tile.x - 1, tile.y});
            const bool hasBottomNeighbor = isWallTile({tile.x, tile.y + 1});

            if (!hasLeftNeighbor) {
                const sf::Vector2f leftShadowUpperLeftCorner = bottomUpperLeftCorner + shadowVector;
                const sf::Vector2f leftShadowUpperRightCorner = bottomUpperLeftCorner;
                const sf::Vector2f leftShadowLowerRightCorner = bottomLowerLeftCorner;
                const sf::Vector2f leftShadowLowerLeftCorner = bottomLowerLeftCorner + shadowVector;

                drawQuad(nullptr, leftShadowUpperLeftCorner, leftShadowUpperRightCorner, leftShadowLowerRightCorner, leftShadowLowerLeftCorner, shadowColor, false);
            }

            if (!hasBottomNeighbor) {
                const sf::Vector2f lowerShadowUpperLeftCorner = bottomLowerLeftCorner;
                const sf::Vector2f lowerShadowUpperRightCorner = bottomLowerRightCorner;
                const sf::Vector2f lowerShadowLowerRightCorner = bottomLowerRightCorner + shadowVector;
                const sf::Vector2f lowerShadowLowerLeftCorner = bottomLowerLeftCorner + shadowVector;

                drawQuad(nullptr, lowerShadowUpperLeftCorner, lowerShadowUpperRightCorner, lowerShadowLowerRightCorner, lowerShadowLowerLeftCorner, shadowColor, false);
            }
        }

        // Then render sides, so they don't overlab with neighbors top
        for (const auto& tile : tiles) {
            const sf::Texture* texture = pickTextureForTile(tile);

            const sf::Vector2f base = m_worldConfig->tileToPixelPosition(tile);
            const float tilePixels = WorldConfig::tilePixels;

            const sf::Vector2f pointUpperLeftCorner = base + sf::Vector2f(0.f, 0.f);
            const sf::Vector2f pointLowerLeftCorner = base + sf::Vector2f(tilePixels, 0.f);
            const sf::Vector2f pointUpperRightCorner = base + sf::Vector2f(tilePixels, tilePixels);
            const sf::Vector2f pointLowerRightCorner = base + sf::Vector2f(0.f, tilePixels);

            const sf::Vector2f bottomUpperLeftCorner = worldStates.transform.transformPoint(pointUpperLeftCorner);
            const sf::Vector2f bottomLowerLeftCorner = worldStates.transform.transformPoint(pointLowerLeftCorner);
            const sf::Vector2f bottomUpperRightCorner = worldStates.transform.transformPoint(pointUpperRightCorner);
            const sf::Vector2f bottomLowerRightCorner = worldStates.transform.transformPoint(pointLowerRightCorner);

            const bool isFront = true;
            if (isFront != drawFrontGroup) continue;

            const sf::Vector2f topUpperLeftCorner = bottomUpperLeftCorner + up;
            const sf::Vector2f topLowerLeftCorner = bottomLowerLeftCorner + up;
            const sf::Vector2f topUpperRightCorner = bottomUpperRightCorner + up;
            const sf::Vector2f topLowerRightCorner = bottomLowerRightCorner + up;

            // Just render sidewalls, if there is no neighbor
            const bool hasLeftNeighbor   = isWallTile({tile.x - 1, tile.y});
            const bool hasBottomNeighbor = isWallTile({tile.x, tile.y + 1});

            if (!hasLeftNeighbor) {
                drawQuad(texture, bottomUpperLeftCorner, bottomLowerRightCorner, topLowerRightCorner, topUpperLeftCorner, leftSideColor, true);
            }
            if (!hasBottomNeighbor) {
                drawQuad(texture, bottomUpperRightCorner, bottomLowerRightCorner, topLowerRightCorner, topUpperRightCorner, lowerSideColor, true);
            }
        }

        // Lastly render Top, to cover up walls, that lay behind them
        for (const auto& tile : tiles) {
            const sf::Texture* tex = pickTextureForTile(tile);

            const sf::Vector2f base = m_worldConfig->tileToPixelPosition(tile);
            const float tilePixels = WorldConfig::tilePixels;

            const sf::Vector2f pointUpperLeftCorner = base + sf::Vector2f(0.f, 0.f);
            const sf::Vector2f pointUpperRightCorner = base + sf::Vector2f(tilePixels,   0.f);
            const sf::Vector2f pointLowerRightCorner = base + sf::Vector2f(tilePixels,   tilePixels);
            const sf::Vector2f pointLowerLEftCorner = base + sf::Vector2f(0.f, tilePixels);

            const sf::Vector2f bottomUpperLeftCorner = worldStates.transform.transformPoint(pointUpperLeftCorner);
            const sf::Vector2f bottomUpperRightCorner = worldStates.transform.transformPoint(pointUpperRightCorner);
            const sf::Vector2f bottomLowerRightCorner = worldStates.transform.transformPoint(pointLowerRightCorner);
            const sf::Vector2f bottomLowerLeftCorner = worldStates.transform.transformPoint(pointLowerLEftCorner);

            const bool isFront = true;
            if (isFront != drawFrontGroup) continue;

            const sf::Vector2f topUpperLeftCorner = bottomUpperLeftCorner + up;
            const sf::Vector2f topUpperRightCorner = bottomUpperRightCorner + up;
            const sf::Vector2f topLowerRightCorner = bottomLowerRightCorner + up;
            const sf::Vector2f topLowerLeftCorner = bottomLowerLeftCorner + up;

            drawQuad(tex, topUpperLeftCorner, topUpperRightCorner, topLowerRightCorner, topLowerLeftCorner, topColor, true);
        }
    }

    bool Walls::inBounds(const sf::Vector2i& tile) const {
        return tile.x >= 0 && tile.y >= 0 && tile.x < m_worldConfig->tilesX && tile.y < m_worldConfig->tilesY;
    }

    bool Walls::isWallTile(const sf::Vector2i& tile) const {
        if (!inBounds(tile)) return false;
        return m_wallTileSet.contains(tile);
    }

    const sf::Texture* Walls::pickTextureForTile(const sf::Vector2i& tile) const {
        for (int i = static_cast<int>(m_biomes.size()) - 1; i >= 0; --i) {
            if (containsTile(m_biomes[static_cast<size_t>(i)].region, tile)) {
                return &m_biomeTextures[static_cast<size_t>(i)];
            }
        }
        return &m_defaultTexture;
    }

    const WorldConfig* m_worldConfig = nullptr;

    sf::Texture m_defaultTexture;

    std::vector<BiomeData> m_biomes;
    std::vector<sf::Texture> m_biomeTextures;

    std::vector<TileRect> m_wallRects;
    std::vector<sf::Vector2i> m_wallTiles;

    std::unordered_set<sf::Vector2i, Vec2iHash, Vec2iEq> m_wallTileSet;

    sf::RectangleShape m_tileShape;
