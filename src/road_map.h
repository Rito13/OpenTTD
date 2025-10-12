/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file road_map.h Map accessors for roads. */

#ifndef ROAD_MAP_H
#define ROAD_MAP_H

#include "track_func.h"
#include "depot_type.h"
#include "rail_type.h"
#include "road_func.h"
#include "tile_map.h"
#include "road_type.h"


/** The different types of road tiles. */
enum RoadTileType : uint8_t {
	ROAD_TILE_NORMAL = 0, ///< Normal road
	ROAD_TILE_DEPOT  = 2, ///< Depot (one entrance)
};

bool MayHaveRoad(Tile t);

/**
 * Check if tile can store road type.
 * @param i Tile to check.
 * @return true if tile can store road type false otherwise.
 */
debug_inline bool MayHaveRoad(TileIndex i)
{
	return MayHaveRoad(Tile(i));
}

/**
 * Get the type of the road tile.
 * @param t Tile to query.
 * @pre IsTileType(t, MP_ROAD)
 * @return The road tile type.
 */
debug_inline static RoadTileType GetRoadTileType(Tile t)
{
	assert(IsTileType(t, MP_ROAD));
	return (RoadTileType)GB(t.m5(), 6, 2);
}

/**
 * Get the type of the road tile.
 * @param i Tile to query.
 * @pre IsTileType(t, MP_ROAD)
 * @return The road tile type.
 */
debug_inline static RoadTileType GetRoadTileType(TileIndex i)
{
	return GetRoadTileType(Tile::GetByType(i, MP_ROAD));
}

/**
 * Return whether a tile is a normal road.
 * @param t Tile to query.
 * @pre IsTileType(t, MP_ROAD)
 * @return True if normal road.
 */
debug_inline static bool IsNormalRoad(Tile t)
{
	return GetRoadTileType(t) == ROAD_TILE_NORMAL;
}

/**
 * Return whether a tile is a normal road.
 * @param i Tile to query.
 * @pre IsTileType(t, MP_ROAD)
 * @return True if normal road.
 */
debug_inline static bool IsNormalRoad(TileIndex i)
{
	return IsNormalRoad(Tile::GetByType(i, MP_ROAD));
}

/**
 * Return whether a tile is a normal road tile.
 * @param t Tile to query.
 * @return True if normal road tile.
 */
debug_inline static bool IsNormalRoadTile(Tile t)
{
	return IsTileType(t, MP_ROAD) && IsNormalRoad(t);
}

/**
 * Return whether a tile is a normal road tile.
 * @param i Tile to query.
 * @return True if normal road tile.
 */
debug_inline static bool IsNormalRoadTile(TileIndex i)
{
	return IsNormalRoadTile(Tile::GetByType(i, MP_ROAD));
}

/**
 * Return whether a tile is a road depot.
 * @param t Tile to query.
 * @pre IsTileType(t, MP_ROAD)
 * @return True if road depot.
 */
debug_inline static bool IsRoadDepot(Tile t)
{
	return GetRoadTileType(t) == ROAD_TILE_DEPOT;
}

/**
 * Return whether a tile is a road depot.
 * @param i Tile to query.
 * @pre IsTileType(t, MP_ROAD)
 * @return True if road depot.
 */
debug_inline static bool IsRoadDepot(TileIndex i)
{
	return IsRoadDepot(Tile::GetByType(i, MP_ROAD));
}

/**
 * Return whether a tile is a road depot tile.
 * @param t Tile to query.
 * @return True if road depot tile.
 */
debug_inline static bool IsRoadDepotTile(Tile t)
{
	return IsTileType(t, MP_ROAD) && IsRoadDepot(t);
}

/**
 * Return whether a tile is a road depot tile.
 * @param i Tile to query.
 * @return True if road depot tile.
 */
debug_inline static bool IsRoadDepotTile(TileIndex i)
{
	return IsRoadDepotTile(Tile::GetByType(i, MP_ROAD));
}

/**
 * Get the present road bits for a specific road type.
 * @param t  The tile to query.
 * @param rt Road type.
 * @pre IsNormalRoad(t)
 * @return The present road bits for the road type.
 */
