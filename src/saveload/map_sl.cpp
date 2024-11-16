/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file map_sl.cpp Code handling saving and loading of map. */

#include "../stdafx.h"

#include "saveload.h"
#include "compat/map_sl_compat.h"

#include "../map_func.h"
#include "../core/bitmath_func.hpp"
#include "../fios.h"

#include "../safeguards.h"

/** Iterator for iterating over all raw Tiles in the map. */
struct RawMapIterator {
	/**
	 * Get an iterator to the first tile.
	 * @return The iterator to the first tile.
	 */
	static RawMapIterator begin()
	{
		return RawMapIterator(Map::base_tiles.begin(), Map::base_tiles.front().begin(), Map::extended_tiles.begin(), Map::extended_tiles.front().begin());
	}

	/**
	 * Get an iterator to one past the last tile.
	 * @return The iterator to one past the last tile.
	 */
	static RawMapIterator end()
	{
		return RawMapIterator(Map::base_tiles.end(), std::vector<Map::TileBase>::iterator(), Map::extended_tiles.end(), std::vector<Map::TileExtended>::iterator());
	}

	/**
	 * Equality comparison.
	 * @param rhs The other iterator to compare to.
	 * @return \c true iff the tile of both iterators is the same.
	 */
	bool operator ==(const RawMapIterator &rhs) const { return this->y == rhs.y && this->tile == rhs.tile && this->y_extended == rhs.y_extended && this->tile_extended == rhs.tile_extended; }

	/**
	 * Inequality comparison.
	 * @param rhs The other iterator to compare to.
	 * @return \c false iff the tile of both iterators is the same.
	 */
	bool operator !=(const RawMapIterator &rhs) const { return !(*this == rhs); }

	/**
	 * Get the tile we are currently at.
	 * @return The tile we are at.
	 */
	Tile operator *() { return Tile(&(*tile), &(*tile_extended)); }

	/**
	 * Prefix increment. Increments this iterator by one to the next tile.
	 * @return Reference to the incremented iterator.
	 */
	RawMapIterator &operator ++()
	{
		if (++tile == y->end()) {
			++y;
			tile = y == Map::base_tiles.end() ? std::vector<Map::TileBase>::iterator() : y->begin();
		}
		if (++tile_extended == y_extended->end()) {
			++y_extended;
			tile_extended = y_extended == Map::extended_tiles.end() ? std::vector<Map::TileExtended>::iterator() : y_extended->begin();
		}
		return *this;
	}

	/**
	 * Postfix increment. Increments this iterator by one to the next tile.
	 * @return An unincremented iterator.
	 */
	RawMapIterator operator ++(int)
	{
		RawMapIterator old(*this);
		++(*this);
		return old;
	}
private:
	std::vector<std::vector<Map::TileBase>>::iterator y; ///< Low level iterator to the chunk of the map array.
	std::vector<Map::TileBase>::iterator tile; ///< Low level iterator to the tile inside the chunk of the map array.

	std::vector<std::vector<Map::TileExtended>>::iterator y_extended; ///< Low level iterator to the chunk of the extended map array.
	std::vector<Map::TileExtended>::iterator tile_extended; ///< Low level iterator to the tile inside the chunk of the extended map array.

	/**
	 * Creates new raw iterator for map storage.
	 * @param y @copydoc RawMapIterator::y
	 * @param tile @copydoc RawMapIterator::tile
	 * @param y_extended @copydoc RawMapIterator::y_extended
	 * @param tile_extended @copydoc RawMapIterator::tile_extended
	 */
	RawMapIterator(std::vector<std::vector<Map::TileBase>>::iterator &&y, std::vector<Map::TileBase>::iterator &&tile,
			std::vector<std::vector<Map::TileExtended>>::iterator &&y_extended, std::vector<Map::TileExtended>::iterator &&tile_extended
			) : y(y), tile(tile), y_extended(y_extended), tile_extended(tile_extended) { }
};

static uint32_t _map_dim_x;
static uint32_t _map_dim_y;

