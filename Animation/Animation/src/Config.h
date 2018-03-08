#pragma once

// Chain Configuration
#define SIMPLE_CHAIN	0
#define HUMANOID_CHAIN	1

// Spline Configuration
#define SPLINE_CUBIC		2
#define SPLINE_HERMITE		3


struct CurrentConfig
{
	int CHAIN_CONFIG	= SIMPLE_CHAIN;
	int SPLINE_CONFIG	= SPLINE_HERMITE;

} CONFIG;