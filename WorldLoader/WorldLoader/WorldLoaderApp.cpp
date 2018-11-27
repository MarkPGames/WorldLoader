#include "WorldLoaderApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "TileObject.h"
#include "imgui.h"


WorldLoaderApp::WorldLoaderApp() {

}

WorldLoaderApp::~WorldLoaderApp() {

}

bool WorldLoaderApp::startup() {

	m_2dRenderer = new aie::Renderer2D();

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("./font/consolas.ttf", 26);
	cameraPos = { 0,0 };
	mouseClickedPos = { 0, 0 };
	origin = { 0.0f, 0.0f };
	radius = 1;
	speed = 320.0f;
	chunkBorderColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	chunkAlpha = 0.2f;
	m_grassTexture = new aie::Texture("./textures/grass.png");

	grassObject = new TileObject({ 0, 0 }, m_grassTexture);
	//Chunk manager takes in the Tile type and tile size
	m_chunkManager = new ChunkManager<TileObject>(2);


	drawAllChunks = true;
	drawChunkBorders = true;
	followPlayer = false;


	return true;
}

void WorldLoaderApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
	delete grassObject;
	delete m_chunkManager;
	delete m_grassTexture;
}

void WorldLoaderApp::update(float deltaTime) {

	// input example
	aie::Input* input = aie::Input::getInstance();



	glm::vec2 mouseScreenPos = { input->getMouseX(), input->getMouseY() };

	if (input->wasMouseButtonPressed(0))
	{
		startedDraggingOnWindow = ImGui::IsMouseHoveringAnyWindow();

		mouseClickedPos = mouseScreenPos + cameraPos;
	}
	else if (startedDraggingOnWindow == false && input->isMouseButtonDown(0))
	{
		cameraPos = mouseClickedPos - mouseScreenPos;
	}

	if (followPlayer)
	{

		glm::vec2 vecBetween = origin - cameraPos;
		if (glm::length(vecBetween) > 50.0f)
		{
			cameraPos += vecBetween * deltaTime * 5.0f;
		}
	}


	if (input->wasKeyPressed(aie::INPUT_KEY_SPACE))
	{
		cameraPos = { 0, 0 };
	}


	if (input->isKeyDown(aie::INPUT_KEY_LEFT) || input->isKeyDown(aie::INPUT_KEY_A))
	{
		origin.x -= speed * deltaTime;
	}
	if (input->isKeyDown(aie::INPUT_KEY_RIGHT) || input->isKeyDown(aie::INPUT_KEY_D))
	{
		origin.x += speed * deltaTime;
	}
	if (input->isKeyDown(aie::INPUT_KEY_UP) || input->isKeyDown(aie::INPUT_KEY_W))
	{
		origin.y += speed * deltaTime;
	}
	if (input->isKeyDown(aie::INPUT_KEY_DOWN) || input->isKeyDown(aie::INPUT_KEY_S))
	{
		origin.y -= speed * deltaTime;
	}

	if (input->wasKeyReleased(aie::INPUT_KEY_MINUS))
	{
		if (radius >= 1)
		{
			radius--;
		}

	}
	if (input->wasKeyReleased(aie::INPUT_KEY_EQUAL))
	{
		radius++;
	}

	UpdateChunks();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	ImGui();
}

void WorldLoaderApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// set the camera position before we begin rendering
	m_2dRenderer->setCameraPos(cameraPos.x - float(getWindowWidth()) * 0.5f, cameraPos.y - float(getWindowHeight()) * 0.5f);

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!

	if (drawAllChunks)
	{
		DrawChunks();
	}

	DrawLoadedChunks();

	m_2dRenderer->setRenderColour(1, 1, 1, 1);
	m_2dRenderer->drawCircle(origin.x, origin.y, m_grassTexture->getWidth() * 0.5f);
	m_2dRenderer->setRenderColour(1, 1, 1, 1);

	glm::vec2 cameraBotLeft = cameraPos - glm::vec2(getWindowWidth() >> 1, getWindowHeight() >> 1);

	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", cameraBotLeft.x, cameraBotLeft.y);



	glm::vec2 cameraTopLeft = cameraPos + -glm::vec2(getWindowWidth() >> 1, -(int)(getWindowHeight() >> 1));

	// output some text, uses the last used colour
	//char fps[32];

	////Chunks in world
	//sprintf_s(fps, 32, "Total Chunks: %i", m_chunkManager->GetTotalChunks());
	//m_2dRenderer->drawText(m_font, fps, 0 + cameraBotLeft.x, getWindowHeight() - 26 + cameraBotLeft.y);
	//
	////Loaded Chunks
	//sprintf_s(fps, 32, "Loaded Chunks: %i", int(m_chunkManager->GetLoadedChunks().size()));
	//m_2dRenderer->drawText(m_font, fps, 0 + cameraBotLeft.x, getWindowHeight() - 52 + cameraBotLeft.y);
	//
	////Loaded Tiles
	//int loadedTiles = int(m_chunkManager->GetLoadedChunks().size()) * (int(m_chunkManager->GetChunkWidth())) * 2;
	//sprintf_s(fps, 32, "Loaded Tiles: %i", loadedTiles);
	//m_2dRenderer->drawText(m_font, fps, 0 + cameraBotLeft.x, getWindowHeight() - 78 + cameraBotLeft.y);
	//
	////Radius
	//sprintf_s(fps, 32, "Radius: %i", radius);
	//m_2dRenderer->drawText(m_font, fps, 0 + cameraBotLeft.x, getWindowHeight() - 104 + cameraBotLeft.y);

	// done drawing sprites
	m_2dRenderer->end();
}

