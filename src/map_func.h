/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file map_func.h Functions related to maps. */

#ifndef MAP_FUNC_H
#define MAP_FUNC_H

#include "core/bitmath_func.hpp"
#include "core/enum_type.hpp"
#include "core/math_func.hpp"
#include "tile_type.h"
#include "map_type.h"
#include "direction_func.h"

/**
 * Wrapper class to abstract away the way the tiles are stored. It is
 * intended to be used to access the "map" data of a single tile.
 *
 * The wrapper is expected to be fully optimized away by the compiler, even
 * with low optimization levels except when completely disabling it.
 */
class Tile {
private:
	friend struct Map;
	/**
	 * Data that is stored per tile.
	 * Look at docs/landscape.html for the exact meaning of the members.
	 */
	struct TileStorage {
		uint16_t part2; ///< m2 for all tiles.
		uint16_t part6; ///< m8 for sub tiles and type for base tiles.
		uint8_t part1; ///< m1 for all tiles.
		uint8_t part3; ///< m3 for all tiles.
		uint8_t part4; ///< m7 for sub tiles and height for base tiles.
		uint8_t part5; ///< m5 for sub tiles and offset for base tiles. @important Offset is runtime only.
	};

	static_assert(sizeof(TileStorage) == 8);

	static constexpr uint16_t ONLY_ROAD_AND_RAIL = to_underlying(TileType::Road) | (to_underlying(TileType::Railway) << TILE_TYPE_BITS) | (to_underlying(TileType::Invalid) << (TILE_TYPE_BITS * 2)) | (to_underlying(TileType::Invalid) << (TILE_TYPE_BITS * 3));
	static constexpr auto BASE_TILES = TileTypes{TileType::Clear, TileType::Water, TileType::Trees, TileType::Void};
	static constexpr uint16_t SUB_TILES_COUNT_MASK = 0x7; ///< Specifies where in the type part is stored sub tiles quantity. @note Used instead of %GB for performance reason.

	static std::unique_ptr<TileStorage[]> base_tiles; ///< Pointer to the tile-array.
	static std::unique_ptr<std::vector<TileStorage>[]> extended_tiles; ///< Pointer to the extended tile-array.

	TileIndex tile; ///< The tile to access the map data for.

public:
	/**
	 * Create the tile wrapper for the given tile.
	 * @param tile The tile to access the map for.
	 */
	[[debug_inline]] inline Tile(TileIndex tile) : tile(tile) {}

	/**
	 * Create the tile wrapper for the given tile.
	 * @param tile The tile to access the map for.
	 */
	Tile(uint tile) : tile(tile) {}

	/**
	 * Implicit conversion to the TileIndex.
	 * @return The converted tile index.
	 */
	[[debug_inline]] inline constexpr operator TileIndex() const { return this->tile; }

	/**
	 * Implicit conversion to the uint for bounds checking.
	 * @return The (unsigned) integer representation of the tile location.
	 */
	[[debug_inline]] inline constexpr operator uint() const { return this->tile.base(); }

	/**
	 * The type (bits 4..7), bridges (2..3), rainforest/desert (0..1)
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @return reference to the byte holding the data.
	 */
	[[debug_inline]] inline uint8_t &type()
	{
		return base_tiles[this->tile.base()].part3;
	}

	/**
	 * The types of sub tiles.
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @return reference to the uint16_t holding the data.
	 */
	[[debug_inline]] inline uint16_t &sub_tiles_types()
	{
		return base_tiles[this->tile.base()].part6;
	}

	/**
	 * The height of the northern corner
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @return reference to the byte holding the height.
	 */
	[[debug_inline]] inline uint8_t &height()
	{
		return base_tiles[this->tile.base()].part4;
	}

	/**
	 * Get the offset in sub tiles array for this tile from offsets array.
	 * @return Reference to the offset in sub tiles array.
	 */
	[[debug_inline]] inline uint8_t &offset()
	{
		return base_tiles[this->tile.base()].part5;
	}

