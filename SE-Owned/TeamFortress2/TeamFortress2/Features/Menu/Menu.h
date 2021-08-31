#pragma once

#include "../Vars.h"

class CMenu
{
private:
	Rect_t m_LastWidget	= {};
	Rect_t m_LastGroupBox = {};

	void Separator();
	bool CheckBox(CVar<bool> &Var, const wchar_t* const szToolTip);

	bool ComboBox(CVar<int> &Var, const std::vector<CVar<int>> &List);
	bool InputFloat(CVar<float> &Var, float Min, float Max, float Step = 1.0f, const wchar_t* Fmt = L"%f");
	bool InputInt(CVar<int> &Var, int Min, int Max, int Step = 1);
	bool InputColor(Color_t &Var, const wchar_t *Label);
	bool InputString(const wchar_t *Label, std::wstring &output);
	bool InputConstChar(const wchar_t *Label, std::string &output);
	bool InputCString(const wchar_t *Label, std::string &output);
	bool InputKey(CVar<int> &output, bool bAllowNone = true);
	void GroupBoxStart();
	void GroupBoxEnd(const wchar_t *Label, int Width);
	void DrawTooltip();

private:
	bool m_bReopened = false;
	std::wstring m_szCurTip = L"";

public:
	bool Button(const wchar_t* Label, bool Active = false, int WidthOverride = 0, int HeightOverride = 0);
	float m_flFadeAlpha		= 0.0f;
	float m_flFadeElapsed	= 0.0f;
	float m_flFadeDuration	= 0.1f;

public:
	bool m_bOpen = false;
	bool m_bTyping = false;
	void Run();
};

inline CMenu g_Menu;