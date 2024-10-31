#pragma once

#include "../../external/ImGuiColorTextEdit/TextEditor.h"

#include <functional>

namespace CodeEditor
{

    struct Tab
    {
        std::string title;
        std::string code;
        std::function<void(Tab &)>
            save   = [](auto) {},
            revert = [](auto) {},
            closed = [](auto) {};

        TextEditor::LanguageDefinition languageDefinition;

        TextEditor textEditor;
        bool dirty = false;
        int tabSize = 4;

      private:
        bool initialized = false, close = false;
        friend void drawGUI(ImFont *);
    };

    extern std::vector<Tab> tabs;

    void drawGUI(ImFont *codeFont=NULL);

};
