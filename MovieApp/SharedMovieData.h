#pragma once
#ifndef SHAREDMOVIEDATA_H
#define SHAREDMOVIEDATA_H


#include <vector>
#include<mutex>
#include <unordered_map>


struct Movie {
    std::string id;
    std::string url;
    std::string title;
    std::string primaryImage;
    std::string description;
    int startYear;
    int endYear;
    int runtimeMinutes;
    std::string contentRating;
    double averageRating;
    int numVotes;
    std::string type;
};


class SharedMovieData {
public:
    void addMovie(const Movie& movie);
    void markImageReady(const std::string& movieId);
    std::vector<Movie> getMovies() const;
    void setDataReady(bool ready);
    bool isDataReady() const;
    std::atomic<bool> exit_flag = false;

private:
    std::vector<Movie> movies;
    std::unordered_map<std::string, bool> imageReady;
    bool data_ready = false;
    mutable std::mutex data_mutex;  // `mutable` allows const methods to lock it
};

#endif // SHAREDMOVIEDATA.H