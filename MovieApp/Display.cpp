#include "Display.h"
#include "../ImGuiLib/GuiMain.h"
#include "SharedMovieData.h"
#include <imgui.h>
#include <string>



// show the result on screen 
// show the movie 


void CenteredText(const std::string& text) {
    float columnWidth = ImGui::GetColumnWidth();
    float textWidth = ImGui::CalcTextSize(text.c_str()).x;
    float offsetX = (columnWidth - textWidth) * 0.5f; // Center the text
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
    ImGui::Text("%s", text.c_str());
}


void DrawMovieWindow(void* sharedData) {
    ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_FirstUseEver);
    auto data = (SharedMovieData*)sharedData;
    auto movies = data->getMovies();

    ImGui::Begin("Top 250 Movies");

    int columns = 4; // Number of movies per row
    if (ImGui::BeginTable("MovieTable", columns, ImGuiTableFlags_SizingStretchSame)) {
        int movieIndex = 0;

        for (const auto& movie : movies) {
            if (movieIndex % columns == 0) {
                ImGui::TableNextRow(); // Start a new row after every 'columns' movies
            }

            ImGui::TableNextColumn();

            int imageWidth = 0, imageHeight = 0;
            std::string imagePath = movie.id + "_image.jpg";
            ID3D11ShaderResourceView* texture = LoadTexture(imagePath.c_str(), imageWidth, imageHeight);

            float columnWidth = ImGui::GetColumnWidth();
            float imageSizeX = 200.0f, imageSizeY = 200.0f; // Adjust as needed
            float imageOffsetX = (columnWidth - imageSizeX) * 0.5f;

            if (texture) {


                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + imageOffsetX);
                ImGui::Image((ImTextureID)texture, ImVec2(imageSizeX, imageSizeY));
            }
            else {
                CenteredText("Image not available");
            }


            CenteredText(movie.title);
            CenteredText("Year: " + std::to_string(movie.startYear));
            CenteredText("Rating: " + std::to_string(movie.averageRating));


            float buttonWidth = 120.0f; // Adjust as needed
            float buttonOffsetX = (columnWidth - buttonWidth) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + buttonOffsetX);

            if (ImGui::Button("View More", ImVec2(buttonWidth, 30))) {
                std::string command = "xdg-open " + movie.url;
                system(command.c_str());
            }

            ImGui::Separator();
            movieIndex++;
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void Display::operator()(SharedMovieData& sharedData){
	GuiMain(DrawMovieWindow, &sharedData);
	sharedData.exit_flag = true;
}


