/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file water_map.h Map accessors for water tiles. */

#ifndef WATER_MAP_H
#define WATER_MAP_H

#include "depot_type.h"
#include "tile_map.h"

/**
 * Bit field layout of m5 for water tiles.
 */
static constexpr uint8_t WBL_TYPE_BEGIN = 4; ///< Start of the 'type' bitfield.
static constexpr uint8_t WBL_TYPE_COUNT = 4; ///< Length of the 'type' bitfield.

static constexpr uint8_t WBL_LOCK_ORIENT_BEGIN = 0; ///< Start of lock orientation bitfield.
static constexpr uint8_t WBL_LOCK_ORIENT_COUNT = 2; ///< Length of lock orientation bitfield.
static constexpr uint8_t WBL_LOCK_PART_BEGIN = 2; ///< Start of lock part bitfield.
static constexpr uint8_t WBL_LOCK_PART_COUNT = 2; ///< Length of lock part bitfield.

static constexpr uint8_t WBL_DEPOT_PART = 0; ///< Depot part flag.
static constexpr uint8_t WBL_DEPOT_AXIS = 1; ///< Depot axis flag.

/** Available water tile types. */
enum WaterTileType : uint8_t {
	WATER_TILE_CLEAR, ///< Plain water.
	WATER_TILE_COAST, ///< Coast.
	WATER_TILE_LOCK,  ///< Water lock.
	WATER_TILE_DEPOT, ///< Water Depot.
};

/** classes of water (for #WATER_TILE_CLEAR water tile type). */
enum WaterClass : uint8_t {
	WATER_CLASS_SEA,     ///< Sea.
	WATER_CLASS_CANAL,   ///< Canal.
	WATER_CLASS_RIVER,   ///< River.
	WATER_CLASS_INVALID, ///< Used for industry tiles on land (also for oilrig if newgrf says so).
};

/**
 * Checks if a water class is valid.
 *
 * @param wc The value to check
 * @return true if the given value is a valid water class.
 */
inline bool IsValidWaterClass(WaterClass wc)
{
	return wc < WATER_CLASS_INVALID;
}

/** Sections of the water depot. */
enum DepotPart : uint8_t {
	DEPOT_PART_NORTH = 0, ///< Northern part of a depot.
	DEPOT_PART_SOUTH = 1, ///< Southern part of a depot.
	DEPOT_PART_END
};

/** Sections of the water lock. */
enum LockPart : uint8_t {
	LOCK_PART_MIDDLE = 0, ///< Middle part of a lock.
	LOCK_PART_LOWER  = 1, ///< Lower part of a lock.
	LOCK_PART_UPPER  = 2, ///< Upper part of a lock.
};

bool IsPossibleDockingTile(TileIndex t);

/**
 * Get the water tile type of a tile.
 * @param t Water tile to query.
 * @return Water tile type at the tile.
 */
inline WaterTileType GetWaterTileType(Tile t)
{
	assert(IsTileType(t, MP_WATER));
	return static_cast<WaterTileType>(GB(t.m5(), WBL_TYPE_BEGIN, WBL_TYPE_COUNT));
}

/**
 * Get the water tile type of a tile.
 * @param i Water tile to query.
 * @return Water tile type at the tile.
 */
inline WaterTileType GetWaterTileType(TileIndex i)
{
	return GetWaterTileType(Tile::GetByType(i, MP_WATER));
}

/**
 * Set the water tile type of a tile.
 * @param t Water tile to set.
 * @param type Water tile type of the tile.
 */
inline void SetWaterTileType(Tile t, WaterTileType type)
{
	assert(IsTileType(t, MP_WATER));
	SB(t.m5(), WBL_TYPE_BEGIN, WBL_TYPE_COUNT, to_underlying(type));
}

/**
 * Set the water tile type of a tile.
 * @param i Water tile to set.
 * @param type Water tile type of the tile.
 */
inline void SetWaterTileType(TileIndex i, WaterTileType type)
{
	SetWaterTileType(Tile::GetByType(i, MP_WATER), type);
}

/**
 * Checks whether the tile has an waterclass associated.
 * You can then subsequently call GetWaterClass().
 * @param t Tile to query.
 * @return True if the tiletype has a waterclass.
 */
