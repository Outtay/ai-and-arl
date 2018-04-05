#include "W4Random.h"

W4Random::~W4Random() {
	// Nothing to do
}

W4RandomXorshift128::W4RandomXorshift128(uint64_t seed, uint64_t seed2) {
	if (seed == 0) {
		seed = 1;
	}
	if (seed2 == 0) {
		seed2 = 1;
	}

	m_state[0] = seed;
	m_state[1] = seed2;
}

int W4RandomXorshift128::randomInt(int minimum, int maximum) {
	return (next() % (maximum - minimum)) + minimum;
}

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t W4RandomXorshift128::next() {
	const uint64_t s0 = m_state[0];
	uint64_t s1 = m_state[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	m_state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
	m_state[1] = rotl(s1, 36);

	return result;
}

W4RandomXorshift128::~W4RandomXorshift128() {
	// Nothing to do
}