static const SaveLoad _map_desc[] = {
	SLEG_CONDVAR("dim_x", _map_dim_x, SLE_UINT32, SaveLoadVersion::MultipleRoadStops, SaveLoadVersion::MaxVersion),
	SLEG_CONDVAR("dim_y", _map_dim_y, SLE_UINT32, SaveLoadVersion::MultipleRoadStops, SaveLoadVersion::MaxVersion),
};

struct MAPSChunkHandler : ChunkHandler {
	MAPSChunkHandler() : ChunkHandler('MAPS', ChunkType::Table) {}

	void Save() const override
	{
		SlTableHeader(_map_desc);

		_map_dim_x = Map::SizeX();
		_map_dim_y = Map::SizeY();

		SlSetArrayIndex(0);
		SlGlobList(_map_desc);
	}

	void Load() const override
	{
		const std::vector<SaveLoad> slt = SlCompatTableHeader(_map_desc, _map_sl_compat);

		if (!IsSavegameVersionBefore(SaveLoadVersion::RiffToArray) && SlIterateArray() == -1) return;
		SlGlobList(slt);
		if (!IsSavegameVersionBefore(SaveLoadVersion::RiffToArray) && SlIterateArray() != -1) SlErrorCorrupt("Too many MAPS entries");

		Map::Allocate(_map_dim_x, _map_dim_y);
	}

	void LoadCheck(size_t) const override
	{
		const std::vector<SaveLoad> slt = SlCompatTableHeader(_map_desc, _map_sl_compat);

		if (!IsSavegameVersionBefore(SaveLoadVersion::RiffToArray) && SlIterateArray() == -1) return;
		SlGlobList(slt);
		if (!IsSavegameVersionBefore(SaveLoadVersion::RiffToArray) && SlIterateArray() != -1) SlErrorCorrupt("Too many MAPS entries");

		_load_check_data.map_size_x = _map_dim_x;
		_load_check_data.map_size_y = _map_dim_y;
	}
};

struct MAPRChunkHandler : ChunkHandler {
	MAPRChunkHandler() : ChunkHandler('MAPR', CH_RIFF) {}

	void Load() const override
	{
		std::vector<uint> buf(Map::SizeY());

		/* Resize each map line to the final length. */
		SlCopy(buf.data(), Map::SizeY(), SLE_UINT);
		for (uint i = 0; i < Map::SizeY(); i++) Map::base_tiles[i].resize(buf[i]);

		/* Load offset table. */
		SlCopy(Map::offsets.data(), Map::Size(), SLE_UINT16);
	}

	void Save() const override
	{
		SlSetLength(SlVarSize(SLE_UINT) * Map::SizeY() + SlVarSize(SLE_UINT16) * Map::Size());

		/* Save length of each map line. */
		std::vector<uint> buf(Map::SizeY());
		for (uint i = 0; i < Map::SizeY(); i++) buf[i] = static_cast<uint>(Map::base_tiles[i].size());
		SlCopy(buf.data(), Map::SizeY(), SLE_UINT);

		/* Save offset table. */
		SlCopy(Map::offsets.data(), Map::Size(), SLE_UINT16);
	}
};

static const uint MAP_SL_BUF_SIZE = 4096;

struct MAPTChunkHandler : ChunkHandler {
	MAPTChunkHandler() : ChunkHandler('MAPT', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT8);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).type() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).type();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct MAPHChunkHandler : ChunkHandler {
	MAPHChunkHandler() : ChunkHandler('MAPH', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT8);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).height() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).height();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct MAPOChunkHandler : ChunkHandler {
	MAPOChunkHandler() : ChunkHandler('MAPO', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT8);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).m1() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m1();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct MAP2ChunkHandler : ChunkHandler {
	MAP2ChunkHandler() : ChunkHandler('MAP2', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint16_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk,
				/* In those versions the m2 was 8 bits */
				IsSavegameVersionBefore(SaveLoadVersion::BigMap) ? VarFileType::U8 | VarMemType::U16 : SLE_UINT16
			);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).m2() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint16_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size * sizeof(uint16_t));
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m2();
			SlCopy(buf.data(), chunk, SLE_UINT16);
			size -= chunk;
		}
	}
};

