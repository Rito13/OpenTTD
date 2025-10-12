/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file depot_map.h Map related accessors for depots. */

#ifndef DEPOT_MAP_H
#define DEPOT_MAP_H

#include "order_type.h"
#include "station_map.h"

/**
 * Check if a tile is a depot and it is a depot of the given type.
 */
inline bool IsDepotTypeTile(TileIndex tile, TransportType type)
{
	switch (type) {
		default: NOT_REACHED();
		case TRANSPORT_RAIL:
			return IsRailDepotTile(Tile::GetByType(tile, MP_RAILWAY));

		case TRANSPORT_ROAD:
			return IsRoadDepotTile(Tile::GetByType(tile, MP_ROAD));

		case TRANSPORT_WATER:
			return IsShipDepotTile(Tile::GetByType(tile, MP_WATER));

		case TRANSPORT_AIR:
			return IsHangarTile(Tile::GetByType(tile, MP_STATION));
	}
}

/**
 * Is the given tile a tile with a depot on it?
 * @param tile the tile to check
 * @return true if and only if there is a depot on the tile.
 */
inline bool IsDepotTile(TileIndex tile)
{
	return IsRailDepotTile(Tile::GetByType(tile, MP_RAILWAY)) || IsRoadDepotTile(Tile::GetByType(tile, MP_ROAD)) ||
			IsShipDepotTile(Tile::GetByType(tile, MP_WATER)) || IsHangarTile(Tile::GetByType(tile, MP_STATION));
}

/**
 * Get the depot tile at a tile index.
 * @param index The tile index to get the depot tile from.
 * @return The actual depot tile.
 * @pre IsDepotTile
 */
inline Tile GetDepotTile(TileIndex index)
{
	assert(IsDepotTile(index));
	for (auto type : {MP_RAILWAY, MP_ROAD, MP_WATER, MP_STATION}) {
		if (Tile tile = Tile::GetByType(index, type); tile.IsValid()) return tile;
	}
	return INVALID_TILE;
}

/**
 * Get the index of which depot is attached to the tile.
 * @param t the tile
 * @pre IsRailDepotTile(t) || IsRoadDepotTile(t) || IsShipDepotTile(t)
 * @return DepotID
 */
inline DepotID GetDepotIndex(Tile t)
{
	/* Hangars don't have a Depot class, thus store no DepotID. */
	assert(IsRailDepotTile(t) || IsRoadDepotTile(t) || IsShipDepotTile(t));
	return DepotID{t.m2()};
}

/**
 * Get the destination index of a 'depot'. For hangars that's the station index, for the rest a depot index.
 * @param t the tile
 * @pre IsDepotTile
 * @return DepotID
 */
inline DestinationID GetDepotDestinationIndex(TileIndex t)
{
	if (Tile hangar = Tile::GetByType(t, MP_STATION); IsHangarTile(hangar)) return GetStationIndex(hangar);
	return GetDepotIndex(GetDepotTile(t));
}

/**
 * Get the index of which depot is attached to the tile.
 * @param index the tile
 * @pre IsRailDepotTile(t) || IsRoadDepotTile(t) || IsShipDepotTile(t)
 * @return DepotID
 */
inline DepotID GetDepotIndex(TileIndex index)
{
	Tile tile = Tile::GetByType(index, MP_RAILWAY);
	if (!tile.IsValid()) tile = Tile::GetByType(index, MP_ROAD);
	if (!tile.IsValid()) tile = Tile::GetByType(index, MP_WATER);
	return GetDepotIndex(tile);
}

/**
 * Get the owner of a depot tile.
 * @param tile Tile to get the owner of.
 * @return The depot owner.
 * @pre IsDepotTile(t)
 */
inline Owner GetDepotOwner(TileIndex tile)
{
	return GetTileOwner(GetDepotTile(tile));
}

/**
 * Check if a depot belongs to a given owner.
 * @param tile The tile to check.
 * @param owner The owner to check against.
 * @return True if the depot belongs the the given owner.
 */
inline bool IsDepotOwner(TileIndex tile, Owner o)
{
	return GetDepotOwner(tile) == o;
}

/**
 * Get the type of vehicles that can use a depot
 * @param t The tile
 * @pre IsDepotTile(t)
 * @return the type of vehicles that can use the depot
 */
inline VehicleType GetDepotVehicleType(TileIndex t)
{
	assert(IsDepotTile(t));

	if (Tile::HasType(t, MP_RAILWAY)) return VEH_TRAIN;
	switch (GetTileType(Tile(t))) {
		default: NOT_REACHED();
		case MP_ROAD:    return VEH_ROAD;
		case MP_WATER:   return VEH_SHIP;
		case MP_STATION: return VEH_AIRCRAFT;
	}
}

#endif /* DEPOT_MAP_H */
