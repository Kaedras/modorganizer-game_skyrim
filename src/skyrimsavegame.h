#ifndef SKYRIMSAVEGAME_H
#define SKYRIMSAVEGAME_H

#include "gamebryosavegame.h"

#ifdef __unix__
#include <linux/compatibility.h>
#else
#include <Windows.h>
#endif

namespace MOBase
{
class IPluginGame;
}

class GameSkyrim;

class SkyrimSaveGame : public GamebryoSaveGame
{
public:
  SkyrimSaveGame(QString const& fileName, GameSkyrim const* game);

protected:
  // Fetch easy-to-access information.
  void fetchInformationFields(FileWrapper& wrapper, uint32_t& saveNumber,
                              QString& playerName, uint16_t& playerLevel,
                              QString& playerLocation, FILETIME& creationTime) const;

  std::unique_ptr<DataFields> fetchDataFields() const override;
};

#endif  // SKYRIMSAVEGAME_H
