
#include "CodeEditor.h"
#include <imgui.h>
#include <iostream>
#include <include/GLFW/glfw3.h>

#include "../../../external/ImGuiColorTextEdit/TextEditor.cpp"

std::vector<CodeEditor::Tab> CodeEditor::tabs;

void CodeEditor::drawGUI(ImFont *codeFont)
{
    static Tab *tabToClose = NULL;

    if (tabs.empty())
        return;

    if (!ImGui::Begin("Code Editor"))
    {
        ImGui::End();
        return;
    }

    bool doSave = ImGui::Button("Save");
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(GLFW_KEY_S))
        doSave = true;

    ImGui::SameLine();
    bool doRevert = ImGui::Button("Revert");

    if (ImGui::BeginTabBar("code tabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs))
    {
        for (int i = tabs.size() - 1; i >= 0; i--)
        {
            Tab &tab = tabs[i];

            bool open = true;

            if (ImGui::BeginTabItem(tab.title.c_str(), &open, tab.dirty ? ImGuiTabItemFlags_UnsavedDocument : 0))
            {
                if (!tab.initialized)
                {
                    tab.textEditor.SetText(tab.code);
                    tab.initialized = true;
                }

                tab.textEditor.SetTabSize(tab.tabSize);
                tab.textEditor.SetLanguageDefinition(tab.languageDefinition);

                ImGui::PushFont(codeFont);
                tab.textEditor.Render(tab.title.c_str());

                if (tab.textEditor.IsTextChanged())
                    tab.dirty = true;

                ImGui::PopFont();

                ImGui::EndTabItem();

                if (doSave)
                {
                    tab.code = tab.textEditor.GetText();
                    tab.save(tab);
                    tab.dirty = false;
                }
                if (doRevert)
                {
                    tab.revert(tab);
                    tab.textEditor.SetText(tab.code);
                    tab.dirty = false;
                }
            }

            if (!open && tab.dirty)
            {
                ImGui::OpenPopup("Closing unsaved");
                tabToClose = &tab; // todo, I think this is pretty unsafe because tab is stored inside a vector
            }
            else if (!open)
                tab.close = true;

            if (tab.close)
            {
                tab.closed(tab);
                tabs[i] = tabs.back();
                tabs.pop_back();
            }
        }

        if (tabToClose && ImGui::BeginPopupModal("Closing unsaved", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Some changes might not have been saved!");
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                tabToClose->close = true;
                ImGui::CloseCurrentPopup();
                tabToClose = NULL;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
                ImGui::CloseCurrentPopup();

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
