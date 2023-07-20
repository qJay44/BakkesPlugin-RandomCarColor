#include "pch.h"
#include "BakkesPluginTemplate1.h"


BAKKESMOD_PLUGIN(BakkesPluginTemplate1, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void BakkesPluginTemplate1::onLoad()
{
	_globalCvarManager = cvarManager;
  srand((unsigned)time(NULL));

  LOG("Plugin loaded");
  this->loadHooks();

  _globalCvarManager->registerCvar("plugin_enabled", "0", "Enable plugin", true, true, 0, true, 1)
    .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
      enabled = cvar.getBoolValue();
  });
}

void BakkesPluginTemplate1::loadHooks() {
  gameWrapper->HookEvent("Function TAGame.GFxData_Chat_TA.OnChatMessage",
    [this](std::string eventName) {
      if (enabled)
        randomCarColor();
    });
}

void BakkesPluginTemplate1::randomCarColor() {
  if (!gameWrapper->IsInFreeplay()) {
    LOG("Usable only in freeplay");
    return;
  }

  CarWrapper car = gameWrapper->GetLocalCar();
  if (!car) {
    LOG("Couldn't get the car");
    return;
  }

  float r = ((float) rand() / RAND_MAX);
  float g = ((float) rand() / RAND_MAX);
  float b = ((float) rand() / RAND_MAX);

	LinearColor lc = { r, g, b, 1.f };

	car.SetCarColor(lc, lc);
  LOG("Color of the car has been changed ({}, {}, {})", r, g, b);
}

