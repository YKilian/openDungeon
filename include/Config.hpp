#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

/**
     * Describing the Dimensions of a rect element
     *
     * @param x x Position (>)
     * @param y y Position (v)
     * @param w width (left-to-right >)
     * @param h height (top-to-bottom v)
     */
struct TileRect {
    int x{};
    int y{};
    int w{};
    int h{};
};

struct WorldConfig {
    int tilesX;
    int tilesY;

    static constexpr float tilePixels = 64.f;
    static constexpr int textureTilePixels = 16;

    sf::Vector2f worldPixelSize() const {
        return {tilesX * tilePixels, tilesY * tilePixels};
    }

    sf::Vector2f tileToPixelPosition(const sf::Vector2i& tile) const {
        return {tile.x * tilePixels, tile.y * tilePixels};
    }

    static int tilesToTexturePixels(int tiles) {
        return tiles * textureTilePixels;
    }

    sf::FloatRect tileRectToPixelRect(const TileRect& rect) const {
        return sf::FloatRect({rect.x * tilePixels, rect.y * tilePixels}, {rect.w * tilePixels, rect.h * tilePixels});
    }

    static sf::IntRect tileRectToTextureRect(const TileRect& rect) {
        return sf::IntRect({0, 0}, {tilesToTexturePixels(rect.w), tilesToTexturePixels(rect.h)});
    }
};

struct BiomeData {
    std::string name;
    TileRect region;
    std::string floorTexture;
    std::string wallTexture;
};

struct LightData {
    static sf::Vector2f shadowVector(const float size) { return {-18.f, size * 0.7f};};
    inline static sf::Color topLightColor{180, 180, 200};
    inline static sf::Color sideLightColorBase{110, 110, 140};
    inline static sf::Color sideLightColorDark{40, 40, 60};
    inline static sf::Color bottomShadowColorStart{20, 20, 40, 140};
    inline static sf::Color bottomShadowColorEnd{20, 20, 40, 20};
};

struct MapData {
    WorldConfig world;
    sf::Vector2i playerStart{0, 0};
    std::string defaultFloorTexture = "../assets/floor.png";
    std::string defaultWallTexture = "../assets/wall.png";
    std::vector<BiomeData> biomes;
    std::vector<TileRect> walls;
    LightData lightData;
};

struct Vec2iHash {
    size_t operator()(const sf::Vector2i& v) const noexcept {
        // simple 2-int hash combine
        const auto hx = static_cast<size_t>(static_cast<uint32_t>(v.x));
        const auto hy = static_cast<size_t>(static_cast<uint32_t>(v.y));
        return hx * 0x9e3779b97f4a7c15ULL ^ (hy + 0x9e3779b97f4a7c15ULL + (hx << 6) + (hx >> 2));
    }
};

struct Vec2iEq {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const noexcept {
        return a.x == b.x && a.y == b.y;
    }
};
