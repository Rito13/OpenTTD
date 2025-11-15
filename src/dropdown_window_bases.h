/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file dropdown_window_bases.h Custom base types for dropdown window. */

#include "dropdown_type.h"
#include "rail.h"
#include "road.h"

struct RailTypeDropdownWindowBase {
	void SetSortCriteria(int) {}
	StringID GetSortCriteriaString() const;
	void SetSortOrderInverted(bool is_sort_order_inverted);
	bool IsSortOrderInverted() const { return _railtypes_invert_sort_order; }
	DropDownList GetDropDownList(const BadgeFilterChoices &badge_filter_choices) const;
	GrfSpecFeature GetGrfSpecFeature() const { return GSF_RAILTYPES; }
	DropDownList GetSortDropDownList() const;
};

struct RoadTypeDropdownWindowBase {
	void SetSortCriteria(int) {}
	StringID GetSortCriteriaString() const;
	void SetSortOrderInverted(bool is_sort_order_inverted);
	bool IsSortOrderInverted() const { return _roadtypes_invert_sort_order; }
	DropDownList GetDropDownList(const BadgeFilterChoices &badge_filter_choices) const;
	GrfSpecFeature GetGrfSpecFeature() const { return GSF_ROADTYPES; }
	DropDownList GetSortDropDownList() const;
};

struct TramTypeDropdownWindowBase {
	void SetSortCriteria(int) {}
	StringID GetSortCriteriaString() const;
	void SetSortOrderInverted(bool is_sort_order_inverted);
	bool IsSortOrderInverted() const { return _tramtypes_invert_sort_order; }
	DropDownList GetDropDownList(const BadgeFilterChoices &badge_filter_choices) const;
	GrfSpecFeature GetGrfSpecFeature() const { return GSF_TRAMTYPES; }
	DropDownList GetSortDropDownList() const;
};
