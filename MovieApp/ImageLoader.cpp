
#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class ImageLoader {
public:
    static GLuint LoadTextureFromMemory(const std::vector<unsigned char>& imageData) {
        int width, height, channels;
        unsigned char* data = stbi_load_from_memory(imageData.data(), imageData.size(), &width, &height, &channels, 4);
        if (!data) return 0;

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return textureID;
    }

    static bool DownloadImage(const std::string& imageUrl, std::vector<unsigned char>& imageData) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, imageUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return res == CURLE_OK;
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::vector<unsigned char>*)userp)->insert(((std::vector<unsigned char>*)userp)->end(),
            (unsigned char*)contents,
            (unsigned char*)contents + size * nmemb);
        return size * nmemb;
    }
};