/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file map_storage.cpp Test if structs and unions from map_func.h are consistent. */

#include "../stdafx.h"

#include "../3rdparty/catch2/catch.hpp"

#include "../map_func.h"

#include "../safeguards.h"

static constexpr uint64_t BASE_VALUE = 0b0000000011111110000000111111000000111110000011110000111000110010; // Needs to be 64 bits.

TEST_CASE("Map storage tests.")
{
	/* Store some unique data. */
	Map::Allocate(MIN_MAP_SIZE, MIN_MAP_SIZE);
	Tile t(TileXY(0, 0));
	t.TypeAndOffset() = BASE_VALUE;

	CHECK(t.GetOffset() == GB(BASE_VALUE, 0, BITS_FOR_SUB_TILES_OFFSET));
	CHECK(t.height() == GB(BASE_VALUE, 24, 8));
	CHECK(t.type() == GB(BASE_VALUE, 32, 8));
	CHECK(t.m1() == GB(BASE_VALUE, 40, 8));
	CHECK(t.m3() == GB(BASE_VALUE, 48, 8));
	CHECK(t.m4() == GB(BASE_VALUE, 56, 8));

	CHECK(t.GetBaseTileAs<MP_CLEAR>().sub_tiles == GB(BASE_VALUE, 32, 7));
	CHECK(t.GetBaseTileAs<MP_CLEAR>().ground == GB(BASE_VALUE, 40, 3));
	CHECK(t.GetBaseTileAs<MP_CLEAR>().density == GB(BASE_VALUE, 43, 2));
	CHECK(t.GetBaseTileAs<MP_CLEAR>().update == GB(BASE_VALUE, 45, 3));
	CHECK(t.GetBaseTileAs<MP_CLEAR>().bit_offset1 == GB(BASE_VALUE, 51, 1));
	CHECK(t.GetBaseTileAs<MP_CLEAR>().bit_offset2 == GB(BASE_VALUE, 56, 2));
}
