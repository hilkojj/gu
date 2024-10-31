#pragma once

class Screen
{
  public:
    virtual void render(double deltaTime) = 0;
    virtual void onResize() {}
    virtual ~Screen() = default;
};
