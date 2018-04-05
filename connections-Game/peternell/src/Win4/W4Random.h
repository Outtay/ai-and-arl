#pragma once

#include "W4Board.h"

class W4Random
{
public:
	virtual int randomInt(int minimum, int maximum) = 0;

	virtual ~W4Random();
};

class W4RandomXorshift128: public W4Random
{
	// Credit: Xorshift128 algorithm inspired by http://xoroshiro.di.unimi.it/xoroshiro128plus.c

public:
	W4RandomXorshift128() : W4RandomXorshift128(1, 1) { };
	W4RandomXorshift128(uint64_t seed, uint64_t seed2=876234567123987432L);
	int randomInt(int minimum, int maximum);
	~W4RandomXorshift128();

private:
	uint64_t next();

	uint64_t m_state[2];
};