inline RoadBits GetRoadBits(Tile t, RoadTramType rtt)
{
	assert(IsNormalRoad(t));
	if (rtt == RTT_TRAM) return (RoadBits)GB(t.m3(), 0, 4);
	return (RoadBits)GB(t.m5(), 0, 4);
}

/**
 * Get the present road bits for a specific road type.
 * @param i  The tile to query.
 * @param rt Road type.
 * @pre IsNormalRoad(t)
 * @return The present road bits for the road type.
 */
inline RoadBits GetRoadBits(TileIndex i, RoadTramType rtt)
{
	return GetRoadBits(Tile::GetByType(i, MP_ROAD), rtt);
}

/**
 * Get all set RoadBits on the given tile
 *
 * @param tile The tile from which we want to get the RoadBits
 * @return all set RoadBits of the tile
 */
inline RoadBits GetAllRoadBits(Tile tile)
{
	return GetRoadBits(tile, RTT_ROAD) | GetRoadBits(tile, RTT_TRAM);
}

/**
 * Get all set RoadBits on the given tile
 *
 * @param i The tile from which we want to get the RoadBits
 * @return all set RoadBits of the tile
 */
inline RoadBits GetAllRoadBits(TileIndex i)
{
	return GetAllRoadBits(Tile::GetByType(i, MP_ROAD));
}

/**
 * Set the present road bits for a specific road type.
 * @param t  The tile to change.
 * @param r  The new road bits.
 * @param rt Road type.
 * @pre IsNormalRoad(t)
 */
inline void SetRoadBits(Tile t, RoadBits r, RoadTramType rtt)
{
	assert(IsNormalRoad(t)); // XXX incomplete
	if (rtt == RTT_TRAM) {
		SB(t.m3(), 0, 4, r);
	} else {
		SB(t.m5(), 0, 4, r);
	}
}

/**
 * Set the present road bits for a specific road type.
 * @param i  The tile to change.
 * @param r  The new road bits.
 * @param rt Road type.
 * @pre IsNormalRoad(t)
 */
inline void SetRoadBits(TileIndex i, RoadBits r, RoadTramType rtt)
{
	SetRoadBits(Tile::GetByType(i, MP_ROAD), r, rtt);
}

inline RoadType GetRoadTypeRoad(Tile t)
{
	assert(MayHaveRoad(t));
	return (RoadType)GB(t.m4(), 0, 6);
}

inline RoadType GetRoadTypeRoad(TileIndex i)
{
	return GetRoadTypeRoad(Tile(i));
}

inline RoadType GetRoadTypeTram(Tile t)
{
	assert(MayHaveRoad(t));
	return (RoadType)GB(t.m8(), 6, 6);
}

inline RoadType GetRoadTypeTram(TileIndex i)
{
	return GetRoadTypeTram(Tile(i));
}

inline RoadType GetRoadType(Tile t, RoadTramType rtt)
{
	return (rtt == RTT_TRAM) ? GetRoadTypeTram(t) : GetRoadTypeRoad(t);
}

inline RoadType GetRoadType(TileIndex i, RoadTramType rtt)
{
	return GetRoadType(Tile(i), rtt);
}

/**
 * Get the present road types of a tile.
 * @param t The tile to query.
 * @return Present road types.
 */
inline RoadTypes GetPresentRoadTypes(Tile t)
{
	RoadTypes result{};
	if (MayHaveRoad(t)) {
		if (GetRoadTypeRoad(t) != INVALID_ROADTYPE) result.Set(GetRoadTypeRoad(t));
		if (GetRoadTypeTram(t) != INVALID_ROADTYPE) result.Set(GetRoadTypeTram(t));
	}
	return result;
}

/**
 * Get the present road types of a tile.
 * @param i The tile to query.
 * @return Present road types.
 */
inline RoadTypes GetPresentRoadTypes(TileIndex i)
{
	return GetPresentRoadTypes(Tile(i));
}

inline bool HasRoadTypeRoad(Tile t)
{
	return GetRoadTypeRoad(t) != INVALID_ROADTYPE;
}

