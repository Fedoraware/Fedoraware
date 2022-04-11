#pragma once
#include "../../../SDK/SDK.h"
#include "../ImGui/TextEditor.h"

struct CustomMaterial {
	std::string Name = "Default";
	std::wstring FileName = L"Default.vmt";
	IMaterial* Material{ };
};

class CMaterialEditor {
	std::wstring GetMaterialPath(const std::wstring& matFileName);
	void LoadMaterials();
	void WriteMaterial(const CustomMaterial& material, const std::string& content);
	void MainWindow();
	void EditorWindow();

	TextEditor TextEditor;
	std::string CurrentFile;

	bool EditorOpen = false;
	std::wstring MaterialFolder;
	CustomMaterial CurrentMaterial;

public:
	void Render();
	void Init();
	IMaterial* GetByName(const std::string& name);

	bool IsOpen = false;
	std::vector<CustomMaterial> MaterialList;
};

inline CMaterialEditor g_MaterialEditor;