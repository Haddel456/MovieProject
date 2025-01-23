
#include "SharedMovieData.h"

void SharedMovieData::addMovie(const Movie& movie) {
    std::lock_guard<std::mutex> lock(data_mutex);
    movies.push_back(movie);
}

std::vector<Movie> SharedMovieData::getMovies() const {
    std::lock_guard<std::mutex> lock(data_mutex);
    return movies;
}

void SharedMovieData::setDataReady(bool ready) {
    std::lock_guard<std::mutex> lock(data_mutex);
    data_ready = ready;
}

bool SharedMovieData::isDataReady() const {
    std::lock_guard<std::mutex> lock(data_mutex);
    return data_ready;
}
