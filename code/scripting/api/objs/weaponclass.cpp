//
//

#include "weaponclass.h"
#include "model.h"
#include "weapon/weapon.h"
#include "graphics/matrix.h"
#include "vecmath.h"
#include "mission/missioncampaign.h"
#include "missionui/missionscreencommon.h"
#include "model/modelrender.h"
#include "utils/string_utils.h"

namespace scripting {
namespace api {


//**********HANDLE: Weaponclass
ADE_OBJ(l_Weaponclass, int, "weaponclass", "Weapon class handle");

ADE_FUNC(__tostring, l_Weaponclass, NULL, "Weapon class name", "string", "Weapon class name, or an empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Weapon_info[idx].get_display_name());
}

ADE_FUNC(__eq, l_Weaponclass, "weaponclass, weaponclass", "Checks if the two classes are equal", "boolean", "true if equal, false otherwise")
{
	int idx1,idx2;
	if(!ade_get_args(L, "oo", l_Weaponclass.Get(&idx1), l_Weaponclass.Get(&idx2)))
		return ade_set_error(L, "b", false);

	if(idx1 < 0 || idx1 >= weapon_info_size())
		return ade_set_error(L, "b", false);

	if(idx2 < 0 || idx2 >= weapon_info_size())
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", idx1 == idx2);
}

ADE_VIRTVAR(Name, l_Weaponclass, "string", "Weapon class name. This is the possibly untranslated name. Use tostring(class) to get the string which should be shown to the user.", "string", "Weapon class name, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != nullptr) {
		auto len = sizeof(Weapon_info[idx].name);
		strncpy(Weapon_info[idx].name, s, len);
		Weapon_info[idx].name[len - 1] = 0;
	}

	return ade_set_args(L, "s", Weapon_info[idx].name);
}

ADE_VIRTVAR(AltName, l_Weaponclass, "string", "The alternate weapon class name.", "string", "Alternate weapon class name, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != nullptr) {
		auto len = sizeof(Weapon_info[idx].display_name);
		strncpy(Weapon_info[idx].display_name, s, len);
		Weapon_info[idx].display_name[len - 1] = 0;
	}

	return ade_set_args(L, "s", Weapon_info[idx].display_name);
}

ADE_VIRTVAR(TurretName, l_Weaponclass, "string", "The name displayed for a turret if the turret's first weapon is this weapon class.", "string", "Turret name (aka alternate subsystem name), or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != nullptr) {
		auto len = sizeof(Weapon_info[idx].altSubsysName);
		strncpy(Weapon_info[idx].altSubsysName, s, len);
		Weapon_info[idx].altSubsysName[len - 1] = 0;
	}

	return ade_set_args(L, "s", Weapon_info[idx].altSubsysName);
}

ADE_VIRTVAR(Title, l_Weaponclass, "string", "Weapon class title", "string", "Weapon class title, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != nullptr) {
		auto len = sizeof(Weapon_info[idx].title);
		strncpy(Weapon_info[idx].title, s, len);
		Weapon_info[idx].title[len - 1] = 0;
	}

	return ade_set_args(L, "s", Weapon_info[idx].title);
}

ADE_VIRTVAR(Description, l_Weaponclass, "string", "Weapon class description string", "string", "Description string, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	weapon_info *wip = &Weapon_info[idx];

	if(ADE_SETTING_VAR) {
		wip->desc = util::unique_copy(s, true);
	}

	if(wip->desc != nullptr)
		return ade_set_args(L, "s", wip->desc.get());
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(TechTitle, l_Weaponclass, "string", "Weapon class tech title", "string", "Tech title, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != nullptr) {
		auto len = sizeof(Weapon_info[idx].tech_title);
		strncpy(Weapon_info[idx].tech_title, s, len);
		Weapon_info[idx].tech_title[len - 1] = 0;
	}

	return ade_set_args(L, "s", Weapon_info[idx].tech_title);
}

