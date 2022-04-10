#include "MaterialEditor.h"
#include <fstream>

#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_stdlib.h"

void CMaterialEditor::MainWindow()
{
	if (!IsOpen) { return; }
	using namespace ImGui;

	if (Begin("Material Manager", &IsOpen, ImGuiWindowFlags_NoCollapse))
	{
		static std::string newName;
		if (InputTextWithHint("Name###MaterialName", "New Material name", &newName, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			MaterialList.push_back(newName.c_str());
		}

		static int currentItem = 0;
		if (ListBox("Materials", &currentItem, MaterialList.data(), MaterialList.size()))
		{
			TextEditor.SetText(MaterialList.at(currentItem));
			EditorOpen = true;
		}

		End();
	}
}

void CMaterialEditor::EditorWindow()
{
	if (!EditorOpen || !IsOpen) { return; }
	using namespace ImGui;

	SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
	PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	if (Begin("Material Editor", &EditorOpen, ImGuiWindowFlags_NoCollapse))
	{
		// TODO: Use a menu bar instead. But it doesn't work for some reason
		{
			Button("Save and apply");
		}

		// Text editor
		TextEditor.Render("TextEditor");

		End();
	}
	PopStyleVar();
}

void CMaterialEditor::Render()
{
	MainWindow();
	EditorWindow();
}

void CMaterialEditor::Init()
{
	// TODO: Add syntax / language

	/*static const char* fileToEdit = "test.vtf";

	{
		std::ifstream t(fileToEdit);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			textEditor.SetText(str);
		}
	}*/
	TextEditor.SetText("\"VertexLitGeneric\"\n{\n\n}");
}