#include "MaterialEditor.h"

#include <filesystem>
#include <fstream>

#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_stdlib.h"
#include "../ConfigManager/ConfigManager.h"
#include "../../Chams/DMEChams.h"

const std::string DEFAULT_MATERIAL = "\"VertexLitGeneric\"\n{\n}";

std::string CMaterialEditor::GetMaterialPath(const std::string& matFileName)
{
	std::string matPath = MaterialFolder + "\\" + matFileName;
	return matPath;
}

IMaterial* CMaterialEditor::GetByName(const std::string& name)
{
	return MaterialMap[name].Material;
}

/* Reloads all material files and creates the corresponding material */
void CMaterialEditor::LoadMaterials()
{
	MaterialMap.clear();

	// Load material files
	for (const auto& entry : std::filesystem::directory_iterator(MaterialFolder))
	{
		// Ignore all non-Material files
		if (std::filesystem::path(entry).filename().string().find(".vmt") == std::string_view::npos)
		{
			continue;
		}

		// Get the material name
		const std::string matPath = entry.path().filename().string();
		std::string matName = matPath;
		matName.erase(matName.end() - 4, matName.end());

		// Create Material
		std::ifstream inStream(entry.path());
		if (inStream.good())
		{
			const std::string str((std::istreambuf_iterator(inStream)), std::istreambuf_iterator<char>());
			const auto kv = new KeyValues(matName.c_str());

			g_KeyValUtils.LoadFromBuffer(kv, matName.c_str(), str.c_str());
			IMaterial* newMaterial = F::DMEChams.CreateNRef(std::string("m_pmat" + matName).c_str(), kv);
			MaterialMap[matName] = { matPath, newMaterial };
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
			if (Button("Refresh"))
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
			if (Button("Open Folder"))
			{
				ShellExecuteA(nullptr, "open", MaterialFolder.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
			}
		}

		PushItemWidth(GetWindowSize().x - 2 * GetStyle().WindowPadding.x);

		// New material input
		static std::string newName;
		if (InputTextWithHint("###MaterialName", "New Material name", &newName, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			if (!std::filesystem::exists(GetMaterialPath(newName + ".vmt")))
			{
				// Create a new CustomMaterial and add it to our map
				const auto kv = new KeyValues(newName.c_str());
				g_KeyValUtils.LoadFromBuffer(kv, newName.c_str(), DEFAULT_MATERIAL.c_str());
				IMaterial* defMaterial = F::DMEChams.CreateNRef(std::string("m_pmat" + newName).c_str(), kv);

				const CustomMaterial newMaterial = { newName + ".vmt", defMaterial };

				WriteMaterial(newMaterial, DEFAULT_MATERIAL);
				MaterialMap[newName] = newMaterial;
				LoadMaterials();

				newName.clear();
			}
		}

		// Material list
		if (BeginChild("ListChild###Materials"))
		{
			if (ListBoxHeader("###MaterialList", { GetWindowWidth(), GetWindowHeight() }))
			{
				for (auto const& [name, mat] : MaterialMap)
				{
					if (name.empty()) { continue; }

					if (Selectable(name.c_str(), CurrentMaterial.FileName == mat.FileName))
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

			SameLine();
			if (Button("Cancel"))
			{
				LoadMaterials();
				EditorOpen = false;
			}

			Text("Editing: %s", CurrentMaterial.FileName.c_str());
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
	MaterialFolder = g_CFG.GetConfigPath() + "\\Materials";
	LoadMaterials();
}