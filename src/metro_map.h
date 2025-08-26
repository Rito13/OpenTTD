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
#include "clear_map.h"

/** The direction of tracks when only two are available */
enum DoubleTrackDirection : bool {
	DOUBLE_TRACK_DIR_HORZ = false,
    DOUBLE_TRACK_DIR_VERT = true,
};

/** 
 * As metro uses only 3 bits for track reservation it
 * uses this value for every combination of two tracks.
 */
static const uint8_t TRACK_RESERVATION_BOTH = 0x07;

/**
 * Checks if tile can have or has metro
 * @param t the tile to get the information from
 * @return true if tile has metro or can have it
 */
debug_inline bool IsMetroTile(Tile t)
{
    return !IsTileType(t, MP_VOID); ///< metro is not possible under void
}

/**
 * Checks if a metro tile has signals.
 * @param t the tile to get the information from
 * @return true if and only if the tile has signals
 */
debug_inline bool InnerMetroHasSignals(Tile t)
{
	return HasBit(t.m8(), 15);
}

/**
 * Add/remove the 'has signal' bit from the RailTileType
 * @param tile the tile to add/remove the signals to/from
 * @param signals whether the metro tile should have signals or not
 */
debug_inline void InnerSetMetroHasSignals(Tile tile, bool signals)
{
	AssignBit(tile.m8(), 15, signals);
}

/**
 * Checks if a metro tile has double track.
 * @param t the tile to get the information from
 * @return true if and only if the tile has double track
 */
debug_inline bool InnerMetroHasDoubleTrack(Tile t)
{
	return HasBit(t.m8(), 14);
}

/**
 * Sets the has double track bit
 * @param tile the tile to set the bit of
 * @param dt whether the metro tile should have double track or not
 */
