//
//

#include "freespace.h" //For frametime

#include "shipclass.h"
#include "model.h"
#include "cockpit_display.h"
#include "species.h"
#include "shiptype.h"
#include "team_colors.h"
#include "vecmath.h"
#include "ship/ship.h"
#include "playerman/player.h"
#include "weapon/weapon.h"
#include "mission/missioncampaign.h"
#include "missionui/missionweaponchoice.h"
#include "graphics/matrix.h"
#include "missionui/missionscreencommon.h"
#include "scripting/api/objs/weaponclass.h"
#include "model/modelrender.h"
#include "utils/string_utils.h"

namespace scripting {
namespace api {


//**********HANDLE: default primary
ADE_OBJ(l_Default_Primary, int, "default_primary", "weapon index");

ADE_INDEXER(l_Default_Primary,
	"number idx",
	"Array of ship default primaries for each bank. Returns the Weapon Class or "
	"nil if the bank is invalid for the ship class.",
	"weaponclass",
	"The weapon index")
{
	int current;
	int idx = -1;
	if (!ade_get_args(L, "oi", l_Default_Primary.Get(&current), &idx))
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		LuaError(L, "This property is read only.");
	}

	ship_info* sip = &Ship_info[current];

	if (idx < 1 || idx > sip->num_primary_banks) {
		return ADE_RETURN_NIL;
	};
	idx--; // Convert to Lua's 1 based index system

	return ade_set_args(L, "o", l_Weaponclass.Set(sip->primary_bank_weapons[idx]));
}

ADE_FUNC(__len,
	l_Default_Primary,
	nullptr,
	"The number of primary banks with defaults",
	"number",
	"The number of primary banks.")
{
	int current;
	ade_get_args(L, "o", l_Default_Primary.Get(&current));
	ship_info* sip = &Ship_info[current];
	return ade_set_args(L, "i", sip->num_primary_banks);
}

//**********HANDLE: default secondary
ADE_OBJ(l_Default_Secondary, int, "default_secondary", "weapon index");

ADE_INDEXER(l_Default_Secondary,
	"number idx",
	"Array of ship default secondaries for each bank. Returns the Weapon Class or "
	"nil if the bank is invalid for the ship class.",
	"weaponclass",
	"The weapon index")
{
	int current;
	int idx = -1;
	if (!ade_get_args(L, "oi", l_Default_Secondary.Get(&current), &idx))
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		LuaError(L, "This property is read only.");
	}

	ship_info* sip = &Ship_info[current];

	if (idx < 1 || idx > sip->num_secondary_banks) {
		return ADE_RETURN_NIL;
	};
	idx--; // Convert to Lua's 1 based index system

	return ade_set_args(L, "o", l_Weaponclass.Set(sip->secondary_bank_weapons[idx]));
}

ADE_FUNC(__len,
	l_Default_Secondary,
	nullptr,
	"The number of secondary banks with defaults",
	"number",
	"The number of secondary banks.")
{
	int current;
	ade_get_args(L, "o", l_Default_Secondary.Get(&current));
	ship_info* sip = &Ship_info[current];
	return ade_set_args(L, "i", sip->num_secondary_banks);
}
	
//**********HANDLE: Shipclass
ADE_OBJ(l_Shipclass, int, "shipclass", "Ship class handle");

ADE_FUNC(__tostring, l_Shipclass, NULL, "Ship class name", "string", "Ship class name, or an empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Ship_info[idx].name);
}

ADE_FUNC(__eq, l_Shipclass, "shipclass, shipclass", "Checks if the two classes are equal", "boolean", "true if equal, false otherwise")
{
	int idx1,idx2;
	if(!ade_get_args(L, "oo", l_Shipclass.Get(&idx1), l_Shipclass.Get(&idx2)))
		return ade_set_error(L, "b", false);

	if(idx1 < 0 || idx1 >= ship_info_size())
		return ade_set_error(L, "b", false);

	if(idx2 < 0 || idx2 >= ship_info_size())
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", idx1 == idx2);
}


ADE_VIRTVAR(Name, l_Shipclass, "string", "Ship class name", "string", "Ship class name, or an empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		auto len = sizeof(Ship_info[idx].name);
		strncpy(Ship_info[idx].name, s, len);
		Ship_info[idx].name[len - 1] = 0;
	}

	return ade_set_args(L, "s", Ship_info[idx].name);
}

ADE_VIRTVAR(ShortName, l_Shipclass, "string", "Ship class short name", "string", "Ship short name, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		auto len = sizeof(Ship_info[idx].short_name);
		strncpy(Ship_info[idx].short_name, s, len);
		Ship_info[idx].short_name[len - 1] = 0;
	}

	return ade_set_args(L, "s", Ship_info[idx].short_name);
}

static int handle_ship_class_optional_string(lua_State *L, std::unique_ptr<char[]> ship_info::*optional_string_field)
{
	int idx;
	const char* s = nullptr;
	if (!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if (!SCP_vector_inbounds(Ship_info, idx))
		return ade_set_error(L, "s", "");

	auto sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		sip->*optional_string_field = util::unique_copy(s, true);
	}

	return ade_set_args(L, "s", coalesce((sip->*optional_string_field).get(), ""));
}

ADE_VIRTVAR(TypeString, l_Shipclass, "string", "Ship class type string", "string", "Type string, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::type_str);
}