ADE_VIRTVAR(TechAnimationFilename, l_Weaponclass, "string", "Weapon class animation filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != nullptr) {
		auto len = sizeof(Weapon_info[idx].tech_anim_filename);
		strncpy(Weapon_info[idx].tech_anim_filename, s, len);
		Weapon_info[idx].tech_anim_filename[len - 1] = 0;
	}

	return ade_set_args(L, "s", Weapon_info[idx].tech_anim_filename);
}

ADE_VIRTVAR(SelectIconFilename, l_Weaponclass, "string", "Weapon class select icon filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Select Icon is not supported");
	}

	return ade_set_args(L, "s", Weapon_info[idx].icon_filename);
}

ADE_VIRTVAR(SelectAnimFilename, l_Weaponclass, "string", "Weapon class select animation filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Select Anim is not supported");
	}

	return ade_set_args(L, "s", Weapon_info[idx].anim_filename);
}

ADE_VIRTVAR(TechDescription, l_Weaponclass, "string", "Weapon class tech description string", "string", "Description string, or empty string if handle is invalid")
{
	int idx;
	const char* s = nullptr;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "s", "");

	weapon_info *wip = &Weapon_info[idx];

	if(ADE_SETTING_VAR) {
		wip->tech_desc = util::unique_copy(s, true);
	}

	if(wip->tech_desc != nullptr)
		return ade_set_args(L, "s", wip->tech_desc.get());
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(Model, l_Weaponclass, "model", "Model", "model", "Weapon class model, or invalid model handle if weaponclass handle is invalid")
{
	int weapon_info_idx=-1;
	model_h *mdl = nullptr;
	if(!ade_get_args(L, "o|o", l_Weaponclass.Get(&weapon_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(model_h(-1)));

	if(weapon_info_idx < 0 || weapon_info_idx >= weapon_info_size())
		return ade_set_error(L, "o", l_Model.Set(model_h(-1)));

	weapon_info *wip = &Weapon_info[weapon_info_idx];

	int mid = (mdl ? mdl->GetID() : -1);

	if(ADE_SETTING_VAR && mid > -1) {
		wip->model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(wip->model_num)));
}

ADE_VIRTVAR(ArmorFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship hull (0-1.0)", "number", "Armor factor, or empty string if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].armor_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].armor_factor);
}

ADE_VIRTVAR(Damage, l_Weaponclass, "number", "Amount of damage that weapon deals", "number", "Damage amount, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].damage = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].damage);
}

ADE_VIRTVAR(DamageType, l_Weaponclass, nullptr, nullptr, "number", "Damage Type index, or 0 if handle is invalid. Index is index into armor.tbl")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "i", 0);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Damage Type is not supported");
	}

	//Convert to Lua's 1 based index here
	return ade_set_args(L, "i", Weapon_info[idx].damage_type_idx_sav + 1);
}

ADE_VIRTVAR(FireWait, l_Weaponclass, "number", "Weapon fire wait (cooldown time) in seconds", "number", "Fire wait time, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		if (Weapon_info[idx].wi_flags[Weapon::Info_Flags::Cmeasure]) {
			LuaError(L, "Setting Countermeasure Fire Wait is not supported");
		} else {
			Weapon_info[idx].fire_wait = f;
		}
	}

	float fw = Weapon_info[idx].fire_wait;
	if (Weapon_info[idx].wi_flags[Weapon::Info_Flags::Cmeasure]) {
		fw = static_cast<float>(Weapon_info[idx].cmeasure_firewait) / 1000.0f;
	}

	return ade_set_args(L, "f", fw);
}

ADE_VIRTVAR(FreeFlightTime, l_Weaponclass, "number", "The time the weapon will fly before turing onto its target", "number", "Free flight time or empty string if invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].free_flight_time = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].free_flight_time);
}

static int swarm_info_helper(lua_State* L)
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Swarm Info is not supported");
	}

	bool flag = false;
	if (Weapon_info[idx].wi_flags[Weapon::Info_Flags::Swarm])
		flag = true;

	return ade_set_args(L, "bii", flag, Weapon_info[idx].swarm_count, Weapon_info[idx].SwarmWait);
}

