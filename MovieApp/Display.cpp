#include "Display.h"
#include "../ImGuiLib/GuiMain.h"
#include "SharedMovieData.h"
#include <imgui.h>


// show the result on screen 
// show the movie 

void DrawMovieWindow(void* sharedData) {

	// ImGui::Begin("Top 250 Movies");
	 auto data = (SharedMovieData*)sharedData;
	 auto movies = data->getMovies();

	 // Check if the data is ready
	 if (!data->isDataReady()) {
	     ImGui::Text("No data available. Please wait...");
	     return;
	 }

	// Display each movie's information

	     for (const auto& movie : movies) {
			 int imageWidth = 0, imageHeight = 0;
			 std::string imagePath = movie.id + "_image.jpg";

			 //if (LoadTexture(imagePath.c_str(), imageWidth, imageHeight)) {
				// // Use the shader resource view for the movie image (assume srv is available)
				/* ImGui::Image((ImTextureID)srv, ImVec2((float)imageWidth, (float)imageHeight));*/
			 //}
			 //else {
				// ImGui::Text("Image not available for %s", movie.title.c_str());
			 //}

			 ID3D11ShaderResourceView* texture = LoadTexture(imagePath.c_str(), imageWidth, imageHeight);

			 if (texture) {
				 // Display the image using the SRV
				 ImGui::Image((ImTextureID)texture, ImVec2((float)imageWidth, (float)imageHeight));
			 }
			 else {
				 ImGui::Text("Image not available for %s", movie.title.c_str());
			 }

	         ImGui::Separator();
	         ImGui::Text("Title: %s", movie.title.c_str());
	         ImGui::Text("Description: %s", movie.description.c_str());
	         ImGui::Text("Year: %d", movie.startYear);
	         ImGui::Text("Rating: %.1f", movie.averageRating);
	         ImGui::Text("Votes: %d", movie.numVotes);
	         ImGui::Text("Type: %s", movie.type);

	         // Display the movie's URL as a clickable link
	         if (ImGui::Button("View More")) {
	             // Implement an action for the button, such as opening the URL in a browser
	             std::string command = "xdg-open " + movie.url;
	             system(command.c_str());
	         }
	         ImGui::Separator();
	    
	     
	 }
	 ImGui::End();

}

void Display::operator()(SharedMovieData& sharedData){
	GuiMain(DrawMovieWindow, &sharedData);
	sharedData.exit_flag = true;
}




