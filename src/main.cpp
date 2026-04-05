#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <vector>

#include "Player.hpp"
#include "Floor.hpp"
#include "Walls.hpp"
#include "Config.hpp"

namespace {

    using json = nlohmann::json;

    // TODO - Check the semantic of the jsonFile
    bool validateGameMapConfig(json jsonFile) {
        return true;
    }

    MapData loadMapFromJsonFile(const std::string& path) {
        std::ifstream in(path);
        if (!in) throw std::runtime_error("Cannot open map json: " + path);

        json jsonFile;
        in >> jsonFile;

        if (!validateGameMapConfig(jsonFile)) {
            throw std::runtime_error("Invalid map json: " + path);
        }

        MapData data;

        data.world.tilesX = jsonFile.at("map").at("tilesX").get<int>();
        data.world.tilesY = jsonFile.at("map").at("tilesY").get<int>();

        data.playerStart.x = jsonFile.at("playerStart").at("x").get<int>();
        data.playerStart.y = jsonFile.at("playerStart").at("y").get<int>();

        if (jsonFile.contains("textures")) {
            const auto& textures = jsonFile.at("textures");
            if (textures.contains("floor")) data.defaultFloorTexture = textures.at("floor").get<std::string>();
            if (textures.contains("wall")) data.defaultWallTexture = textures.at("wall").get<std::string>();
        }

        if (jsonFile.contains("biomes")) {
            for (const auto& biomes : jsonFile.at("biomes")) {
                BiomeData biome;
                biome.name = biomes.value("name", std::string{"unnamed"});
                const auto& region = biomes.at("region");
                biome.region = TileRect{region.at("x").get<int>(), region.at("y").get<int>(), region.at("w").get<int>(), region.at("h").get<int>()};
                biome.floorTexture = biomes.at("floorTexture").get<std::string>();
                biome.wallTexture = biomes.at("wallTexture").get<std::string>();
                data.biomes.push_back(std::move(biome));
            }
        }

        for (const auto& wall : jsonFile.at("walls")) {
            data.walls.push_back(TileRect{
                wall.at("x").get<int>(),
                wall.at("y").get<int>(),
                wall.at("w").get<int>(),
                wall.at("h").get<int>()
            });
        }

        return data;
    }

    sf::Transform makeTiltTransform(const sf::Vector2f& pivot) {
        constexpr float rotationDeg = -18.f;
        constexpr float shearX = 0.45f;
        constexpr float scaleY = 0.70f;

        // Shear-Matrix: x' = x + shearX * y
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
}

int main() {
    MapData map;
    try {
        map = loadMapFromJsonFile("../GameMapConfig.json");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    sf::RenderWindow window(
        sf::VideoMode({960u, 540u}),
        "openDungeon",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    sf::View camera(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)}
    ));

    Floor floor;
    if (!floor.load(map.world, map.defaultFloorTexture, map.biomes)) {
        std::cerr << "Failed to load floor textures\n";
        return 1;
    }

    Walls walls;
    if (!walls.load(map.world, map.defaultWallTexture, map.biomes)) {
        std::cerr << "Failed to load wall textures\n";
        return 1;
    }

    for (const auto& w : map.walls) walls.addWallTiles(w);

    Player player(map.world);
    player.setTilePosition(map.playerStart);

    sf::Clock deltaClock;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                camera.setSize({
                    static_cast<float>(resized->size.x),
                    static_cast<float>(resized->size.y)
                });
            }
        }

        const float dt = deltaClock.restart().asSeconds();
        player.update(dt, walls);

        camera.setCenter(player.positionPx());
        window.setView(camera);

        sf::RenderStates worldStates = sf::RenderStates::Default;
        worldStates.transform = makeTiltTransform(camera.getCenter());

        const float playerScreenY = worldStates.transform.transformPoint(player.positionPx()).y;

        window.clear(sf::Color(20, 20, 28));
        floor.draw(window, worldStates);
        walls.drawBehindPlayer(window, worldStates, playerScreenY);
        player.draw(window, worldStates);
        walls.drawInFrontOfPlayer(window, worldStates, playerScreenY);
        window.display();
    }

    return 0;
}