	/**
	 * Get how many sub tiles does this tile has.
	 * @return The number of available sub tiles.
	 */
	[[debug_inline]] inline uint8_t GetNumberOfSubTiles()
	{
		return 1; // Temporarly there is one sub tile.
		return this->type() & Tile::SUB_TILES_COUNT_MASK;
	}

	/**
	 * Get the reference to tile storage based on given %TileType.
	 * @tparam tile_type The type of sub tile to get data for.
	 * @return Reference to the sub tile's storage.
	 */
	template <TileType tile_type> [[debug_inline]] inline Tile::TileStorage &GetTileStorage();

	/**
	 * Primarily used for ownership information
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @tparam tile_type For which type of tile access the memory piece.
	 * @return reference to the byte holding the data.
	 */
	template <TileType tile_type = TileType::OldStorage>
	[[debug_inline]] inline uint8_t &m1()
	{
		return this->GetTileStorage<tile_type>().part1;
	}

	/**
	 * Primarily used for indices to towns, industries and stations
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @tparam tile_type For which type of tile access the memory piece.
	 * @return reference to the uint16_t holding the data.
	 */
	template <TileType tile_type = TileType::OldStorage>
	[[debug_inline]] inline uint16_t &m2()
	{
		return this->GetTileStorage<tile_type>().part2;
	}

	/**
	 * General purpose
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @tparam tile_type For which type of tile access the memory piece.
	 * @return reference to the byte holding the data.
	 */
	template <TileType tile_type = TileType::OldStorage>
	[[debug_inline]] inline uint8_t &m3()
	{
		static_assert(!Tile::BASE_TILES.Test(tile_type));
		return this->GetTileStorage<tile_type>().part3;
	}

	/**
	 * Used when loading old saves. @see afterload.cpp.
	 * @return reference to the byte holding the data.
	 */
	[[debug_inline]] inline uint8_t &m4()
	{
		return base_tiles[this->tile.base()].part1; // Temporarly use m1 for it.
	}

	/**
	 * General purpose
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @tparam tile_type For which type of tile access the memory piece.
	 * @return reference to the byte holding the data.
	 */
	template <TileType tile_type = TileType::OldStorage>
	[[debug_inline]] inline uint8_t &m5()
	{
		static_assert(!Tile::BASE_TILES.Test(tile_type));
		return this->GetTileStorage<tile_type>().part5;
	}

	/**
	 * Used when loading old saves. @see afterload.cpp.
	 * @return reference to the byte holding the data.
	 */
	[[debug_inline]] inline uint8_t &m6()
	{
		return base_tiles[this->tile.base()].part3; // Temporarly use m3, because m2 is uint16_t and not uint8_t.
	}

	/**
	 * Primarily used for newgrf support
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @tparam tile_type For which type of tile access the memory piece.
	 * @return reference to the byte holding the data.
	 */
	template <TileType tile_type = TileType::OldStorage>
	[[debug_inline]] inline uint8_t &m7()
	{
		static_assert(!Tile::BASE_TILES.Test(tile_type));
		return this->GetTileStorage<tile_type>().part4;
	}

	/**
	 * General purpose
	 *
	 * Look at docs/landscape.html for the exact meaning of the data.
	 * @tparam tile_type For which type of tile access the memory piece.
	 * @return reference to the uint16_t holding the data.
	 */
	template <TileType tile_type = TileType::OldStorage>
	[[debug_inline]] inline uint16_t &m8()
	{
		static_assert(!Tile::BASE_TILES.Test(tile_type));
		return this->GetTileStorage<tile_type>().part6;
	}

	/**
	 * Set the %TileType of one of the sub tiles.
	 * @param sub_tile The numeric index of sub tile to set the type of. It is in range 0..(MAX_SUB_TILES_QUANTITY - 1).
	 * @param new_type The new %TileType to set.
	 */
	void SetSubTileType(uint8_t sub_tile, TileType new_type)
	{
		SB(this->sub_tiles_types(), sub_tile * TILE_TYPE_BITS, TILE_TYPE_BITS, to_underlying(new_type));
	}

