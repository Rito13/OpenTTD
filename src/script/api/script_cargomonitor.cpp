/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file script_cargomonitor.cpp Code to monitor cargo pickup and deliveries by companies. */

#include "../../stdafx.h"
#include "script_cargo.hpp"
#include "script_cargomonitor.hpp"
#include "../../town.h"
#include "../../industry.h"
#include "../../game/game_instance.hpp"

#include "../../safeguards.h"

/* static */ SQInteger ScriptCargoMonitor::GetTownDeliveryAmount(ScriptCompany::CompanyID company, CargoType cargo, TownID town_id, bool keep_monitoring)
{
	::CompanyID cid = ScriptCompany::FromScriptCompanyID(ScriptCompany::ResolveCompanyID((company)));
	if (cid >= MAX_COMPANIES) return -1;
	if (!ScriptCargo::IsValidCargo(cargo)) return -1;
	if (!::Town::IsValidID(town_id)) return -1;

	GameInstance *game = dynamic_cast<GameInstance *>(&ScriptObject::GetActiveInstance());
	assert(game != nullptr);
	CargoMonitorID monitor = EncodeCargoTownMonitor(cid, cargo, town_id, game->GetID());
	return GetDeliveryAmount(monitor, keep_monitoring);
}

/* static */ SQInteger ScriptCargoMonitor::GetIndustryDeliveryAmount(ScriptCompany::CompanyID company, CargoType cargo, IndustryID industry_id, bool keep_monitoring)
{
	::CompanyID cid = ScriptCompany::FromScriptCompanyID(ScriptCompany::ResolveCompanyID((company)));
	if (cid >= MAX_COMPANIES) return -1;
	if (!ScriptCargo::IsValidCargo(cargo)) return -1;
	if (!::Industry::IsValidID(industry_id)) return -1;

	GameInstance *game = dynamic_cast<GameInstance *>(&ScriptObject::GetActiveInstance());
	assert(game != nullptr);
	CargoMonitorID monitor = EncodeCargoIndustryMonitor(cid, cargo, industry_id, game->GetID());
	return GetDeliveryAmount(monitor, keep_monitoring);
}

/* static */ SQInteger ScriptCargoMonitor::GetTownPickupAmount(ScriptCompany::CompanyID company, CargoType cargo, TownID town_id, bool keep_monitoring)
{
	::CompanyID cid = ScriptCompany::FromScriptCompanyID(ScriptCompany::ResolveCompanyID((company)));
	if (cid >= MAX_COMPANIES) return -1;
	if (!ScriptCargo::IsValidCargo(cargo)) return -1;
	if (!::Town::IsValidID(town_id)) return -1;

	GameInstance *game = dynamic_cast<GameInstance *>(&ScriptObject::GetActiveInstance());
	assert(game != nullptr);
	CargoMonitorID monitor = EncodeCargoTownMonitor(cid, cargo, town_id, game->GetID());
	return GetPickupAmount(monitor, keep_monitoring);
}

/* static */ SQInteger ScriptCargoMonitor::GetIndustryPickupAmount(ScriptCompany::CompanyID company, CargoType cargo, IndustryID industry_id, bool keep_monitoring)
{
	::CompanyID cid = ScriptCompany::FromScriptCompanyID(ScriptCompany::ResolveCompanyID((company)));
	if (cid >= MAX_COMPANIES) return -1;
	if (!ScriptCargo::IsValidCargo(cargo)) return -1;
	if (!::Industry::IsValidID(industry_id)) return -1;

	GameInstance *game = dynamic_cast<GameInstance *>(&ScriptObject::GetActiveInstance());
	assert(game != nullptr);
	CargoMonitorID monitor = EncodeCargoIndustryMonitor(cid, cargo, industry_id, game->GetID());
	return GetPickupAmount(monitor, keep_monitoring);
}

/* static */ void ScriptCargoMonitor::StopAllMonitoring()
{
	GameInstance *game = dynamic_cast<GameInstance *>(&ScriptObject::GetActiveInstance());
	assert(game != nullptr);
	ClearCargoPickupMonitoring(::INVALID_OWNER, game->GetID());
	ClearCargoDeliveryMonitoring(::INVALID_OWNER, game->GetID());
}

