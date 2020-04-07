
#include <imgui.h>
#include <cmath>
#include <functional>
#include "profiler.h"

namespace gu::profiler
{

std::list<ZoneTime> frames = { ZoneTime() };
bool showGUI = false;
int fps = 0, takeAverageOfNFrames = 20;

void beginNewFrame()
{
    assert(frames.back().activeSubZone.empty());
    frames.emplace_back();
    while (frames.size() > takeAverageOfNFrames) frames.pop_front();
}

void drawProfilerImGUI()
{
    const float DISTANCE = 10.0f;
    static int corner = 0;

    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.15f); // Transparent background
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 30), ImVec2(300, 10000));
    if (ImGui::Begin("profiler", &showGUI, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ZoneTime avg = getAverageFrame();

        ImGui::Text("%dFPS (%.1fms)", fps, avg.time * 1000);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));
        ImGui::Columns(2, NULL, false);
        ImGui::SetColumnWidth(0, 230);
        ImGui::Separator();

        struct funcs
        {
            static void showZone(const char *prefix, ZoneTime &zone)
            {
                ImGui::PushID(prefix);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
                ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.

                bool node_open = !zone.subZones.empty();
                if (!node_open)
                    ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_None, "%s", prefix);
                else
                    node_open = ImGui::TreeNode("Object", "%s", prefix);

                ImGui::NextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%.3fms", zone.time * 1000);
                ImGui::NextColumn();
                if (node_open)
                {
                    for (auto &sub : zone.subZones)
                        funcs::showZone(sub.first.c_str(), sub.second);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        };
        for (auto &sub : avg.subZones)
            funcs::showZone(sub.first.c_str(), sub.second);

        ImGui::Columns(1);
        ImGui::PopStyleVar();

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (ImGui::MenuItem("Close")) showGUI = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void addToAvg(ZoneTime &avg, const ZoneTime &zone, int nrOfFrames)
{
    auto *parent = avg.getActiveSubZone();
    parent->time += zone.time / nrOfFrames;
    for (auto &sub : zone.subZones)
    {
        parent->activeSubZone = sub.first;
        addToAvg(avg, sub.second, nrOfFrames);
    }
    parent->activeSubZone.clear();
}

ZoneTime getAverageFrame()
{
    assert(frames.back().activeSubZone.empty());
    ZoneTime avg;

    int nrOfFrames = frames.size();
    for (ZoneTime &frame : frames)
        addToAvg(avg, frame, nrOfFrames);

    return avg;
}

}