	/**
	 * Get the %TileType of one of the sub tiles.
	 * @param sub_tile The numeric index of sub tile to get the type of. It is in range 0..(MAX_SUB_TILES_QUANTITY - 1).
	 * @return The %TileType of given sub tile.
	 */
	TileType GetSubTileType(uint8_t sub_tile)
	{
		return TileType(GB(this->sub_tiles_types(), sub_tile * TILE_TYPE_BITS, TILE_TYPE_BITS));
	}

	/**
	 * Get what sub tiles this tile has.
	 * @return Bit mask, where if n-th bit is set, then sub tile of TileType == n is available.
	 */
	inline TileTypes GetSubtiles()
	{
		return {this->GetSubTileType(0), this->GetSubTileType(1), this->GetSubTileType(2), this->GetSubTileType(3)};
	}

	/**
	 * Creates new sub tile in the sub tiles array for this tile index.
	 * @tparam tile_type Of what type is the new sub tile.
	 */
	template <TileType tile_type>
	void AddSubTile()
	{
		assert(!TileTypes{this->GetSubtiles()}.Test(tile_type));

		auto chunk = this->tile.base() / INDEXES_PER_SUB_TILES_CHUNK;
		uint8_t new_offset = 0;

		auto finalize = [this, new_offset](const std::vector<Tile::TileStorage> &old_storage, std::vector<Tile::TileStorage> new_storage) {
			if constexpr (tile_type == TileType::Railway) {
				if (this->GetNumberOfSubTiles() == 1 && this->GetSubtiles().Test(TileType::Road)) {
					new_storage[new_offset] = old_storage[this->offset()]; // Copy the road.
					new_storage[new_offset + 1] = Tile::TileStorage{}; // Clear the new tile.
					this->sub_tiles_types() = Tile::ONLY_ROAD_AND_RAIL;
				} else {
					for (int j = this->GetNumberOfSubTiles() - 1; j >= 0; --j) {
						new_storage[new_offset + 1 + j] = old_storage[this->offset() + j];
						this->SetSubTileType(j + 1, this->GetSubTileType(j));
					}
					new_storage[new_offset] = Tile::TileStorage{}; // Clear the new tile.
					this->SetSubTileType(0, tile_type);
				}
			} else if constexpr (tile_type == TileType::Road) {
				int end = 0;
				if (this->GetNumberOfSubTiles() >= 2) {
					end = 1;
					new_storage[new_offset] = old_storage[this->offset()]; // Copy the first sub tile.
				}
				new_storage[new_offset + end] = Tile::TileStorage{}; // Clear the new tile.
				for (int j = this->GetNumberOfSubTiles() - 1; j >= end; --j) {
					new_storage[new_offset + 1 + j] = old_storage[this->offset() + j];
					this->SetSubTileType(j + 1, this->GetSubTileType(j));
				}
				this->SetSubTileType(end, tile_type);
			} else {
				for (int j = 0; j < this->GetNumberOfSubTiles(); ++j) {
					new_storage[new_offset + j] = old_storage[this->offset() + j];
				}
				this->SetSubTileType(this->GetNumberOfSubTiles(), tile_type);
			}
		};

		if (extended_tiles[chunk].size() > MAX_SUB_TILES_OFFSET) {
			/* No space for new sub tile, clear unused parts. */
			size_t available_sub_tiles = 0;
			for (TileIndex i{chunk * INDEXES_PER_SUB_TILES_CHUNK}; i < (chunk + 1) * INDEXES_PER_SUB_TILES_CHUNK; ++i) {
				available_sub_tiles += Tile(i).GetNumberOfSubTiles();
			}
			std::vector<Tile::TileStorage> new_state(available_sub_tiles + 1);
			for (TileIndex i{chunk * INDEXES_PER_SUB_TILES_CHUNK}; i < (chunk + 1) * INDEXES_PER_SUB_TILES_CHUNK; ++i) {
				if (i == this->tile) continue;
				Tile t = Tile(i);
				for (int j = 0; j < t.GetNumberOfSubTiles(); ++j) {
					new_state[new_offset + j] = extended_tiles[chunk][t.offset() + j];
				}
				t.offset() = new_offset;
				new_offset += t.GetNumberOfSubTiles();
			}
			finalize(extended_tiles[chunk], new_state);
			new_state.swap(extended_tiles[chunk]);
		} else {
			new_offset = extended_tiles[chunk].size();
			extended_tiles[chunk].resize(new_offset + this->GetNumberOfSubTiles() + 1);
			finalize(extended_tiles[chunk], extended_tiles[chunk]);
		}

		this->offset() = new_offset;
		this->type() = (this->type() & ~Tile::SUB_TILES_COUNT_MASK) | (this->type() & Tile::SUB_TILES_COUNT_MASK + 1);
	}

