#include "BMLoadout.h"
#include "pch.h"
#include "BakkesPluginTemplate1.h"

BAKKESMOD_PLUGIN(BakkesPluginTemplate1, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void BakkesPluginTemplate1::onLoad()
{
	_globalCvarManager = cvarManager;

  LOG("Plugin loaded");
  this->loadCVars();
  this->loadHooks();
}

void BakkesPluginTemplate1::loadCVars() {
  enabled = std::make_shared<bool>(true);
  overrideBothCars = std::make_shared<bool>(false);

  _globalCvarManager->registerCvar("plugin_enabled", "0", "Enable plugin", true, true, 0, true, 1)
    .bindTo(enabled);

  _globalCvarManager->registerCvar("override_both_cars", "0", "Override both cars", true, true, 0, true, 1)
    .bindTo(enabled);

  _globalCvarManager->registerNotifier("setRandomCarColor", [this](std::vector<std::string> args) {
      setRandomCarColor();
  }, "", PERMISSION_ALL);
}

void BakkesPluginTemplate1::loadHooks() {
  gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded",
    [this](std::string eventName) {
      if (enabled)
        setRandomCarColor();
    });
}

void BakkesPluginTemplate1::setRandomCarColor() {
  if (gameWrapper->IsInGame()) {
    LOG("Usable only in out of the game");
    return;
  }

  bool blueIsOrange = false;
  auto cvarBlueIsOrange = cvarManager->getCvar("override_both_cars");
  if (!cvarBlueIsOrange.IsNull())
    blueIsOrange = cvarBlueIsOrange.getBoolValue();

  BMLoadout::BMLoadout loadout;
  loadout.body.blue_is_orange = blueIsOrange;

  // Blue colors
  loadout.body.blueColor.should_override = true;
  loadout.body.blueColor.primary_colors.randomize();
  loadout.body.blueColor.secondary_colors.randomize();

  if (!blueIsOrange) {
    // Orange colors
    loadout.body.orangeColor.should_override = true;
    loadout.body.orangeColor.primary_colors.randomize();
    loadout.body.orangeColor.secondary_colors.randomize();
  }

  LOG("Color of the orange car: ({}, {}, {})",
      loadout.body.orangeColor.primary_colors.r,
      loadout.body.orangeColor.primary_colors.g,
      loadout.body.orangeColor.primary_colors.b
  );

  LOG("Color of the blue car: ({}, {}, {})",
      loadout.body.blueColor.primary_colors.r,
      loadout.body.blueColor.primary_colors.g,
      loadout.body.blueColor.primary_colors.b
  );

  // Generate code and apply
  setBMCode(BMLoadout::save(loadout));
}

void BakkesPluginTemplate1::setBMCode(std::string code) {
	_globalCvarManager->executeCommand("cl_itemmod_enabled 1; cl_itemmod_code \"" + code + "\"");
}

