#pragma once
#include "../../../SDK/SDK.h"
#include "../ImGui/TextEditor.h"

struct CustomMaterial
{
	std::string FileName = "Default.vmt";
	IMaterial* Material = nullptr;
};

class CMaterialEditor
{
	std::string GetMaterialPath(const std::string& matFileName);
	void LoadMaterials();
	void WriteMaterial(const CustomMaterial& material, const std::string& content);
	void MainWindow();
	void EditorWindow();

	TextEditor TextEditor;
	std::string CurrentFile;

	bool EditorOpen = false;
	std::string MaterialFolder;
	CustomMaterial CurrentMaterial;

public:
	void Render();
	void Init();
	IMaterial* GetByName(const std::string& name);

	bool IsOpen = false;
	std::unordered_map<std::string, CustomMaterial> MaterialMap;
};

ADD_FEATURE(CMaterialEditor, MaterialEditor)