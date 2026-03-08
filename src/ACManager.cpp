#include "ACManager.h"
#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void ACManager::saveConfig()
{
    std::string configPath = std::string(getenv("HOME")) + "/.config/OpenACLauncher/config.json";
    std::filesystem::path configDir = std::filesystem::path(configPath).parent_path();

    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }

    json j;
    j["acPath"] = acPath;
    j["steamPath"] = steamPathOverride;

    std::ofstream file(configPath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error saving settings in " << configPath << std::endl;
        return;
    }

    file << j.dump(4) << "\n";
    file.close();
}

void ACManager::loadConfig()
{
    std::string configPath = std::string(getenv("HOME")) + "/.config/OpenACLauncher/config.json";

    if (!std::filesystem::exists(configPath)) return;

    std::ifstream file(configPath);
    if (!file.is_open()) return;

    try {
        json j;
        file >> j;

        if (j.contains("acPath") && j["acPath"].is_string()) 
            acPath = j["acPath"].get<std::string>();

        if (j.contains("steamPath") && j["steamPath"].is_string()) 
            steamPathOverride = j["steamPath"].get<std::string>();
    } catch (json::parse_error& e) {
        std::cerr << "Error parsing configuration: " << e.what() << std::endl;
        acPath.clear();
        steamPathOverride.clear();
    }
}

void ACManager::setACPath(const std::string& path)
{
    acPath = path;
}

void ACManager::setSteamPath(const std::string& path)
{
    steamPathOverride = path;
}

ACManager::ACManager(const std::string& gamePath)
    : acPath(gamePath)
{}

std::vector<std::string> ACManager::listCars()
{
    std::vector<std::string> cars;
    std::filesystem::path carsPath = acPath + "/content/cars";

    std::cout << "Looking for cars in: " << carsPath << std::endl;

    if (std::filesystem::exists(carsPath))
    {
        for (const auto& entry : std::filesystem::directory_iterator(carsPath))
        {
            cars.push_back(entry.path().filename().string());
        }
    }

    return cars;
}

std::vector<std::string> ACManager::listTracks()
{
    std::vector<std::string> tracks;
    std::filesystem::path tracksPath = acPath + "/content/tracks";

    if (std::filesystem::exists(tracksPath))
    {
        for (const auto& entry : std::filesystem::directory_iterator(tracksPath))
        {
            tracks.push_back(entry.path().filename().string());
        }
    }

    return tracks;
}

void ACManager::launchGame()
{
    std::string cmd;

    if (std::filesystem::exists("/usr/bin/steam") || std::filesystem::exists("/bin/steam"))
    {
        cmd = "steam steam://run/244210";
    }
    else
    {

        int ret = system("flatpak list | grep -q com.valvesoftware.Steam");
        if (ret == 0)
        {
            cmd = "flatpak run com.valvesoftware.Steam steam://run/244210";
        }
        else
        {
            std::cerr << "Error: Steam not found on the system (neither normal nor flatpak)." << std::endl;
            return;
        }
    }

    std::cout << "Launching Assetto Corsa using command: " << cmd << std::endl;
    system(cmd.c_str());
}