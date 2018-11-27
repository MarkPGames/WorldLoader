#include "TileObject.h"
#include "Texture.h"
#include <random>


TileObject::~TileObject()
{
}

TileObject::TileObject(glm::ivec2 a_position, aie::Texture * a_grassTexture)
{
	m_grassTexture = a_grassTexture;
	position = a_position;
	rgba = { (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, 1.0f};
}

bool TileObject::startup()
{
	return true;
}

void TileObject::shutdown()
{
}

void TileObject::update(aie::Input * input, float deltaTime)
{
}

void TileObject::draw(aie::Renderer2D * a_2dRenderer, glm::ivec2 coords)
{
	a_2dRenderer->setRenderColour(rgba.r, rgba.g, rgba.b, rgba.a);

	if (m_grassTexture != nullptr)
	{
		float textureWidth = float(m_grassTexture->getWidth());

		a_2dRenderer->drawSprite(m_grassTexture, coords.x * textureWidth + textureWidth * 0.5f , coords.y *  textureWidth + textureWidth * 0.5f, textureWidth, textureWidth);
	}
	else
	{
		//for debugging
		a_2dRenderer->drawCircle(0, 0, 32, 0);
	}

}
