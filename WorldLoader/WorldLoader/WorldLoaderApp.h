#pragma once
#include "ChunkManager.h"
#include "Application.h"
#include "Renderer2D.h"

class TileObject;

class WorldLoaderApp : public aie::Application {
public:

	WorldLoaderApp();
	virtual ~WorldLoaderApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void UpdateChunks();
	void DrawLoadedChunks();
	void DrawChunks();
	void ImGui();

protected:
	ChunkManager<TileObject> * m_chunkManager;
	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	aie::Texture* m_grassTexture;
	TileObject* m_grassObject;


	glm::vec2 cameraPos;
	glm::vec2 mouseClickedPos;
	TileObject* grassObject;
	glm::vec2 origin;
	float speed;
	int radius;

	bool startedDraggingOnWindow;

	// ImGui options
	bool drawAllChunks;
	bool drawChunkBorders;
	bool followPlayer;
	float chunkAlpha;
	glm::vec4 chunkBorderColor;
	//chunk size?



};