inline bool HasRoadTypeRoad(TileIndex i)
{
	return HasRoadTypeRoad(Tile(i));
}

inline bool HasRoadTypeTram(Tile t)
{
	return GetRoadTypeTram(t) != INVALID_ROADTYPE;
}

inline bool HasRoadTypeTram(TileIndex i)
{
	return HasRoadTypeTram(Tile(i));
}

/**
 * Check if a tile has a road or a tram road type.
 * @param t  The tile to check.
 * @param tram True to check tram, false to check road.
 * @return True if the tile has the specified road type.
 */
inline bool HasTileRoadType(Tile t, RoadTramType rtt)
{
	return GetRoadType(t, rtt) != INVALID_ROADTYPE;
}

/**
 * Check if a tile has a road or a tram road type.
 * @param i  The tile to check.
 * @param tram True to check tram, false to check road.
 * @return True if the tile has the specified road type.
 */
inline bool HasTileRoadType(TileIndex i, RoadTramType rtt)
{
	return HasTileRoadType(Tile(i), rtt);
}

/**
 * Check if a tile has one of the specified road types.
 * @param t  The tile to check.
 * @param rts Allowed road types.
 * @return True if the tile has one of the specified road types.
 */
inline bool HasTileAnyRoadType(Tile t, RoadTypes rts)
{
	if (!MayHaveRoad(t)) return false;
	return GetPresentRoadTypes(t).Any(rts);
}

/**
 * Check if a tile has one of the specified road types.
 * @param i  The tile to check.
 * @param rts Allowed road types.
 * @return True if the tile has one of the specified road types.
 */
inline bool HasTileAnyRoadType(TileIndex i, RoadTypes rts)
{
	return HasTileAnyRoadType(Tile(i), rts);
}

/**
 * Get the owner of a specific road type.
 * @param t  The tile to query.
 * @param rtt RoadTramType.
 * @return Owner of the given road type.
 */
inline Owner GetRoadOwner(Tile t, RoadTramType rtt)
{
	assert(MayHaveRoad(t));
	if (rtt == RTT_ROAD) return (Owner)GB(IsNormalRoadTile(t) ? t.m1() : t.m7(), 0, 5);

	/* Trams don't need OWNER_TOWN, and remapping OWNER_NONE
	 * to OWNER_TOWN makes it use one bit less */
	Owner o = (Owner)GB(t.m3(), 4, 4);
	return o == OWNER_TOWN ? OWNER_NONE : o;
}

/**
 * Get the owner of a specific road type.
 * @param i  The tile to query.
 * @param rtt RoadTramType.
 * @return Owner of the given road type.
 */
inline Owner GetRoadOwner(TileIndex i, RoadTramType rtt)
{
	return GetRoadOwner(Tile(i), rtt);
}

/**
 * Set the owner of a specific road type.
 * @param t  The tile to change.
 * @param rtt RoadTramType.
 * @param o  New owner of the given road type.
 */
inline void SetRoadOwner(Tile t, RoadTramType rtt, Owner o)
{
	if (rtt == RTT_ROAD) {
		SB(IsNormalRoadTile(t) ? t.m1() : t.m7(), 0, 5, o.base());
	} else {
		SB(t.m3(), 4, 4, (o == OWNER_NONE ? OWNER_TOWN : o).base());
	}
}

/**
 * Set the owner of a specific road type.
 * @param i  The tile to change.
 * @param rtt RoadTramType.
 * @param o  New owner of the given road type.
 */
inline void SetRoadOwner(TileIndex i, RoadTramType rtt, Owner o)
{
	SetRoadOwner(Tile(i), rtt, o);
}

/**
 * Check if a specific road type is owned by an owner.
 * @param t  The tile to query.
 * @param tram True to check tram, false to check road.
 * @param o  Owner to compare with.
 * @pre HasTileRoadType(t, rt)
 * @return True if the road type is owned by the given owner.
 */
inline bool IsRoadOwner(Tile t, RoadTramType rtt, Owner o)
{
	assert(HasTileRoadType(t, rtt));
	return (GetRoadOwner(t, rtt) == o);
}

