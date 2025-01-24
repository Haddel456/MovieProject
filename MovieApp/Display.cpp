#include "Display.h"
#include "../ImGuiLib/GuiMain.h"
#include "SharedMovieData.h"
#include <imgui.h>


// show the result on screen 
// show the movie 

void DrawAppWindow(void* common_ptr) {

	// ImGui::Begin("Top 250 Movies");

// auto movies = sharedData.getMovies();

// // Check if the data is ready
// if (!sharedData.isDataReady()) {
//     ImGui::Text("No data available. Please wait...");
//     return;
// }

// // Display each movie's information
///* if (ImGui::Begin("Movie List")) {*/
//     for (const auto& movie : movies) {
//         ID3D11ShaderResourceView* texture = nullptr;
//         int imageWidth = 0, imageHeight = 0;
//         std::string imagePath = movie.id + "_image.jpg";

//         if (LoadTextureFromFile(imagePath.c_str(), &texture, &imageWidth, &imageHeight)) {
//             // Display the image
//             ImGui::Image((ImTextureID)texture, ImVec2((float) imageWidth, (float)imageHeight));  
//         }
//         else {
//             ImGui::Text("Image not available for %s", movie.title.c_str());
//         }

//         ImGui::Separator();
//         ImGui::Text("Title: %s", movie.title.c_str());
//         ImGui::Text("Description: %s", movie.description.c_str());
//         ImGui::Text("Year: %d", movie.startYear);
//         ImGui::Text("Rating: %.1f", movie.averageRating);
//         ImGui::Text("Votes: %d", movie.numVotes);
//         ImGui::Text("Type: %s", movie.type);

//         // Display the movie's URL as a clickable link
//         if (ImGui::Button("View More")) {
//             // Implement an action for the button, such as opening the URL in a browser
//             std::string command = "xdg-open " + movie.url;
//             system(command.c_str());
//         }
//         ImGui::Separator();
//     //}
//     
// }
// ImGui::End();

}

void Display::operator()(SharedMovieData& sharedData){
	GuiMain(DrawAppWindow, &sharedData);

}




