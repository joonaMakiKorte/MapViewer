#include "Graphics.hpp"

Graphics::Graphics(double lat_max, double lat_min, double lon_max, double lon_min,
	float window_width, float window_height) :
	lat_max(lat_max), lat_min(lat_min), lon_max(lon_max), lon_min(lon_min),
	window_width(window_width), window_height(window_height)
{}

sf::Vector2f Graphics::transformToSFML(double lat, double lon)
{
	// Calculate normalized latitude and longitude for correct graphics coordinates
	double normalized_y = (lat_max - lat) / (lat_max - lat_min);
	double normalized_x = (lon - lon_min) / (lon_max - lon_min);

	// Scale to window size
	float x = static_cast<float>(normalized_x * window_width);
	float y = static_cast<float>(normalized_y * window_height);

	return sf::Vector2f(x, y);
}