/**
 * Check if a specific road type is owned by an owner.
 * @param i  The tile to query.
 * @param tram True to check tram, false to check road.
 * @param o  Owner to compare with.
 * @pre HasTileRoadType(t, rt)
 * @return True if the road type is owned by the given owner.
 */
inline bool IsRoadOwner(TileIndex i, RoadTramType rtt, Owner o)
{
	return IsRoadOwner(Tile(i), rtt, o);
}

/**
 * Checks if given tile has town owned road
 * @param t tile to check
 * @pre IsTileType(t, MP_ROAD)
 * @return true iff tile has road and the road is owned by a town
 */
inline bool HasTownOwnedRoad(Tile t)
{
	return HasTileRoadType(t, RTT_ROAD) && IsRoadOwner(t, RTT_ROAD, OWNER_TOWN);
}

/**
 * Checks if given tile has town owned road
 * @param i tile to check
 * @pre IsTileType(t, MP_ROAD)
 * @return true iff tile has road and the road is owned by a town
 */
inline bool HasTownOwnedRoad(TileIndex i)
{
	return HasTownOwnedRoad(Tile(i));
}

/**
 * Checks if a DisallowedRoadDirections is valid.
 *
 * @param wc The value to check
 * @return true if the given value is a valid DisallowedRoadDirections.
 */
inline bool IsValidDisallowedRoadDirections(DisallowedRoadDirections drt)
{
	return drt < DRD_END;
}

/**
 * Gets the disallowed directions
 * @param t the tile to get the directions from
 * @return the disallowed directions
 */
inline DisallowedRoadDirections GetDisallowedRoadDirections(Tile t)
{
	assert(IsNormalRoad(t));
	return (DisallowedRoadDirections)GB(t.m5(), 4, 2);
}

/**
 * Gets the disallowed directions
 * @param i the tile to get the directions from
 * @return the disallowed directions
 */
inline DisallowedRoadDirections GetDisallowedRoadDirections(TileIndex i)
{
	return GetDisallowedRoadDirections(Tile::GetByType(i, MP_ROAD));
}

/**
 * Sets the disallowed directions
 * @param t   the tile to set the directions for
 * @param drd the disallowed directions
 */
inline void SetDisallowedRoadDirections(Tile t, DisallowedRoadDirections drd)
{
	assert(IsNormalRoad(t));
	assert(drd < DRD_END);
	SB(t.m5(), 4, 2, drd);
}

/**
 * Sets the disallowed directions
 * @param i   the tile to set the directions for
 * @param drd the disallowed directions
 */
inline void SetDisallowedRoadDirections(TileIndex i, DisallowedRoadDirections drd)
{
	SetDisallowedRoadDirections(Tile::GetByType(i, MP_ROAD), drd);
}

/**
 * Check if a road tile has snow/desert.
 * @param t The tile to query.
 * @return True if the tile has snow/desert.
 */
inline bool IsOnSnowOrDesert(Tile t)
{
	assert(IsTileType(t, MP_ROAD));
	return HasBit(t.m7(), 5);
}

/**
 * Check if a road tile has snow/desert.
 * @param i The tile to query.
 * @return True if the tile has snow/desert.
 */
inline bool IsOnSnowOrDesert(TileIndex i)
{
	return IsOnSnowOrDesert(Tile::GetByType(i, MP_ROAD));
}

/**
 * Toggle the snow/desert state of a road tile.
 * @param t The tile to change.
 */
inline void ToggleSnowOrDesert(Tile t)
{
	assert(IsTileType(t, MP_ROAD));
	ToggleBit(t.m7(), 5);
}

/**
 * Toggle the snow/desert state of a road tile.
 * @param i The tile to change.
 */
inline void ToggleSnowOrDesert(TileIndex i)
{
	ToggleSnowOrDesert(Tile::GetByType(i, MP_ROAD));
}


