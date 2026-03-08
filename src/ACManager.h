#ifndef ACMANAGER_H
#define ACMANAGER_H

#include <string>
#include <vector>

class ACManager
{
public:
    ACManager(const std::string& gamePath);

    std::vector<std::string> listCars();
    std::vector<std::string> listTracks();

    void launchGame();

    void setACPath(const std::string& path);
    void setSteamPath(const std::string& path);
    std::string getACPath() const { return acPath; }
    std::string getSteamPath() const { return steamPathOverride; }

    void loadConfig();
    void saveConfig();

private:
    std::string acPath;
    std::string steamPathOverride;

};

#endif // ACMANAGER_H