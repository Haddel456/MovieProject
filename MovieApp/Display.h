#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H
#include "SharedMovieData.h"
class Display {
public:
	void operator()(SharedMovieData& sharedData);
	
};

#endif