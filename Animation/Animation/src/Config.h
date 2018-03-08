#pragma once

// Chain Configuration
#define SIMPLE_CHAIN	0
#define HUMANOID_CHAIN	1

// Spline Configuration
#define SPLINE_NONE			-1
#define SPLINE_CUBIC		1
#define SPLINE_HERMITE		2
#define SPLINE_BEIZER		3
#define SPLINE_B			4


struct CurrentConfig
{
	int CHAIN_CONFIG	= HUMANOID_CHAIN;
	int SPLINE_CONFIG	= SPLINE_NONE;

} CONFIG;