ADE_VIRTVAR(ManeuverabilityString, l_Shipclass, "string", "Ship class maneuverability string", "string", "Maneuverability string, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::maneuverability_str);
}

ADE_VIRTVAR(ArmorString, l_Shipclass, "string", "Ship class armor string", "string", "Armor string, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::armor_str);
}

ADE_VIRTVAR(ManufacturerString, l_Shipclass, "string", "Ship class manufacturer", "string", "Manufacturer, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::manufacturer_str);
}

ADE_VIRTVAR(LengthString, l_Shipclass, "string", "Ship class length", "string", "Length, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::ship_length);
}

ADE_VIRTVAR(GunMountsString, l_Shipclass, "string", "Ship class gun mounts", "string", "Gun mounts, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::gun_mounts);
}

ADE_VIRTVAR(MissileBanksString, l_Shipclass, "string", "Ship class missile banks", "string", "Missile banks, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::missile_banks);
}

ADE_VIRTVAR(VelocityString, l_Shipclass, "string", "Ship class velocity", "string", "velocity, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Velocity is not supported");
	}

	char str[100];
	sprintf(str, XSTR("%d m/s", 743), fl2i((float)sip->max_vel.xyz.z * Hud_speed_multiplier));

	return ade_set_args(L, "s", str);
}

ADE_VIRTVAR(Description, l_Shipclass, "string", "Ship class description", "string", "Description, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::desc);
}

ADE_VIRTVAR(SelectIconFilename, l_Shipclass, "string", "Ship class select icon filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if (!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Select Icon is not supported");
	}

	return ade_set_args(L, "s", Ship_info[idx].icon_filename);
}

ADE_VIRTVAR(SelectAnimFilename, l_Shipclass, "string", "Ship class select animation filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if (!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Select Anim is not supported");
	}

	return ade_set_args(L, "s", Ship_info[idx].anim_filename);
}

ADE_VIRTVAR(SelectOverheadFilename, l_Shipclass, "string", "Ship class select overhead filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if (!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Select Overhead Image is not supported");
	}

	return ade_set_args(L, "s", Ship_info[idx].overhead_filename);
}

ADE_VIRTVAR(TechDescription, l_Shipclass, "string", "Ship class tech description", "string", "Tech description, or empty string if handle is invalid")
{
	return handle_ship_class_optional_string(L, &ship_info::tech_desc);
}

ADE_VIRTVAR(numPrimaryBanks,
	l_Shipclass,
	nullptr,
	"Number of primary banks on this ship class",
	"number",
	"number of banks or nil is ship handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	ship_info* sip = &Ship_info[idx];

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting number of banks is not supported");
	}

	return ade_set_args(L, "i", sip->num_primary_banks);
}

ADE_FUNC(getPrimaryBankCapacity,
	l_Shipclass,
	"number index",
	"Returns the capacity of the specified primary bank",
	"number",
	"The bank capacity or nil if the index is invalid")
{
	int shipIdx;
	int idx;
	if (!ade_get_args(L, "oi", l_Shipclass.Get(&shipIdx), &idx))
		return ADE_RETURN_NIL;

	ship_info* sip = &Ship_info[shipIdx];

	if (idx < 1 || idx > sip->num_primary_banks) {
		return ADE_RETURN_NIL;
	};

	idx--; // Convert from Lua's 1 based index system

	return ade_set_args(L, "i", sip->primary_bank_ammo_capacity[idx]);
}

ADE_VIRTVAR(numSecondaryBanks,
	l_Shipclass,
	nullptr,
	"Number of secondary banks on this ship class",
	"number",
	"number of banks or nil is ship handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	ship_info* sip = &Ship_info[idx];

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting number of banks is not supported");
	}

	return ade_set_args(L, "i", sip->num_secondary_banks);
}

ADE_FUNC(getSecondaryBankCapacity,
	l_Shipclass,
	"number index",
	"Returns the capacity of the specified secondary bank",
	"number",
	"The bank capacity or nil if the index is invalid")
{
	int shipIdx;
	int idx;
	if (!ade_get_args(L, "oi", l_Shipclass.Get(&shipIdx), &idx))
		return ADE_RETURN_NIL;

	ship_info* sip = &Ship_info[shipIdx];

	if (idx < 1 || idx > sip->num_secondary_banks) {
		return ADE_RETURN_NIL;
	};

	idx--; // Convert from Lua's 1 based index system

	return ade_set_args(L, "i", sip->secondary_bank_ammo_capacity[idx]);
}

ADE_VIRTVAR(defaultPrimaries,
	l_Shipclass,
	"number",
	"Array of default primary weapons",
	"default_primary",
	"The weapons array or nil if handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		LuaError(L, "This property is read only.");
	}

	return ade_set_args(L, "o", l_Default_Primary.Set(idx));
}

ADE_VIRTVAR(defaultSecondaries,
	l_Shipclass,
	"number",
	"Array of default secondary weapons",
	"default_secondary",
	"The weapons array or nil if handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		LuaError(L, "This property is read only.");
	}

	return ade_set_args(L, "o", l_Default_Secondary.Set(idx));
}

