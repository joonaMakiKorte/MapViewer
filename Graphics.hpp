#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Graphics
{
public:
	// Constructor
	Graphics(double lat_max, double lat_min, double lon_max, double lon_min,
		float window_width, float window_height);

	// Transform given coordinates of latitude and longitude to graphics coordinates
	sf::Vector2f transformToSFML(double lat, double lon);

private:
	double lat_max, lat_min;
	double lon_max, lon_min;
	float window_width;
	float window_height;
};