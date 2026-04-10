#include "Config.hpp"

class Floor {
public:
    struct Layer {
        TileRect region;
        sf::Texture texture;
        sf::RectangleShape shape;
    };

    bool load(const WorldConfig& worldConfig, const LightData& lightData, const std::string& defaultFloorTexturePath, const std::vector<BiomeData>& biomes);
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;

private:
    const WorldConfig* m_worldConfig = nullptr;
    sf::Texture m_defaultTexture;
    sf::RectangleShape m_baseShape;
    std::vector<Layer> m_layers;
};