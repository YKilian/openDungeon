#include "Walls.hpp"

#include <algorithm>

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

void Walls::drawBehindPlayer(sf::RenderTarget& target, const sf::RenderStates& worldStates, sf::Vector2f playerScreenPos) {
    drawSplitByPlayer(target, worldStates, playerScreenPos, false);
}

void Walls::drawInFrontOfPlayer(sf::RenderTarget& target, const sf::RenderStates& worldStates, sf::Vector2f playerScreenPos) {
    drawSplitByPlayer(target, worldStates, playerScreenPos, true);
}

    bool Walls::collidesPixelRect(const sf::FloatRect& playerBounds) const {
        for (const auto& wall : m_wallRects) {
            if (intersects(playerBounds, m_worldConfig->tileRectToPixelRect(wall))) return true;
        }
        return false;
    }

void Walls::drawSplitByPlayer(sf::RenderTarget& target,
                          const sf::RenderStates& worldStates,
                          sf::Vector2f playerScreenPos,
                          bool drawFrontGroup) {

    constexpr float wallHeightPx = 43.f;
    const sf::Vector2f up = {0.f, -wallHeightPx};
    const sf::Vector2f shadowVector = LightData::shadowVector(wallHeightPx);

    const sf::Color topColor = LightData::topLightColor;
    const sf::Color sideColorBase = LightData::sideLightColorBase;
    const sf::Color sideColorDark = LightData::sideLightColorDark;
    const sf::Color shadowColorStart = LightData::bottomShadowColorStart;
    const sf::Color shadowColorEnd = LightData::bottomShadowColorEnd;

    auto drawQuad = [&](const sf::Texture* texture,
                        const sf::Vector2f& a, const sf::Vector2f& b,
                        const sf::Vector2f& c, const sf::Vector2f& d,
                        const sf::Color& colTop, const sf::Color& colBottom,
                        bool textured) {
        sf::VertexArray quad(sf::PrimitiveType::TriangleStrip, 4);
        quad[0].position = a;
        quad[1].position = b;
        quad[2].position = d;
        quad[3].position = c;

        quad[0].color = colTop;
        quad[1].color = colTop;
        quad[2].color = colBottom;
        quad[3].color = colBottom;

        sf::RenderStates rs = sf::RenderStates::Default;
        if (textured) {
            rs.texture = texture;
            const float texSize = static_cast<float>(WorldConfig::textureTilePixels);
            quad[0].texCoords = {0.f, 0.f};
            quad[1].texCoords = {texSize, 0.f};
            quad[2].texCoords = {0.f, texSize};
            quad[3].texCoords = {texSize, texSize};
        }
            target.draw(quad, rs);
    };

    std::vector<sf::Vector2i> tiles = m_wallTiles;
    std::sort(tiles.begin(), tiles.end(), [](const sf::Vector2i& a, const sf::Vector2i& b) {
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    });

    // Draw the shadows for the walls
    if (!drawFrontGroup) {
        for (const auto& tile : tiles) {
            const sf::Vector2f base = m_worldConfig->tileToPixelPosition(tile);
            const float tp = WorldConfig::tilePixels;

            const sf::Vector2f bUL = worldStates.transform.transformPoint(base);
            const sf::Vector2f bUR = worldStates.transform.transformPoint(base + sf::Vector2f(tp, 0.f));
            const sf::Vector2f bLR = worldStates.transform.transformPoint(base + sf::Vector2f(tp, tp));
            const sf::Vector2f bLL = worldStates.transform.transformPoint(base + sf::Vector2f(0.f, tp));

            const bool hasLeft = isWallTile({tile.x - 1, tile.y});
            const bool hasBottom = isWallTile({tile.x, tile.y + 1});

            if (!hasLeft) {
                drawQuad(nullptr,  bLL + shadowVector, bUL + shadowVector, bUL, bLL,
                         shadowColorEnd, shadowColorStart, false);
            }
            if (!hasBottom) {
                drawQuad(nullptr, bLL, bLR, bLR + shadowVector, bLL + shadowVector,
                         shadowColorStart, shadowColorEnd, false);
            }
        }
    }

    for (const auto& tile : m_wallTiles) {
        const sf::Vector2f base = m_worldConfig->tileToPixelPosition(tile);
        const float tp = WorldConfig::tilePixels;

        const sf::Vector2f bottomUpperLeft = worldStates.transform.transformPoint(base);
        const sf::Vector2f bottomUUpperRight = worldStates.transform.transformPoint(base + sf::Vector2f(tp, 0.f));
        const sf::Vector2f bottomLowerRight = worldStates.transform.transformPoint(base + sf::Vector2f(tp, tp));
        const sf::Vector2f bottomLowerLeft = worldStates.transform.transformPoint(base + sf::Vector2f(0.f, tp));

        const bool hasLeftNeighbor = isWallTile({tile.x - 1, tile.y});

        const sf::Texture* tileTexture = pickTextureForTile(tile);
        const sf::Vector2f topUpperLeft = bottomUpperLeft + up;
        const sf::Vector2f topUpperRight = bottomUUpperRight + up;
        const sf::Vector2f topLowerRight = bottomLowerRight + up;
        const sf::Vector2f topLowerLeft = bottomLowerLeft + up;

        const bool hasBottomNeighbor = isWallTile({tile.x, tile.y + 1});

        const bool isFront = (
            (bottomLowerRight.y >= playerScreenPos.y && bottomLowerRight.x - 3.f <= playerScreenPos.x) ||
            (bottomUpperLeft.y >= playerScreenPos.y && bottomUpperLeft.x - 23.f <= playerScreenPos.x)
        );

        // Draw the top of the wall
        drawQuad(tileTexture, topUpperLeft, topUpperRight, topLowerRight, topLowerLeft, topColor, topColor, true);

        if (isFront != drawFrontGroup) continue;

        // Draw the left wall
        if (!hasLeftNeighbor) {
            drawQuad(tileTexture, topUpperLeft, topLowerLeft, bottomLowerLeft, bottomUpperLeft, sideColorBase, sideColorDark, true);
        }

        // Draw the lower wall
        if (!hasBottomNeighbor) {
            drawQuad(tileTexture, topLowerLeft, topLowerRight, bottomLowerRight, bottomLowerLeft, topColor, sideColorBase, true);
        }
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
