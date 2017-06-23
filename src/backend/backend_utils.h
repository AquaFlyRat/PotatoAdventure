#ifndef BACKEND_UTILS_INCLUDED
#define BACKEND_UTILS_INCLUDED

#include <random>
#include <ctime>
#include <vector>

namespace Backend
{
    enum class GoodnessRating
    {
        ReallyBad = 0,
        Bad = 1,
        Neutral = 2,
        Good = 3,
        Great = 4
    };

    enum class Gender
    {
        Male,
        Female
    };

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

        /// Random number between the rannge of min -> max (Inclusive)
        int RandomInt32(int min, int max)
        {
            return std::uniform_int_distribution<int>(min, max)(mt);
        }

        bool RandomBoolean()
        {
            return RandomInt32(0, 1) ? true : false;    // this technically could probably be return RandomInt32(0,1) == 1
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