	/**
	 * Removes sub tile in the sub tiles array for this tile index.
	 * @tparam tile_type Specifies which sub tile to remove.
	 */
	template <TileType tile_type>
	void RemoveSubTile()
	{
		assert(TileTypes{this->GetSubtiles()}.Test(tile_type));

		if constexpr (tile_type == TileType::Railway) {
			auto chunk = this->tile.base() / INDEXES_PER_SUB_TILES_CHUNK;
			if (this->sub_tiles_types() != Tile::ONLY_ROAD_AND_RAIL) {
				for (int i = 1; i < this->GetNumberOfSubTiles(); ++i) {
					extended_tiles[chunk][this->offset() + i - 1] = extended_tiles[chunk][this->offset() + i];
					this->SetSubTileType(i - 1, this->GetSubTileType(i));
				}
			}
		} else if constexpr (tile_type == TileType::Road) {
			auto chunk = this->tile.base() / INDEXES_PER_SUB_TILES_CHUNK;
			auto last_sub_tile_index = this->GetNumberOfSubTiles() - 1;
			extended_tiles[chunk][this->offset() + last_sub_tile_index - 1] = extended_tiles[chunk][this->offset() + last_sub_tile_index];
			this->SetSubTileType(last_sub_tile_index - 1, this->GetSubTileType(last_sub_tile_index));
		}

		this->SetSubTileType(this->GetNumberOfSubTiles() - 1, TileType::Invalid);
		this->type() = (this->type() & ~Tile::SUB_TILES_COUNT_MASK) | (this->type() & Tile::SUB_TILES_COUNT_MASK - 1);
	}
};

template <TileType tile_type>
[[debug_inline]] inline Tile::TileStorage &Tile::GetTileStorage()
{
	static_assert(!Tile::BASE_TILES.Test(tile_type));
	/* Return the last sub tile. */
	return extended_tiles[this->tile.base() / INDEXES_PER_SUB_TILES_CHUNK][this->offset() + this->GetNumberOfSubTiles() - 1];
}

template <> [[debug_inline]] inline Tile::TileStorage &Tile::GetTileStorage<TileType::Railway>()
{
	/* Return second sub tile iff has only rail and road sub tiles. Return first sub tile otherwise. */
	return Tile::extended_tiles[this->tile.base() / INDEXES_PER_SUB_TILES_CHUNK][this->offset() + size_t(this->sub_tiles_types() == Tile::ONLY_ROAD_AND_RAIL)];
}

template <> [[debug_inline]] inline Tile::TileStorage &Tile::GetTileStorage<TileType::Road>()
{
	/* Return first sub tile if has two or less sub tiles and second otherwise. */
	return Tile::extended_tiles[this->tile.base() / INDEXES_PER_SUB_TILES_CHUNK][this->offset() + size_t(this->GetNumberOfSubTiles() > 2)];
}