ADE_VIRTVAR_DEPRECATED(SwarmInfo, l_Weaponclass, nullptr, nullptr, "boolean",
	"Returns whether the weapon has the swarm flag, or nil if the handle is invalid.",
	gameversion::version(24, 2), "Deprecated in favor of weaponclass:getSwarmInfo(), since virtvars can only return a single value.")
{
	return swarm_info_helper(L);
}

ADE_FUNC(getSwarmInfo, l_Weaponclass, nullptr, nullptr, "boolean, number, number",
	"Returns three values: a) whether the weapon has the swarm flag, b) the number of swarm missiles fired, c) the swarm wait. Returns nil if the handle is invalid.")
{
	return swarm_info_helper(L);
}

static int corkscrew_info_helper(lua_State* L)
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Corkscrew Info is not supported");
	}

	bool crotate = false;
	if (Weapon_info[idx].cs_crotate)
		crotate = true;

	bool flag = false;
	if (Weapon_info[idx].wi_flags[Weapon::Info_Flags::Corkscrew])
		flag = true;

	return ade_set_args(L,
		"bifibf",
		flag,
		Weapon_info[idx].cs_num_fired,
		Weapon_info[idx].cs_radius,
		Weapon_info[idx].cs_delay,
		crotate,
		Weapon_info[idx].cs_twist);
}

ADE_VIRTVAR_DEPRECATED(CorkscrewInfo, l_Weaponclass, nullptr, nullptr, "boolean, number, number, number, boolean, number",
	"Returns whether the weapon has the corkscrew flag, or nil if the handle is invalid.",
	gameversion::version(24, 2), "Deprecated in favor of weaponclass:getCorkscrewInfo(), since virtvars can only return a single value.")
{
	return corkscrew_info_helper(L);
}

ADE_FUNC(getCorkscrewInfo, l_Weaponclass, nullptr, nullptr, "boolean, number, number, number, boolean, number",
	"Returns six values: a) whether the weapon has the corkscrew flag, b) the number of corkscrew missiles fired, c) the radius, d) the fire delay, e) whether the weapon counter-rotations, f) the twist value. Returns nil if the handle is invalid.")
{
	return corkscrew_info_helper(L);
}

ADE_VIRTVAR(LifeMax, l_Weaponclass, "number", "Life of weapon in seconds", "number", "Life of weapon, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].lifetime = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].lifetime);
}

ADE_VIRTVAR(Range, l_Weaponclass, "number", "Range of weapon in meters", "number", "Weapon Range, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].weapon_range = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].weapon_range);
}

ADE_VIRTVAR(Mass, l_Weaponclass, "number", "Weapon mass", "number", "Weapon mass, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].mass = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].mass);
}

ADE_VIRTVAR(ShieldFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship shields (0-1.0)", "number", "Shield damage factor, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].shield_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].shield_factor);
}

ADE_VIRTVAR(SubsystemFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship subsystems (0-1.0)", "number", "Subsystem damage factor, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].subsystem_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].subsystem_factor);
}

ADE_VIRTVAR(TargetLOD, l_Weaponclass, "number", "LOD used for weapon model in the targeting computer", "number", "LOD number, or 0 if handle is invalid")
{
	int idx;
	int lod = 0;
	if(!ade_get_args(L, "o|i", l_Weaponclass.Get(&idx), &lod))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].hud_target_lod = lod;
	}

	return ade_set_args(L, "i", Weapon_info[idx].hud_target_lod);
}

ADE_VIRTVAR(Speed, l_Weaponclass, "number", "Weapon max speed, aka $Velocity in weapons.tbl", "number", "Weapon speed, or 0 if handle is invalid")
{
	int idx;
	float spd = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &spd))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].max_speed = spd;
	}

	return ade_set_args(L, "f", Weapon_info[idx].max_speed);
}