ADE_FUNC(isWeaponAllowedOnShip,
	l_Shipclass,
	"number index, [number bank]",
	"Gets whether or not a weapon is allowed on a ship class. "
	"Optionally check a specific bank. Banks are 1 to a maximum of 7 where the first banks are Primaries and rest are "
	"Secondaries. "
	"Exact numbering depends on the ship class being checked. Note also that this will consider dogfight weapons only "
	"if "
	"a dogfight mission has been loaded. Index is index into Weapon Classes.",
	"boolean",
	"True if allowed, false if not.")
{
	int idx;
	int wepidx;
	int bank = 0;
	if (!ade_get_args(L, "oi|i", l_Shipclass.Get(&idx), &wepidx, &bank))
		return ade_set_error(L, "b", false);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "b", false);
	wepidx--; // Convert from Lua
	if (wepidx < 0 || wepidx >= weapon_info_size())
		return ade_set_error(L, "b", false);
	if (bank != 0) {
		if (bank < 0 || bank > (Ship_info[idx].num_primary_banks + Ship_info[idx].num_secondary_banks))
			return ade_set_error(L, "b", false);
	};
	bank--; // Convert from Lua

	bool retv = false;
	if ((bank >= 0) && (eval_weapon_flag_for_game_type(Ship_info[idx].restricted_loadout_flag[bank]))) {
		if (eval_weapon_flag_for_game_type(Ship_info[idx].allowed_bank_restricted_weapons[bank][wepidx]))
			retv = true;
	} else if (eval_weapon_flag_for_game_type(Ship_info[idx].allowed_weapons[wepidx])) {
		retv = true;
	}

	return ade_set_args(L, "b", retv);
}

ADE_VIRTVAR(AfterburnerFuelMax, l_Shipclass, "number", "Afterburner fuel capacity", "number", "Afterburner capacity, or 0 if handle is invalid")
{
	int idx;
	float fuel = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &fuel))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && fuel >= 0.0f)
		Ship_info[idx].afterburner_fuel_capacity = fuel;

	return ade_set_args(L, "f", Ship_info[idx].afterburner_fuel_capacity);
}

ADE_VIRTVAR(ScanTime, l_Shipclass, nullptr, "Ship scan time", "number", "Time required to scan, or 0 if handle is invalid. This property is read-only")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "i", 0);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting ScanTime is not supported");
	}

	return ade_set_args(L, "i", Ship_info[idx].scan_time);
}

ADE_VIRTVAR(CountermeasureClass, l_Shipclass, "weaponclass", "The default countermeasure class assigned to this ship class", "weaponclass", "Countermeasure hardpoint weapon class, or invalid weaponclass handle if no countermeasure class or ship handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	ship_info* sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting CountermeasureClass is not supported");
	}

	if (sip->cmeasure_type > -1) {
		return ade_set_args(L, "o", l_Weaponclass.Set(sip->cmeasure_type));
	} else {
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));
	}
}

ADE_VIRTVAR(CountermeasuresMax, l_Shipclass, "number", "Maximum number of countermeasures the ship can carry", "number", "Countermeasure capacity, or 0 if handle is invalid")
{
	int idx;
	int i = -1;
	if(!ade_get_args(L, "o|i", l_Shipclass.Get(&idx), &i))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR && i > -1) {
		Ship_info[idx].cmeasure_max = i;
	}

	return ade_set_args(L, "i", Ship_info[idx].cmeasure_max);
}

ADE_VIRTVAR(Model, l_Shipclass, "model", "Model", "model", "Ship class model, or invalid model handle if shipclass handle is invalid")
{
	int ship_info_idx=-1;
	model_h *mdl = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(model_h(-1)));

	if(ship_info_idx < 0 || ship_info_idx >= ship_info_size())
		return ade_set_error(L, "o", l_Model.Set(model_h(-1)));

	ship_info *sip = &Ship_info[ship_info_idx];

	int mid = (mdl ? mdl->GetID() : -1);

	if(ADE_SETTING_VAR && mid > -1) {
		sip->model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(sip->model_num)));
}

ADE_VIRTVAR(CockpitModel, l_Shipclass, "model", "Model used for first-person cockpit", "model", "Cockpit model")
{
	int ship_info_idx=-1;
	model_h *mdl = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	if(ship_info_idx < 0 || ship_info_idx >= ship_info_size())
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	ship_info *sip = &Ship_info[ship_info_idx];

	int mid = (mdl ? mdl->GetID() : -1);

	if(ADE_SETTING_VAR) {
		sip->cockpit_model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(sip->cockpit_model_num)));
}

ADE_VIRTVAR(CockpitDisplays, l_Shipclass, "cockpitdisplays", "Gets the cockpit display information array of this ship class", "cockpitdisplays", "Array handle containing the information or invalid handle on error")
{
	int ship_info_idx=-1;
	cockpit_displays_info_h *cdih = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_CockpitDisplayInfos.GetPtr(&cdih)))
		return ade_set_error(L, "o", l_CockpitDisplayInfos.Set(cockpit_displays_info_h()));

	if(ship_info_idx < 0 || ship_info_idx >= ship_info_size())
		return ade_set_error(L, "o", l_CockpitDisplayInfos.Set(cockpit_displays_info_h()));

	if(ADE_SETTING_VAR) {
		LuaError(L, "Attempted to use incomplete feature: Cockpit display information copy");
	}

	return ade_set_args(L, "o", l_CockpitDisplayInfos.Set(cockpit_displays_info_h(ship_info_idx)));
}

