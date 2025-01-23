#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include"MovieFetcher.h"
#include <nlohmann/json.hpp>
#include "SharedMovieData.h"


using namespace std;
using json = nlohmann::json;


void MovieFetcher :: downloadImage(const std::string& imageUrl, const std::string& fileName) {
    try {

        const std::string host = "m.media-amazon.com";

        // Extract the path from the image URL
        size_t path_start = imageUrl.find(host) + host.length();
        std::string path = imageUrl.substr(path_start);

        httplib::Client cli("https://m.media-amazon.com");  // Using the image URL to download
        auto res = cli.Get(path.c_str());  // Fetch the image

        if (res && res->status == 200) {
            // Save the image to a file
            std::ofstream file(fileName, std::ios::binary);
            if (file.is_open()) {
                file.write(res->body.c_str(), res->body.size());
                file.close();
                std::cout << "Image saved as " << fileName << std::endl;
            }
            else {
                std::cerr << "Failed to save image!" << std::endl;
            }
        }
        else {
            std::cerr << "Failed to download image: " << (res ? res->status : 0) << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void MovieFetcher ::operator()(SharedMovieData& sharedData) {

    try {
        httplib::Client cli("https://imdb236.p.rapidapi.com");
        // should be change 
        std::string url = "/imdb/top250-movies";
        httplib::Headers headers = {
            {"x-rapidapi-key", "760d915b17mshb2e1c6ba376c55ep14f703jsnf9845a14111d"},
            {"x-rapidapi-host", "imdb236.p.rapidapi.com"}
        };

        auto res = cli.Get(url.c_str(), headers);
        if (res && res->status == 200) {
            auto json_result = json::parse(res->body);

            std::vector<std::thread> threads;

            for (const auto& movieJson : json_result) {
                Movie movie;
                movie.id = movieJson.value("id", "");
                movie.url = movieJson.value("url", "");
                movie.title = movieJson.value("title", "");
                movie.primaryImage = movieJson.value("primaryImage", "");
                movie.description = movieJson.value("description", "");
                movie.startYear = movieJson.value("startYear", 0);
                movie.endYear = movieJson.value("endYear", 0);
                movie.runtimeMinutes = movieJson.value("runtimeMinutes", 0);
                movie.contentRating = movieJson.value("contentRating", "");
                movie.averageRating = movieJson.value("averageRating", 0.0);
                movie.numVotes = movieJson.value("numVotes", 0);
                movie.type = movieJson.value("type", "");

                // Add movie to the shared data
                sharedData.addMovie(movie);
                threads.push_back(std::thread(&MovieFetcher::downloadImage, this, movie.primaryImage, movie.id + "_image.jpg"));
            }

            for (auto& t : threads) {
                t.join();
            }

            // Mark data as ready
            sharedData.setDataReady(true);
        }
        else {
            std::cerr << "HTTP Error: " << (res ? res->status : 0) << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}