template <> [[debug_inline]] inline Tile::TileStorage &Tile::GetTileStorage<TileType::Clear>()
{
	return base_tiles[this->tile.base()];
}

template <> [[debug_inline]] inline Tile::TileStorage &Tile::GetTileStorage<TileType::Water>()
{
	return base_tiles[this->tile.base()];
}

template <> [[debug_inline]] inline Tile::TileStorage &Tile::GetTileStorage<TileType::Trees>()
{
	return base_tiles[this->tile.base()];
}

template <> [[debug_inline]] inline Tile::TileStorage &Tile::GetTileStorage<TileType::Void>()
{
	return base_tiles[this->tile.base()];
}

/**
 * Size related data of the map.
 */
struct Map {
private:
	/**
	 * Iterator to iterate all Tiles
	 */
	struct Iterator {
		typedef Tile value_type;
		typedef Tile *pointer;
		typedef Tile &reference;
		typedef size_t difference_type;
		typedef std::forward_iterator_tag iterator_category;

		explicit Iterator(TileIndex index) : index(index) {}
		bool operator==(const Iterator &other) const { return this->index == other.index; }
		Tile operator*() const { return this->index; }
		Iterator & operator++() { this->index++; return *this; }
	private:
		TileIndex index;
	};

	/** Iterable ensemble of all %Tiles. */
	struct IterateWrapper {
		Iterator begin() { return Iterator(TileIndex{}); }
		Iterator end() { return Iterator(TileIndex{Map::Size()}); }
		bool empty() { return false; }
	};

	static uint log_x;     ///< 2^_map_log_x == _map_size_x
	static uint log_y;     ///< 2^_map_log_y == _map_size_y
	static uint size_x;    ///< Size of the map along the X
	static uint size_y;    ///< Size of the map along the Y
	static uint size;      ///< The number of tiles on the map
	static uint tile_mask; ///< _map_size - 1 (to mask the mapsize)

	static uint initial_land_count; ///< Initial number of land tiles on the map.

public:
	static void Allocate(uint size_x, uint size_y);
	static void CountLandTiles();

	/**
	 * Logarithm of the map size along the X side.
	 * @note try to avoid using this one
	 * @return 2^"return value" == Map::SizeX()
	 */
	[[debug_inline]] inline static uint LogX()
	{
		return Map::log_x;
	}

	/**
	 * Logarithm of the map size along the y side.
	 * @note try to avoid using this one
	 * @return 2^"return value" == Map::SizeY()
	 */
	static inline uint LogY()
	{
		return Map::log_y;
	}

	/**
	 * Get the size of the map along the X
	 * @return the number of tiles along the X of the map
	 */
	[[debug_inline]] inline static uint SizeX()
	{
		return Map::size_x;
	}

	/**
	 * Get the size of the map along the Y
	 * @return the number of tiles along the Y of the map
	 */
	static inline uint SizeY()
	{
		return Map::size_y;
	}

	/**
	 * Get the size of the map
	 * @return the number of tiles of the map
	 */
	[[debug_inline]] inline static uint Size()
	{
		return Map::size;
	}

	/**
	 * Gets the maximum X coordinate within the map, including TileType::Void
	 * @return the maximum X coordinate
	 */
	[[debug_inline]] inline static uint MaxX()
	{
		return Map::SizeX() - 1;
	}

	/**
	 * Gets the maximum Y coordinate within the map, including TileType::Void
	 * @return the maximum Y coordinate
	 */
	static inline uint MaxY()
	{
		return Map::SizeY() - 1;
	}

	/**
	 * Scales the given value by the number of water tiles.
	 * @param n the value to scale
	 * @return the scaled size
	 */
	static inline uint ScaleByLandProportion(uint n)
	{
		/* Use 64-bit arithmetic to avoid overflow. */
		return static_cast<uint>(static_cast<uint64_t>(n) * Map::initial_land_count / Map::size);
	}

