#ifndef GU_PROFILER_H
#define GU_PROFILER_H

#include "gl_includes.h"

#include <map>
#include <iostream>
#include <list>

namespace gu::profiler
{
    struct ZoneTime
    {
        double time = 0;

        std::map<std::string, ZoneTime> subZones;
        std::string activeSubZone;

        ZoneTime *getActiveSubZone()
        {
            return activeSubZone.empty() ? this : subZones[activeSubZone].getActiveSubZone();
        }
    };

    extern std::list<ZoneTime> frames;
    extern int fps, takeAverageOfNFrames;
    extern bool showGUI;

    struct Zone
    {
        Zone(std::string name)
        {
            parent = frames.back().getActiveSubZone();
            parent->activeSubZone = std::move(name);
        }

        ~Zone()
        {
            frames.back().getActiveSubZone()->time += glfwGetTime() - startTime;
            parent->activeSubZone.clear();
        }

      private:
        double startTime = glfwGetTime();
        ZoneTime *parent;
    };

    void beginNewFrame();

    void drawProfilerImGUI();

    ZoneTime getAverageFrame();
}

#endif