struct M3LOChunkHandler : ChunkHandler {
	M3LOChunkHandler() : ChunkHandler('M3LO', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT8);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).m3() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m3();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct M3HIChunkHandler : ChunkHandler {
	M3HIChunkHandler() : ChunkHandler('M3HI', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT8);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).m4() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m4();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct MAP5ChunkHandler : ChunkHandler {
	MAP5ChunkHandler() : ChunkHandler('MAP5', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT8);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).m5() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m5();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct MAPEChunkHandler : ChunkHandler {
	MAPEChunkHandler() : ChunkHandler('MAPE', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		if (IsSavegameVersionBefore(SaveLoadVersion::BridgeWormhole)) {
			RawMapIterator i = RawMapIterator::begin();
			while (size > 0) {
				size_t chunk = std::min(size, buf.size());
				/* There are four tiles packed into one byte! */
				SlCopy(buf.data(), chunk / 4, SLE_UINT8);
				for (uint j = 0; j != 1024; j++) {
					(*i++).m6() = GB(buf[j], 0, 2);
					(*i++).m6() = GB(buf[j], 2, 2);
					(*i++).m6() = GB(buf[j], 4, 2);
					(*i++).m6() = GB(buf[j], 6, 2);
				}
				size -= chunk;
			}
		} else {
			RawMapIterator i = RawMapIterator::begin();
			while (size > 0) {
				size_t chunk = std::min(size, buf.size());
				SlCopy(buf.data(), chunk, SLE_UINT8);
				for (size_t j = 0; j < chunk; ++j, ++i) (*i).m6() = buf[j];
				size -= chunk;
			}
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m6();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct MAP7ChunkHandler : ChunkHandler {
	MAP7ChunkHandler() : ChunkHandler('MAP7', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT8);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).m7() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint8_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size);
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m7();
			SlCopy(buf.data(), chunk, SLE_UINT8);
			size -= chunk;
		}
	}
};

struct MAP8ChunkHandler : ChunkHandler {
	MAP8ChunkHandler() : ChunkHandler('MAP8', ChunkType::Riff) {}

	void Load() const override
	{
		std::array<uint16_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			SlCopy(buf.data(), chunk, SLE_UINT16);
			for (size_t j = 0; j < chunk; ++j, ++i) (*i).m8() = buf[j];
			size -= chunk;
		}
	}

	void Save() const override
	{
		std::array<uint16_t, MAP_SL_BUF_SIZE> buf;
		size_t size = Map::GetTotalTileCount();

		SlSetLength(size * sizeof(uint16_t));
		RawMapIterator i = RawMapIterator::begin();
		while (size > 0) {
			size_t chunk = std::min(size, buf.size());
			for (size_t j = 0; j < chunk; ++j, ++i) buf[j] = (*i).m8();
			SlCopy(buf.data(), chunk, SLE_UINT16);
			size -= chunk;
		}
	}
};

static const MAPSChunkHandler MAPS;
static const MAPRChunkHandler MAPR;
static const MAPTChunkHandler MAPT;
static const MAPHChunkHandler MAPH;
static const MAPOChunkHandler MAPO;
static const MAP2ChunkHandler MAP2;
static const M3LOChunkHandler M3LO;
static const M3HIChunkHandler M3HI;
static const MAP5ChunkHandler MAP5;
static const MAPEChunkHandler MAPE;
static const MAP7ChunkHandler MAP7;
static const MAP8ChunkHandler MAP8;
static const ChunkHandlerRef map_chunk_handlers[] = {
	MAPS,
	MAPR,
	MAPT,
	MAPH,
	MAPO,
	MAP2,
	M3LO,
	M3HI,
	MAP5,
	MAPE,
	MAP7,
	MAP8,
};

extern const ChunkHandlerTable _map_chunk_handlers(map_chunk_handlers);