	/**
	 * 'Wraps' the given "tile" so it is within the map.
	 * It does this by masking the 'high' bits of.
	 * @param tile the tile to 'wrap'.
	 * @return The wrapped tile.
	 */
	static inline TileIndex WrapToMap(TileIndex tile)
	{
		return TileIndex{tile.base() & Map::tile_mask};
	}

	/**
	 * Scales the given value by the map size, where the given value is
	 * for a 256 by 256 map.
	 * @param n the value to scale
	 * @return the scaled size
	 */
	static inline uint ScaleBySize(uint n)
	{
		/* Subtract 12 from shift in order to prevent integer overflow
		 * for large values of n. It's safe since the min mapsize is 64x64. */
		return CeilDiv(n << (Map::LogX() + Map::LogY() - 12), 1 << 4);
	}

	/**
	 * Scales the given value by the maps circumference, where the given
	 * value is for a 256 by 256 map
	 * @param n the value to scale
	 * @return the scaled size
	 */
	static inline uint ScaleBySize1D(uint n)
	{
		/* Normal circumference for the X+Y is 256+256 = 1<<9
		 * Note, not actually taking the full circumference into account,
		 * just half of it. */
		return CeilDiv((n << Map::LogX()) + (n << Map::LogY()), 1 << 9);
	}

	/**
	 * Check whether the map has been initialized, as to not try to save the map
	 * during crashlog when the map is not there yet.
	 * @return true when the map has been allocated/initialized.
	 */
	static bool IsInitialized()
	{
		return Tile::base_tiles != nullptr;
	}

	/**
	 * Returns an iterable ensemble of all Tiles
	 * @return an iterable ensemble of all Tiles
	 */
	static IterateWrapper Iterate() { return IterateWrapper(); }
};

/**
 * Returns the TileIndex of a coordinate.
 *
 * @param x The x coordinate of the tile
 * @param y The y coordinate of the tile
 * @return The TileIndex calculated by the coordinate
 */
[[debug_inline]] inline static TileIndex TileXY(uint x, uint y)
{
	return TileIndex{(y << Map::LogX()) + x};
}

/**
 * Calculates an offset for the given coordinate(-offset).
 *
 * This function calculate an offset value which can be added to a
 * #TileIndex. The coordinates can be negative.
 *
 * @param x The offset in x direction
 * @param y The offset in y direction
 * @return The resulting offset value of the given coordinate
 * @see ToTileIndexDiff(TileIndexDiffC)
 */
inline TileIndexDiff TileDiffXY(int x, int y)
{
	/* Multiplication gives much better optimization on MSVC than shifting.
	 * 0 << shift isn't optimized to 0 properly.
	 * Typically x and y are constants, and then this doesn't result
	 * in any actual multiplication in the assembly code.. */
	return (y * Map::SizeX()) + x;
}

/**
 * Get a tile from the virtual XY-coordinate.
 * @param x The virtual x coordinate of the tile.
 * @param y The virtual y coordinate of the tile.
 * @return The TileIndex calculated by the coordinate.
 */
[[debug_inline]] inline static TileIndex TileVirtXY(uint x, uint y)
{
	return TileIndex{(y >> 4 << Map::LogX()) + (x >> 4)};
}

TileIndex TileVirtXYClampedToMap(int x, int y);

/**
 * Get the X component of a tile
 * @param tile the tile to get the X component of
 * @return the X component
 */
[[debug_inline]] inline static uint TileX(TileIndex tile)
{
	return tile.base() & Map::MaxX();
}

/**
 * Get the Y component of a tile
 * @param tile the tile to get the Y component of
 * @return the Y component
 */
[[debug_inline]] inline static uint TileY(TileIndex tile)
{
	return tile.base() >> Map::LogX();
}

/**
 * Return the offset between two tiles from a TileIndexDiffC struct.
 *
 * This function works like #TileDiffXY(int, int) and returns the
 * difference between two tiles.
 *
 * @param tidc The coordinate of the offset as TileIndexDiffC
 * @return The difference between two tiles.
 * @see TileDiffXY(int, int)
 */