/** The possible road side decorations. */
enum Roadside : uint8_t {
	ROADSIDE_BARREN           = 0, ///< Road on barren land
	ROADSIDE_GRASS            = 1, ///< Road on grass
	ROADSIDE_PAVED            = 2, ///< Road with paved sidewalks
	ROADSIDE_STREET_LIGHTS    = 3, ///< Road with street lights on paved sidewalks
	/* 4 is unused for historical reasons */
	ROADSIDE_TREES            = 5, ///< Road with trees on paved sidewalks
	ROADSIDE_GRASS_ROAD_WORKS = 6, ///< Road on grass with road works
	ROADSIDE_PAVED_ROAD_WORKS = 7, ///< Road with sidewalks and road works
};

/**
 * Get the decorations of a road.
 * @param tile The tile to query.
 * @return The road decoration of the tile.
 */
inline Roadside GetRoadside(Tile tile)
{
	return (Roadside)GB(tile.m6(), 3, 3);
}

/**
 * Set the decorations of a road.
 * @param tile The tile to change.
 * @param s    The new road decoration of the tile.
 */
inline void SetRoadside(Tile tile, Roadside s)
{
	SB(tile.m6(), 3, 3, s);
}

/**
 * Check if a tile has road works.
 * @param t The tile to check.
 * @return True if the tile has road works in progress.
 */
inline bool HasRoadWorks(Tile t)
{
	return GetRoadside(t) >= ROADSIDE_GRASS_ROAD_WORKS;
}

/**
 * Increase the progress counter of road works.
 * @param t The tile to modify.
 * @return True if the road works are in the last stage.
 */
inline bool IncreaseRoadWorksCounter(Tile t)
{
	AB(t.m7(), 0, 4, 1);

	return GB(t.m7(), 0, 4) == 15;
}

/**
 * Start road works on a tile.
 * @param t The tile to start the work on.
 * @pre !HasRoadWorks(t)
 */
inline void StartRoadWorks(Tile t)
{
	assert(!HasRoadWorks(t));
	/* Remove any trees or lamps in case or roadwork */
	switch (GetRoadside(t)) {
		case ROADSIDE_BARREN:
		case ROADSIDE_GRASS:  SetRoadside(t, ROADSIDE_GRASS_ROAD_WORKS); break;
		default:              SetRoadside(t, ROADSIDE_PAVED_ROAD_WORKS); break;
	}
}

/**
 * Terminate road works on a tile.
 * @param t Tile to stop the road works on.
 * @pre HasRoadWorks(t)
 */
inline void TerminateRoadWorks(Tile t)
{
	assert(HasRoadWorks(t));
	SetRoadside(t, (Roadside)(GetRoadside(t) - ROADSIDE_GRASS_ROAD_WORKS + ROADSIDE_GRASS));
	/* Stop the counter */
	SB(t.m7(), 0, 4, 0);
}


/**
 * Get the direction of the exit of a road depot.
 * @param t The tile to query.
 * @return Diagonal direction of the depot exit.
 */
inline DiagDirection GetRoadDepotDirection(Tile t)
{
	assert(IsRoadDepot(t));
	return (DiagDirection)GB(t.m5(), 0, 2);
}

/**
 * Get the direction of the exit of a road depot.
 * @param i The tile to query.
 * @return Diagonal direction of the depot exit.
 */
inline DiagDirection GetRoadDepotDirection(TileIndex i)
{
	return GetRoadDepotDirection(Tile::GetByType(i, MP_ROAD));
}

RoadBits GetAnyRoadBits(Tile tile, RoadTramType rtt, bool straight_tunnel_bridge_entrance = false);

/**
 * Returns the RoadBits on an arbitrary tile
 * @see GetAnyRoadBits(Tile tile, RoadTramType rtt, bool straight_tunnel_bridge_entrance)
 * @param index the tile to get the road bits for
 * @param rt the road type to get the road bits form
 * @param straight_tunnel_bridge_entrance whether to return straight road bits for tunnels/bridges.
 * @return the road bits of the given tile
 */
RoadBits GetAnyRoadBits(TileIndex index, RoadTramType rtt, bool straight_tunnel_bridge_entrance = false)
{
	return GetAnyRoadBits(Tile(index), rtt, straight_tunnel_bridge_entrance);
}

