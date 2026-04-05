#include "Floor.hpp"

    bool Floor::load(const WorldConfig& worldConfig,
              const std::string& defaultFloorTexturePath,
              const std::vector<BiomeData>& biomes) {

        const sf::Color bottomColor(150, 150, 150, 255);

        m_worldConfig = &worldConfig;

        if (!m_defaultTexture.loadFromFile(defaultFloorTexturePath)) return false;
        m_defaultTexture.setRepeated(true);

        m_baseShape.setPosition({0.f, 0.f});
        m_baseShape.setSize(m_worldConfig->worldPixelSize());
        m_baseShape.setFillColor(bottomColor);
        m_baseShape.setTexture(&m_defaultTexture);
        m_baseShape.setTextureRect(sf::IntRect(
            {0, 0},
            {WorldConfig::tilesToTexturePixels(m_worldConfig->tilesX),
             WorldConfig::tilesToTexturePixels(m_worldConfig->tilesY)}
        ));

        m_layers.clear();
        m_layers.reserve(biomes.size());

        for (const auto& biome : biomes) {
            m_layers.emplace_back();
            Layer& layer = m_layers.back();

            layer.region = biome.region;

            if (!layer.texture.loadFromFile(biome.floorTexture)) return false;
            layer.texture.setRepeated(true);

            const sf::FloatRect px = m_worldConfig->tileRectToPixelRect(layer.region);
            layer.shape.setPosition(px.position);
            layer.shape.setSize(px.size);

            layer.shape.setFillColor(bottomColor);
            layer.shape.setTexture(&layer.texture);
            layer.shape.setTextureRect(WorldConfig::tileRectToTextureRect(layer.region));
        }

        return true;
    }

    void Floor::draw(sf::RenderTarget& target, const sf::RenderStates& states) const {
        target.draw(m_baseShape, states);
        for (const auto& layer : m_layers) {
            target.draw(layer.shape, states);
        }
    }