ADE_VIRTVAR(HitpointsMax, l_Shipclass, "number", "Ship class hitpoints", "number", "Hitpoints, or 0 if handle is invalid")
{
	int idx;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && f >= 0.0f) {
		Ship_info[idx].max_hull_strength = f;
	}

	return ade_set_args(L, "f", Ship_info[idx].max_hull_strength);
}

ADE_VIRTVAR(ShieldHitpointsMax, l_Shipclass, nullptr, "Ship class shield hitpoints", "number", "Shield hitpoints, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Shield Max Hitpoints is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].max_shield_strength);
}

ADE_VIRTVAR(Species, l_Shipclass, "species", "Ship class species", "species", "Ship class species, or invalid species handle if shipclass handle is invalid")
{
	int idx;
	int sidx = -1;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Species.Get(&sidx)))
		return ade_set_error(L, "o", l_Species.Set(-1));

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "o", l_Species.Set(-1));

	if(ADE_SETTING_VAR && sidx > -1 && sidx < (int)Species_info.size()) {
		Ship_info[idx].species = sidx;
	}

	return ade_set_args(L, "o", l_Species.Set(Ship_info[idx].species));
}

ADE_VIRTVAR(Type, l_Shipclass, "shiptype", "Ship class type", "shiptype", "Ship type, or invalid handle if shipclass handle is invalid")
{
	int idx;
	int sidx = -1;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Shiptype.Get(&sidx)))
		return ade_set_error(L, "o", l_Shiptype.Set(-1));

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "o", l_Shiptype.Set(-1));

	if(ADE_SETTING_VAR && sidx > -1 && sidx < (int)Ship_types.size()) {
		Ship_info[idx].class_type = sidx;
	}

	return ade_set_args(L, "o", l_Shiptype.Set(Ship_info[idx].class_type));
}

ADE_VIRTVAR(AltName, l_Shipclass, "string", "Alternate name for ship class", "string", "Alternate string or empty string if handle is invalid")
{
	const char* newName = nullptr;
	int idx;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &newName))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && newName != NULL) {
		if (strlen(newName) >= NAME_LENGTH)
		{
			LuaError(L, "Cannot set alternate name value to '%s' because it is too long, maximum length is %d!", newName, NAME_LENGTH - 1);
			return ade_set_error(L, "s", "");
		}

		strcpy_s(Ship_info[idx].display_name, newName);
	}

	return ade_set_args(L, "s", Ship_info[idx].display_name);
}

ADE_VIRTVAR(VelocityMax, l_Shipclass, "vector", "Ship's lateral and forward speeds", "vector", "Maximum velocity, or null vector if handle is invalid")
{
	int idx;
	vec3d* vec = NULL;
	if (!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Vector.GetPtr(&vec)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting VelocityMax is not supported");
	}

	return ade_set_args(L, "o", l_Vector.Set(Ship_info[idx].max_vel));
}

ADE_VIRTVAR(VelocityDamping, l_Shipclass, "number", "Damping, the natural period (1 / omega) of the dampening effects on top of the acceleration model. ", "number", "Damping, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting VelocityDamping is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].damp);
}

ADE_VIRTVAR(RearVelocityMax, l_Shipclass, "number", "The maximum rear velocity of the ship", "number", "Speed, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting RearVelocityMax is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].max_rear_vel);
}

ADE_VIRTVAR(ForwardAccelerationTime, l_Shipclass, "number", "Forward acceleration time", "number", "Forward acceleration time, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting ForwardAccelerationTime is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].forward_accel);
}

ADE_VIRTVAR(ForwardDecelerationTime, l_Shipclass, "number", "Forward deceleration time", "number", "Forward deceleration time, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting ForwardDecelerationTime is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].forward_decel);
}

ADE_VIRTVAR(RotationTime, l_Shipclass, "vector", "Maximum rotation time on each axis", "vector", "Full rotation time for each axis, or null vector if handle is invalid")
{
	int idx;
	vec3d* vec = NULL;
	if (!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Vector.GetPtr(&vec)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting RotationTime is not supported");
	}

	return ade_set_args(L, "o", l_Vector.Set(Ship_info[idx].rotation_time));
}

ADE_VIRTVAR(RotationalVelocityDamping, l_Shipclass, "number", "Rotational damping, ie derivative of rotational speed", "number", "Rotational damping, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting RotationalVelocityDamping is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].rotdamp);
}

ADE_VIRTVAR(AfterburnerAccelerationTime, l_Shipclass, "number", "Afterburner acceleration time", "number", "Afterburner acceleration time, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting AfterburnerAccelerationTime is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].afterburner_forward_accel);
}

ADE_VIRTVAR(AfterburnerVelocityMax, l_Shipclass, "vector", "Afterburner max velocity", "vector", "Afterburner max velocity, or null vector if handle is invalid")
{
	int idx;
	vec3d* vec = NULL;
	if (!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Vector.GetPtr(&vec)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting AfterburnerVelocityMax is not supported");
	}

	return ade_set_args(L, "o", l_Vector.Set(Ship_info[idx].afterburner_max_vel));
}

