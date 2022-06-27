#include "Renderer.h"

#include <iostream>

#include "Walnut/Random.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { x / (float)m_FinalImage->GetWidth(), y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // -1 -> 1

			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage()
{
	return m_FinalImage;
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	int red = 255;
	int green = 0;
	int blue = 255;

	float cameraDistanceZ = 2.0f;
	float cameraDistanceX = 0.0f;
	float cameraDistanceY = 0.0f;

	float lightDistanceZ = 2.0f;
	float lightDistanceX = 0.0f;
	float lightDistanceY = 0.0f;

	float radius = 0.5f;
	glm::vec3 rayOrigin(cameraDistanceX, cameraDistanceY, cameraDistanceZ);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	rayDirection = glm::normalize(rayDirection);

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// Quadratic formula discriminant:
	// b^2 - 4ac

	// Actual quadratic formula
	// x = (-b +- sqrt(b*b - 4ac) / 2ac)

	const float discriminant = b * b - 4.0f * a * c;
	//const float positiveQuad = (b + sqrt(b * b - 4 * a * c)) / 2 * a;

	// Checks if it is hit with light, if it is, make it magenta
	if (discriminant >= 0.0f)
	{
		const float quad = abs((b + sqrt(b * b - 4 * a * c)) / 2 * a);
		return (255 << 24) + ((static_cast<int>(blue / quad) & 0xff) << 16) + ((static_cast<int>(green / quad) & 0xff) << 8)
			+ (static_cast<int>(red / quad) & 0xff);
	}

	return 0xff000000;
}
