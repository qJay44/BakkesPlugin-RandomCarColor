#include "BMLoadout.h"
#include "pch.h"
#include "BakkesPluginTemplate1.h"
#include <stdlib.h>

BAKKESMOD_PLUGIN(BakkesPluginTemplate1, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void BakkesPluginTemplate1::onLoad()
{
	_globalCvarManager = cvarManager;
  srand((unsigned)time(NULL));

  LOG("Plugin loaded");
  this->loadCVars();
  this->loadHooks();
  this->loadLastLoadout();
}

void BakkesPluginTemplate1::loadCVars() {
  enabledPtr = std::make_shared<bool>(true);
  overrideBothCarsPtr = std::make_shared<bool>(false);
	lastLoadoutCodePtr = std::make_shared<std::string>("");

  _globalCvarManager->registerCvar("plugin_enabled", "1", "Enable plugin", true, true, 0, true, 1)
    .bindTo(enabledPtr);

  _globalCvarManager->registerCvar("override_both_cars", "0", "Override both cars", true, true, 0, true, 1)
    .bindTo(overrideBothCarsPtr);

  _globalCvarManager->registerCvar("last_loadout", "")
    .bindTo(lastLoadoutCodePtr);

  _globalCvarManager->registerNotifier("setRandomCarColor", [this](std::vector<std::string> args) {
      setRandomCarColor();
  }, "", PERMISSION_ALL);

  _globalCvarManager->registerNotifier("loadLastLoadout", [this](std::vector<std::string> args) {
      loadLastLoadout();
  }, "", PERMISSION_ALL);
}

void BakkesPluginTemplate1::loadHooks() {
  gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded",
    [this](std::string eventName) {
      CVarWrapper enabledCVar = _globalCvarManager->getCvar("plugin_enabled");
      if (enabledCVar) {
        if (enabledCVar.getBoolValue())
            setRandomCarColor();
        else
          LOG("Plugin disabled");
      }
    });
}

void BakkesPluginTemplate1::loadLastLoadout() {
  if (gameWrapper->IsInGame()) {
    LOG("Usable only out of the game");
    return;
  }

  CVarWrapper lastLoadoutCodeCVar = _globalCvarManager->getCvar("last_loadout");
  if (lastLoadoutCodeCVar)
    setBMCode(lastLoadoutCodeCVar.getStringValue());
}

void BakkesPluginTemplate1::setRandomCarColor() {
  if (gameWrapper->IsInGame()) {
    LOG("Usable only out of the game");
    return;
  }

  bool overrideBothCars = false;
  CVarWrapper overrideBothCarsCVar = _globalCvarManager->getCvar("override_both_cars");
  if (overrideBothCarsCVar)
    overrideBothCars = overrideBothCarsCVar.getBoolValue();

  BMLoadout::BMLoadout loadout;
  loadout.body.blue_is_orange = *overrideBothCarsPtr.get();

  // Blue colors
  loadout.body.blueColor.should_override = true;
  loadout.body.blueColor.primary_colors.randomize();
  loadout.body.blueColor.secondary_colors.randomize();

  if (!overrideBothCars) {
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

  std::string loadoutCode = BMLoadout::save(loadout);
  CVarWrapper lastLoadoutCodeCVar = _globalCvarManager->getCvar("last_loadout");
  if (lastLoadoutCodeCVar)
    lastLoadoutCodeCVar.setValue(loadoutCode);

  // Generate code and apply
  setBMCode(loadoutCode);

  // Save CVars to config
  _globalCvarManager->executeCommand("writeconfig", false);
}

void BakkesPluginTemplate1::setBMCode(std::string code) {
  _globalCvarManager->executeCommand("cl_itemmod_enabled 1; cl_itemmod_code \"" + code + "\"");
}

