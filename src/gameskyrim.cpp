#include "gameskyrim.h"

#include "skyrimbsainvalidation.h"
#include "skyrimdataarchives.h"
#include "skyrimgameplugins.h"
#include "skyrimmoddatachecker.h"
#include "skyrimmoddatacontent.h"
#include "skyrimsavegame.h"
#include "skyrimscriptextender.h"

#include "executableinfo.h"
#include "pluginsetting.h"

#include <gamebryogameplugins.h>
#include <gamebryolocalsavegames.h>
#include <gamebryosavegameinfo.h>
#include <gamebryounmanagedmods.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QVersionNumber>
#include <QtDebug>

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef __unix__
// use AppData folder inside the wine prefix
static const QString appID = QStringLiteral("72850");
#define LOCAL_APP_FOLDER localAppFolder(appID)
#else
#define LOCAL_APP_FOLDER localAppFolder()
#endif

using namespace MOBase;

GameSkyrim::GameSkyrim() {}

bool GameSkyrim::init(IOrganizer* moInfo)
{
  if (!GameGamebryo::init(moInfo)) {
    return false;
  }

  auto dataArchives = std::make_shared<SkyrimDataArchives>(this);
  registerFeature(std::make_shared<SkyrimScriptExtender>(this));
  registerFeature(dataArchives);
  registerFeature(std::make_shared<SkyrimBSAInvalidation>(dataArchives.get(), this));
  registerFeature(std::make_shared<GamebryoSaveGameInfo>(this));
  registerFeature(std::make_shared<GamebryoLocalSavegames>(this, "Skyrim.ini"));
  registerFeature(std::make_shared<SkyrimModDataChecker>(this));
  registerFeature(std::make_shared<SkyrimModDataContent>(m_Organizer->gameFeatures()));
  registerFeature(std::make_shared<SkyrimGamePlugins>(moInfo));
  registerFeature(std::make_shared<GamebryoUnmangedMods>(this));

  return true;
}

QString GameSkyrim::gameName() const
{
  return "Skyrim";
}

QList<ExecutableInfo> GameSkyrim::executables() const
{
  return QList<ExecutableInfo>()
         << ExecutableInfo("SKSE",
                           findInGameFolder(m_Organizer->gameFeatures()
                                                ->gameFeature<MOBase::ScriptExtender>()
                                                ->loaderName()))
         << ExecutableInfo("SBW", findInGameFolder("SBW.exe"))
         << ExecutableInfo("Skyrim", findInGameFolder(binaryName()))
         << ExecutableInfo("Skyrim Launcher", findInGameFolder(getLauncherName()))
         << ExecutableInfo("BOSS", findInGameFolder("BOSS/BOSS.exe"))
         << ExecutableInfo("LOOT", QFileInfo(getLootPath()))
                .withArgument("--game=\"Skyrim\"")
         << ExecutableInfo("Creation Kit", findInGameFolder("CreationKit.exe"))
                .withSteamAppId("202480");
}

QList<ExecutableForcedLoadSetting> GameSkyrim::executableForcedLoads() const
{
  return QList<ExecutableForcedLoadSetting>();
}

QString GameSkyrim::name() const
{
  return "Skyrim Support Plugin";
}

QString GameSkyrim::localizedName() const
{
  return tr("Skyrim Support Plugin");
}

QString GameSkyrim::author() const
{
  return "Tannin & MO2 Team";
}

QString GameSkyrim::description() const
{
  return tr("Adds support for the game Skyrim");
}

MOBase::VersionInfo GameSkyrim::version() const
{
  return VersionInfo(1, 6, 1, VersionInfo::RELEASE_FINAL);
}

QList<PluginSetting> GameSkyrim::settings() const
{
  QList<PluginSetting> results;
  results.push_back(
      PluginSetting("sse_downloads", "allow Skyrim SE downloads", QVariant(false)));
  return results;
}

void GameSkyrim::initializeProfile(const QDir& path, ProfileSettings settings) const
{
  if (settings.testFlag(IPluginGame::MODS)) {
    copyToProfile(LOCAL_APP_FOLDER + "/Skyrim", path, "plugins.txt");
  }

  if (settings.testFlag(IPluginGame::CONFIGURATION)) {
    if (settings.testFlag(IPluginGame::PREFER_DEFAULTS) ||
        !QFileInfo(myGamesPath() + "/Skyrim.ini").exists()) {
      copyToProfile(gameDirectory().absolutePath(), path, "Skyrim_default.ini",
                    "Skyrim.ini");
    } else {
      copyToProfile(myGamesPath(), path, "Skyrim.ini");
    }

    copyToProfile(myGamesPath(), path, "SkyrimPrefs.ini");
  }
}

QString GameSkyrim::savegameExtension() const
{
  return "ess";
}

QString GameSkyrim::savegameSEExtension() const
{
  return "skse";
}

std::shared_ptr<const GamebryoSaveGame> GameSkyrim::makeSaveGame(QString filepath) const
{
  return std::make_shared<const SkyrimSaveGame>(filepath, this);
}

QString GameSkyrim::steamAPPId() const
{
  return "72850";
}

QStringList GameSkyrim::primaryPlugins() const
{
  return {"Skyrim.esm", "Update.esm"};
}

QString GameSkyrim::binaryName() const
{
  return "TESV.exe";
}

QString GameSkyrim::gameShortName() const
{
  return "Skyrim";
}

QString GameSkyrim::gameNexusName() const
{
  return "skyrim";
}

QStringList GameSkyrim::validShortNames() const
{
  QStringList results;
  if (m_Organizer->pluginSetting(name(), "sse_downloads").toBool()) {
    results.push_back("SkyrimSE");
  }
  return results;
}

QStringList GameSkyrim::iniFiles() const
{
  return {"Skyrim.ini", "SkyrimPrefs.ini"};
}

QStringList GameSkyrim::DLCPlugins() const
{
  return {"Dawnguard.esm",
          "Dragonborn.esm",
          "HearthFires.esm",
          "HighResTexturePack01.esp",
          "HighResTexturePack02.esp",
          "HighResTexturePack03.esp"};
}

IPluginGame::LoadOrderMechanism GameSkyrim::loadOrderMechanism() const
{
  try {
    QString versionString =
        getFileVersion(gameDirectory().absoluteFilePath(binaryName()));
    QVersionNumber version = QVersionNumber::fromString(versionString);

    static const auto v1_5 = QVersionNumber(1, 5);

    // version >= 1.5.x || version >= x.4.26
    if (version >= v1_5 ||
        (version.minorVersion() >= 4 && version.microVersion() >= 26)) {
      return LoadOrderMechanism::PluginsTxt;
    }
  } catch (const std::exception& e) {
    qCritical() << "TESV.exe is invalid: " << e.what();
  }
  return LoadOrderMechanism::FileTime;
}

int GameSkyrim::nexusModOrganizerID() const
{
  return 0;
}

int GameSkyrim::nexusGameID() const
{
  return 110;
}
