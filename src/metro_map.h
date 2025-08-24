/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file rail_map.h Hides the direct accesses to the map array with map accessors */

#ifndef METRO_MAP_H
#define METRO_MAP_H

#include "rail_type.h"
#include "depot_type.h"
#include "signal_func.h"
#include "track_func.h"
#include "tile_map.h"
#include "water_map.h"
#include "signal_type.h"
#include "rail_map.h"

/** The ground 'under' the rail */
enum DoubleTrackDirection : bool {
	DOUBLE_TRACK_DIR_HORZ = false,
    DOUBLE_TRACK_DIR_VERT = true,
};

const uint8_t TRACK_RESERVATION_BOTH = 0x07;

/**
 * Checks if tile can have or has metro
 * @param t the tile to get the information from
 * @return true if tile has metro or can have it
 */
debug_inline bool IsMetroTile(Tile t) {
    return !IsTileType(t, MP_VOID); // metro is not possible under void
}

/**
 * Checks if a metro tile has signals.
 * @param t the tile to get the information from
 * @return true if and only if the tile has signals
 */
debug_inline bool _MetroHasSignals(Tile t)
{
	return HasBit(t.m8(), 15);
}

/**
 * Add/remove the 'has signal' bit from the RailTileType
 * @param tile the tile to add/remove the signals to/from
 * @param signals whether the metro tile should have signals or not
 */
debug_inline void _SetMetroHasSignals(Tile tile, bool signals)
{
	AssignBit(tile.m8(), 15, signals);
}

/**
 * Checks if a metro tile has double track.
 * @param t the tile to get the information from
 * @return true if and only if the tile has double track
 */
debug_inline bool _MetroHasDoubleTrack(Tile t)
{
	return HasBit(t.m8(), 14);
}

/**
 * Sets the has double track bit
 * @param tile the tile to set the bit of
 * @param dt whether the metro tile should have double track or not
 */
debug_inline void _SetMetroHasDoubleTrack(Tile tile, bool dt)
{
	AssignBit(tile.m8(), 14, dt);
}

/**
 * Returns the RailTileType (normal with or without signals)
 * @param t the tile to get the information from
 * @pre IsMetroTile(t)
 * @return the RailTileType
 */
inline static RailTileType GetMetroTileType(Tile t)
{
    assert(IsMetroTile(t));
    if(_MetroHasSignals(t)) {
        return RailTileType::RAIL_TILE_SIGNALS;
    }
    return RailTileType::RAIL_TILE_NORMAL; 
}

/**
 * Checks if a metro tile has signals.
 * @param t the tile to get the information from
 * @pre IsMetroTile(t)
 * @return true if and only if the tile has signals
 */
inline bool MetroHasSignals(Tile t)
{
    assert(IsMetroTile(t));
	return _MetroHasSignals(t);
}

/**
 * Add/remove the 'has signal' bit from the RailTileType
 * @param tile the tile to add/remove the signals to/from
 * @param signals whether the metro tile should have signals or not
 * @pre IsMetroTile(tile)
 */
inline void SetMetroHasSignals(Tile tile, bool signals)
{
    assert(IsMetroTile(tile));
	_SetMetroHasSignals(tile, signals);
}

/**
 * Gets the metro rail type of the given tile
 * @param t the tile to get the metro rail type from
 * @return the metro rail type of the tile
 */
inline RailType GetMetroRailType(Tile t)
{
	return (RailType)GB(t.m8(), 6, 6);
}

/**
 * Sets the metro rail type of the given tile
 * @param t the tile to set the metrp rail type of
 * @param r the new metro rail type for the tile
 */
inline void SetMetroRailType(Tile t, RailType r)
{
	SB(t.m8(), 6, 6, r);
}

/**
 * Gets the track of the given tile
 * @param t the tile to get the track from
 * @return the track of the tile
 */
inline Track GetMetroTrack(Tile t)
{
	return (Track)GB(t.m8(), 3, 3);
}

/**
 * Sets the track of the given tile
 * @param t the tile to set the track of
 * @param tr the new track for the tile
 */
