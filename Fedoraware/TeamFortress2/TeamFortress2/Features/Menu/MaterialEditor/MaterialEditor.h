#pragma once
#include "../ImGui/TextEditor.h"

class CMaterialEditor {
	TextEditor TextEditor;
	std::string CurrentFile;

	bool EditorOpen = false;
	std::vector<const char*> MaterialList;
public:
	void MainWindow();
	void EditorWindow();
	void Render();
	void Init();

	bool IsOpen = false;
};

inline CMaterialEditor g_MaterialEditor;