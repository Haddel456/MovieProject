#include "Display.h"
#include "MovieFetcher.h"
#include "SharedMovieData.h"
#include <iostream>
#include <thread>


#include <iostream>
#include "MovieFetcher.h"
#include "SharedMovieData.h"

//int main() {
//    // Create a MovieFetcher object
//    MovieFetcher fetcher;
//
//    // Create a SharedMovieData object
//    SharedMovieData sharedData;
//
//    // Call the fetcher with sharedData
//    fetcher(sharedData);
//
//    // Check if the data is ready
//    if (sharedData.isDataReady()) {
//        std::cout << "Movies have been successfully fetched and processed!" << std::endl;
//
//        // Retrieve and display movies (assuming getMovies() exists in SharedMovieData)
//        auto movies = sharedData.getMovies();
//        for (const auto& movie : movies) {
//            std::cout << "Movie Title: " << movie.title << std::endl;
//            std::cout << "Description: " << movie.description << std::endl;
//            std::cout << "Rating: " << movie.averageRating << std::endl;
//            std::cout << "-------------------------" << std::endl;
//        }
//    }
//    else {
//        std::cerr << "Failed to fetch movie data." << std::endl;
//    }
//
//    return 0;
//}









int main() {
    SharedMovieData sharedData;
    MovieFetcher movie;
    Display display;
    // Start a thread to download movie data
    auto down_th = std::jthread([&] { movie(sharedData); });
    // Start a thread to draw movie data
    auto draw_th = std::jthread([&] {display(sharedData); });
    down_th.join();
    draw_th.join();
    return 0;
}

