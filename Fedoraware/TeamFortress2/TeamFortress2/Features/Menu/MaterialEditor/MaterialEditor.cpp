#include "MaterialEditor.h"

#include <filesystem>
#include <fstream>

#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_stdlib.h"
#include "../ConfigManager/ConfigManager.h"

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

		std::wstring wMatName = entry.path().filename().wstring();
		wMatName.erase(wMatName.end() - 4, wMatName.end());
		const std::string matName(wMatName.begin(), wMatName.end());
		MaterialList.push_back({ entry.path().wstring(), matName });
	}
}

void CMaterialEditor::WriteMaterial(const CustomMaterial& material, const std::string& content)
{
	std::ofstream outfile(material.Path);
	outfile << content;
	outfile.close();
}

void CMaterialEditor::MainWindow()
{
	if (!IsOpen) { return; }
	using namespace ImGui;

	SetNextWindowSize(ImVec2(400, 450), ImGuiCond_Once);
	if (Begin("Material Manager", &IsOpen, ImGuiWindowFlags_NoCollapse))
	{
		static CustomMaterial selectedMat;

		if (Button("Reload"))
		{
			LoadMaterials();
		}

		SameLine();
		if (Button("Edit"))
		{
			if (std::filesystem::exists(selectedMat.Path))
			{
				std::ifstream t(selectedMat.Path);
				if (t.good())
				{
					const std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
					TextEditor.SetText(str);
					CurrentMaterial = selectedMat;
					EditorOpen = true;
				}
			}
		}

		SameLine();
		if (Button("Remove") && !selectedMat.Path.empty())
		{
			std::filesystem::remove(selectedMat.Path);
			LoadMaterials();
		}

		PushItemWidth(GetWindowSize().x - 2 * GetStyle().WindowPadding.x);

		// New material input
		static std::string newName;
		if (InputTextWithHint("###MaterialName", "New Material name", &newName, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			const std::wstring outstring(newName.begin(), newName.end());
			if (!std::filesystem::exists(MaterialFolder + L"\\" + outstring))
			{
				const CustomMaterial newMaterial = { MaterialFolder + L"\\" + outstring + L".vmt", newName};
				WriteMaterial(newMaterial, "\"VertexLitGeneric\"\n{\n}");
				MaterialList.push_back(newMaterial);
			}
		}

		// Material list
		if (ListBoxHeader("###MaterialList"))
		{
			for (auto const& mat : MaterialList)
			{
				if (Selectable(mat.Name.c_str(), selectedMat.Name == mat.Name))
				{
					selectedMat = mat;
				}
			}

			ListBoxFooter();
		}

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
		// TODO: Use a menu bar instead. But it doesn't work for some reason
		{
			if (Button("Save"))
			{
				WriteMaterial(CurrentMaterial, TextEditor.GetText());
			}

			SameLine();
			if (Button("Save and close"))
			{
				WriteMaterial(CurrentMaterial, TextEditor.GetText());
				EditorOpen = false;
			}
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