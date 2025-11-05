
#ifndef MOVIEFETCHER_H
#define MOVIEFETCHER_H

#include <vector>
#include <string>
#include <mutex>
#include "SharedMovieData.h"

class MovieFetcher
{

public:
	void operator()(SharedMovieData& sharedData);


private:
	void downloadImage(const std::string& imageUrl, const std::string& fileName);

};

#endif // MOVIEFETCHER_H