ADE_VIRTVAR(EnergyConsumed, l_Weaponclass, nullptr, nullptr, "number", "Energy Consumed, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Energy Consumed is not supported");
	}

	return ade_set_args(L, "f", Weapon_info[idx].energy_consumed);
}

ADE_VIRTVAR(ShockwaveDamage, l_Weaponclass, "number", "Damage the shockwave is set to if damage is overridden", "number", "Shockwave Damage if explicitly specified via table, or -1 if unspecified. Returns nil if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", -1.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;

	if(ADE_SETTING_VAR) {
		LuaError(L, "Setting Shockwave Damage is not supported");
	}

	if (Weapon_info[idx].shockwave.damage_overridden) {
		return ade_set_args(L, "f", Weapon_info[idx].shockwave.damage);
	} else {
		return ade_set_args(L, "f", -1.0f);
	}
}


ADE_VIRTVAR(InnerRadius, l_Weaponclass, "number", "Radius at which the full explosion damage is done. Marks the line where damage attenuation begins. Same as $Inner Radius in weapons.tbl", "number", "Inner Radius, or 0 if handle is invalid")
{
	int idx;
	float irad = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &irad))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].shockwave.inner_rad = irad;
	}

	return ade_set_args(L, "f", Weapon_info[idx].shockwave.inner_rad);
}


ADE_VIRTVAR(OuterRadius, l_Weaponclass, "number", "Maximum Radius at which any damage is done with this weapon. Same as $Outer Radius in weapons.tbl", "number", "Outer Radius, or 0 if handle is invalid")
{
	int idx;
	float orad = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &orad))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].shockwave.outer_rad = orad;
	}

	return ade_set_args(L, "f", Weapon_info[idx].shockwave.outer_rad);
}


ADE_VIRTVAR(Bomb, l_Weaponclass, "boolean", "Is weapon class flagged as bomb", "boolean", "New flag")
{
	int idx;
	bool newVal = false;
	if(!ade_get_args(L, "o|b", l_Weaponclass.Get(&idx), &newVal))
		return ADE_RETURN_FALSE;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	weapon_info *info = &Weapon_info[idx];

	if(ADE_SETTING_VAR)
	{
		info->wi_flags.set(Weapon::Info_Flags::Bomb, newVal);
	}


	if (info->wi_flags[Weapon::Info_Flags::Bomb])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(CustomData, l_Weaponclass, nullptr, "Gets the custom data table for this weapon class", "table", "The weapon class's custom data table") 
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;
	
	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;
		
	auto table = luacpp::LuaTable::create(L);
	
	weapon_info *wip = &Weapon_info[idx];

	for (const auto& pair : wip->custom_data)
	{
		table.addValue(pair.first, pair.second);
	}

	return ade_set_args(L, "t", &table);	
}

ADE_FUNC(hasCustomData, l_Weaponclass, nullptr, "Detects whether the weapon class has any custom data", "boolean", "true if the weaponclass's custom_data is not empty, false otherwise") 
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;
	
	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;

	weapon_info *wip = &Weapon_info[idx];

	bool result = !wip->custom_data.empty();
	return ade_set_args(L, "b", result);
}

ADE_VIRTVAR(CustomStrings,
	l_Weaponclass,
	nullptr,
	"Gets the indexed custom string table for this weapon. Each item in the table is a table with the following values: "
	"Name - the name of the custom string, Value - the value associated with the custom string, String - the custom "
	"string itself.",
	"table",
	"The weapon's custom data table")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;

	weapon_info* wip = &Weapon_info[idx];

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Custom Data is not supported");
	}

	auto table = luacpp::LuaTable::create(L);

	int cnt = 0;

	for (const auto& cs : wip->custom_strings) {
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
	l_Weaponclass,
	nullptr,
	"Detects whether the weapon has any custom strings",
	"boolean",
	"true if the weapon's custom_strings is not empty, false otherwise")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;

	weapon_info* wip = &Weapon_info[idx];

	bool result = !wip->custom_strings.empty();
	return ade_set_args(L, "b", result);
}

