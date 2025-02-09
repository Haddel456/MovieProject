#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include"MovieFetcher.h"
#include <nlohmann/json.hpp>
#include "SharedMovieData.h"
#include "semaphore"

using namespace std;
using json = nlohmann::json;

std::mutex dataMutex;  // Protect shared data
std::counting_semaphore<5> downloadSemaphore(5);

void createDirectory(const std::string& path) {
    // Create the directory if it doesn't exist
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directory(path);
    }
}


void MovieFetcher::downloadImage(const std::string& imageUrl, const std::string& fileName) {
    try {

        std::string filePath = fileName;
        if (std::filesystem::exists(filePath)) {
            std::cout << "Image " << fileName << " already exists, skipping download." << std::endl;
            return;
        }

        const std::string host = "m.media-amazon.com";

        // Extract the path from the image URL
        size_t path_start = imageUrl.find(host) + host.length();
        std::string path = imageUrl.substr(path_start);

        httplib::Client cli("https://m.media-amazon.com");  // Using the image URL to download
        auto res = cli.Get(path.c_str());  // Fetch the image

        if (res && res->status == 200) {
            // Save the image to a file
           /* createDirectory("images");*/  // Ensure the folder exists
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
            {"x-rapidapi-key", "acad3e3611mshec113ca7505c9e9p1633ebjsn47de4f6034fa"},
            {"x-rapidapi-host", "imdb236.p.rapidapi.com"}
        };

        auto res = cli.Get(url.c_str(), headers);
        if (res && res->status == 200) {
            auto json_result = json::parse(res->body);
              std::vector<std::thread> imageThreads;

            for (const auto& movieJson : json_result) {
                Movie movie;
                movie.id = movieJson.value("id", "");
                movie.url = movieJson.value("url", "");
                movie.title = movieJson.value("primaryTitle", "");
                movie.primaryImage = movieJson.value("primaryImage", "");
                movie.description = movieJson.value("description", "");
                movie.startYear = movieJson.value("startYear", 0);
                movie.runtimeMinutes = movieJson.value("runtimeMinutes", 0);
                movie.contentRating = movieJson.contains("contentRating") && !movieJson["contentRating"].is_null()
                    ? movieJson["contentRating"].get<std::string>()
                    : "Not Rated";
                movie.averageRating = movieJson.value("averageRating", 0.0);
                movie.numVotes = movieJson.value("numVotes", 0);
                movie.type = movieJson.value("type", "");

                // Add movie to the shared data
                {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    sharedData.addMovie(movie);  // Thread-safe access
                }

                // Spawn a new thread to download the image
                imageThreads.emplace_back([this, movie]() {
                    downloadSemaphore.acquire();  // Limit concurrency
                    downloadImage(movie.primaryImage, movie.id + "_image.jpg");
                    downloadSemaphore.release();
                    });
            }

            // Mark data as ready
            for (auto& thread : imageThreads) {
                if (thread.joinable()) thread.join();
            }

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



