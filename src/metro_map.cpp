/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file metro_map.cpp Complex road accessors. */

#include "metro_map.h"
#include "tile_map.h"
#include "station_map.h"
#include "stdafx.h"

#include "safeguards.h"

/**
 * Returns the metro owner of a tile
 *
 * @param tile The tile to check
 * @return The metro owner of the tile
 * @pre IsMetroTile(tile)
 */
Owner GetMetroTileOwner(Tile tile)
{
	assert(IsMetroTile(tile));
    Owner owner = OWNER_TOWN;
    switch(GetTileType(tile)) {
        case MP_HOUSE:
            break;
        case MP_INDUSTRY: {
            uint8_t owner_bits = GB(tile.m6(), 6, 2);  ///< bits 1, 2
            owner_bits |= HasBit(tile.m1(), 4) << 2;   ///< bit 3
            owner_bits |= HasBit(tile.type(), 6) << 3; ///< bit 4
            owner = (Owner)(owner_bits-1);
            break;
        }
        case MP_ROAD: {
            uint8_t owner_bits = GB(tile.m6(), 0, 2);  ///< bits 1, 2
            owner_bits |= GB(tile.m7(), 5, 2) << 2;    ///< bits 3, 4
            owner = (Owner)(owner_bits-1);
            break;
        }
        case MP_CLEAR:
            owner = (Owner)(GB(tile.m7(), 4, 4)-1);
            break;
        case MP_TUNNELBRIDGE:
            owner = (Owner)(GB(tile.m2(), 12, 4)-1);
            break;
        case MP_STATION:
            if(HasStationRail(tile) || IsAnyRoadStop(tile)) {
                owner = GetTileOwner(tile);
                break;
            }
            [[fallthrough]];
        default:
            owner = (Owner)(GB(tile.m6(), 4, 4)-1);
            break;
    }
    if(owner >= OWNER_TOWN) owner = OWNER_NONE;
	return owner;
}

/**
 * Sets the metro owner of a tile
 *
 * @param tile The tile to change the metro owner status.
 * @param owner The new metro owner.
 * @pre IsMetroTile(tile)
 * @pre (GetTileType(tile) == MP_STATION) && (HasStationRail(tile) || IsAnyRoadStop(tile))
 */
void SetMetroTileOwner(Tile tile, Owner owner)
{
	assert(IsMetroTile(tile));
    if(owner == OWNER_NONE) {
        owner = (Owner)0;
    } else {
        owner = (Owner)(owner.base()+1);
    }
    switch(GetTileType(tile)) {
        case MP_HOUSE:
            break;
        case MP_INDUSTRY:
            SB(tile.m6(), 6, 2, GB(owner.base(), 0, 2));        ///< bits 1, 2
            AssignBit(tile.m1(), 4, HasBit(owner.base(), 3));   ///< bit 3
            AssignBit(tile.type(), 6, HasBit(owner.base(), 4)); ///< bit 4
            break;
        case MP_ROAD: {
            SB(tile.m6(), 0, 2, GB(owner.base(), 0, 2)); ///< bits 1, 2
            SB(tile.m7(), 5, 2, GB(owner.base(), 2, 2)); ///< bits 3, 4
            break;
        }
        case MP_CLEAR:
            SB(tile.m7(), 4, 4, owner.base());
            break;
        case MP_TUNNELBRIDGE:
            SB(tile.m2(), 12, 4, owner.base());
            break;
        case MP_STATION:
            assert((!HasStationRail(tile)) && (!IsAnyRoadStop(tile)));
            [[fallthrough]];
        default:
            SB(tile.m6(), 4, 4, owner.base());
            break;
    }
}

/**
 * Updates the metro owner of a tile
 * It is a setter which prevents invalid data
 *
 * @param tile The tile to change the metro owner status.
 * @param owner The new metro owner.
 * @pre IsMetroTile(tile)
 */
void UpdateMetroTileOwner(Tile tile, Owner owner)
{
    if(GetMetroTrackBits(tile) == 0) return;
    SetMetroTileOwner(tile, owner);
}