ADE_VIRTVAR(InTechDatabase, l_Weaponclass, "boolean", "Gets or sets whether this weapon class is visible in the tech room", "boolean", "True or false")
{
	int idx;
	bool new_value;
	if (!ade_get_args(L, "o|b", l_Weaponclass.Get(&idx), &new_value))
		return ade_set_error(L, "b", false);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "b", false);

	if (ADE_SETTING_VAR) {
		Weapon_info[idx].wi_flags.set(Weapon::Info_Flags::In_tech_database, new_value);
	}

	return ade_set_args(L, "b", Weapon_info[idx].wi_flags[Weapon::Info_Flags::In_tech_database]);
}

ADE_VIRTVAR(AllowedInCampaign, l_Weaponclass, "boolean", "Gets or sets whether this weapon class is allowed in loadouts in campaign mode", "boolean", "True or false")
{
	int idx;
	bool new_value;
	if (!ade_get_args(L, "o|b", l_Weaponclass.Get(&idx), &new_value))
		return ade_set_error(L, "b", false);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "b", false);

	if (ADE_SETTING_VAR) {
		Campaign.weapons_allowed[idx] = new_value;
	}

	return Campaign.weapons_allowed[idx] ? ADE_RETURN_TRUE : ADE_RETURN_FALSE;
}

ADE_VIRTVAR(CargoSize, l_Weaponclass, "number", "The cargo size of this weapon class", "number", "The new cargo size or -1 on error")
{
	int idx;
	float newVal = -1.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &newVal))
		return ade_set_args(L, "f", -1.0f);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "f", -1.0f);

	weapon_info *info = &Weapon_info[idx];

	if(ADE_SETTING_VAR)
	{
		if(newVal > 0)
		{
			info->cargo_size = newVal;
		}
		else
		{
			LuaError(L, "Cargo size must be bigger than zero, got %f!", newVal);
		}
	}

	return ade_set_args(L, "f", info->cargo_size);
}

ADE_VIRTVAR(heatEffectiveness,
	l_Weaponclass,
	"number",
	"The heat effectiveness of this weapon class if it's a countermeasure. Otherwise returns -1",
	"number",
	"The heat effectiveness or -1 on error")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_args(L, "f", -1.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "f", -1.0f);

	weapon_info* info = &Weapon_info[idx];

	if (!info->wi_flags[Weapon::Info_Flags::Cmeasure])
		return ade_set_args(L, "f", -1.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Heat Effectiveness is not supported");
	}

	return ade_set_args(L, "f", info->cm_heat_effectiveness);
}

ADE_VIRTVAR(aspectEffectiveness,
	l_Weaponclass,
	"number",
	"The aspect effectiveness of this weapon class if it's a countermeasure. Otherwise returns -1",
	"number",
	"The aspect effectiveness or -1 on error")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_args(L, "f", -1.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "f", -1.0f);

	weapon_info* info = &Weapon_info[idx];

	if (!info->wi_flags[Weapon::Info_Flags::Cmeasure])
		return ade_set_args(L, "f", -1.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Aspect Effectiveness is not supported");
	}

	return ade_set_args(L, "f", info->cm_aspect_effectiveness);
}

ADE_VIRTVAR(effectiveRange,
	l_Weaponclass,
	"number",
	"The effective range of this weapon class if it's a countermeasure. Otherwise returns -1",
	"number",
	"The effective range or -1 on error")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_args(L, "f", -1.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "f", -1.0f);

	weapon_info* info = &Weapon_info[idx];

	if (!info->wi_flags[Weapon::Info_Flags::Cmeasure])
		return ade_set_args(L, "f", -1.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Effective Range is not supported");
	}

	return ade_set_args(L, "f", info->cm_effective_rad);
}

ADE_VIRTVAR(pulseInterval,
	l_Weaponclass,
	"number",
	"The pulse interval of this weapon class if it's a countermeasure. Otherwise returns -1",
	"number",
	"The pulse interval or -1 on error")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_args(L, "f", -1.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "f", -1.0f);

	weapon_info* info = &Weapon_info[idx];

	if (!info->wi_flags[Weapon::Info_Flags::Cmeasure])
		return ade_set_args(L, "f", -1.0f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "Setting Pulse Interval is not supported");
	}

	return ade_set_args(L, "f", info->cmeasure_timer_interval);
}