inline bool HasTileWaterClass(Tile t)
{
	return IsTileType(t, MP_WATER) || IsTileType(t, MP_STATION) || IsTileType(t, MP_INDUSTRY) || IsTileType(t, MP_OBJECT);
}

/**
 * Checks whether the tile has an waterclass associated.
 * You can then subsequently call GetWaterClass().
 * @param i Tile to query.
 * @return True if the tiletype has a waterclass.
 */
inline bool HasTileWaterClass(TileIndex i)
{
	return HasTileWaterClass(Tile(i));
}

/**
 * Get the water class at a tile.
 * @param t Water tile to query.
 * @pre IsTileType(t, MP_WATER) || IsTileType(t, MP_STATION) || IsTileType(t, MP_INDUSTRY) || IsTileType(t, MP_OBJECT)
 * @return Water class at the tile.
 */
inline WaterClass GetWaterClass(Tile t)
{
	assert(HasTileWaterClass(t));
	return (WaterClass)GB(t.m1(), 5, 2);
}

/**
 * Get the water class at a tile.
 * @param i Water tile to query.
 * @pre HasTileWaterClass(i)
 * @return Water class at the tile.
 */
inline WaterClass GetWaterClass(TileIndex i)
{
	return GetWaterClass(Tile(i));
}

/**
 * Set the water class at a tile.
 * @param t  Water tile to change.
 * @param wc New water class.
 * @pre IsTileType(t, MP_WATER) || IsTileType(t, MP_STATION) || IsTileType(t, MP_INDUSTRY) || IsTileType(t, MP_OBJECT)
 */
inline void SetWaterClass(Tile t, WaterClass wc)
{
	assert(HasTileWaterClass(t));
	SB(t.m1(), 5, 2, wc);
}

/**
 * Set the water class at a tile.
 * @param i  Water tile to change.
 * @param wc New water class.
 * @pre HasTileWaterClass(i)
 */
inline void SetWaterClass(TileIndex i, WaterClass wc)
{
	SetWaterClass(Tile(i), wc);
}

/**
 * Tests if the tile was built on water.
 * @param t the tile to check
 * @pre IsTileType(t, MP_WATER) || IsTileType(t, MP_STATION) || IsTileType(t, MP_INDUSTRY) || IsTileType(t, MP_OBJECT)
 * @return true iff on water
 */
inline bool IsTileOnWater(Tile t)
{
	return (GetWaterClass(t) != WATER_CLASS_INVALID);
}

/**
 * Tests if the tile was built on water.
 * @param i the tile to check
 * @pre HasTileWaterClass(i)
 * @return true iff on water
 */
inline bool IsTileOnWater(TileIndex i)
{
	return IsTileOnWater(Tile(i));
}

/**
 * Is it a plain water tile?
 * @param t Water tile to query.
 * @return \c true if any type of clear water like ocean, river, or canal.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsWater(Tile t)
{
	return GetWaterTileType(t) == WATER_TILE_CLEAR;
}

/**
 * Is it a plain water tile?
 * @param i Water tile to query.
 * @return \c true if any type of clear water like ocean, river, or canal.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsWater(TileIndex i)
{
	return IsWater(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a sea water tile?
 * @param t Water tile to query.
 * @return \c true if it is a sea water tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsSea(Tile t)
{
	return IsWater(t) && GetWaterClass(t) == WATER_CLASS_SEA;
}

/**
 * Is it a sea water tile?
 * @param i Water tile to query.
 * @return \c true if it is a sea water tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsSea(TileIndex i)
{
	return IsSea(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a canal tile?
 * @param t Water tile to query.
 * @return \c true if it is a canal tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsCanal(Tile t)
{
	return IsWater(t) && GetWaterClass(t) == WATER_CLASS_CANAL;
}

/**
 * Is it a canal tile?
 * @param i Water tile to query.
 * @return \c true if it is a canal tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsCanal(TileIndex i)
{
	return IsCanal(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a river water tile?
 * @param t Water tile to query.
 * @return \c true if it is a river water tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsRiver(Tile t)
{
	return IsWater(t) && GetWaterClass(t) == WATER_CLASS_RIVER;
}

/**
 * Is it a river water tile?
 * @param i Water tile to query.
 * @return \c true if it is a river water tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsRiver(TileIndex i)
{
	return IsRiver(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a water tile with plain water?
 * @param t Tile to query.
 * @return \c true if it is a plain water tile.
 */
