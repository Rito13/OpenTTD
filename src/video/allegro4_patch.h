/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file allegro4_patch.h Injection of resizable windows patch for X11 into Allegro 4.4.2. */

#ifndef ALLEGRO4_PATCH_H
#define ALLEGRO4_PATCH_H

#ifdef WITH_ALLEGRO
#include <allegro.h>

#ifdef WITH_X11

#define None X11None /**< Rename None type from x11 library to X11None in order to prevent name conflict with Blitter::PaletteAnimation::None class. */
#define Window X11Window /**< Rename Window type from x11 library to X11Window in order to prevent name conflict with Window class. */
#include "../3rdparty/allegro4_patch/xwin.h"
#undef Window
#undef None

extern GFX_DRIVER gfx_xwin;
extern GFX_DRIVER gfx_xwin_fullscreen;

#endif /* with_x11 */

bool operator==(_DRIVER_INFO a, _DRIVER_INFO b)
{
	return a.id == b.id && a.driver == b.driver && a.autodetect == b.autodetect;
}

inline void InjectResizablePatch()
{
#ifdef WITH_X11
	_DRIVER_INFO *iter = _xwin_gfx_driver_list;
	while (*iter != _DRIVER_INFO{ 0, NULL, 0 }) ++iter; // Find the end of the table.
	--iter; // The last element is X11.
	*iter = _DRIVER_INFO{ GFX_XWINDOWS, &gfx_xwin, TRUE };
	--iter; // The last - 1 element is X11 fullscreen.
	*iter = _DRIVER_INFO{ GFX_XWINDOWS_FULLSCREEN, &gfx_xwin_fullscreen, TRUE };
#endif /* with_x11 */
}

#endif /* WITH_ALLEGRO */
#endif /* ALLEGRO4_PATCH_H */
