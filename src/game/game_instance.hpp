/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file game_instance.hpp The GameInstance tracks games. */

#ifndef GAME_INSTANCE_HPP
#define GAME_INSTANCE_HPP

#include "../script/script_instance.hpp"
#include "game_info.hpp"
#include "game_type.hpp"

/** Runtime information about a game script like a pointer to the squirrel vm and the current state. */
class GameInstance : public ScriptInstance {
public:
	GameInstance();

	/**
	 * Initialize the script and prepare it for its first run.
	 * @param info The GameInfo to start.
	 * @param id The index under whitch the script is initialized.
	 * @param already_used_sub_apis Set of sub APIs that are already used by other Game Scripts.
	 */
	void Initialize(class GameInfo *info, GameID id, GSSubAPIs already_used_sub_apis);

	/**
	 * Test if the sub API is available for this game script.
	 * @param api The sub API to test.
	 * @return \c true iff the API is available.
	 */
	bool IsSubAPIAvailable(GSSubAPI api) { return this->required_sub_apis.Test(api); }

	/**
	 * Get which sub APIs are available for this Game Script.
	 * @return The sub APIs available for this Game Script.
	 */
	GSSubAPIs GetAvailableSubAPIs() { return this->required_sub_apis; }

	/**
	 * Get the index of this Game Script.
	 * @return The index of this Game Script.
	 */
	GameID GetID() { return this->id; }

	int GetSetting(const std::string &name) override;
	ScriptInfo *FindLibrary(const std::string &library, int version) override;

private:
	GSSubAPIs required_sub_apis; ///< Sub APIs required by this Game Script to work properly.
	GameID id; ///< The current index of this Game Script.

	void RegisterAPI() override;
	void Died() override;
	CommandCallbackData *GetDoCommandCallback() override;
	void LoadDummyScript() override {}
};

#endif /* GAME_INSTANCE_HPP */
