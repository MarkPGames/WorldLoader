#pragma once
#include "WorldLoaderApp.h"
#include "Input.h"

class TileObject
{
public:
	TileObject() : m_grassTexture(nullptr), position({ 0, 0 }) {};
	~TileObject();

	TileObject(glm::ivec2 a_position, aie::Texture* a_grassTexture);

	bool startup();
	void shutdown();
	void update(aie::Input* input, float deltaTime);
	void draw(aie::Renderer2D* a_2dRenderer, glm::ivec2 coords);

	void setAlpha(float a) { rgba.a = a; };

private:
	aie::Texture* m_grassTexture;
	glm::ivec2 position;
	glm::vec4 rgba;
};