inline void SetMetroTrack(Tile t, Track tr)
{
	SB(t.m8(), 3, 3, tr);
}

/**
 * Gets the direction of the double track of the given tile
 * @param t the tile to get the direction from
 * @return the direction of the double track of the tile
 */
inline DoubleTrackDirection GetMetroDoubleTrackDirection(Tile t)
{
	return (DoubleTrackDirection)HasBit(t.m8(), 3);
}

/**
 * Sets the direction of the double track of the given tile
 * @param t the tile to set the direction of
 * @param d the new direction of the double track for the tile
 */
inline void SetMetroDoubleTrackDirection(Tile t, DoubleTrackDirection d)
{
	AssignBit(t.m8(), 3, d);
}

/**
 * Gets the track bits of the given tile
 * @param t the tile to get the track bits from
 * @pre IsMetroTile(t)
 * @return the track bits of the tile
 */
inline TrackBits GetMetroTrackBits(Tile t)
{
    assert(IsMetroTile(t));
    if(_MetroHasSignals(t)) {
        if(_MetroHasDoubleTrack(t)) return GetMetroDoubleTrackDirection(t) == DOUBLE_TRACK_DIR_HORZ ? TRACK_BIT_HORZ : TRACK_BIT_VERT;
        return TrackToTrackBits(GetMetroTrack(t));
    }
	return (TrackBits)GB(t.m8(), 0, 6);
}

/**
 * Returns whether the given metro track is present on the given tile.
 * @param tile  the tile to check the metro track presence of
 * @param track the metro track to search for on the tile
 * @pre IsMetroTile(tile)
 * @return true if and only if the given metro track exists on the tile
 */
inline bool HasMetroTrack(Tile tile, Track track)
{
	return HasBit(GetMetroTrackBits(tile), track);
}

/**
 * Checks if value is an power of two
 * @param i the value to check
 */
inline bool IsPowerOfTwo(uint8_t i) {
    return i > 0 && (i & (i-1)) == 0;
}

/**
 * Sets the track bits of the given tile
 * @param t the tile to set the track bits of
 * @param b the new track bits for the tile
 * @pre IsMetroTile(t)
 */
inline void SetMetroTrackBits(Tile t, TrackBits b)
{
    assert(IsMetroTile(t));
    if(_MetroHasSignals(t)) {
        switch(b) {
            case TRACK_BIT_X:
                _SetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_X);
                return;
            case TRACK_BIT_Y:
                _SetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_Y);
                return;
            case TRACK_BIT_UPPER:
                _SetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_UPPER);
                return;
            case TRACK_BIT_LOWER:
                _SetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_LOWER);
                return;
            case TRACK_BIT_LEFT:
                _SetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_LEFT);
                return;
            case TRACK_BIT_RIGHT:
                _SetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_RIGHT);
                return;
            case TRACK_BIT_HORZ:
                _SetMetroHasDoubleTrack(t, true);
                SetMetroDoubleTrackDirection(t, DOUBLE_TRACK_DIR_HORZ);
                return;
            case TRACK_BIT_VERT:
                _SetMetroHasDoubleTrack(t, true);
                SetMetroDoubleTrackDirection(t, DOUBLE_TRACK_DIR_VERT);
                return;
            default:
                _SetMetroHasSignals(t,false);
                break;
        }
    }
	SB(t.m8(), 0, 6, b);
}

/**
 * Returns the reserved metro track bits of the tile
 * @pre IsMetroTile(t)
 * @param t the tile to query
 * @return the metro track bits
 */