ADE_VIRTVAR(BurstShots, l_Weaponclass, "number", "The number of shots in a burst from this weapon.", "number", "Burst shots, 1 for non-burst weapons, or 0 if handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "i", 0);

	if (ADE_SETTING_VAR)
		LuaError(L, "Setting BurstShots is not supported");

	return ade_set_args(L, "i", Weapon_info[idx].burst_shots + 1);
}

ADE_VIRTVAR(BurstDelay, l_Weaponclass, "number", "The time in seconds between shots in a burst.", "number", "Burst delay, or 0 if handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR)
		LuaError(L, "Setting BurstDelay is not supported");

	return ade_set_args(L, "f", Weapon_info[idx].burst_delay);
}

ADE_VIRTVAR(FieldOfFire, l_Weaponclass, "number", "The angular spread for shots of this weapon.", "number", "Fof in degrees, or 0 if handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR)
		LuaError(L, "Setting FieldOfFire is not supported");

	return ade_set_args(L, "f", Weapon_info[idx].field_of_fire);
}

ADE_VIRTVAR(MaxFieldOfFire, l_Weaponclass, "number", "The maximum field of fire this weapon can have if it increases while firing.", "number", "Max Fof in degrees, or 0 if handle is invalid")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR)
		LuaError(L, "Setting MaxFieldOfFire is not supported");

	return ade_set_args(L, "f", Weapon_info[idx].max_fof_spread);
}

ADE_VIRTVAR(BeamLife, l_Weaponclass, "number", "The time in seconds that a beam lasts while firing.", "number", "Beam life, or 0 if handle is invalid or the weapon is not a beam")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR)
		LuaError(L, "Setting BeamLife is not supported");

	if (Weapon_info[idx].is_beam())
		return ade_set_args(L, "f", Weapon_info[idx].b_info.beam_life);

	return ade_set_args(L, "f", 0.0f);
}

ADE_VIRTVAR(BeamWarmup, l_Weaponclass, "number", "The time in seconds that a beam takes to warm up.", "number", "Warmup time, or 0 if handle is invalid or the weapon is not a beam")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR)
		LuaError(L, "Setting BeamWarmup is not supported");

	if (Weapon_info[idx].is_beam())
		return ade_set_args(L, "f", i2fl(Weapon_info[idx].b_info.beam_warmup) / MILLISECONDS_PER_SECOND);

	return ade_set_args(L, "f", 0.0f);
}

ADE_VIRTVAR(BeamWarmdown, l_Weaponclass, "number", "The time in seconds that a beam takes to warm down.", "number", "Warmdown time, or 0 if handle is invalid or the weapon is not a beam")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_error(L, "f", 0.0f);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_error(L, "f", 0.0f);

	if (ADE_SETTING_VAR)
		LuaError(L, "Setting BeamWarmdown is not supported");

	if (Weapon_info[idx].is_beam())
		return ade_set_args(L, "f", i2fl(Weapon_info[idx].b_info.beam_warmdown) / MILLISECONDS_PER_SECOND);

	return ade_set_args(L, "f", 0.0f);
}

