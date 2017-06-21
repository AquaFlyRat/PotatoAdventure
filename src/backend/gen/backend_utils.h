#ifndef BACKEND_UTILS_INCLUDED
#define BACKEND_UTILS_INCLUDED

#include <random>
#include <ctime>
#include <vector>

namespace Backend
{
    struct Random
    {
    private:
        std::mt19937 mt;

    public:
        Random()
        {
            mt = std::mt19937(time(0));
        }

        int RandomInt32()
        {
            return mt();
        }

        int RandomInt32(int max, int min)
        {
            return std::uniform_int_distribution<int>(max, min)(mt);
        }

        template <typename T>
        T Choose(const std::vector<T>& items)
        {
            int index = RandomInt32(0, items.size() - 1);
            return items[index];
        }

        static Random *Instance()
        {
            static Random _random;
            return &_random;
        }
    };
}

#endif