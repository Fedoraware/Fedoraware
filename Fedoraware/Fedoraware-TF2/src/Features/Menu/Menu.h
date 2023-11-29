#pragma once
#include "../Feature.h"

#include <ImGui/imgui_color_gradient.h>

class CMenu
{
	void DrawMenu();
	void DrawTabbar();
	void MenuAimbot();
	void MenuTrigger();
	void MenuVisuals();
	void MenuHvH();
	void MenuMisc();
	void SettingsWindow();
	void MenuDesignWindow();
	void DebugMenu();
	void DrawCameraWindow();
	void DrawCritDrag();


	void AddDraggable(const char* szTitle, DragBox_t& info, bool bShouldDraw, bool setSize = false);

	void DrawKeybinds();
	void LoadStyle();

	enum class MenuTab
	{
		Aimbot,
		Trigger,
		Visuals,
		HvH,
		Misc
	};

	enum class VisualsTab
	{
		ESP,
		Glow,
		Chams,
		Font,
		World,
		Indicators,
		Radar
	};

	enum class ConfigTab
	{
		General,
		Visuals
	};

	MenuTab CurrentTab = MenuTab::Aimbot;
	VisualsTab CurrentVisualsTab = VisualsTab::ESP;
	ConfigTab CurrentConfigTab = ConfigTab::General;

	bool ShowSettings = false;
	bool ShowDesignerWindow = false;
	bool ShowDebugMenu = false;

public:
	void Render(IDirect3DDevice9* pDevice);
	void Init(IDirect3DDevice9* pDevice);

	bool IsOpen = false;
	bool ConfigLoaded = false;
	bool Unload = false;

	float TitleHeight = 22.f;
	float TabHeight = 30.f;
	float SubTabHeight = 0.f;
	float FooterHeight = 20.f;
	float ItemWidth = 130.f;

	std::string FeatureHint;

	// Fonts
	ImFont* VerdanaSmall = nullptr;	// 12px
	ImFont* Verdana = nullptr;		// 14px
	ImFont* VerdanaBold = nullptr;	// 18px

	ImFont* SectionFont = nullptr;	// 16px
	ImFont* TitleFont = nullptr;	// 20px
	ImFont* IconFont = nullptr;		// 16px

	// Gradients
	ImGradient TitleGradient;
	ImGradient MainGradient;
	ImGradient TabGradient;
};

ADD_FEATURE(CMenu, Menu)
