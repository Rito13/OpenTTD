/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file before_metro_map.h Accessors for towns */

#ifndef BEFORE_METRO_MAP_H
#define BEFORE_METRO_MAP_H

#include "house.h"
#include "timer/timer_game_calendar.h"
#include "rail_type.h"
#include "depot_type.h"
#include "signal_func.h"
#include "track_func.h"
#include "signal_type.h"
#include "station_func.h"
#include "rail.h"
#include "road.h"
#include "road_func.h"
#include "tile_map.h"
#include "road_type.h"
#include "bridge.h"
#include "water_map.h"

/*
 * NOTICE
 *
 * This file contains only backward compatibility functions. 
 * They are only used to maintain ability to load old save files, 
 * so do not use them in game code except loading part.
 */

inline HouseID GetCleanHouseType_BeforeMetro(Tile t)
{
	assert(IsTileType(t, MP_HOUSE));
	return GB(t.m8(), 0, 12);
}

inline HouseID GetHouseType_BeforeMetro(Tile t)
{
	return GetTranslatedHouseID(GetCleanHouseType_BeforeMetro(t));
}

inline RailType GetRailType_BeforeMetro(Tile t)
{
	return (RailType)GB(t.m8(), 0, 6);
}

inline StationGfx GetStationGfx_BeforeMetro(Tile t)
{
	assert(IsTileType(t, MP_STATION));
	return t.m5();
}

inline StationType GetStationType_BeforeMetro(Tile t)
{
	assert(IsTileType(t, MP_STATION));
	return (StationType)GB(t.m6(), 3, 4);
}

inline bool IsRailStation_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::Rail;
}

inline bool IsRailWaypoint_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::RailWaypoint;
}

inline bool HasStationRail_BeforeMetro(Tile t)
{
	return IsRailStation_BeforeMetro(t) || IsRailWaypoint_BeforeMetro(t);
}

inline bool IsTruckStop_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::Truck;
}

inline bool IsBusStop_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::Bus;
}

inline bool IsRoadWaypoint_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::RoadWaypoint;
}

inline bool IsAnyRoadStop_BeforeMetro(Tile t)
{
	assert(IsTileType(t, MP_STATION));
	return IsTruckStop_BeforeMetro(t) || IsBusStop_BeforeMetro(t) || IsRoadWaypoint_BeforeMetro(t);
}

inline bool MayHaveRoad_BeforeMetro(Tile t)
{
	switch (GetTileType(t)) {
		case MP_ROAD:
			return true;

		case MP_STATION:
			return IsAnyRoadStop_BeforeMetro(t);

		case MP_TUNNELBRIDGE:
			return (TransportType)GB(t.m5(), 2, 2) == TRANSPORT_ROAD;

		default:
			return false;
	}
}

inline RoadType GetRoadTypeTram_BeforeMetro(Tile t)
{
	assert(MayHaveRoad_BeforeMetro(t));
	return (RoadType)GB(t.m8(), 6, 6);
}

inline RoadType GetRoadTypeRoad_BeforeMetro(Tile t)
{
	assert(MayHaveRoad_BeforeMetro(t));
	return (RoadType)GB(t.m4(), 0, 6);
}

inline RoadType GetRoadType_BeforeMetro(Tile t, RoadTramType rtt)
{
	return (rtt == RTT_TRAM) ? GetRoadTypeTram_BeforeMetro(t) : GetRoadTypeRoad_BeforeMetro(t);
}

inline bool HasTileRoadType_BeforeMetro(Tile t, RoadTramType rtt)
{
	return GetRoadType_BeforeMetro(t, rtt) != INVALID_ROADTYPE;
}

inline static RoadTileType GetRoadTileType_BeforeMetro(Tile t)
{
	assert(IsTileType(t, MP_ROAD));
	return (RoadTileType)GB(t.m5(), 6, 2);
}

inline static bool IsNormalRoad_BeforeMetro(Tile t)
{
	return GetRoadTileType_BeforeMetro(t) == ROAD_TILE_NORMAL;
}

inline static bool IsNormalRoadTile_BeforeMetro(Tile t)
{
	return IsTileType(t, MP_ROAD) && IsNormalRoad_BeforeMetro(t);
}

inline Owner GetRoadOwner_BeforeMetro(Tile t, RoadTramType rtt)
{
	assert(MayHaveRoad_BeforeMetro(t));
	if (rtt == RTT_ROAD) return (Owner)GB(IsNormalRoadTile_BeforeMetro(t) ? t.m1() : t.m7(), 0, 5);

	/* Trams don't need OWNER_TOWN, and remapping OWNER_NONE
	 * to OWNER_TOWN makes it use one bit less */
	Owner o = (Owner)GB(t.m3(), 4, 4);
	return o == OWNER_TOWN ? OWNER_NONE : o;
}

