#pragma once

#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>

template<typename T>
class ChunkManager
{
public:
	typedef std::vector<T> Chunk;
	typedef std::map<int, std::map<int, Chunk>> World;

	ChunkManager() = delete;
	ChunkManager(const unsigned int width) { WIDTH = width; }
	~ChunkManager() {}

	// Returns width of chunk
	const unsigned int GetChunkWidth() const
	{
		return WIDTH;
	}

	// Sets width of chunk
	void SetChunkWidth(const unsigned int width) //const
	{
		WIDTH = width;
	}

	// Calls LoadChunks() & UnloadChunks()
	void Update(const glm::ivec2 origin, const int radius, const T& tile)
	{
		LoadChunks(origin, radius, tile);
		UnloadChunks(origin, radius);
	}

	// Returns a "Tile" of type <T> at given position within the chunk
	T GetTile(const int x, const int y)
	{
		int chunkx = x / WIDTH;
		int chunky = y / WIDTH;

		return m_world[chunkx][chunky][(x % WIDTH) + (y % WIDTH) * WIDTH];
	}

	// Returns a chunk at given position
	Chunk GetChunk(const glm::ivec2 coords)
	{
		return m_world[coords.x][coords.y];
	}

	// Creates chunks with given radius and adds them to loadedChunks <vector>
	void LoadChunks(const glm::ivec2 origin, const int& radius, const T& tile)
	{
		assert(radius >= 0 && "Radius cannot be a negative value");
		std::vector<glm::ivec2> coords = GetChunkCoordsInRadius(origin, radius);

		for (int i = 0; i < coords.size(); i++)
		{
			// If the chunk doesn't exist the chunk will be created
			CreateChunk(coords[i], tile);
			// then load the chunk
			LoadChunk(coords[i]);
		}
	}

	// Adds pre-exisitng chunk to loadedChunk <vector>
	void LoadChunk(const glm::ivec2 coords)
	{
		if (IsChunkLoaded(coords) != true)
		{
			loadedChunks.push_back({ coords, GetChunk(coords) });
		}
	}

	// Removes chunks from loadedChunks <vector> which are not within radius
	void UnloadChunks(const glm::ivec2 origin, const int radius)
	{
		// Assert if radius is neg value
		assert(radius >= 0 && "Radius cannot be a negative value");

		// Store all the coordinates in the radius
		std::vector<glm::ivec2> coords = GetChunkCoordsInRadius(origin, radius);

		//Create an iterator with same type as loadedChunks
		std::vector<std::pair<glm::ivec2, Chunk>>::iterator it = loadedChunks.begin();

		// Loop through each loaded chunk
		for (; it != loadedChunks.end(); )
		{
			// Make sure the iterator isn't past the last element
			if (it != loadedChunks.end())
			{
				// If the current chunk is in the coords
				if (std::find(coords.begin(), coords.end(), it->first) != coords.end())
				{
					it++;
				}
				else // The chunk should be unloaded
				{
					it = loadedChunks.erase(it);
				}
			}
		}
	}

	// Creates a chunk within the <map> (takes in vector of <T>)
	void CreateChunk(const glm::ivec2 coords, const Chunk tiles)
	{
		if (DoesChunkExist(coords))
			return;

		// emplace lets you insert more than one pair (instead of insert)
		m_world[coords.x].emplace(coords.y, tiles);
	}

	// Creates a chunk within the <map> (takes in <T>)
	void CreateChunk(const glm::ivec2 coords, const T& tile)
	{
		std::vector<T> newChunk(WIDTH * WIDTH);

		for (int y = 0; y < int(WIDTH); y++)
		{
			for (int x = 0; x < int(WIDTH); x++)
			{
				newChunk[x + y * WIDTH] = T(tile);
			}
		}
		// Call "original" CreateChunk() to add it to the world<map>
		CreateChunk(coords, newChunk);
	}

	// Removes chunks from loadedChunks <vector> & the world<map> outside the radius
	void DeleteChunksOutsideRadius(const glm::ivec2 origin, const int radius)
	{
		// Easily create custom data type
		typedef	std::vector<std::pair<glm::ivec2, Chunk>>::iterator loadedChunksIter;

		// Loop through each loaded chunk
		for (loadedChunksIter it = loadedChunks.begin(); it != loadedChunks.end(); )
		{
			// Ensure we are not out of range
			if (it != loadedChunks.end())
			{
				// Calculate how far away the chunk is from the origin
				glm::vec2 displacement = (it->first - origin);

				// If the length of that vector is outside the radius
				if (glm::length(displacement) > radius)
				{
					DeleteChunk(it->first);
					// Set the iterator to erase() as it returns an iterator to the next element
					it = loadedChunks.erase(it);
				}
				else
				{
					// iterate
					++it;
				}

			}
		}
	}

	// Removes chunk form the world<map>
	void DeleteChunk(const glm::ivec2 coords)
	{
		if (m_world.find(coords.x) != m_world.end()) // Tests to make sure that row exists in the map.
			m_world[coords.x].erase(coords.y);
	}

	// Checks if chunk exists within the world<map>
	bool DoesChunkExist(const glm::ivec2 coords)
	{
		auto chunkRowIter = m_world.find(coords.x);
		if (chunkRowIter != m_world.end()) // Tests to make sure that row exists in the map.
		{
			auto chunkIter = (*chunkRowIter).second.find(coords.y);
			if (chunkIter != (*chunkRowIter).second.end())
			{
				return true; // THE CHUNK ALREADY EXISTS
			}
		}
		return false;
	}

	// Checks if existing chunk is loaded
	bool IsChunkLoaded(glm::ivec2 coords)
	{
		for (auto& chunk : loadedChunks)
			if (chunk.first == coords)
				return true;

		return false;
	}

	// Returns a reference to loadedChunks <vector>
	std::vector<std::pair<glm::ivec2, Chunk>>& GetLoadedChunks()
	{
		return loadedChunks;
	}

	World GetWorld()
	{
		return m_world;
	}

	//Returns total amount of chunks in the world<map>
	int GetTotalChunks()
	{
		int count = 0;

		for (auto& chunkRow : m_world)
		{
			count += (int)chunkRow.second.size();
		}
		return count;
	}

	// Returns a <vector> of coords within the radius
	std::vector<glm::ivec2> GetChunkCoordsInRadius(const glm::ivec2& origin, const int& radius)
	{
		std::vector<glm::ivec2> coords;

		//Loop through all coords within a box with width and height of radius
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				//Store new coord relative to origin
				glm::ivec2 newChunkCoord = { origin.x + x, origin.y + y };

				//Calculate how far away the chunk is from the origin
				glm::vec2 displacement = (newChunkCoord - origin);

				//If the length of that vector is outside the radius
				if (glm::length(displacement) <= radius)
				{
					coords.push_back(newChunkCoord);
				}
			}
		}
		return coords;
	}

private:
	World m_world;
	unsigned int WIDTH;
	std::vector<std::pair<glm::ivec2, Chunk>> loadedChunks;
};