ADE_VIRTVAR(AfterburnerRearVelocityMax, l_Shipclass, "number", "Afterburner maximum rear velocity", "number", "Rear velocity, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting AfterburnerRearVelocityMax is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].afterburner_max_reverse_vel);
}

ADE_VIRTVAR(Score, l_Shipclass, "string", "The score of this ship class", "number", "The score or -1 on invalid ship class")
{
	int idx;
	int new_score;
	if(!ade_get_args(L, "o|i", l_Shipclass.Get(&idx), &new_score))
		return ade_set_error(L, "i", -1);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "i", -1);

	if(ADE_SETTING_VAR) {
		Ship_info[idx].score = new_score;
	}

	return ade_set_args(L, "i", Ship_info[idx].score);
}

ADE_VIRTVAR(InTechDatabase, l_Shipclass, "boolean", "Gets or sets whether this ship class is visible in the tech room", "boolean", "True or false")
{
	int idx;
	bool new_value;
	if (!ade_get_args(L, "o|b", l_Shipclass.Get(&idx), &new_value))
		return ade_set_error(L, "b", false);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "b", false);

	auto flag = (Player && (Player->flags & PLAYER_FLAGS_IS_MULTI))
		? Ship::Info_Flags::In_tech_database_m
		: Ship::Info_Flags::In_tech_database;

	if (ADE_SETTING_VAR) {
		Ship_info[idx].flags.set(flag, new_value);
	}

	return ade_set_args(L, "b", Ship_info[idx].flags[flag]);
}

ADE_VIRTVAR(AllowedInCampaign, l_Shipclass, "boolean", "Gets or sets whether this ship class is allowed in loadouts in campaign mode", "boolean", "True or false")
{
	int idx;
	bool new_value;
	if (!ade_get_args(L, "o|b", l_Shipclass.Get(&idx), &new_value))
		return ade_set_error(L, "b", false);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "b", false);

	if (ADE_SETTING_VAR) {
		Campaign.ships_allowed[idx] = new_value;
	}

	return Campaign.ships_allowed[idx] ? ADE_RETURN_TRUE : ADE_RETURN_FALSE;
}

ADE_VIRTVAR(PowerOutput, l_Shipclass, "number", "Gets or sets a ship class' power output", "number", "The ship class' current power output")
{
	int idx;
	float new_power;
	if (!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &new_power))
		return ade_set_error(L, "f", -1.0f);

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", -1.0f);

	if (ADE_SETTING_VAR) {
		Ship_info[idx].power_output = new_power;
	}

	return ade_set_args(L, "f", Ship_info[idx].power_output);
}

ADE_VIRTVAR(ScanningTimeMultiplier, l_Shipclass, nullptr, "Time multiplier for scans performed by this ship class", "number", "Scanning time multiplier, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting ScanningTimeMultiplier is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].scanning_time_multiplier);
}

ADE_VIRTVAR(ScanningRangeMultiplier, l_Shipclass, nullptr, "Range multiplier for scans performed by this ship class", "number", "Scanning range multiplier, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting ScanningRangeMultiplier is not supported");
	}

	return ade_set_args(L, "f", Ship_info[idx].scanning_range_multiplier);
}

ADE_VIRTVAR(CustomData, l_Shipclass, nullptr, "Gets the custom data table for this ship class", "table", "The ship class's custom data table") 
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;
	
	if(idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	auto table = luacpp::LuaTable::create(L);
	
	ship_info *sip = &Ship_info[idx];

	for (const auto& pair : sip->custom_data)
	{
		table.addValue(pair.first, pair.second);
	}

	return ade_set_args(L, "t", &table);	
}

ADE_FUNC(hasCustomData, l_Shipclass, nullptr, "Detects whether the ship class has any custom data", "boolean", "true if the shipclass's custom_data is not empty, false otherwise") 
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;
	
	if(idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	ship_info *sip = &Ship_info[idx];

	bool result = !sip->custom_data.empty();
	return ade_set_args(L, "b", result);
}

ADE_VIRTVAR(CustomStrings,
	l_Shipclass,
	nullptr,
	"Gets the indexed custom string table for this ship. Each item in the table is a table with the following values: "
	"Name - the name of the custom string, Value - the value associated with the custom string, String - the custom "
	"string itself.",
	"table",
	"The ship's custom data table")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	ship_info* sip = &Ship_info[idx];
	
	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Custom Data is not supported");
	}

	auto table = luacpp::LuaTable::create(L);

	int cnt = 0;

	for (const auto& cs : sip->custom_strings) {
		cnt++;
		auto item = luacpp::LuaTable::create(L);

		item.addValue("Name", luacpp::LuaValue::createValue(Script_system.GetLuaSession(), cs.name));
		item.addValue("Value", luacpp::LuaValue::createValue(Script_system.GetLuaSession(), cs.value));
		item.addValue("String", luacpp::LuaValue::createValue(Script_system.GetLuaSession(), cs.text));

		table.addValue(cnt, item);
	}

	return ade_set_args(L, "t", &table);
}

ADE_FUNC(hasCustomStrings,
	l_Shipclass,
	nullptr,
	"Detects whether the ship has any custom strings",
	"boolean",
	"true if the ship's custom_strings is not empty, false otherwise")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_NIL;

	ship_info* sip = &Ship_info[idx];

	bool result = !sip->custom_strings.empty();
	return ade_set_args(L, "b", result);
}

