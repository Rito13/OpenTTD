/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file game_type.hpp Base types for Game Scripts. */

#ifndef GAME_TYPE_HPP
#define GAME_TYPE_HPP

using GameID = uint8_t; ///< Type to store Game Script id.
static constexpr GameID MAX_COUNT_OF_GAME_SCRIPTS = 32; ///< How many Game Scripts can run at the same time.
static constexpr GameID INVALID_GS_ID = 0xFF; ///< Invalid marker for GameID.

#endif /* GAME_TYPE_HPP */
