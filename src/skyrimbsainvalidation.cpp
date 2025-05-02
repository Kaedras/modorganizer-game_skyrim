#include "skyrimbsainvalidation.h"

SkyrimBSAInvalidation::SkyrimBSAInvalidation(MOBase::DataArchives* dataArchives,
                                             MOBase::IPluginGame const* game)
    : GamebryoBSAInvalidation(dataArchives, "skyrim.ini", game)
{}

QString SkyrimBSAInvalidation::invalidationBSAName() const
{
  return "Skyrim - Invalidation.bsa";
}

uint32_t SkyrimBSAInvalidation::bsaVersion() const
{
  return 0x68;
}