inline TrackBits GetMetroRailReservationTrackBits(Tile t)
{
	assert(IsMetroTile(t));
    if(_MetroHasSignals(t)) {
        if(_MetroHasDoubleTrack(t)) {
            uint8_t out = 0;
            switch(GetMetroDoubleTrackDirection(t)) {
                case DOUBLE_TRACK_DIR_HORZ:
                    if(HasBit(t.m8(), 2)) out += TRACK_BIT_UPPER;
                    if(HasBit(t.m8(), 5)) out += TRACK_BIT_LOWER;
                    break;
                case DOUBLE_TRACK_DIR_VERT:
                    if(HasBit(t.m8(), 2)) out += TRACK_BIT_LEFT;
                    if(HasBit(t.m8(), 5)) out += TRACK_BIT_RIGHT;
                    break;
            }
            return (TrackBits)out;
        }
        // Has only one track
        if(HasBit(t.m8(), 2))
        return TrackToTrackBits(GetMetroTrack(t));
    }
    uint8_t track_b = GB(t.m8(), 12, 3);
    if (track_b == 0) return TRACK_BIT_NONE;
    if (track_b == TRACK_RESERVATION_BOTH) {
        // determine from track bits the direction of double track
        uint8_t track_bits = GB(t.m8(), 0, 6);
        const uint8_t track_4way = TRACK_BIT_HORZ | TRACK_BIT_VERT;
        if((track_bits & track_4way) == track_4way) return (TrackBits)track_4way;
        if((track_bits & TRACK_BIT_HORZ) == TRACK_BIT_HORZ) return TRACK_BIT_HORZ;
        return TRACK_BIT_VERT;
    }
	Track track = (Track)(track_b - 1);    // map array saves Track+1
	return (TrackBits)TrackToTrackBits(track);
}

/**
 * Sets the reserved metro track bits of the tile
 * @pre IsMetroTile(t) && !TracksOverlap(b)
 * @param t the tile to change
 * @param b the metro track bits
 */
inline void SetMetroTrackReservation(Tile t, TrackBits b)
{
	assert(IsMetroTile(t));
    switch(b) {
        case TRACK_BIT_N:
            b = TRACK_BIT_UPPER;
            break;
        case TRACK_BIT_U:
            b = TRACK_BIT_LOWER;
            break;
        case TRACK_BIT_C:
            b = TRACK_BIT_LEFT;
            break;
        case TRACK_BIT_RC:
            b = TRACK_BIT_RIGHT;
            break;
        default:
            break;
    }
    assert(!TracksOverlap(b));
    if(_MetroHasSignals(t)) {
        if(_MetroHasDoubleTrack(t)) {
            if((b & TRACK_BIT_RIGHT) || (b & TRACK_BIT_LOWER))
                AssignBit(t.m8(), 5, true);
            if(!(b & TRACK_BIT_LEFT) && !(b & TRACK_BIT_UPPER)) {
                AssignBit(t.m8(), 2, false);
                return;
            }
        }
        AssignBit(t.m8(), 2, b != TRACK_BIT_NONE);
        return;
    }
	Track track = RemoveFirstTrack(&b);
	SB(t.m8(), 12, 3, track == INVALID_TRACK ? 0 : track + 1);
	if(b != TRACK_BIT_NONE) SB(t.m8(), 12, 3, TRACK_RESERVATION_BOTH);
}

/**
 * Try to reserve a specific metro track on a tile
 * @pre IsMetroTile(tile) && HasMetroTrack(tile, t)
 * @param tile the tile
 * @param t the metro track to reserve
 * @return true if successful
 */
inline bool TryMetroReserveTrack(Tile tile, Track t)
{
	assert(HasMetroTrack(tile, t));
	TrackBits bits = TrackToTrackBits(t);
	TrackBits res = GetMetroRailReservationTrackBits(tile);
	if ((res & bits) != TRACK_BIT_NONE) return false;  // already reserved
	res |= bits;
	if (TracksOverlap(res)) return false;  // crossing reservation present
	SetMetroTrackReservation(tile, res);
	return true;
}

/**
 * Lift the reservation of a specific metro track on a tile
 * @pre IsMetroTile(tile) && HasMetroTrack(tile, t)
 * @param tile the tile
 * @param t the metro track to free
 */
inline void UnreserveMetroTrack(Tile tile, Track t)
{
	assert(HasMetroTrack(tile, t));
	TrackBits res = GetMetroRailReservationTrackBits(tile);
	res &= ~TrackToTrackBits(t);
	SetMetroTrackReservation(tile, res);
}

#endif /* METRO_MAP_H */