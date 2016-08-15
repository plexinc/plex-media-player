//
// Created by Tobias Hieta on 15/08/16.
//

#include "StartupItemUtils.h"

#include "settings/SettingsSection.h"

/////////////////////////////////////////////////////////////////////////////////////////
StartupItemUtils::StartupItemUtils()
{
  connect(SettingsComponent::Get().getSection(SETTINGS_SECTION_MAIN), &SettingsSection::valuesUpdated,
          this, &StartupItemUtils::mainSettingsUpdated);
}

/////////////////////////////////////////////////////////////////////////////////////////
void StartupItemUtils::mainSettingsUpdated(const QVariantMap& map)
{
  if (map.contains("startOnLogin"))
    updateStartupItem();
}

/////////////////////////////////////////////////////////////////////////////////////////
void StartupItemUtils::updateStartupItem()
{
  if (SettingsComponent::Get().value(SETTINGS_SECTION_MAIN, "startOnLogin").toBool())
    addStartupItem();
  else
    removeStartupItem();
}
