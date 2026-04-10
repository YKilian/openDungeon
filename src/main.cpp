#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <vector>
#include <optional>
#include <string>

#include "Player.hpp"
#include "Floor.hpp"
#include "Walls.hpp"
#include "Config.hpp"
#include "Utils.hpp"

namespace {
    using json = nlohmann::json;

    /**
     * Checks if the given json file is a valid GameMapConfig.
     * @param jsonFile json file to check
     * @return bool True if the jsonFile is a valid GameMapConfig, false otherwise
     */
    bool isValidGameMapConfig(json jsonFile) {
        // TODO - Check the syntax and semantic of the jsonFile
        const bool mapExists = jsonFile.contains("map");
        const bool playerStartExists = jsonFile.contains("playerStart");
        const bool wallsExist = jsonFile.contains("walls");
        return mapExists && playerStartExists && wallsExist;
    }

    /**
     * Parses the given json file and returns a MapData struct with the data from the json file.
     * @param path Path to the json file to parse
     * @return MapData struct with the data from the json file
     */
    MapData loadMapFromJsonFile(const std::string& path) {
        // Validation process
        std::ifstream in(path);
        if (!in) throw std::runtime_error("Cannot open map json: " + path);

        json jsonConfig;
        in >> jsonConfig;

        if (!isValidGameMapConfig(jsonConfig)) {
            throw std::runtime_error("Invalid map json: " + path);
        }

        // Parsing process
        MapData data;

        data.world.tilesX = jsonConfig.at("map").at("tilesX").get<int>();
        data.world.tilesY = jsonConfig.at("map").at("tilesY").get<int>();

        data.playerStart.x = jsonConfig.at("playerStart").at("x").get<int>();
        data.playerStart.y = jsonConfig.at("playerStart").at("y").get<int>();

        // Load the default textures first, so that biomes can override them if needed.
        if (jsonConfig.contains("textures")) {
            const nlohmann::basic_json<> & textures = jsonConfig.at("textures");
            if (textures.contains("floor")) data.defaultFloorTexture = textures.at("floor").get<std::string>();
            if (textures.contains("wall")) data.defaultWallTexture = textures.at("wall").get<std::string>();
        }

        // Now load the biomes, which can override the default textures.
        if (jsonConfig.contains("biomes")) {
            for (const nlohmann::basic_json<> & biomes : jsonConfig.at("biomes")) {
                BiomeData biome;
                biome.name = biomes.value("name", std::string{"unnamed"});
                const nlohmann::basic_json<> & region = biomes.at("region");
                // The region is defined by its top-left corner (x, y) and its width (w) and height (h).
                biome.region = TileRect{
                    region.at("x").get<int>(),
                    region.at("y").get<int>(),
                    region.at("w").get<int>(),
                    region.at("h").get<int>()
                };
                biome.floorTexture = biomes.at("floorTexture").get<std::string>();
                biome.wallTexture = biomes.at("wallTexture").get<std::string>();
                // Biomes can overlap and override each other, so there can be biomes in biomes.
                // Biomes are getting pushed in the order they are defined in the json, so that later biomes can override earlier ones.
                data.biomes.push_back(std::move(biome));
            }
        }

        // Load the walls, which are defined by their top-left corner (x, y) and their width (w) and height (h).
        for (const nlohmann::basic_json<> & wall : jsonConfig.at("walls")) {
            data.walls.push_back(TileRect{
                wall.at("x").get<int>(),
                wall.at("y").get<int>(),
                wall.at("w").get<int>(),
                wall.at("h").get<int>()
            });
        }

        return data;
    }
}

int main() {
    MapData map;

    try {
        map = loadMapFromJsonFile("./GameMapConfig.json");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    sf::RenderWindow window(
        sf::VideoMode({960u, 540u}),
        "openDungeon",
        sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize
    );
    window.setVerticalSyncEnabled(false); // VSync can cause stuttering when the frame time is close to the refresh rate, so it is disabled for smoother gameplay.

    sf::View camera(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)}
    ));

    Floor floor;
    if (!floor.load(map.world, map.lightData, map.defaultFloorTexture, map.biomes)) {
        std::cerr << "Failed to load floor textures\n";
        return 1;
    }

    Walls walls;
    if (!walls.load(map.world, map.defaultWallTexture, map.biomes)) {
        std::cerr << "Failed to load wall textures\n";
        return 1;
    }

    for (const TileRect wall : map.walls) walls.addWallTiles(wall);

    Player player(map.world);
    player.setTilePosition(map.playerStart);

    sf::Clock deltaClock; // Clock to measure the time between frames for smooth movement and animations.

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto *resized  = event->getIf<sf::Event::Resized>()) {
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

        const sf::Vector2f playerScreenPos = worldStates.transform.transformPoint(player.positionPx());

        window.clear(sf::Color(20, 20, 28));
        floor.draw(window, worldStates);
        walls.drawBehindPlayer(window, worldStates, playerScreenPos);
        player.draw(window, worldStates);
        walls.drawInFrontOfPlayer(window, worldStates, playerScreenPos);
        window.display();
    }

    return 0;
}