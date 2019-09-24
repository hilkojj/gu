
#include <vector>
#include <map>
#include <functional>
#include <limits>

const float INFINITY_FLOAT = std::numeric_limits<float>::infinity();

// https://stackoverflow.com/questions/2333728/stdmap-default-value
template <template<class,class,class...> class C, typename K, typename V, typename... Args>
V getWithDefault(const C<K,V,Args...>& m, K const& key, const V & defval)
{
    typename C<K,V,Args...>::const_iterator it = m.find(key);
    if (it == m.end())
        return defval;
    return it->second;
}

// https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
template <class nodeType>
bool findAStarPath(
    nodeType start,
    nodeType goal,
    std::function<float(nodeType &n)> h,
    std::function<float(nodeType &n0, nodeType &n1)> d,
    std::function<std::vector<nodeType>&(nodeType &n)> neighbors,

    std::vector<nodeType> &path
)
{
    std::vector<nodeType> openSet{start}, closedSet;

    std::map<nodeType, nodeType> cameFrom;
    std::map<nodeType, float> gScore{{start, 0}};

    std::map<nodeType, float> fScore{{start, 0}};

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
            path.push_back(current);
            while (current != start)
            {
                current = cameFrom[current];
                path.insert(path.begin(), current);
            }
            return true;
        }
        std::cout << to_string(current->lonLat) << "\n";
        // remove 'current' from openSet:
        openSet.erase(std::find(openSet.begin(), openSet.end(), current));

        closedSet.push_back(current);

        for (auto &nb : neighbors(current))
        {
            // if neighbor in closedSet: continue
            if (std::find(closedSet.begin(), closedSet.end(), nb) != closedSet.end()) continue;

            float tentativeGScore = gScore[current] + d(current, nb);
            if (tentativeGScore < getWithDefault(gScore, nb, INFINITY_FLOAT))
            {
                cameFrom[nb] = current;
                gScore[nb] = tentativeGScore;
                fScore[nb] = tentativeGScore + h(nb);

                if (std::find(openSet.begin(), openSet.end(), nb) == openSet.end()) openSet.push_back(nb);
            }
        }
    }
    return false;
}
