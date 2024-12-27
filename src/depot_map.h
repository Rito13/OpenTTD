/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file depot_map.h Map related accessors for depots. */

#ifndef DEPOT_MAP_H
#define DEPOT_MAP_H

#include "order_type.h"
#include "station_map.h"

/** Dummy class to simplyfy asserts in depot related methods. */
class DepotTile : public Tile {
private:
	friend DepotTile AsDepotTile(const Tile &);
	/** @copydoc Tile::Tile(const Tile&) */
	explicit DepotTile(const Tile &other) : Tile(other) {}
};

/**
 * Check if a tile is a depot and it is a depot of the given type.
 * @param tile The tile to check.
 * @param type The type of transport.
 * @return \c true iff the given tile is a depot for the given transport type.
 */
inline bool IsDepotTypeTile(TileIndex tile, TransportType type)
{
	switch (type) {
		default: NOT_REACHED();
		case TRANSPORT_RAIL:
			return IsRailDepotTile(Tile::GetByType(tile, TileType::Railway));

		case TRANSPORT_ROAD:
			return IsRoadDepotTile(tile);

		case TRANSPORT_WATER:
			return IsShipDepotTile(tile);

		case TRANSPORT_AIR:
			return IsHangarTile(tile);
	}
}

/**
 * Get a given tile or its associated tile as a #DepotTile.
 * @param tile The tile to get as depot tile.
 * @return Valid depot tile iff the \a tile was a depot tile or had an associated depot tile.
 */
inline DepotTile AsDepotTile(const Tile &tile)
{
	DepotTile depot_tile(tile);
	while (depot_tile.IsValid()) {
		if (IsRailDepotTile(depot_tile) || IsRoadDepotTile(depot_tile) || IsShipDepotTile(depot_tile) || IsHangarTile(depot_tile)) break;
		++depot_tile;
	}
	return depot_tile;
}

/**
 * Is the given tile a tile with a depot on it?
 * @param tile the tile to check
 * @return true if and only if there is a depot on the tile.
 */
inline bool IsDepotTile(const DepotTile &tile)
{
	return tile.IsValid();
}

/**
 * Get the depot tile at a tile index.
 * @param index The tile index to get the depot tile from.
 * @return The actual depot tile.
 * @pre IsDepotTile
 */
inline DepotTile GetDepotTile(TileIndex index)
{
	DepotTile tile = AsDepotTile(index);
	assert(IsDepotTile(tile));
	return tile;
}

/**
 * Get the index of which depot is attached to the tile.
 * @param t the tile
 * @pre IsRailDepotTile(t) || IsRoadDepotTile(t) || IsShipDepotTile(t)
 * @return DepotID
 */
inline DepotID GetDepotIndex(const DepotTile &t)
{
	/* Hangars don't have a Depot class, thus store no DepotID. */
	assert(IsRailDepotTile(t) || IsRoadDepotTile(t) || IsShipDepotTile(t));
	return DepotID{t.m2()};
}

/**
 * Get the destination index of a 'depot'. For hangars that's the station index, for the rest a depot index.
 * @param t the tile
 * @pre IsRailDepotTile(t) || IsRoadDepotTile(t) || IsShipDepotTile(t) || IsHangarTile(t)
 * @return DepotID
 */
inline DestinationID GetDepotDestinationIndex(const DepotTile &t)
{
	if (IsHangarTile(t)) return GetStationIndex(t);
	return GetDepotIndex(t);
}

/**
 * Get the owner of a depot if tile has depot.
 * @param index Tile to get the owner of.
 * @return The depot owner or \c INVALID_OWNER.
 * @pre IsDepotTile(t)
 */
inline Owner GetTileOwnerIfDepot(TileIndex index)
{
	DepotTile tile = AsDepotTile(index);
	return IsDepotTile(tile) ? GetTileOwner(tile) : INVALID_OWNER;
}

/**
 * Check if a depot belongs to a given owner.
 * @param tile The tile to check.
 * @param o The owner to check against.
 * @return \c true iff the depot belongs the the given owner.
 */
inline bool IsTileOwnerIfDepot(TileIndex tile, Owner o)
{
	return GetTileOwnerIfDepot(tile) == o;
}

/**
 * Get the type of vehicles that can use a depot
 * @param tile The tile with a depot.
 * @pre IsDepotTile(t)
 * @return the type of vehicles that can use the depot
 */
inline VehicleType GetDepotVehicleType(const DepotTile &tile)
{
	assert(IsDepotTile(tile));

	switch (GetTileType(tile)) {
		default: NOT_REACHED();
		case TileType::Railway: return VehicleType::Train;
		case TileType::Road: return VehicleType::Road;
		case TileType::Water: return VehicleType::Ship;
		case TileType::Station: return VehicleType::Aircraft;
	}
}

#endif /* DEPOT_MAP_H */