ADE_FUNC(isValid, l_Weaponclass, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(renderTechModel,
	l_Weaponclass,
	"number X1, number Y1, number X2, number Y2, [number RotationPercent =0, number PitchPercent =0, number "
	"BankPercent=40, number Zoom=1.3, boolean Lighting=true]",
	"Draws weapon tech model. True for regular lighting, false for flat lighting.",
	"boolean",
	"Whether weapon was rendered")
{
	int x1, y1, x2, y2;
	angles rot_angles = {0.0f, 0.0f, 40.0f};
	int idx;
	float zoom = 1.3f;
	bool lighting = true;
	if (!ade_get_args(L, "oiiii|ffffb", l_Weaponclass.Get(&idx), &x1, &y1, &x2, &y2, &rot_angles.h, &rot_angles.p, &rot_angles.b, &zoom, &lighting))
		return ade_set_error(L, "b", false);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "b", false);

	if (x2 < x1 || y2 < y1)
		return ade_set_args(L, "b", false);

	CLAMP(rot_angles.p, 0.0f, 100.0f);
	CLAMP(rot_angles.b, 0.0f, 100.0f);
	CLAMP(rot_angles.h, 0.0f, 100.0f);

	// Handle angles
	matrix orient = vmd_identity_matrix;
	angles view_angles = {-0.6f, 0.0f, 0.0f};
	vm_angles_2_matrix(&orient, &view_angles);

	rot_angles.p = (rot_angles.p * 0.01f) * PI2;
	rot_angles.b = (rot_angles.b * 0.01f) * PI2;
	rot_angles.h = (rot_angles.h * 0.01f) * PI2;
	vm_rotate_matrix_by_angles(&orient, &rot_angles);

	return ade_set_args(L, "b", render_tech_model(TECH_WEAPON, x1, y1, x2, y2, zoom, lighting, idx, &orient));
}

// Nuke's alternate tech model rendering function
ADE_FUNC(renderTechModel2,
	l_Weaponclass,
	"number X1, number Y1, number X2, number Y2, [orientation Orientation=nil, number Zoom=1.3]",
	"Draws weapon tech model",
	"boolean",
	"Whether weapon was rendered")
{
	int x1, y1, x2, y2;
	int idx;
	float zoom = 1.3f;
	matrix_h* mh = nullptr;
	if (!ade_get_args(L, "oiiiio|f", l_Weaponclass.Get(&idx), &x1, &y1, &x2, &y2, l_Matrix.GetPtr(&mh), &zoom))
		return ade_set_error(L, "b", false);

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "b", false);

	if (x2 < x1 || y2 < y1)
		return ade_set_args(L, "b", false);

	// Handle angles
	matrix* orient = mh->GetMatrix();

	return ade_set_args(L, "b", render_tech_model(TECH_WEAPON, x1, y1, x2, y2, zoom, true, idx, orient));
}

ADE_FUNC(renderSelectModel,
	l_Weaponclass,
	"boolean restart, number x, number y, [number width = 629, number height = 355, number currentEffectSetting = default, number zoom = 0.65]",
	"Draws the 3D select weapon model with the chosen effect at the specified coordinates. Restart should "
	"be true on the first frame this is called and false on subsequent frames. Note that primary weapons "
	"will not render anything if they do not have a valid pof model defined! Valid selection effects are 1 (fs1) or 2 (fs2), "
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
	float zoom = 0.65f;
	if (!ade_get_args(L, "obii|iiif", l_Weaponclass.Get(&idx), &restart, &x1, &y1, &x2, &y2, &effect, &zoom))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "b", false);

	if (effect == 0 || effect > 2) {
		LuaError(L, "Valid effect values are 1 or 2, got %i", effect);
		return ade_set_args(L, "b", false);
	}

	if (restart) {
		anim_timer_start = timer_get_milliseconds();
	}

	weapon_info* wip = &Weapon_info[idx];

	if (effect == -1) {
		effect = wip->selection_effect;
	}

	int modelNum;
	if (VALID_FNAME(wip->tech_model)) {
		modelNum = model_load(wip->tech_model, nullptr, ErrorType::WARNING);
	} else if (wip->render_type != WRT_LASER) {
		modelNum = model_load(wip->pofbitmap_name, nullptr, ErrorType::FATAL_ERROR);
	} else {
		return ade_set_args(L, "b", false);
	}

	static float WepRot = 0.0f;

	model_render_params render_info;

	draw_model_rotating(&render_info,
		modelNum,
		x1,
		y1,
		x2,
		y2,
		&WepRot,
		&wip->closeup_pos,
		wip->closeup_zoom * zoom,
		REVOLUTION_RATE,
		MR_IS_MISSILE | MR_AUTOCENTER | MR_NO_FOGGING,
		GR_RESIZE_NONE,
		effect);

	return ade_set_args(L, "b", true);
}

