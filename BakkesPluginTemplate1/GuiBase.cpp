#include "pch.h"
#include "GuiBase.h"
#include "BakkesPluginTemplate1.h"

void BakkesPluginTemplate1::RenderSettings() {
  ImGui::TextUnformatted("Sample text");

  //======= Enable option =======//

  CVarWrapper enableCvar = cvarManager->getCvar("plugin_enabled");
  if (!enableCvar) return;

  bool enabled = enableCvar.getBoolValue();
  if (ImGui::Checkbox(enableCvar.getDescription().c_str(), &enabled)) {
    enableCvar.setValue(enabled);
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Toggle plugin");
  }

  //===========================//
}

std::string SettingsWindowBase::GetPluginName()
{
	return "BakkesPluginTemplate1";
}

void SettingsWindowBase::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

std::string PluginWindowBase::GetMenuName()
{
	return "BakkesPluginTemplate1";
}

std::string PluginWindowBase::GetMenuTitle()
{
	return menuTitle_;
}

void PluginWindowBase::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

bool PluginWindowBase::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

bool PluginWindowBase::IsActiveOverlay()
{
	return true;
}

void PluginWindowBase::OnOpen()
{
	isWindowOpen_ = true;
}

void PluginWindowBase::OnClose()
{
	isWindowOpen_ = false;
}

void PluginWindowBase::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	RenderWindow();

	ImGui::End();

	if (!isWindowOpen_)
	{
		_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}