/**
 * Set the road road type of a tile.
 * @param t The tile to change.
 * @param rt The road type to set.
 */
inline void SetRoadTypeRoad(Tile t, RoadType rt)
{
	assert(MayHaveRoad(t));
	assert(rt == INVALID_ROADTYPE || RoadTypeIsRoad(rt));
	SB(t.m4(), 0, 6, rt);
}

/**
 * Set the tram road type of a tile.
 * @param t The tile to change.
 * @param rt The road type to set.
 */
inline void SetRoadTypeTram(Tile t, RoadType rt)
{
	assert(MayHaveRoad(t));
	assert(rt == INVALID_ROADTYPE || RoadTypeIsTram(rt));
	SB(t.m8(), 6, 6, rt);
}

/**
 * Set the road type of a tile.
 * @param t The tile to change.
 * @param rtt Set road or tram type.
 * @param rt The road type to set.
 */
inline void SetRoadType(Tile t, RoadTramType rtt, RoadType rt)
{
	if (rtt == RTT_TRAM) {
		SetRoadTypeTram(t, rt);
	} else {
		SetRoadTypeRoad(t, rt);
	}
}

/**
 * Set the present road types of a tile.
 * @param t  The tile to change.
 * @param road_rt The road roadtype to set for the tile.
 * @param tram_rt The tram roadtype to set for the tile.
 */
inline void SetRoadTypes(Tile t, RoadType road_rt, RoadType tram_rt)
{
	SetRoadTypeRoad(t, road_rt);
	SetRoadTypeTram(t, tram_rt);
}

/**
 * Make a normal road tile.
 * @param t       Tile to make a normal road.
 * @param bits    Road bits to set for all present road types.
 * @param road_rt The road roadtype to set for the tile.
 * @param tram_rt The tram roadtype to set for the tile.
 * @param town    Town ID if the road is a town-owned road.
 * @param road    New owner of road.
 * @param tram    New owner of tram tracks.
 */
inline void MakeRoadNormal(Tile t, RoadBits bits, RoadType road_rt, RoadType tram_rt, TownID town, Owner road, Owner tram)
{
	if (!MayHaveAssociatedTile(t.tile_type())) ClrBit(t.m8(), 14);;

	SetTileType(t, MP_ROAD);
	SetTileOwner(t, road);
	t.m2() = town.base();
	t.m3() = (tram_rt != INVALID_ROADTYPE ? bits : 0);
	t.m5() = (road_rt != INVALID_ROADTYPE ? bits : 0) | ROAD_TILE_NORMAL << 6;
	SB(t.m6(), 2, 4, 0);
	t.m7() = 0;
	SetRoadTypes(t, road_rt, tram_rt);
	SetRoadOwner(t, RTT_TRAM, tram);
}

/**
 * Sets the exit direction of a road depot.
 * @param tile Tile of the depot.
 * @param dir  Direction of the depot exit.
 */
inline void SetRoadDepotExitDirection(Tile tile, DiagDirection dir)
{
	assert(IsRoadDepotTile(tile));
	SB(tile.m5(), 0, 2, dir);
}

/**
 * Make a road depot.
 * @param tile      Tile to make a depot on.
 * @param owner     New owner of the depot.
 * @param depot_id  New depot ID.
 * @param dir       Direction of the depot exit.
 * @param rt        Road type of the depot.
 */
inline void MakeRoadDepot(Tile tile, Owner owner, DepotID depot_id, DiagDirection dir, RoadType rt)
{
	SetTileType(tile, MP_ROAD);
	SetTileOwner(tile, owner);
	tile.m2() = depot_id.base();
	tile.m3() = 0;
	tile.m4() = INVALID_ROADTYPE;
	tile.m5() = ROAD_TILE_DEPOT << 6 | dir;
	SB(tile.m6(), 2, 4, 0);
	tile.m7() = owner.base();
	tile.m8() = INVALID_ROADTYPE << 6;
	SetRoadType(tile, GetRoadTramType(rt), rt);
	SetRoadOwner(tile, RTT_TRAM, owner);
}

#endif /* ROAD_MAP_H */
