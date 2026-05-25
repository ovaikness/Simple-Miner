#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/ThirdParty/Squirrel/RawNoise.hpp"

#include <stdlib.h>


RandomNumberGenerator __RNG(0);
constexpr int MAX_RAND = 0x7FFF;

RandomNumberGenerator::RandomNumberGenerator(unsigned int seed)
    : m_seed(seed)
    , m_position(0)
{
}
RandomNumberGenerator::RandomNumberGenerator()
    : m_seed(__RNG.RollRandomIntInRange(0, MAX_RAND))
    , m_position(0)
{
}

int RandomNumberGenerator::RollRandomIntLessThan(int minNotInclusive)
{
    return Get1dNoiseUint(m_position++, m_seed) % minNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
    //Need +1 because the remainder will be zero if the rand() value is a multiple of it the 1 overshoots the value
    //and gives the expected inclusive result

    return minInclusive + (Get1dNoiseUint(m_position++, m_seed) % ((maxInclusive - minInclusive) + 1));
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
    return static_cast<float>(Get1dNoiseUint(m_position++, m_seed)) / static_cast<float>(~0ui32);
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
    return minInclusive + RollRandomFloatZeroToOne() * (maxInclusive - minInclusive);
}

