/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/* This file contains code to downgrade the API from 16 to 15. */

/* 16 adds direction to GetRailType. */
AIRail.GetRailTypeCompat15 <- AIRail.GetRailType;
AIRail.GetRailType <- function(tile)
{
	local tile_x = AIMap.GetTileX(tile);
	local tile_y = AIMap.GetTileY(tile);
	local dirs = [[-1, 0], [0, -1], [1, 0], [0, 1]];

	foreach (d in dirs) {
		local rt = AIRail.GetRailTypeCompat15(tile, AIMap.GetTileIndex(tile_x + d[0], tile_y + d[1]));
		if (rt != AIRail.RAILTYPE_INVALID) return rt;
	}
	return AIRail.RAILTYPE_INVALID;
}
