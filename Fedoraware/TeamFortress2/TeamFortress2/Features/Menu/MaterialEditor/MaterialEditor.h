#pragma once
#include "../ImGui/TextEditor.h"

struct CustomMaterial {
	std::wstring Path;
	std::string Name;
};

class CMaterialEditor {
	void LoadMaterials();
	void WriteMaterial(const CustomMaterial& material, const std::string& content);
	void MainWindow();
	void EditorWindow();

	TextEditor TextEditor;
	std::string CurrentFile;

	bool EditorOpen = false;
	std::vector<CustomMaterial> MaterialList;
	std::wstring MaterialFolder;
	CustomMaterial CurrentMaterial;

public:
	void Render();
	void Init();

	bool IsOpen = false;
};

inline CMaterialEditor g_MaterialEditor;