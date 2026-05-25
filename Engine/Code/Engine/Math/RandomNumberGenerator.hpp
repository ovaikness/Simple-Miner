#pragma once

#include <cstdint>

class RandomNumberGenerator
{
public:
	RandomNumberGenerator();
	RandomNumberGenerator(unsigned int seed);

	int   RollRandomIntLessThan(int minNotInclusive);
	int	  RollRandomIntInRange(int minInclusive, int maxInclusive);
	float RollRandomFloatZeroToOne();
	float RollRandomFloatInRange(float minInclusive, float maxInclusive);
private:
	unsigned int m_seed;
	int			 m_position;
};