debug_inline void InnerSetMetroHasDoubleTrack(Tile tile, bool dt)
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
    if(InnerMetroHasSignals(t)) {
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
	return InnerMetroHasSignals(t);
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
    if(InnerMetroHasSignals(t)) {
        if(InnerMetroHasDoubleTrack(t)) {
            return GetMetroDoubleTrackDirection(t) == DOUBLE_TRACK_DIR_HORZ ? TRACK_BIT_HORZ : TRACK_BIT_VERT;
        }
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
    if(InnerMetroHasSignals(t)) {
        switch(b) {
            case TRACK_BIT_X:
                InnerSetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_X);
                return;
            case TRACK_BIT_Y:
                InnerSetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_Y);
                return;
            case TRACK_BIT_UPPER:
                InnerSetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_UPPER);
                return;
            case TRACK_BIT_LOWER:
                InnerSetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_LOWER);
                return;
            case TRACK_BIT_LEFT:
                InnerSetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_LEFT);
                return;
            case TRACK_BIT_RIGHT:
                InnerSetMetroHasDoubleTrack(t, false);
                SetMetroTrack(t, TRACK_RIGHT);
                return;
            case TRACK_BIT_HORZ:
                InnerSetMetroHasDoubleTrack(t, true);
                SetMetroDoubleTrackDirection(t, DOUBLE_TRACK_DIR_HORZ);
                return;
            case TRACK_BIT_VERT:
                InnerSetMetroHasDoubleTrack(t, true);
                SetMetroDoubleTrackDirection(t, DOUBLE_TRACK_DIR_VERT);
                return;
            default:
                InnerSetMetroHasSignals(t,false);
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
    if(InnerMetroHasSignals(t)) {
        if(InnerMetroHasDoubleTrack(t)) {
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
        if((track_bits & TRACK_BIT_VERT) != TRACK_BIT_VERT) return TRACK_BIT_HORZ;
        if((track_bits & TRACK_BIT_HORZ) != TRACK_BIT_HORZ) return TRACK_BIT_VERT;
        return TRACK_BIT_HORZ | TRACK_BIT_VERT;
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
    if(InnerMetroHasSignals(t)) {
        if(InnerMetroHasDoubleTrack(t)) {
            if((b & TRACK_BIT_RIGHT) || (b & TRACK_BIT_LOWER)) {
                AssignBit(t.m8(), 5, true);
            }
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

/**
 * Add/remove the 'has signal' bit from the RailTileType
 * May be slow as reservation and track storage method
 * depends on value of this bit.
 * @param tile the tile to add/remove the signals to/from
 * @param signals whether the metro tile should have signals or not
 * @pre IsMetroTile(tile)
 */
inline void SetMetroHasSignals(Tile tile, bool signals)
{
    assert(IsMetroTile(tile));
    TrackBits reservation = GetMetroRailReservationTrackBits(tile);
    TrackBits tracks = GetMetroTrackBits(tile);
	InnerSetMetroHasSignals(tile, signals);
    SetMetroTrackBits(tile, tracks);
    SetMetroTrackReservation(tile, reservation);
}

/**
 * Returns metro signal type on a given track
 * @param t the tile to get the information from
 * @param track the track to get the information from
 * @pre IsMetroTile(t)
 * @pre MetroHasSignals(t)
 * @return signal type
 */
inline SignalType GetMetroSignalType(Tile t, Track track)
{
	assert(MetroHasSignals(t));
	return HasBit(t.m8(), 13) ? SIGTYPE_PBS : SIGTYPE_PBS_ONEWAY;
}

/**
 * Sets metro signal type on a given track
 * @param t the tile to get the information from
 * @param track the track to get the information from
 * @param s signal type
 * @pre IsMetroTile(t)
 * @pre MetroHasSignals(t)
 * @pre IsPbsSignal(s)
 */
inline void SetMetroSignalType(Tile t, Track track, SignalType s)
{
	assert(MetroHasSignals(t));
    assert(IsPbsSignal(s));
    AssignBit(t.m8(), 13, s == SIGTYPE_PBS);
}

/** DO NOT USE - Helper function, should be deleted */
inline bool IsMetroPresignalEntry(Tile t, Track track) { return false; }
/** DO NOT USE - Helper function, should be deleted */
inline bool IsMetroPresignalExit(Tile t, Track track) { return false; }

/**
 * Checks if metro signal on a given track is one way
 * @param t the tile to get the information from
 * @param track the track to get the information from
 * @pre IsMetroTile(t)
 * @pre MetroHasSignals(t)
 * @return true if and only if signal on a given track is one way signal
 */
inline bool IsMetroOnewaySignal(Tile t, Track track)
{
    assert(MetroHasSignals(t));
	return !HasBit(t.m8(), 13);
}

/**
 * Cycles signal on a given track through possible positions on that track
 * @param t the tile to get the information from
 * @param track the track to get the information from
 * @pre IsMetroTile(t)
 * @pre MetroHasSignals(t)
 */
inline void CycleMetroSignalSide(Tile t, Track track)
{
	assert(MetroHasSignals(t));
    AssignBit(t.m8(), 0, !HasBit(t.m8(), 0));
}

/**
 * Returns signal variant (e.g. electric, semaphore) on a given track
 * @param t the tile to get the information from
 * @param track the track to get the information from
 * @pre IsMetroTile(t)
 * @pre MetroHasSignals(t)
 */
inline SignalVariant GetMetroSignalVariant(Tile t, Track track)
{
	assert(MetroHasSignals(t));
	return (SignalVariant)GB(t.m8(), 12, 1);
}

/**
 * Sets signal variant (e.g. electric, semaphore) on a given track
 * @param t the tile to get the information from
 * @param track the track to get the information from
 * @param v signal variant (e.g. electric, semaphore)
 * @pre IsMetroTile(t)
 * @pre MetroHasSignals(t)
 */
inline void SetMetroSignalVariant(Tile t, Track track, SignalVariant v)
{
	assert(MetroHasSignals(t));
	SB(t.m8(), 12, 1, v);
}

/**
 * Returns the direction of the double track of the given tile as an track value
 * Used mainly for signal related functions as one metro tile can have max one signal
 * @param t the tile to get the information from
 */
inline Track GetMetroDoubleTrackDirAsTrack(Tile t) {
    /* check if operations for signalled double track tiles are valid */
    static_assert(((1U << (1+DOUBLE_TRACK_DIR_HORZ)) | false) == TRACK_UPPER);
    static_assert(((1U << (1+DOUBLE_TRACK_DIR_HORZ)) | true ) == TRACK_LOWER);
    static_assert(((1U << (1+DOUBLE_TRACK_DIR_VERT)) | false) == TRACK_LEFT );
    static_assert(((1U << (1+DOUBLE_TRACK_DIR_VERT)) | true ) == TRACK_RIGHT);
    return (Track)((1U << (1+GetMetroDoubleTrackDirection(t))) | HasBit(t.m8(), 4));
}

/**
 * Checks for the presence of metro signal on the given track on the given rail tile.
 * @param t the tile to get the information from
 * @param track the track to get the information from
 * @pre IsValidTrack(track)
 * @return true if and only if there is a signal on the given track
 */
inline bool HasMetroSignalOnTrack(Tile t, Track track)
{
	assert(IsValidTrack(track));
    if(InnerMetroHasDoubleTrack(t)) {
        return MetroHasSignals(t) && GetMetroDoubleTrackDirAsTrack(t) == track;
    }
	return MetroHasSignals(t) && GetMetroTrack(t) == track;
}

/**
 * Checks for the presence of signals along the given metro trackdir on the given
 * metro tile.
 *
 * Along meaning if you are currently driving on the given metro trackdir, this is
 * the signal that is facing us (for which we stop when it's red).
 * @param t the tile to get the information from
 * @param trackdir the track and the direction to get the information from
 * @pre IsValidTrackdir(trackdir)
 * @return true if and only if there is a signal on the given trackdir
 */
inline bool HasMetroSignalOnTrackdir(Tile t, Trackdir trackdir)
{
	assert (IsValidTrackdir(trackdir));
    if(!MetroHasSignals(t)) return false;
    Track track = GetMetroTrack(t);
    if(InnerMetroHasDoubleTrack(t)) track = GetMetroDoubleTrackDirAsTrack(t);
	return HasBit(t.m8(),0) ? TrackToTrackdir(track) == trackdir : TrackToTrackdir(track) == ReverseTrackdir(trackdir);
}

/**
 * Gets the state of the metro signal along the given metro trackdir.
 *
 * Along meaning if you are currently driving on the given metro trackdir, this is
 * the signal that is facing us (for which we stop when it's red).
 * @param t the tile to get the information from
 * @param trackdir the track and the direction to get the information from
 * @pre IsValidTrackdir(trackdir)
 * @pre HasMetroSignalOnTrack(tile, TrackdirToTrack(trackdir))
 * @return signal state e.g. Red, Green
 */
inline SignalState GetMetroSignalStateByTrackdir(Tile tile, Trackdir trackdir)
{
    bool has_signal = HasMetroSignalOnTrackdir(tile, trackdir);
	assert(HasMetroSignalOnTrack(tile, TrackdirToTrack(trackdir)));
	return has_signal && HasBit(tile.m8(), 1) ?
		SIGNAL_STATE_GREEN : SIGNAL_STATE_RED;
}

/**
 * Sets the state of the metro signal along the given metro trackdir.
 * @param t the tile to get the information from
 * @param trackdir the track and the direction to get the information from
 * @param state the new signal state e.g. Red, Green
 * @pre IsValidTrackdir(trackdir)
 * @pre HasMetroSignalOnTrackdir(tile, trackdir)
 */
inline void SetMetroSignalStateByTrackdir(Tile tile, Trackdir trackdir, SignalState state)
{
    assert(HasMetroSignalOnTrackdir(tile, trackdir));
	AssignBit(tile.m8(), 1, state == SIGNAL_STATE_GREEN);
}

/**
 * Is a pbs signal present along the metro trackdir?
 * @param tile the tile to check
 * @param td the metro trackdir to check
 */
inline bool HasPbsMetroSignalOnTrackdir(Tile tile, Trackdir td)
{
	return IsMetroTile(tile) && HasMetroSignalOnTrackdir(tile, td);
}

/**
 * Is a one-way signal blocking the metro trackdir? A one-way signal on the
 * metro trackdir against will block
 * @param tile the tile to check
 * @param td the metro trackdir to check
 */
inline bool HasOnewayMetroSignalBlockingTrackdir(Tile tile, Trackdir td)
{
	return IsMetroTile(tile) && IsMetroOnewaySignal(tile, TrackdirToTrack(td)) &&
            HasMetroSignalOnTrackdir(tile, ReverseTrackdir(td));
}



#endif /* METRO_MAP_H */