ADE_FUNC(isValid, l_Shipclass, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(isInTechroom, l_Shipclass, NULL, "Gets whether or not the ship class is available in the techroom", "boolean", "Whether ship has been revealed in the techroom, false if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_error(L, "b", false);

	bool b = false;
	if(Player != NULL && (Player->flags & PLAYER_FLAGS_IS_MULTI) && (Ship_info[idx].flags[Ship::Info_Flags::In_tech_database_m])) {
		b = true;
	} else if(Ship_info[idx].flags[Ship::Info_Flags::In_tech_database]) {
		b = true;
	}

	return ade_set_args(L, "b", b);
}

ADE_FUNC(renderTechModel,
	l_Shipclass,
	"number X1, number Y1, number X2, number Y2, [number RotationPercent =0, number PitchPercent =0, number "
	"BankPercent=40, number Zoom=1.3, boolean Lighting=true, teamcolor TeamColor=nil]",
	"Draws ship model as if in techroom. True for regular lighting, false for flat lighting.",
	"boolean",
	"Whether ship was rendered")
{
	int x1,y1,x2,y2;
	angles rot_angles = {0.0f, 0.0f, 40.0f};
	int idx;
	float zoom = 1.3f;
	bool lighting = true;
	int tc_idx = -1;
	if(!ade_get_args(L, "oiiii|ffffbo", l_Shipclass.Get(&idx), &x1, &y1, &x2, &y2, &rot_angles.h, &rot_angles.p, &rot_angles.b, &zoom, &lighting, l_TeamColor.Get(&tc_idx)))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_args(L, "b", false);

	if(x2 < x1 || y2 < y1)
		return ade_set_args(L, "b", false);

	ship_info* sip = &Ship_info[idx];

	CLAMP(rot_angles.p, 0.0f, 100.0f);
	CLAMP(rot_angles.b, 0.0f, 100.0f);
	CLAMP(rot_angles.h, 0.0f, 100.0f);

	//Handle angles
	matrix orient = vmd_identity_matrix;
	angles view_angles = {-0.6f, 0.0f, 0.0f};
	vm_angles_2_matrix(&orient, &view_angles);

	rot_angles.p = (rot_angles.p*0.01f) * PI2;
	rot_angles.b = (rot_angles.b*0.01f) * PI2;
	rot_angles.h = (rot_angles.h*0.01f) * PI2;
	vm_rotate_matrix_by_angles(&orient, &rot_angles);

	SCP_string tcolor = sip->default_team_name;
	if (SCP_vector_inbounds(Team_Names, tc_idx)) {
		const auto& it = Team_Colors.find(Team_Names[tc_idx]);
		if (it != Team_Colors.end()) {
			tcolor = Team_Names[tc_idx];
		}
	}

	return ade_set_args(L, "b", render_tech_model(TECH_SHIP, x1, y1, x2, y2, zoom, lighting, idx, &orient, tcolor));
}

// Nuke's alternate tech model rendering function
ADE_FUNC(renderTechModel2, l_Shipclass, "number X1, number Y1, number X2, number Y2, [orientation Orientation=nil, number Zoom=1.3, teamcolor TeamColor=nil]", "Draws ship model as if in techroom", "boolean", "Whether ship was rendered")
{
	int x1,y1,x2,y2;
	int idx;
	float zoom = 1.3f;
	matrix_h *mh = nullptr;
	int tc_idx = -1;
	if(!ade_get_args(L, "oiiiio|fo", l_Shipclass.Get(&idx), &x1, &y1, &x2, &y2,  l_Matrix.GetPtr(&mh), &zoom, l_TeamColor.Get(&tc_idx)))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_args(L, "b", false);

	if(x2 < x1 || y2 < y1)
		return ade_set_args(L, "b", false);

	ship_info* sip = &Ship_info[idx];

	//Handle angles
	matrix *orient = mh->GetMatrix();

	SCP_string tcolor = sip->default_team_name;
	if (SCP_vector_inbounds(Team_Names, tc_idx)) {
		const auto& it = Team_Colors.find(Team_Names[tc_idx]);
		if (it != Team_Colors.end()) {
			tcolor = Team_Names[tc_idx];
		}
	}

	return ade_set_args(L, "b", render_tech_model(TECH_SHIP, x1, y1, x2, y2, zoom, true, idx, orient, tcolor));
}

ADE_FUNC(renderSelectModel,
	l_Shipclass,
	"boolean restart, number x, number y, [number width = 629, number height = 355, number currentEffectSetting = default, number zoom = 1.3, teamcolor TeamColor=nil]",
	"Draws the 3D select ship model with the chosen effect at the specified coordinates. Restart should "
	"be true on the first frame this is called and false on subsequent frames. Valid selection effects are 1 (fs1) or 2 (fs2), "
	"defaults to the mod setting or the model's setting. Zoom is a multiplier to the model's closeup_zoom value.",
	"boolean",
	"true if rendered, false if error")
{
	int idx;
	bool restart;
	int x1;
	int y1;
	int x2 = 629;
	int y2 = 355;
	int effect = -1;
	float zoom = 1.3f;
	int tc_idx = -1;
	if (!ade_get_args(L, "obii|iiifo", l_Shipclass.Get(&idx), &restart, &x1, &y1, &x2, &y2, &effect, &zoom, l_TeamColor.Get(&tc_idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_args(L, "b", false);

	if (effect == 0 || effect > 2) {
		LuaError(L, "Valid effect values are 1 or 2, got %i", effect);
		return ade_set_args(L, "b", false);
	}

	if (restart) {
		anim_timer_start = timer_get_milliseconds();
	}

	ship_info* sip = &Ship_info[idx];
	
	if (effect == -1) {
		effect = sip->selection_effect;
	}

	float rev_rate = REVOLUTION_RATE;
	if (sip->is_big_ship()) {
		rev_rate *= 1.7f;
	}
	if (sip->is_huge_ship()) {
		rev_rate *= 3.0f;
	}

	int modelNum = model_load(sip->pof_file, sip);
	static float ShipRot = 0.0f;

	model_render_params render_info;

	if (sip->uses_team_colors) {
		SCP_string tcolor = sip->default_team_name;

		if (SCP_vector_inbounds(Team_Names, tc_idx)) {
			const auto& it = Team_Colors.find(Team_Names[tc_idx]);
			if (it != Team_Colors.end()) {
				tcolor = Team_Names[tc_idx];
			}
		}
		render_info.set_team_color(tcolor, "none", 0, 0);
	}

	if (sip->replacement_textures.size() > 0) {
		render_info.set_replacement_textures(modelNum, sip->replacement_textures);
	}

	draw_model_rotating(&render_info,
		modelNum,
		x1,
		y1,
		x2,
		y2,
		&ShipRot,
		&sip->closeup_pos,
		sip->closeup_zoom * zoom,
		rev_rate,
		MR_AUTOCENTER | MR_NO_FOGGING,
		GR_RESIZE_NONE,
		effect);

	return ade_set_args(L, "b", true);
}

ADE_FUNC(renderOverheadModel,
	l_Shipclass,
	"number x, number y, [number width = 467, number height = 362, number|table /* selectedSlot = -1 or empty table */, number selectedWeapon = -1, number hoverSlot = -1, "
	"number bank1_x = 170, number bank1_y = 203, number bank2_x = 170, number bank2_y = 246, number bank3_x = 170, number bank3_y = 290, "
	"number bank4_x = 552, number bank4_y = 203, number bank5_x = 552, number bank5_y = 246, number bank6_x = 552, number bank6_y = 290, "
	"number bank7_x = 552, number bank7_y = 333, number style = 0, teamcolor TeamColor=nil]",
	"Draws the 3D overhead ship model with the lines pointing from bank weapon selections to bank firepoints. SelectedSlot refers to loadout "
	"ship slots 1-12 where wing 1 is 1-4, wing 2 is 5-8, and wing 3 is 9-12. SelectedWeapon is the index into weapon classes. HoverSlot refers "
	"to the bank slots 1-7 where 1-3 are primaries and 4-6 are secondaries. Lines will be drawn from any bank containing the SelectedWeapon to "
	"the firepoints on the model of that bank. Similarly, lines will be drawn from the bank defined by HoverSlot to the firepoints on the model "
	"of that slot. Line drawing for HoverSlot takes precedence over line drawing for SelectedWeapon. Set either or both to -1 to stop line drawing. "
	"The bank coordinates are the coordinates from which the lines for that bank will be drawn. It is expected that primary slots will be on the "
	"left of the ship model and secondaries will be on the right. The lines have a hard-coded curve expecing to be drawn from those directions. "
	"Style can be 0 or 1. 0 for the ship to be drawn stationary from top down, 1 for the ship to be rotating.",
	"boolean",
	"true if rendered, false if error")
{
	int idx;
	int x1;
	int y1;
	int x2 = 467;
	int y2 = 362;
	int selectedSlot = -1;
	auto weapon_table = luacpp::LuaTable::create(L);
	int selectedWeapon = -1;
	int hoverSlot = -1;

	//Bank coords
	int bank1_x = 170;
	int bank1_y = 203;
	int bank2_x = 170;
	int bank2_y = 246;
	int bank3_x = 170;
	int bank3_y = 290;
	int bank4_x = 552;
	int bank4_y = 203;
	int bank5_x = 552;
	int bank5_y = 246;
	int bank6_x = 552;
	int bank6_y = 290;
	int bank7_x = 552;
	int bank7_y = 333;
	int style = 0;

	int weapon_list[MAX_SHIP_WEAPONS] = {-1, -1, -1, -1, -1, -1, -1};

	int tc_idx = -1;

	if (lua_isnumber(L, 6)) {

		if (!ade_get_args(L,
				"oii|iiiiiiiiiiiiiiiiiiiio",
				l_Shipclass.Get(&idx),
				&x1,
				&y1,
				&x2,
				&y2,
				&selectedSlot,
				&selectedWeapon,
				&hoverSlot,
				&bank1_x,
				&bank1_y,
				&bank2_x,
				&bank2_y,
				&bank3_x,
				&bank3_y,
				&bank4_x,
				&bank4_y,
				&bank5_x,
				&bank5_y,
				&bank6_x,
				&bank6_y,
				&bank7_x,
				&bank7_y,
				&style,
				l_TeamColor.Get(&tc_idx)))
			return ADE_RETURN_NIL;

		// Convert this from the Lua index
		selectedSlot--;

		if (selectedSlot < 0)
			return ADE_RETURN_FALSE;

		for (int i = 0; i < (MAX_SHIP_WEAPONS); i++) {
			weapon_list[i] = Wss_slots[selectedSlot].wep[i];
		}
	} else {
		if (!ade_get_args(L,
				"oii|iitiiiiiiiiiiiiiiiiio",
				l_Shipclass.Get(&idx),
				&x1,
				&y1,
				&x2,
				&y2,
				&weapon_table,
				&selectedWeapon,
				&hoverSlot,
				&bank1_x,
				&bank1_y,
				&bank2_x,
				&bank2_y,
				&bank3_x,
				&bank3_y,
				&bank4_x,
				&bank4_y,
				&bank5_x,
				&bank5_y,
				&bank6_x,
				&bank6_y,
				&bank7_x,
				&bank7_y,
				&style,
				l_TeamColor.Get(&tc_idx)))
			return ADE_RETURN_NIL;

		int count = 0;
		if (weapon_table.isValid()) {
			for (const auto& item : weapon_table) {
				if (item.second.is(luacpp::ValueType::NUMBER)) {
					// This'll lua-error internally if it's not fed only numbers. Additionally, catch the lua exception
					// and then carry on
					try {
						int wep = item.second.getValue<int>();
						wep--; // convert from the lua index
						weapon_list[count++] = wep;
					} catch (const luacpp::LuaException& /*e*/) {
						// We were likely fed a userdata that was not a number.
						// Since we can't actually tell whether that's the case before we try to get the value, and the
						// attempt to get the value is printing a LuaError itself, just eat the exception here and
						// return
						return ADE_RETURN_FALSE;
					}
				} else {
					// This happens on a non-userdata value, i.e. a string
					LuaError(L, "Weapon index table contained non-number values! Aborting...");
					return ADE_RETURN_FALSE;
				}
			}
		}
	}

	if (idx < 0 || idx >= ship_info_size())
		return ade_set_args(L, "b", false);

	//Convert these from Lua indecies
	selectedWeapon--;
	hoverSlot--;

	if ((style < 0) || (style > 1))
		LuaError(L, "Overhead style can only be 0 or 1!");

	if (selectedWeapon < 0 || selectedWeapon >= weapon_info_size())
		selectedWeapon = -1;

	if (hoverSlot < 0 || hoverSlot >= (MAX_SHIP_PRIMARY_BANKS + MAX_SHIP_SECONDARY_BANKS))
		hoverSlot = -1;

	ship_info* sip = &Ship_info[idx];

	SCP_string tcolor = sip->default_team_name;
	if (SCP_vector_inbounds(Team_Names, tc_idx)) {
		const auto& it = Team_Colors.find(Team_Names[tc_idx]);
		if (it != Team_Colors.end()) {
			tcolor = Team_Names[tc_idx];
		}
	}

	int modelNum = model_load(sip->pof_file, sip);
	model_page_in_textures(modelNum, idx);
	static float ShipRot = 0.0f;

	draw_3d_overhead_view(modelNum,
		idx,
		&ShipRot,
		flFrametime,
		weapon_list,
		selectedWeapon,
		hoverSlot,
		x1,
		y1,
		x2,
		y2,
		GR_RESIZE_NONE,
		bank1_x,
		bank1_y,
		bank2_x,
		bank2_y,
		bank3_x,
		bank3_y,
		bank4_x,
		bank4_y,
		bank5_x,
		bank5_y,
		bank6_x,
		bank6_y,
		bank7_x,
		bank7_y,
		0,
		0,
		0,
		(overhead_style)style,
		tcolor);

	return ade_set_args(L, "b", true);
}

ADE_FUNC(isModelLoaded, l_Shipclass, "[boolean Load = false]", "Checks if the model used for this shipclass is loaded or not and optionally loads the model, which might be a slow operation.", "boolean", "If the model is loaded or not")
{
	int idx;
	bool load_check = false;
	if(!ade_get_args(L, "o|b", l_Shipclass.Get(&idx), &load_check))
		return ADE_RETURN_FALSE;

	ship_info *sip = &Ship_info[idx];

	if (sip == NULL)
		return ADE_RETURN_FALSE;

	if(load_check){
		sip->model_num = model_load(sip->pof_file, sip);
	}

	if (sip->model_num > -1)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isPlayerAllowed,
	l_Shipclass,
	nullptr,
	"Detects whether the ship has the player allowed flag",
	"boolean",
	"true if player allowed, false otherwise, nil if a syntax/type error occurs")
{
	int idx;
	if (!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= ship_info_size())
		return ADE_RETURN_FALSE;

	ship_info* sip = &Ship_info[idx];

	if (sip->flags[Ship::Info_Flags::Player_ship]) {
		return ADE_RETURN_TRUE;
	}

	return ADE_RETURN_FALSE;
}

ADE_FUNC(getShipClassIndex, l_Shipclass, nullptr, "Gets the index value of the ship class", "number", "index value of the ship class")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_args(L, "i", -1);

	if(idx < 0 || idx >= ship_info_size())
		return ade_set_args(L, "i", -1);

	return ade_set_args(L, "i", idx + 1); // Lua is 1-based
}


}
}
