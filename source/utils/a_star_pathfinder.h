#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <limits>
#include <queue>
#include <unordered_set>

const float INFINITY_FLOAT = std::numeric_limits<float>::infinity();

template <typename K, typename V>
V getWithDefault(const std::unordered_map<K, V> &map, const K &key, const V &defaultValue)
{
    auto it = map.find(key);
    if (it == map.end())
    {
        return defaultValue;
    }
    return it->second;
}

// https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
template <class nodeType>
bool findAStarPath(
    nodeType start,
    nodeType goal,
    std::function<float(const nodeType &n)> h,
    std::function<float(const nodeType &n0, const nodeType &n1)> d,
    std::function<void(const nodeType &n, std::vector<nodeType> &out)> neighborsGetter,

    std::list<nodeType> &path,

    bool bToClosestNodeOnFail = false,

    std::unordered_set<nodeType> *outClosedSet = nullptr
)
{
    using openPair = std::pair<float, nodeType>;

    std::priority_queue<openPair, std::vector<openPair>, std::greater<openPair>> openQueue;
    openQueue.push({0.0f, start});

    std::unordered_set<nodeType> openSet{start};
    std::unordered_set<nodeType> localClosedSet;
    std::unordered_set<nodeType> &closedSet = outClosedSet ? *outClosedSet : localClosedSet;
    std::vector<nodeType> neighbors;

    std::unordered_map<nodeType, nodeType> cameFrom;
    std::unordered_map<nodeType, float> gScore{{start, 0}};

    std::unordered_map<nodeType, float> fScore{{start, 0}};

    float lowestHValue = INFINITY_FLOAT;
    nodeType finalNode = start;

    while (!openSet.empty())
    {
        auto [currentF, current] = openQueue.top();
        openQueue.pop();

        if (closedSet.count(current)) continue;
        openSet.erase(current);

        // nodeType current = *openSet.begin();
        // float lowestFScore = fScore[current];
        // for (auto &n : openSet)
        // {
        //     float f = fScore[n];
        //     if (f < lowestFScore)
        //     {
        //         current = n;
        //         lowestFScore = f;
        //     }
        // }

        if (current == goal)
        {
            finalNode = goal;
            break;
        }
        // remove 'current' from openSet:
        closedSet.insert(current);

        neighbors.clear();
        neighborsGetter(current, neighbors);
        for (auto &nb : neighbors)
        {
            // if neighbor in closedSet: continue
            if (closedSet.find(nb) != closedSet.end()) continue;

            float tentativeGScore = gScore[current] + d(current, nb);
            if (tentativeGScore < getWithDefault(gScore, nb, INFINITY_FLOAT))
            {
                cameFrom[nb] = current;
                gScore[nb] = tentativeGScore;
                const float hValue = h(nb);
                const float fValue = tentativeGScore + hValue;
                fScore[nb] = fValue;

                openQueue.push({ fValue, nb });

                if (openSet.insert(nb).second)
                {
                    if (hValue < lowestHValue)
                    {
                        lowestHValue = hValue;
                        finalNode = nb;
                    }
                }
            }
        }
    }

    if (finalNode == goal || bToClosestNodeOnFail)
    {
        nodeType current = finalNode;
        path.push_back(current);
        while (current != start)
        {
            current = cameFrom[current];
            path.push_front(current);
        }
        return true;
    }
    return false;
    /*
    std::vector<nodeType> openSet{start};
    std::vector<nodeType> localClosedSet;
    std::vector<nodeType> &closedSet = outClosedSet ? *outClosedSet : localClosedSet;
    std::vector<nodeType> neighbors;

    std::unordered_map<nodeType, nodeType> cameFrom;
    std::unordered_map<nodeType, float> gScore{{start, 0}};

    std::unordered_map<nodeType, float> fScore{{start, 0}};

    float lowestHValue = INFINITY_FLOAT;
    nodeType finalNode = start;

    while (!openSet.empty())
    {
        nodeType current = openSet[0];
        float lowestFScore = fScore[current];
        for (auto &n : openSet)
        {
            float f = fScore[n];
            if (f < lowestFScore)
            {
                current = n;
                lowestFScore = f;
            }
        }

        if (current == goal)
        {
            finalNode = goal;
            break;
        }
        // remove 'current' from openSet:
        openSet.erase(std::find(openSet.begin(), openSet.end(), current));

        closedSet.push_back(current);

        neighbors.clear();
        neighborsGetter(current, neighbors);
        for (auto &nb : neighbors)
        {
            // if neighbor in closedSet: continue
            if (std::find(closedSet.begin(), closedSet.end(), nb) != closedSet.end()) continue;

            float tentativeGScore = gScore[current] + d(current, nb);
            if (tentativeGScore < getWithDefault(gScore, nb, INFINITY_FLOAT))
            {
                cameFrom[nb] = current;
                gScore[nb] = tentativeGScore;
                const float hValue = h(nb);
                fScore[nb] = tentativeGScore + hValue;

                if (std::find(openSet.begin(), openSet.end(), nb) == openSet.end())
                {
                    openSet.push_back(nb);
                    if (hValue < lowestHValue)
                    {
                        lowestHValue = hValue;
                        finalNode = nb;
                    }
                }
            }
        }
    }

    if (finalNode == goal || bToClosestNodeOnFail)
    {
        nodeType current = finalNode;
        path.push_back(current);
        while (current != start)
        {
            current = cameFrom[current];
            path.push_front(current);
        }
        return true;
    }
    return false;
    */
}
