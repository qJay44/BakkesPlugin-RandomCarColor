#include "pch.h"
#include "GuiBase.h"
#include "RandomCarColor.h"

void RandomCarColor::RenderSettings() {

  //======= Enable plugin option =======//

  CVarWrapper enableCvar = cvarManager->getCvar("plugin_enabled");
  if (enableCvar) {
    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox(enableCvar.getDescription().c_str(), &enabled)) {
      enableCvar.setValue(enabled);
      _globalCvarManager->executeCommand("writeconfig", false);
    }

    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Toggle plugin");
  }

  //======= Enable color override for both cars option =======//

  CVarWrapper overrideBothCars = cvarManager->getCvar("override_both_cars");
  if (overrideBothCars) {
    bool enableOverrideBothCars = overrideBothCars.getBoolValue();
    if (ImGui::Checkbox(overrideBothCars.getDescription().c_str(), &enableOverrideBothCars)) {
      overrideBothCars.setValue(enableOverrideBothCars);
      _globalCvarManager->executeCommand("writeconfig", false);
    }

    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Override colors for both orange and blue team");
  }
}

void SettingsWindowBase::SetImGuiContext(uintptr_t ctx) { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx)); }
void PluginWindowBase::SetImGuiContext(uintptr_t ctx)   { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx)); }

std::string SettingsWindowBase::GetPluginName() { return "RandomCarColor"; }
std::string PluginWindowBase::GetMenuName()     { return "RandomCarColor"; }
std::string PluginWindowBase::GetMenuTitle()    { return menuTitle_; }

bool PluginWindowBase::ShouldBlockInput() { return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard; }
bool PluginWindowBase::IsActiveOverlay()  { return true; }

void PluginWindowBase::OnOpen()  { isWindowOpen_ = true; }
void PluginWindowBase::OnClose() { isWindowOpen_ = false; }

void PluginWindowBase::Render() {
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None)) { // Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	RenderWindow();

	ImGui::End();

	if (!isWindowOpen_) {
		_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

