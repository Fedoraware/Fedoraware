#include "MaterialEditor.h"

#include <filesystem>
#include <fstream>

#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_stdlib.h"
#include "../ConfigManager/ConfigManager.h"

const std::string defaultMaterial = "\"VertexLitGeneric\"\n{\n}";

std::wstring CMaterialEditor::GetMaterialPath(const std::wstring& matFileName)
{
	std::wstring matPath = MaterialFolder + L"\\" + matFileName;
	return matPath;
}

IMaterial* CMaterialEditor::GetByName(const std::string& name)
{
	const auto fMat = std::find_if(MaterialList.begin(), MaterialList.end(), [&name](const CustomMaterial& mat) { return mat.Name == name; });
	if (fMat != MaterialList.end())
	{
		return fMat->Material;
	}

	return nullptr;
}

/* Reloads all material files and creates the corresponding material */
void CMaterialEditor::LoadMaterials()
{
	MaterialList.clear();

	// Load material files
	for (const auto& entry : std::filesystem::directory_iterator(MaterialFolder))
	{
		if (std::string(std::filesystem::path(entry).filename().string()).find(".vmt") == std::string_view::npos)
		{
			continue;
		}

		// Get the material name
		std::wstring wMatFile = entry.path().filename().wstring();
		// .erase(wMatName.end() - 4, wMatName.end());
		std::string matName(wMatFile.begin(), wMatFile.end());
		matName.erase(matName.end() - 4, matName.end());

		// Create Material
		std::ifstream inStream(entry.path());
		if (inStream.good())
		{
			const std::string str((std::istreambuf_iterator(inStream)), std::istreambuf_iterator<char>());
			const auto kv = new KeyValues(matName.c_str());
			
			g_KeyValUtils.LoadFromBuffer(kv, matName.c_str(), str.c_str());
			IMaterial* newMaterial = g_Interfaces.MatSystem->Create(matName.c_str(), kv);
			MaterialList.push_back({ matName, wMatFile, newMaterial });
		}
	}
}

/* Writes the given material to it's file */
void CMaterialEditor::WriteMaterial(const CustomMaterial& material, const std::string& content)
{
	std::ofstream outStream(GetMaterialPath(material.FileName));
	outStream << content;
	outStream.close();
}

void CMaterialEditor::MainWindow()
{
	if (!IsOpen) { return; }
	using namespace ImGui;

	SetNextWindowSize(ImVec2(400, 380), ImGuiCond_Once);
	if (Begin("Material Manager", &IsOpen, ImGuiWindowFlags_NoCollapse))
	{
		// Toolbar
		{
			if (Button("Reload"))
			{
				LoadMaterials();
			}

			SameLine();
			if (Button("Edit"))
			{
				if (std::filesystem::exists(GetMaterialPath(CurrentMaterial.FileName)))
				{
					std::ifstream inStream(GetMaterialPath(CurrentMaterial.FileName));
					if (inStream.good())
					{
						const std::string str((std::istreambuf_iterator(inStream)), std::istreambuf_iterator<char>());
						TextEditor.SetText(str);
						EditorOpen = true;
					}
				}
			}

			SameLine();
			if (Button("Remove") && !GetMaterialPath(CurrentMaterial.FileName).empty())
			{
				std::filesystem::remove(GetMaterialPath(CurrentMaterial.FileName));
				LoadMaterials();
			}

			SameLine();
			if (Button("Open Folder") && !GetMaterialPath(CurrentMaterial.FileName).empty())
			{
				ShellExecuteW(nullptr, L"open", MaterialFolder.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
			}
		}

		PushItemWidth(GetWindowSize().x - 2 * GetStyle().WindowPadding.x);

		// New material input
		static std::string newName;
		if (InputTextWithHint("###MaterialName", "New Material name", &newName, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			const std::wstring outstring(newName.begin(), newName.end());
			if (!std::filesystem::exists(GetMaterialPath(outstring + L".vmt")))
			{
				// Create a new CustomMaterial and add it to our list
				const auto kv = new KeyValues(newName.c_str());
				g_KeyValUtils.LoadFromBuffer(kv, newName.c_str(), defaultMaterial.c_str());
				IMaterial* defMaterial = g_Interfaces.MatSystem->Create(newName.c_str(), kv);

				const CustomMaterial newMaterial = { newName, outstring + L".vmt", defMaterial };

				WriteMaterial(newMaterial, defaultMaterial);
				MaterialList.push_back(newMaterial);
				LoadMaterials();

				newName.clear();
			}
		}

		// Material list
		if (BeginChild("ListChild"))
		{
			if (ListBoxHeader("###MaterialList", { GetWindowWidth(), GetWindowHeight() }))
			{
				for (auto const& mat : MaterialList)
				{
					if (Selectable(mat.Name.c_str(), CurrentMaterial.Name == mat.Name))
					{
						CurrentMaterial = mat;
						EditorOpen = false;
					}
				}

				ListBoxFooter();
			}
		}
		EndChild();

		PopItemWidth();
		End();
	}
}

void CMaterialEditor::EditorWindow()
{
	if (!EditorOpen || !IsOpen) { return; }
	using namespace ImGui;

	SetNextWindowSize(ImVec2(450, 400), ImGuiCond_Once);
	PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	if (Begin("Material Editor", &EditorOpen, ImGuiWindowFlags_NoCollapse))
	{
		// Toolbar
		{
			if (Button("Save"))
			{
				WriteMaterial(CurrentMaterial, TextEditor.GetText());
				LoadMaterials();
			}

			SameLine();
			if (Button("Save and close"))
			{
				WriteMaterial(CurrentMaterial, TextEditor.GetText());
				LoadMaterials();
				EditorOpen = false;
			}

			Text("Editing: %s", CurrentMaterial.Name.c_str());
		}

		// Text editor
		TextEditor.Render("TextEditor");

		End();
	}
	PopStyleVar();
}

void CMaterialEditor::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(10, 10));
	MainWindow();
	EditorWindow();
	ImGui::PopStyleVar(2);
}

void CMaterialEditor::Init()
{
	MaterialFolder = g_CFG.m_sConfigPath + L"\\Materials";
	LoadMaterials();
}