ADE_FUNC(getWeaponClassIndex, l_Weaponclass, NULL, "Gets the index value of the weapon class", "number", "index value of the weapon class")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_args(L, "i", -1);

	if(idx < 0 || idx >= weapon_info_size())
		return ade_set_args(L, "i", -1);

	return ade_set_args(L, "i", idx + 1);
}

ADE_FUNC(isLaser, l_Weaponclass, nullptr, "Return true if the weapon is a 'laser' weapon, which also includes ballistic (ammo-based) weapons.  This also includes most beams, but not necessarily all of them.  See also isPrimary().", "boolean", "true if the weapon is a laser weapon, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_LASER)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isMissile, l_Weaponclass, nullptr, "Return true if the weapon is a 'missile' weapon.  See also isSecondary().", "boolean", "true if the weapon is a missile weapon, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_MISSILE)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isPrimary, l_Weaponclass, nullptr, "Return true if the weapon is a primary weapon.  This also includes most beams, but not necessarily all of them.  This function is equivalent to isLaser().", "boolean", "true if the weapon is a primary, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_LASER)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isNonBeamPrimary, l_Weaponclass, nullptr, "Return true if the weapon is a primary weapon that is not a beam.", "boolean", "true if the weapon is a non-beam primary, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].is_non_beam_primary())
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isSecondary, l_Weaponclass, nullptr, "Return true if the weapon is a secondary weapon.  This function is equivalent to isMissile().", "boolean", "true if the weapon is a secondary, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_MISSILE)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isBeam, l_Weaponclass, nullptr, "Return true if the weapon is a beam", "boolean", "true if the weapon is a beam, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].is_beam())
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isCountermeasure, l_Weaponclass, nullptr, "Return true if the weapon is a countermeasure", "boolean", "true if the weapon is a countermeasure, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].wi_flags[Weapon::Info_Flags::Cmeasure])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isWeaponRequired,
	l_Weaponclass,
	nullptr,
	"Checks if a weapon is required for the currently loaded mission",
	"boolean",
	"true if required, false if otherwise. Nil if the weapon class is invalid or a mission has not been loaded")
{
	int idx = -1;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_NIL;

	//This could be requested before Common_team has been initialized, so let's check.
	if (Common_select_inited) {
		return ade_set_args(L, "b", Team_data[Common_team].weapon_required[idx]);
	} else {
		return ADE_RETURN_NIL;
	}
}

ADE_FUNC(isPlayerAllowed,
	l_Weaponclass,
	nullptr,
	"Detects whether the weapon has the player allowed flag",
	"boolean",
	"true if player allowed, false otherwise, nil if a syntax/type error occurs")
{
	int idx;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	weapon_info* wip = &Weapon_info[idx];

	if (wip->wi_flags[Weapon::Info_Flags::Player_allowed]) {
		return ADE_RETURN_TRUE;
	}

	return ADE_RETURN_FALSE;
}

// Checks if a weapon has been paged in (counted as used)
ADE_FUNC(isWeaponUsed, l_Weaponclass, NULL, "Return true if the weapon is paged in.", "boolean", "True if the weapon is paged in, false if otherwise")
{
	int idx = -1;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (!weapon_used(idx)) {
		return ADE_RETURN_FALSE;
	} else {
		return ADE_RETURN_TRUE;
	}
}

// Pages in a weapon
ADE_FUNC(loadWeapon, l_Weaponclass, NULL, "Pages in a weapon. Returns True on success.", "boolean", "True if page in was successful, false otherwise.")
{
	int idx = -1;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= weapon_info_size())
		return ADE_RETURN_FALSE;

	if (!weapon_page_in(idx)) {
		return ADE_RETURN_FALSE;
	} else {
		return ADE_RETURN_TRUE;
	}
}


}
}