inline TileIndexDiff ToTileIndexDiff(TileIndexDiffC tidc)
{
	return TileDiffXY(tidc.x, tidc.y);
}

/* Helper functions to provide explicit +=/-= operators for TileIndex and TileIndexDiff. */
constexpr TileIndex &operator+=(TileIndex &tile, TileIndexDiff offset) { tile = tile + TileIndex(offset); return tile; }
constexpr TileIndex &operator-=(TileIndex &tile, TileIndexDiff offset) { tile = tile - TileIndex(offset); return tile; }

/**
 * Adds a given offset to a tile.
 *
 * @param tile The tile to add an offset to.
 * @param offset The offset to add.
 * @return The resulting tile.
 */
#ifndef _DEBUG
	constexpr TileIndex TileAdd(TileIndex tile, TileIndexDiff offset) { return tile + offset; }
#else
	TileIndex TileAdd(TileIndex tile, TileIndexDiff offset);
#endif

/**
 * Adds a given offset to a tile.
 *
 * @param tile The tile to add an offset to.
 * @param x The x offset to add to the tile.
 * @param y The y offset to add to the tile.
 * @return The resulting tile.
 */
inline TileIndex TileAddXY(TileIndex tile, int x, int y)
{
	return TileAdd(tile, TileDiffXY(x, y));
}

TileIndex TileAddWrap(TileIndex tile, int addx, int addy);

/**
 * Returns the TileIndexDiffC offset from a DiagDirection.
 *
 * @param dir The given direction
 * @return The offset as TileIndexDiffC value
 */
inline TileIndexDiffC TileIndexDiffCByDiagDir(DiagDirection dir)
{
	extern const TileIndexDiffC _tileoffs_by_diagdir[DIAGDIR_END];

	assert(IsValidDiagDirection(dir));
	return _tileoffs_by_diagdir[dir];
}

/**
 * Returns the TileIndexDiffC offset from a Direction.
 *
 * @param dir The given direction
 * @return The offset as TileIndexDiffC value
 */
inline TileIndexDiffC TileIndexDiffCByDir(Direction dir)
{
	extern const TileIndexDiffC _tileoffs_by_dir[DIR_END];

	assert(IsValidDirection(dir));
	return _tileoffs_by_dir[dir];
}

/**
 * Add a TileIndexDiffC to a TileIndex and returns the new one.
 *
 * Returns tile + the diff given in diff. If the result tile would end up
 * outside of the map, INVALID_TILE is returned instead.
 *
 * @param tile The base tile to add the offset on
 * @param diff The offset to add on the tile
 * @return The resulting TileIndex
 */
inline TileIndex AddTileIndexDiffCWrap(TileIndex tile, TileIndexDiffC diff)
{
	int x = TileX(tile) + diff.x;
	int y = TileY(tile) + diff.y;
	/* Negative value will become big positive value after cast */
	if ((uint)x >= Map::SizeX() || (uint)y >= Map::SizeY()) return INVALID_TILE;
	return TileXY(x, y);
}

/**
 * Returns the diff between two tiles
 *
 * @param tile_a from tile
 * @param tile_b to tile
 * @return the difference between tila_a and tile_b
 */
inline TileIndexDiffC TileIndexToTileIndexDiffC(TileIndex tile_a, TileIndex tile_b)
{
	TileIndexDiffC difference;

	difference.x = TileX(tile_a) - TileX(tile_b);
	difference.y = TileY(tile_a) - TileY(tile_b);

	return difference;
}

