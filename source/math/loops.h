
#ifndef MATH_LOOPS_H
#define MATH_LOOPS_H

#include "math_utils.h"

#include <functional>

namespace mu
{

/**
 * Executes 'callback' size^2 times in a spiral loop.
 * 'callback' must return true to continue, or false to beak the loop.
 * 
 * Example:
 * if size is 3 then 'callback' will be called 9 times with these positions:
 * (0, 0) (1, 0) (1, 1) (0, 1) (-1, 1) (-1, 0) (-1, -1) (0, -1) (1, -1)
 */
inline void spiral(int size, const std::function<bool(ivec2 pos)> &callback)
{
    ivec2 pos(0), dir(1, 0);
    int steps = 1, stepsToDo = 1, changeSteps = 2;
    for (int i = 0; i < size * size; i++)
    {
        if (!callback(pos)) break;

        pos += dir;
        if (--stepsToDo == 0)
        {
            dir = ivec2(-dir.y, dir.x);
            if (--changeSteps == 0)
            {
                changeSteps = 2;
                steps++;
            }
            stepsToDo = steps;
        }
    }
}

inline void loop3d(int size, const std::function<bool(int x, int y, int z)> &callback)
{
    for (int z = 0; z < size; z++)
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++)
                if (!callback(x, y, z)) return;
}

inline void loop3d(const ivec3 &from, const ivec3 &to, const std::function<bool(const ivec3 &)> &callback)
{
    ivec3 curr;
    ivec3 direction = ivec3(    // Note: this is different from sign() because sign() can return 0.
        from.x > to.x ? -1 : 1,
        from.y > to.y ? -1 : 1,
        from.z > to.z ? -1 : 1
    );
    for (curr.z = from.z; curr.z != to.z + direction.z; curr.z += direction.z)
        for (curr.y = from.y; curr.y != to.y + direction.y; curr.y += direction.y)
            for (curr.x = from.x; curr.x != to.x + direction.x; curr.x += direction.x)
                if (!callback(curr)) return;
}

inline bool findRayOriginInsideGrid(vec3 &rayOrigin, const vec3 &rayDirection, const uvec3 &gridSize)
{
    for (int axis = 0; axis < 3; axis++)
    {
        float diff = 0.0f;

        if (rayOrigin[axis] < 0.0f)
        {
            // Outside grid.

            diff = -rayOrigin[axis];
        }
        else if (rayOrigin[axis] > float(gridSize[axis]))
        {
            // Outside grid.

            diff = float(gridSize[axis]) - rayOrigin[axis];
        }
        if (diff != 0.0f)
        {
            rayOrigin[axis] += diff;
            for (int otherAxis = 0; otherAxis < 3; otherAxis++)
            {
                if (axis == otherAxis)
                {
                    continue;
                }
                rayOrigin[otherAxis] += diff * (rayDirection[otherAxis] / rayDirection[axis]);
            }
        }
    }
    for (int axis = 0; axis < 3; axis++)
    {
        if (rayOrigin[axis] < 0.0f || rayOrigin[axis] > float(gridSize[axis]))
        {
            return false;
        }
    }
    return true;
}

/**
 * Traverses all voxels (with unit size 1.0f) in a 3D grid that the ray intersects.
 * Implementation should be close to the algorithm introduced by John Amanatides and Andrew Woo:
 * https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.42.3443&rep=rep1&type=pdf
 *
 * The callback will be called for every intersected voxel. When false is returned, the traversing will stop.
 */
inline void fastVoxelTraversal(vec3 rayOrigin, const vec3 &rayDirection, const uvec3 &gridSize, const std::function<bool(const ivec3 &intersectedVoxel)> &callback)
{
    // find ray origin that is within the grid:
    if (!findRayOriginInsideGrid(rayOrigin, rayDirection, gridSize))
    {
        return;
    }

    // find voxel in grid adjacent to rayOrigin:
    ivec3 currentVoxel(rayOrigin);
    for (int axis = 0; axis < 3; axis++)
    {
        if (currentVoxel[axis] >= gridSize[axis])
        {
            currentVoxel[axis] = gridSize[axis] - 1;
        }
    }

    // sign of direction's components:
    ivec3 step;
    for (int axis = 0; axis < 3; axis++)
    {
        if (rayDirection[axis] == 0.0f)
        {
            step[axis] = 0;
        }
        else
        {
            step[axis] = (rayDirection[axis] < 0.0f) ? -1 : 1;
        }
    }

    // calculate for each axis how far along the ray we must travel in `t` in order to have travelled voxel's unit size along that axis.
    vec3 tDelta;
    for (int axis = 0; axis < 3; axis++)
    {
        if (step[axis] == 0)
        {
            tDelta[axis] = std::numeric_limits<float>::max();
            continue;
        }
        tDelta[axis] = float(step[axis]) / rayDirection[axis];
    }

    // calculate for each component, how far we can travel along that axis (in `t`) until we hit a voxel's boundary.
    vec3 tMax;
    for (int axis = 0; axis < 3; axis++)
    {
        if (step[axis] == 0)
        {
            tMax[axis] = std::numeric_limits<float>::max();
            continue;
        }
        float posInVoxel = (rayOrigin[axis] >= float(gridSize[axis])) ? 1.0f : mod(rayOrigin[axis], 1.0f);
        float diffAlongAxis = step[axis] == 1 ? (1.0f - posInVoxel) : -posInVoxel;
        tMax[axis] = diffAlongAxis / rayDirection[axis];
    }

    while (true)
    {
        if (!callback(currentVoxel))
        {
            return;
        }
        if (tMax.x < tMax.y)
        {
            if (tMax.x < tMax.z)
            {
                currentVoxel.x += step.x;
                if (currentVoxel.x == gridSize.x || currentVoxel.x == -1)
                {
                    return;
                }
                tMax.x += tDelta.x;
            }
            else
            {
                currentVoxel.z += step.z;
                if (currentVoxel.z == gridSize.z || currentVoxel.z == -1)
                {
                    return;
                }
                tMax.z += tDelta.z;
            }
        }
        else
        {
            if (tMax.y < tMax.z)
            {
                currentVoxel.y += step.y;
                if (currentVoxel.y == gridSize.y || currentVoxel.y == -1)
                {
                    return;
                }
                tMax.y += tDelta.y;
            }
            else
            {
                currentVoxel.z += step.z;
                if (currentVoxel.z == gridSize.z || currentVoxel.z == -1)
                {
                    return;
                }
                tMax.z += tDelta.z;
            }
        }
    }
}

} // namespace mu

#endif
