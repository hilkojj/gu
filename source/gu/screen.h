
#ifndef SCREEN_H
#define SCREEN_H

class Screen
{
  public:
    virtual void render(double deltaTime) = 0;
    virtual void onResize() {}
};

#endif