/* Functions to calculate distances */
uint DistanceManhattan(TileIndex, TileIndex); ///< also known as L1-Norm. Is the shortest distance one could go over diagonal tracks (or roads)
uint DistanceSquare(TileIndex, TileIndex); ///< Euclidean- or L2-Norm squared
uint DistanceMax(TileIndex, TileIndex); ///< also known as L-Infinity-Norm
uint DistanceMaxPlusManhattan(TileIndex, TileIndex); ///< Max + Manhattan
uint DistanceFromEdge(TileIndex); ///< shortest distance from any edge of the map
uint DistanceFromEdgeDir(TileIndex, DiagDirection); ///< distance from the map edge in given direction

/**
 * Convert an Axis to a TileIndexDiff
 *
 * @param axis The Axis
 * @return The resulting TileIndexDiff in southern direction (either SW or SE).
 */
inline TileIndexDiff TileOffsByAxis(Axis axis)
{
	extern const TileIndexDiffC _tileoffs_by_axis[];

	assert(IsValidAxis(axis));
	return ToTileIndexDiff(_tileoffs_by_axis[axis]);
}

/**
 * Convert a DiagDirection to a TileIndexDiff
 *
 * @param dir The DiagDirection
 * @return The resulting TileIndexDiff
 * @see TileIndexDiffCByDiagDir
 */
inline TileIndexDiff TileOffsByDiagDir(DiagDirection dir)
{
	extern const TileIndexDiffC _tileoffs_by_diagdir[DIAGDIR_END];

	assert(IsValidDiagDirection(dir));
	return ToTileIndexDiff(_tileoffs_by_diagdir[dir]);
}

/**
 * Convert a Direction to a TileIndexDiff.
 *
 * @param dir The direction to convert from
 * @return The resulting TileIndexDiff
 */
inline TileIndexDiff TileOffsByDir(Direction dir)
{
	extern const TileIndexDiffC _tileoffs_by_dir[DIR_END];

	assert(IsValidDirection(dir));
	return ToTileIndexDiff(_tileoffs_by_dir[dir]);
}

/**
 * Adds a Direction to a tile.
 *
 * @param tile The current tile
 * @param dir The direction in which we want to step
 * @return the moved tile
 */
inline TileIndex TileAddByDir(TileIndex tile, Direction dir)
{
	return TileAdd(tile, TileOffsByDir(dir));
}

/**
 * Adds a DiagDir to a tile.
 *
 * @param tile The current tile
 * @param dir The direction in which we want to step
 * @return the moved tile
 */
inline TileIndex TileAddByDiagDir(TileIndex tile, DiagDirection dir)
{
	return TileAdd(tile, TileOffsByDiagDir(dir));
}

/**
 * Determines the DiagDirection to get from one tile to another.
 * The tiles do not necessarily have to be adjacent.
 * @param tile_from Origin tile
 * @param tile_to Destination tile
 * @return DiagDirection from tile_from towards tile_to, or INVALID_DIAGDIR if the tiles are not on an axis
 */
inline DiagDirection DiagdirBetweenTiles(TileIndex tile_from, TileIndex tile_to)
{
	int dx = (int)TileX(tile_to) - (int)TileX(tile_from);
	int dy = (int)TileY(tile_to) - (int)TileY(tile_from);
	if (dx == 0) {
		if (dy == 0) return INVALID_DIAGDIR;
		return (dy < 0 ? DIAGDIR_NW : DIAGDIR_SE);
	} else {
		if (dy != 0) return INVALID_DIAGDIR;
		return (dx < 0 ? DIAGDIR_NE : DIAGDIR_SW);
	}
}

/**
 * Get a random tile out of a given seed.
 * @param r the random 'seed'
 * @return a valid tile
 */
inline TileIndex RandomTileSeed(uint32_t r)
{
	return Map::WrapToMap(TileIndex{r});
}

/**
 * Get a valid random tile.
 * @note a define so 'random' gets inserted in the place where it is actually
 *       called, thus making the random traces more explicit.
 * @return a valid tile
 */
#define RandomTile() RandomTileSeed(Random())

uint GetClosestWaterDistance(TileIndex tile, bool water);

#endif /* MAP_FUNC_H */