inline bool IsWaterTile(Tile t)
{
	return IsTileType(t, MP_WATER) && IsWater(t);
}

/**
 * Is it a water tile with plain water?
 * @param i Tile to query.
 * @return \c true if it is a plain water tile.
 */
inline bool IsWaterTile(TileIndex i)
{
	return IsWaterTile(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a coast tile?
 * @param t Water tile to query.
 * @return \c true if it is a sea water tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsCoast(Tile t)
{
	return GetWaterTileType(t) == WATER_TILE_COAST;
}

/**
 * Is it a coast tile?
 * @param i Water tile to query.
 * @return \c true if it is a sea water tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsCoast(TileIndex i)
{
	return IsCoast(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a coast tile
 * @param t Tile to query.
 * @return \c true if it is a coast.
 */
inline bool IsCoastTile(Tile t)
{
	return IsTileType(t, MP_WATER) && IsCoast(t);
}

/**
 * Is it a coast tile
 * @param i Tile to query.
 * @return \c true if it is a coast.
 */
inline bool IsCoastTile(TileIndex i)
{
	return IsCoastTile(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a water tile with a ship depot on it?
 * @param t Water tile to query.
 * @return \c true if it is a ship depot tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsShipDepot(Tile t)
{
	return GetWaterTileType(t) == WATER_TILE_DEPOT;
}

/**
 * Is it a water tile with a ship depot on it?
 * @param i Water tile to query.
 * @return \c true if it is a ship depot tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsShipDepot(TileIndex i)
{
	return IsShipDepot(Tile::GetByType(i, MP_WATER));
}

/**
 * Is it a ship depot tile?
 * @param t Tile to query.
 * @return \c true if it is a ship depot tile.
 */
inline bool IsShipDepotTile(Tile t)
{
	return IsTileType(t, MP_WATER) && IsShipDepot(t);
}

/**
 * Is it a ship depot tile?
 * @param i Tile to query.
 * @return \c true if it is a ship depot tile.
 */
inline bool IsShipDepotTile(TileIndex i)
{
	return IsShipDepotTile(Tile::GetByType(i, MP_WATER));
}

/**
 * Get the axis of the ship depot.
 * @param t Water tile to query.
 * @return Axis of the depot.
 * @pre IsShipDepotTile(t)
 */
inline Axis GetShipDepotAxis(Tile t)
{
	assert(IsShipDepotTile(t));
	return (Axis)GB(t.m5(), WBL_DEPOT_AXIS, 1);
}

/**
 * Get the axis of the ship depot.
 * @param i Water tile to query.
 * @return Axis of the depot.
 * @pre IsShipDepotTile(t)
 */
inline Axis GetShipDepotAxis(TileIndex i)
{
	return GetShipDepotAxis(Tile::GetByType(i, MP_WATER));
}

/**
 * Get the part of a ship depot.
 * @param t Water tile to query.
 * @return Part of the depot.
 * @pre IsShipDepotTile(t)
 */
inline DepotPart GetShipDepotPart(Tile t)
{
	assert(IsShipDepotTile(t));
	return (DepotPart)GB(t.m5(), WBL_DEPOT_PART, 1);
}

/**
 * Get the part of a ship depot.
 * @param i Water tile to query.
 * @return Part of the depot.
 * @pre IsShipDepotTile(t)
 */
inline DepotPart GetShipDepotPart(TileIndex i)
{
	return GetShipDepotPart(Tile::GetByType(i, MP_WATER));
}

/**
 * Get the direction of the ship depot.
 * @param t Water tile to query.
 * @return Direction of the depot.
 * @pre IsShipDepotTile(t)
 */
inline DiagDirection GetShipDepotDirection(Tile t)
{
	return XYNSToDiagDir(GetShipDepotAxis(t), GetShipDepotPart(t));
}

/**
 * Get the direction of the ship depot.
 * @param i Water tile to query.
 * @return Direction of the depot.
 * @pre IsShipDepotTile(t)
 */
inline DiagDirection GetShipDepotDirection(TileIndex i)
{
	return GetShipDepotDirection(Tile::GetByType(i, MP_WATER));
}

/**
 * Get the other tile of the ship depot.
 * @param index Tile to query, containing one section of a ship depot.
 * @return Tile containing the other section of the depot.
 * @pre IsShipDepotTile(t)
 */
inline TileIndex GetOtherShipDepotTile(TileIndex index)
{
	Tile tile = Tile::GetByType(index, MP_WATER);
	return index + (GetShipDepotPart(tile) != DEPOT_PART_NORTH ? -1 : 1) * TileOffsByAxis(GetShipDepotAxis(tile));
}

/**
 * Get the most northern tile of a ship depot.
 * @param index One of the tiles of the ship depot.
 * @return The northern tile of the depot.
 * @pre IsShipDepotTile(t)
 */
inline TileIndex GetShipDepotNorthTile(TileIndex index)
{
	assert(IsShipDepot(Tile::GetByType(index, MP_WATER)));
	TileIndex index2 = GetOtherShipDepotTile(index);

	return index < index2 ? index : index2;
}

/**
 * Is there a lock on a given water tile?
 * @param t Water tile to query.
 * @return \c true if it is a water lock tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsLock(Tile t)
{
	return GetWaterTileType(t) == WATER_TILE_LOCK;
}

/**
 * Is there a lock on a given water tile?
 * @param i Water tile to query.
 * @return \c true if it is a water lock tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline bool IsLock(TileIndex i)
{
	return IsLock(Tile::GetByType(i, MP_WATER));
}

/**
 * Get the direction of the water lock.
 * @param t Water tile to query.
 * @return Direction of the lock.
 * @pre IsTileType(t, MP_WATER) && IsLock(t)
 */
inline DiagDirection GetLockDirection(Tile t)
{
	assert(IsLock(t));
	return (DiagDirection)GB(t.m5(), WBL_LOCK_ORIENT_BEGIN, WBL_LOCK_ORIENT_COUNT);
}

/**
 * Get the direction of the water lock.
 * @param i Water tile to query.
 * @return Direction of the lock.
 * @pre IsTileType(t, MP_WATER) && IsLock(t)
 */
inline DiagDirection GetLockDirection(TileIndex i)
{
	return GetLockDirection(Tile::GetByType(i, MP_WATER));
}

/**
 * Get the part of a lock.
 * @param t Water tile to query.
 * @return The part.
 * @pre IsTileType(t, MP_WATER) && IsLock(t)
 */
inline uint8_t GetLockPart(Tile t)
{
	assert(IsLock(t));
	return GB(t.m5(), WBL_LOCK_PART_BEGIN, WBL_LOCK_PART_COUNT);
}

/**
 * Get the part of a lock.
 * @param i Water tile to query.
 * @return The part.
 * @pre IsTileType(t, MP_WATER) && IsLock(t)
 */
inline uint8_t GetLockPart(TileIndex i)
{
	return GetLockPart(Tile::GetByType(i, MP_WATER));
}

/**
 * Get the random bits of the water tile.
 * @param t Water tile to query.
 * @return Random bits of the tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline uint8_t GetWaterTileRandomBits(Tile t)
{
	assert(IsTileType(t, MP_WATER));
	return t.m4();
}

/**
 * Get the random bits of the water tile.
 * @param i Water tile to query.
 * @return Random bits of the tile.
 * @pre IsTileType(t, MP_WATER)
 */
inline uint8_t GetWaterTileRandomBits(TileIndex i)
{
	return GetWaterTileRandomBits(Tile::GetByType(i, MP_WATER));
}

/**
 * Checks whether the tile has water at the ground.
 * That is, it is either some plain water tile, or a object/industry/station/... with water under it.
 * @return true iff the tile has water at the ground.
 * @note Coast tiles are not considered waterish, even if there is water on a halftile.
 */
inline bool HasTileWaterGround(Tile t)
{
	return HasTileWaterClass(t) && IsTileOnWater(t) && !IsCoastTile(t);
}

/**
 * Checks whether the tile has water at the ground.
 * That is, it is either some plain water tile, or a object/industry/station/... with water under it.
 * @return true iff the tile has water at the ground.
 * @note Coast tiles are not considered waterish, even if there is water on a halftile.
 */
inline bool HasTileWaterGround(TileIndex i)
{
	return HasTileWaterGround(Tile(i));
}

/**
 * Set the docking tile state of a tile. This is used by pathfinders to reach their destination.
 * As well as water tiles, half-rail tiles, buoys and aqueduct ends can also be docking tiles.
 * @param t the tile
 * @param b the docking tile state
 */
inline void SetDockingTile(Tile t, bool b)
{
	assert(IsTileType(t, MP_WATER) || IsTileType(t, MP_STATION) || IsTileType(t, MP_TUNNELBRIDGE));
	AssignBit(t.m1(), 7, b);
}

/**
 * Set the docking tile state of a tile. This is used by pathfinders to reach their destination.
 * As well as water tiles, half-rail tiles, buoys and aqueduct ends can also be docking tiles.
 * @param i the tile
 * @param b the docking tile state
 */
inline void SetDockingTile(TileIndex i, bool b)
{
	SetDockingTile(Tile(i), b);
}

/**
 * Checks whether the tile is marked as a dockling tile.
 * @return true iff the tile is marked as a docking tile.
 */
inline bool IsDockingTile(Tile t)
{
	return (IsTileType(t, MP_WATER) || IsTileType(t, MP_STATION) || IsTileType(t, MP_TUNNELBRIDGE)) && HasBit(t.m1(), 7);
}

/**
 * Checks whether the tile is marked as a dockling tile.
 * @return true iff the tile is marked as a docking tile.
 */
inline bool IsDockingTile(TileIndex i)
{
	return IsDockingTile(Tile(i));
}


/**
 * Helper function to make a coast tile.
 * @param t The tile to change into water
 */
inline void MakeShore(Tile t)
{
	if (!MayHaveAssociatedTile(t.tile_type())) ClrBit(t.m8(), 14); // Guard against garbage.
	SetTileType(t, MP_WATER);
	SetTileOwner(t, OWNER_WATER);
	SetWaterClass(t, WATER_CLASS_SEA);
	SetDockingTile(t, false);
	t.m2() = 0;
	t.m3() = 0;
	t.m4() = 0;
	t.m5() = 0;
	SetWaterTileType(t, WATER_TILE_COAST);
	SB(t.m6(), 2, 4, 0);
	t.m7() = 0;
	t.m8() &= 1 << 14; // Clear everything except the associated tile flag.
}

/**
 * Helper function for making a watery tile.
 * @param t The tile to change into water
 * @param o The owner of the water
 * @param wc The class of water the tile has to be
 * @param random_bits Eventual random bits to be set for this tile
 */
inline void MakeWater(Tile t, Owner o, WaterClass wc, uint8_t random_bits)
{
	if (!MayHaveAssociatedTile(t.tile_type())) ClrBit(t.m8(), 14); // Guard against garbage.
	SetTileType(t, MP_WATER);
	SetTileOwner(t, o);
	SetWaterClass(t, wc);
	SetDockingTile(t, false);
	t.m2() = 0;
	t.m3() = 0;
	t.m4() = random_bits;
	t.m5() = 0;
	SetWaterTileType(t, WATER_TILE_CLEAR);
	SB(t.m6(), 2, 4, 0);
	t.m7() = 0;
	t.m8() &= 1 << 14; // Clear everything except the associated tile flag.
}

/**
 * Make a sea tile.
 * @param t The tile to change into sea
 */
inline void MakeSea(Tile t)
{
	MakeWater(t, OWNER_WATER, WATER_CLASS_SEA, 0);
}

/**
 * Make a river tile
 * @param t The tile to change into river
 * @param random_bits Random bits to be set for this tile
 */
inline void MakeRiver(Tile t, uint8_t random_bits)
{
	MakeWater(t, OWNER_WATER, WATER_CLASS_RIVER, random_bits);
}

/**
 * Make a canal tile
 * @param t The tile to change into canal
 * @param o The owner of the canal
 * @param random_bits Random bits to be set for this tile
 */
inline void MakeCanal(Tile t, Owner o, uint8_t random_bits)
{
	assert(o != OWNER_WATER);
	MakeWater(t, o, WATER_CLASS_CANAL, random_bits);
}

/**
 * Make a ship depot section.
 * @param t    Tile to place the ship depot section.
 * @param o    Owner of the depot.
 * @param did  Depot ID.
 * @param part Depot part (either #DEPOT_PART_NORTH or #DEPOT_PART_SOUTH).
 * @param a    Axis of the depot.
 * @param original_water_class Original water class.
 */
inline void MakeShipDepot(Tile t, Owner o, DepotID did, DepotPart part, Axis a, WaterClass original_water_class)
{
	if (!MayHaveAssociatedTile(t.tile_type())) ClrBit(t.m8(), 14); // Guard against garbage.
	SetTileType(t, MP_WATER);
	SetTileOwner(t, o);
	SetWaterClass(t, original_water_class);
	SetDockingTile(t, false);
	t.m2() = did.base();
	t.m3() = 0;
	t.m4() = 0;
	t.m5() = part << WBL_DEPOT_PART | a << WBL_DEPOT_AXIS;
	SetWaterTileType(t, WATER_TILE_DEPOT);
	SB(t.m6(), 2, 4, 0);
	t.m7() = 0;
	t.m8() &= 1 << 14; // Clear everything except the associated tile flag.
}

/**
 * Make a lock section.
 * @param t Tile to place the water lock section.
 * @param o Owner of the lock.
 * @param part Part to place.
 * @param dir Lock orientation
 * @param original_water_class Original water class.
 * @see MakeLock
 */
inline void MakeLockTile(Tile t, Owner o, LockPart part, DiagDirection dir, WaterClass original_water_class)
{
	if (!MayHaveAssociatedTile(t.tile_type())) ClrBit(t.m8(), 14); // Guard against garbage.
	SetTileType(t, MP_WATER);
	SetTileOwner(t, o);
	SetWaterClass(t, original_water_class);
	SetDockingTile(t, false);
	t.m2() = 0;
	t.m3() = 0;
	t.m4() = 0;
	t.m5() = part << WBL_LOCK_PART_BEGIN | dir << WBL_LOCK_ORIENT_BEGIN;
	SetWaterTileType(t, WATER_TILE_LOCK);
	SB(t.m6(), 2, 4, 0);
	t.m7() = 0;
	t.m8() &= 1 << 14; // Clear everything except the associated tile flag.
}

/**
 * Make a water lock.
 * @param t Tile to place the water lock section.
 * @param o Owner of the lock.
 * @param d Direction of the water lock.
 * @param wc_lower Original water class of the lower part.
 * @param wc_upper Original water class of the upper part.
 * @param wc_middle Original water class of the middle part.
 */
inline void MakeLock(TileIndex t, Owner o, DiagDirection d, WaterClass wc_lower, WaterClass wc_upper, WaterClass wc_middle)
{
	TileIndexDiff delta = TileOffsByDiagDir(d);
	Tile lower_tile = Tile(t - delta);
	Tile upper_tile = Tile(t + delta);

	/* Keep the current waterclass and owner for the tiles.
	 * It allows to restore them after the lock is deleted */
	MakeLockTile(Tile(t), o, LOCK_PART_MIDDLE, d, wc_middle);
	MakeLockTile(lower_tile, IsWaterTile(lower_tile) ? GetTileOwner(lower_tile) : o, LOCK_PART_LOWER, d, wc_lower);
	MakeLockTile(upper_tile, IsWaterTile(upper_tile) ? GetTileOwner(upper_tile) : o, LOCK_PART_UPPER, d, wc_upper);
}

/**
 * Set the non-flooding water tile state of a tile.
 * @param t the tile
 * @param b the non-flooding water tile state
 */
inline void SetNonFloodingWaterTile(Tile t, bool b)
{
	assert(IsTileType(t, MP_WATER));
	AssignBit(t.m3(), 0, b);
}

/**
 * Set the non-flooding water tile state of a tile.
 * @param i the tile
 * @param b the non-flooding water tile state
 */
inline void SetNonFloodingWaterTile(TileIndex i, bool b)
{
	SetNonFloodingWaterTile(Tile::GetByType(i, MP_WATER), b);
}

/**
 * Checks whether the tile is marked as a non-flooding water tile.
 * @return true iff the tile is marked as a non-flooding water tile.
 */
inline bool IsNonFloodingWaterTile(Tile t)
{
	assert(IsTileType(t, MP_WATER));
	return HasBit(t.m3(), 0);
}

/**
 * Checks whether the tile is marked as a non-flooding water tile.
 * @return true iff the tile is marked as a non-flooding water tile.
 */
inline bool IsNonFloodingWaterTile(TileIndex i)
{
	return IsNonFloodingWaterTile(Tile::GetByType(i, MP_WATER));
}

#endif /* WATER_MAP_H */
