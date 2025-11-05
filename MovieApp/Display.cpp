#include "Display.h"
#include "../ImGuiLib/GuiMain.h"
#include "SharedMovieData.h"
#include <imgui.h>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

// Thread-safe texture cache
std::unordered_map<std::string, ID3D11ShaderResourceView*> textureCache;
std::mutex textureCacheMutex;

// Queue for texture loading tasks
std::queue<std::string> textureLoadQueue;
std::mutex queueMutex;
std::condition_variable queueCV;
bool stopThread = false;

// Function to load textures in a background thread
void TextureLoadingThread() {
    while (true) {
        std::string imagePath;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [] { return !textureLoadQueue.empty() || stopThread; });

            if (stopThread && textureLoadQueue.empty()) {
                break; // Exit the thread if stopped and queue is empty
            }

            if (!textureLoadQueue.empty()) {
                imagePath = textureLoadQueue.front();
                textureLoadQueue.pop();
            }
        }

        if (!imagePath.empty()) {
            int imageWidth = 0, imageHeight = 0;
            ID3D11ShaderResourceView* texture = LoadTexture(imagePath.c_str(), imageWidth, imageHeight);

            if (texture) {
                std::lock_guard<std::mutex> lock(textureCacheMutex);
                textureCache[imagePath] = texture;
            }
        }
    }
}

// Function to get or queue a texture for loading
ID3D11ShaderResourceView* GetOrQueueTexture(const std::string& imagePath) {
    std::lock_guard<std::mutex> lock(textureCacheMutex);
    auto it = textureCache.find(imagePath);
    if (it != textureCache.end()) {
        return it->second; // Texture is already loaded
    }

    // Queue the texture for loading
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        textureLoadQueue.push(imagePath);
        queueCV.notify_one();
    }

    return nullptr; // Texture is not yet loaded
}

void CenteredText(const std::string& text) {
    float columnWidth = ImGui::GetColumnWidth();
    float textWidth = ImGui::CalcTextSize(text.c_str()).x;
    float offsetX = (columnWidth - textWidth) * 0.5f; // Center the text
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
    ImGui::Text("%s", text.c_str());
}

void CenteredTruncatedText(const std::string& text, float maxWidth) {
    std::string displayText = text;
    float textWidth = ImGui::CalcTextSize(text.c_str()).x;

    if (textWidth > maxWidth) {
        displayText = text.substr(0, maxWidth / ImGui::CalcTextSize("A").x) + "...";
    }

    float columnWidth = ImGui::GetColumnWidth();
    float offsetX = (columnWidth - ImGui::CalcTextSize(displayText.c_str()).x) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
    ImGui::Text("%s", displayText.c_str());

    // Tooltip to show full text on hover
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", text.c_str());
    }
}



void DrawMovieWindow(void* sharedData) {
    auto data = (SharedMovieData*)sharedData;
    auto movies = data->getMovies();

    if (data->isDataReady()) {
        ImGui::Begin("Top 250 Movies");

        int columns = 4; // Number of movies per row
        if (ImGui::BeginTable("MovieTable", columns, ImGuiTableFlags_SizingStretchSame)) {
            int movieIndex = 0;

            for (const auto& movie : movies) {
                if (movieIndex % columns == 0) {
                    ImGui::TableNextRow(); // Start a new row after every 'columns' movies
                }

                ImGui::TableNextColumn();

                std::string imagePath = movie.id + "_image.jpg";
                ID3D11ShaderResourceView* texture = GetOrQueueTexture(imagePath);

                float columnWidth = ImGui::GetColumnWidth();
                float imageSizeX = 200.0f, imageSizeY = 200.0f; // Adjust as needed
                float imageOffsetX = (columnWidth - imageSizeX) * 0.5f;

                if (texture) {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + imageOffsetX);
                    ImGui::Image((ImTextureID)texture, ImVec2(imageSizeX, imageSizeY));
                }
                else {
                    CenteredText("Loading...");
                }

                CenteredTruncatedText(movie.title, ImGui::GetColumnWidth());
                CenteredText("Year: " + std::to_string(movie.startYear));
                CenteredText("Rating: " + std::to_string(movie.averageRating));

                float buttonWidth = 120.0f; // Adjust as needed
                float buttonOffsetX = (columnWidth - buttonWidth) * 0.5f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + buttonOffsetX);

                std::string buttonLabel = "Details##" + movie.id;
                if (ImGui::Button(buttonLabel.c_str(), ImVec2(buttonWidth, 0))) {
                    std::string command = "start " + movie.url;
                    system(command.c_str());
                }

                ImGui::Separator();
                movieIndex++;
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }
}

void Display::operator()(SharedMovieData& sharedData) {
    // Start the texture loading thread
    std::thread textureThread(TextureLoadingThread);

    // Run the GUI
    GuiMain(DrawMovieWindow, &sharedData);

    // Stop the texture loading thread
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopThread = true;
        queueCV.notify_all();
    }
    textureThread.join();
}