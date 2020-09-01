
#ifndef SCREEN_H
#define SCREEN_H

#include <utils/delegate.h>

class Screen
{
  public:
    virtual void render(double deltaTime) = 0;
    virtual void onResize() {}
    virtual ~Screen() = default;
};

#endif