inline bool IsRoadOwner_BeforeMetro(Tile t, RoadTramType rtt, Owner o)
{
	assert(HasTileRoadType_BeforeMetro(t, rtt));
	return (GetRoadOwner_BeforeMetro(t, rtt) == o);
}

inline bool HasTownOwnedRoad_BeforeMetro(Tile t)
{
	return HasTileRoadType_BeforeMetro(t, RTT_ROAD) && IsRoadOwner_BeforeMetro(t, RTT_ROAD, OWNER_TOWN);
}

inline bool IsAirport_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::Airport;
}

inline bool IsAirportTile_BeforeMetro(Tile t)
{
	return IsTileType(t, MP_STATION) && IsAirport_BeforeMetro(t);
}

inline bool IsBuoy_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::Buoy;
}

inline bool IsBuoyTile_BeforeMetro(Tile t)
{
	return IsTileType(t, MP_STATION) && IsBuoy_BeforeMetro(t);
}

inline bool IsDock_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::Dock;
}

inline bool IsAnyRoadStopTile_BeforeMetro(Tile t)
{
	return IsTileType(t, MP_STATION) && IsAnyRoadStop_BeforeMetro(t);
}

inline bool IsDriveThroughStopTile_BeforeMetro(Tile t)
{
	return IsAnyRoadStopTile_BeforeMetro(t) && GetStationGfx_BeforeMetro(t) >= GFX_TRUCK_BUS_DRIVETHROUGH_OFFSET;
}

inline bool IsOilRig_BeforeMetro(Tile t)
{
	return GetStationType_BeforeMetro(t) == StationType::Oilrig;
}

inline bool IsStationRoadStop_BeforeMetro(Tile t)
{
	assert(IsTileType(t, MP_STATION));
	return IsTruckStop_BeforeMetro(t) || IsBusStop_BeforeMetro(t);
}

inline void MakeRailNormal_BeforeMetro(Tile t, Owner o, TrackBits b, RailType r)
{
	SetTileType(t, MP_RAILWAY);
	SetTileOwner(t, o);
	SetDockingTile(t, false);
	t.m2() = 0;
	t.m3() = 0;
	t.m4() = 0;
	t.m5() = RAIL_TILE_NORMAL << 6 | b;
	SB(t.m6(), 2, 4, 0);
	t.m7() = 0;
	t.m8() = r;
}

inline void SetHouseProtected_BeforeMetro(Tile t, bool house_protected)
{
	assert(IsTileType(t, MP_HOUSE));
	SB(t.m3(), 5, 1, house_protected ? 1 : 0);
}

inline void SetHouseType_BeforeMetro(Tile t, HouseID house_id)
{
	assert(IsTileType(t, MP_HOUSE));
	SB(t.m8(), 0, 12, house_id);
}

inline void SetRailStationReservation_BeforeMetro(Tile t, bool b)
{
	assert(HasStationRail_BeforeMetro(t));
	AssignBit(t.m6(), 2, b);
}

inline void SetRailType_BeforeMetro(Tile t, RailType r)
{
	SB(t.m8(), 0, 6, r);
}

inline void SetRoadTypeRoad_BeforeMetro(Tile t, RoadType rt)
{
	assert(MayHaveRoad_BeforeMetro(t));
	assert(rt == INVALID_ROADTYPE || RoadTypeIsRoad(rt));
	SB(t.m4(), 0, 6, rt);
}

inline void SetRoadTypeTram_BeforeMetro(Tile t, RoadType rt)
{
	assert(MayHaveRoad_BeforeMetro(t));
	assert(rt == INVALID_ROADTYPE || RoadTypeIsTram(rt));
	SB(t.m8(), 6, 6, rt);
}

inline void SetRoadTypes_BeforeMetro(Tile t, RoadType road_rt, RoadType tram_rt)
{
	SetRoadTypeRoad_BeforeMetro(t, road_rt);
	SetRoadTypeTram_BeforeMetro(t, tram_rt);
}

inline void SetSignalStates_BeforeMetro(Tile tile, uint state)
{
	SB(tile.m4(), 4, 4, state);
}

inline void SetStationGfx_BeforeMetro(Tile t, StationGfx gfx)
{
	assert(IsTileType(t, MP_STATION));
	t.m5() = gfx;
}

#endif /* BEFORE_METRO_MAP_H */