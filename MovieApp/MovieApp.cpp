#include "Display.h"
#include "MovieFetcher.h"
#include "SharedMovieData.h"
#include <iostream>
#include <thread>


int main() {
    SharedMovieData sharedData;
    MovieFetcher movie;
    Display display;
    // Start a thread to download movie data
    auto down_th = std::jthread([&] { movie(sharedData); });
    // Start a thread to draw movie data
    auto draw_th = std::jthread([&] {display(sharedData); });
    //down_th.join();
    //draw_th.join();
    return 0;
}

// how to use 
//  std::atomic<bool> exit_flag = false;