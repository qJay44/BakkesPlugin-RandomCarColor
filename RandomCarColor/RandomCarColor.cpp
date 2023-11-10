#include "BMLoadout.h"
#include "pch.h"
#include "RandomCarColor.h"

BAKKESMOD_PLUGIN(RandomCarColor, "Randomizes car colors every main menu enter", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void RandomCarColor::onLoad() {
	_globalCvarManager = cvarManager;
  srand((unsigned)time(NULL));

  this->loadCVars();
  this->loadHooks();
  this->loadLastLoadout();
  LOG("Plugin loaded");
}

void RandomCarColor::loadCVars() {
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

void RandomCarColor::loadHooks() {
  gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded",
    [this](std::string eventName) {
      setRandomCarColor();
  });
}

void RandomCarColor::loadLastLoadout() {
  if (gameWrapper->IsInGame()) {
    LOG("Usable only out of the game");
    return;
  }

  CVarWrapper lastLoadoutCodeCVar = _globalCvarManager->getCvar("last_loadout");
  if (lastLoadoutCodeCVar)
    setBMCode(lastLoadoutCodeCVar.getStringValue());
}

void RandomCarColor::setRandomCarColor() {
  CVarWrapper enabledCVar = _globalCvarManager->getCvar("plugin_enabled");
  if (enabledCVar)
    if (!enabledCVar.getBoolValue()) {
      LOG("Plugin disabled");
      return;
    }

  if (gameWrapper->IsInGame()) {
    LOG("Usable only out of the game");
    return;
  }

  bool overrideBothCars = false;
  CVarWrapper overrideBothCarsCVar = _globalCvarManager->getCvar("override_both_cars");
  if (overrideBothCarsCVar)
    overrideBothCars = overrideBothCarsCVar.getBoolValue();

  BMLoadout::BMLoadout loadout;
  loadout.body.blue_is_orange = overrideBothCars;

  // Blue team colors
  loadout.body.blueColor.should_override = true;
  loadout.body.blueColor.primary_colors.randomize();
  loadout.body.blueColor.secondary_colors.randomize();

  if (!overrideBothCars) {
    // Orange team colors
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

void RandomCarColor::setBMCode(std::string code) {
  _globalCvarManager->executeCommand("cl_itemmod_enabled 1; cl_itemmod_code \"" + code + "\"");
}