void WorldLoaderApp::UpdateChunks()
{
	glm::vec2 playerChunkPos = origin / glm::vec2{ 32 * m_chunkManager->GetChunkWidth(), 32 * m_chunkManager->GetChunkWidth() };

	if (playerChunkPos.x < 0)
		playerChunkPos.x = std::floor(playerChunkPos.x);
	if (playerChunkPos.y < 0)
		playerChunkPos.y = std::floor(playerChunkPos.y);

	// Store all the coordinates in the radius
	std::vector<glm::ivec2> coords = m_chunkManager->GetChunkCoordsInRadius(playerChunkPos, radius);

	for (int i = 0; i < coords.size(); i++)
	{
		if (m_chunkManager->DoesChunkExist(coords[i]))
		{
			if (m_chunkManager->IsChunkLoaded(coords[i]))
			{

			}
			else
			{
				m_chunkManager->LoadChunk(coords[i]);
			}
		}
		else
		{
			//Create tiles
			std::vector<TileObject> tiles;

			for (int x = 0; x < (int)m_chunkManager->GetChunkWidth(); x++)
			{
				for (int y = 0; y < (int)m_chunkManager->GetChunkWidth(); y++)
				{
					tiles.push_back(TileObject({ 0, 0 }, m_grassTexture));
				}
			}

			m_chunkManager->CreateChunk(coords[i], tiles);
			m_chunkManager->LoadChunk(coords[i]);
		}
	}
	m_chunkManager->UnloadChunks(playerChunkPos, radius);
}

void WorldLoaderApp::DrawLoadedChunks()
{
	std::vector<std::pair<glm::ivec2, std::vector<TileObject>>> loadedChunks = m_chunkManager->GetLoadedChunks();

	int chunkWidth = m_chunkManager->GetChunkWidth();

	for (auto& value : loadedChunks)
	{
		glm::ivec2 chunkPos = value.first * chunkWidth;

		int i = 0;
		for (auto& tile : value.second)
		{
			glm::ivec2 tileChunkPos = { i / chunkWidth, i % chunkWidth };

			glm::ivec2 tileWorldPos = chunkPos + tileChunkPos;
			tile.setAlpha(1.0f);
			tile.draw(m_2dRenderer, tileWorldPos);


			++i;
		}
	}
}

void WorldLoaderApp::DrawChunks()
{
	std::map<int, std::map<int, std::vector<TileObject>>> worldChunks = m_chunkManager->GetWorld();

	int chunkWidth = m_chunkManager->GetChunkWidth();

	for (auto& chunkRow : worldChunks)
	{
		int chunkX = chunkRow.first;
		for (auto& chunk : chunkRow.second)
		{
			int chunkY = chunk.first;

			glm::ivec2 chunkPos{ chunkX, chunkY };

			int i = 0;
			for (auto& tile : chunk.second)
			{
				glm::ivec2 tileChunkPos = { i / chunkWidth, i % chunkWidth };

				glm::ivec2 tileWorldPos = chunkPos * chunkWidth + tileChunkPos;
				tile.setAlpha(chunkAlpha);
				tile.draw(m_2dRenderer, tileWorldPos);

				++i;
			}

			if (drawChunkBorders)
			{
				int pixelWidth = m_grassTexture->getWidth();
				glm::vec2 pos = chunkPos * chunkWidth * pixelWidth;

				m_2dRenderer->setRenderColour(chunkBorderColor.r, chunkBorderColor.g, chunkBorderColor.b, chunkBorderColor.a);
				m_2dRenderer->drawLine(pos.x, pos.y, pos.x + (float)chunkWidth * (float)pixelWidth, pos.y, 1.0f, -1.0f);
				m_2dRenderer->drawLine(pos.x, pos.y, pos.x, pos.y + (float)chunkWidth * (float)pixelWidth, 1.0f, -1.0f);
			}
		}
	}
}

void WorldLoaderApp::ImGui()
{
	ImGui::Begin("Editor");

	char totalChunks[32];
	sprintf_s(totalChunks, 32, "Total Chunks: %i", m_chunkManager->GetTotalChunks());
	ImGui::Text(totalChunks);

	char loadedChunks[32];
	sprintf_s(loadedChunks, 32, "Loaded Chunks: %i", int(m_chunkManager->GetLoadedChunks().size()));
	ImGui::Text(loadedChunks);

	char loadedTilesC[32];
	int loadedTiles = int(m_chunkManager->GetLoadedChunks().size()) * (int(m_chunkManager->GetChunkWidth())) * 2;
	sprintf_s(loadedTilesC, 32, "Loaded Tiles: %i", loadedTiles);
	ImGui::Text(loadedTilesC);
	ImGui::Text("\n");
	ImGui::Checkbox("Draw All Chunks", &drawAllChunks);
	if (drawAllChunks)
	{
		ImGui::SliderFloat("DARKNESS", &chunkAlpha, 0, 1);
	}
	ImGui::Checkbox("Draw Chunk Borders", &drawChunkBorders);

	if (drawChunkBorders)
	{
		ImGui::ColorEdit4("Colour", &chunkBorderColor[0]);
	}

	ImGui::SliderInt("Loaded Chunk Radius", &radius, 0, 50);
	
	ImGui::SliderFloat("Player Speed", &speed, 0.0f, 5000.0f);
	ImGui::Checkbox("Camera Follow Player", &followPlayer);

	ImGui::Text("\t\tControls:\nMove: Up Down Left Right, WASD\nCamera: Click & drag, Space\nRadius: +/-");

	ImGui::End();

}

