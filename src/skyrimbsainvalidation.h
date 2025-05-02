#ifndef SKYRIMBSAINVALIDATION_H
#define SKYRIMBSAINVALIDATION_H

#include "gamebryobsainvalidation.h"
#include "skyrimdataarchives.h"

#include <memory>

class SkyrimBSAInvalidation : public GamebryoBSAInvalidation
{
public:
  SkyrimBSAInvalidation(MOBase::DataArchives* dataArchives,
                        MOBase::IPluginGame const* game);

private:
  virtual QString invalidationBSAName() const override;
  virtual uint32_t bsaVersion() const override;
};

#endif  // SKYRIMBSAINVALIDATION_H
