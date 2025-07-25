/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 



#ifndef __FS2_BEAM_WEAPON_HEADER_FILE
#define __FS2_BEAM_WEAPON_HEADER_FILE

// ------------------------------------------------------------------------------------------------
// BEAM WEAPON DEFINES/VARS
//
#include "globalincs/globals.h"
#include "model/model.h"
#include "utils/modular_curves.h"

// prototypes
class object;
class ship_subsys;
struct obj_pair;
struct beam_weapon_info;
struct vec3d;

enum class WeaponState : uint32_t
{
	INVALID,

	// States for laser weapons
	NORMAL, //!< For laser weapons that have only one state

	// Missile states following
	FREEFLIGHT, //!< The initial flight state where the missile is "unpowered"
	IGNITION, //!< The moment when the missile comes out of free flight
	HOMED_FLIGHT, //!< The missile is homing in on its target
	UNHOMED_FLIGHT, //!< The missile does not currently target an object

	// Beam states following
	WARMUP, //!< The beam is charging up
	FIRING, //!< The beam is firing, and there's a beam and everything
	PAUSED, //!< The beam is technically firing, but it's in between bursts
	WARMDOWN, //!< The beam is cooling down
};
struct WeaponStateHash {
	size_t operator()(const WeaponState& state) const {
		return static_cast<size_t>(state);
	}
};

typedef enum class BeamType {
	DIRECT_FIRE,	// unidirectional beam; used to be BEAM_TYPE_A
	SLASHING,		// "slash" in one direction; used to be BEAM_TYPE_B
	TARGETING,		// targeting lasers (only lasts one frame); used to be BEAM_TYPE_C
	ANTIFIGHTER,	// similar to the type A beams, but takes multiple shots and "chases" fighters around; used to be BEAM_TYPE_D
	NORMAL_FIRE,	// stupid beam. like type A, only it doesn't aim. it just shoots directly out of the turret; used to be BEAM_TYPE_E
	OMNI			// SCP type, highly flexible and configurable; used to be BEAM_TYPE_F
} BeamType;

// max # of "shots" an individual beam will take
#define MAX_BEAM_SHOTS				5
#define MAX_BEAMS					500

// apply damage
#define BEAM_DAMAGE_TIME			170

// uses to define beam behavior ahead of time - needed for multiplayer
typedef struct beam_info {
	vec3d			dir_a, dir_b;						// direction vectors for beams	
	vec3d			rot_axis;
	ubyte				shot_count;							// # of shots	
	float				shot_aim[MAX_BEAM_SHOTS];		// accuracy. this is a constant multiple of radius. anything < 1.0 will guarantee a hit	
} beam_info;

#define BFIF_IS_FIGHTER_BEAM	(1<<0)
#define BFIF_FORCE_FIRING		(1<<1)
#define BFIF_TARGETING_COORDS	(1<<2)
#define BFIF_FLOATING_BEAM		(1<<3)

// to ensure validity of fire_info, the related fields MUST be provided
#define BFM_TURRET_FIRED         0   // objp, subsys, target
#define BFM_TURRET_FORCE_FIRED   1   // objp, subsys, target OR target_pos
#define BFM_FIGHTER_FIRED        2   // objp, subsys
#define BFM_SPAWNED              3   // starting pos, target OR target_pos
#define BFM_SEXP_FLOATING_FIRED  4   // starting pos, target OR target_pos
#define BFM_SUBSPACE_STRIKE      5   // starting pos, target

// pass to beam fire 
typedef struct beam_fire_info {
	int				beam_info_index;				// weapon info index 
	object			*shooter;						// whos shooting
	vec3d			local_fire_postion;		// offset from the center of the object (for fighter beams only)
	ship_subsys		*turret;						// where he's shooting from
	float			accuracy;						// 0.0 to 1.0 (only really effects targeting on small ships)
	object			*target;							// who's getting shot
	ship_subsys		*target_subsys;						// (optional), specific subsystem to be targeted on the target 
	vec3d			target_pos1;							// if we're shooting off into space
	vec3d			target_pos2;							// if we're shooting off into space (optional second point)
	vec3d			starting_pos;							// starting positiong for floating beams -MageKing17
	beam_info		*beam_info_override;			// (optional), pass this in to override all beam movement info (for multiplayer)
	int				num_shots;						// (optional), only used for type D weapons
	int bank;									// for fighters, which bank of the primary weapons are they in
	int point;									// for fighters, which point on the bank it is from
	int bfi_flags;
	char team;									// for floating beams, determines which team the beam is on
	int burst_seed;								// used for sharing random targets if part of the same burst
	int  fire_method;
	float per_burst_rotation;                         // for type 5 beams
	int burst_index;
} beam_fire_info;

typedef struct fighter_beam_fire_info {
	int				beam_info_index;				// weapon info index 
	object			*shooter;						// whos shooting
	vec3d			local_fire_postion;		// offset from the center of the object (for fighter beams only)
	ship_subsys		*turret;							// where he's shooting from
	float				accuracy;						// 0.0 to 1.0 (only really effects targeting on small ships)
	object			*target;							// whos getting shot
	ship_subsys		*target_subsys;				// (optional), specific subsystem to be targeted on the target 
	beam_info		*beam_info_override;			// (optional), pass this in to override all beam movement info (for multiplayer)
	int				num_shots;						// (optional), only used for type D weapons
	int fighter_beam_loop_sound;			//loop sound used by fighter beams -Bobboau
	int warmup_stamp;
	int warmdown_stamp;
	float life_left;	
	float life_total;
} fighter_beam_fire_info;

// max # of collisions we'll allow per frame
#define MAX_FRAME_COLLISIONS		10

// collision info
typedef struct beam_collision {
	mc_info			cinfo;							// collision info
	int				c_objnum;						// objnum of the guy we recently collided with
	int				c_sig;							// object sig
	int				c_stamp;							// when we should next apply damage	
	int				quadrant;						// shield quadrant this beam hits if any -Bobboau
	bool			is_exit_collision;					//does this occur when the beam is exiting the ship
} beam_collision;

// beam flag defines
#define BF_SAFETY						(1<<0)		// if this is set, don't collide or render for this frame. lifetime still increases though
#define BF_SHRINK						(1<<1)		// if this is set, the beam is shrinking in width and approaching the warmdown phase
#define BF_FORCE_FIRING					(1<<2)
#define BF_IS_FIGHTER_BEAM				(1<<3)
#define BF_TARGETING_COORDS				(1<<4)
#define BF_FLOATING_BEAM				(1<<5)
#define BF_GROW				            (1<<6)     // if this is set, the beam is growing in width
#define BF_FINISHED_GROWING				(1<<7)     // to stop grow from re-triggering

// beam struct (the actual weapon/object)
typedef struct beam {
	// low-level data
	int		objnum;					// our own objnum
	int		weapon_info_index;
	int		sig;						// signature for the shooting object
	object	*objp;					// the shooting object (who owns the turret that I am being fired from)
	object	*target;					// target object
	ship_subsys *target_subsys;	// targeted subsys
	vec3d	target_pos1;				// if we're targeting a location in space
	vec3d	target_pos2;				// if we're targeting a location in space (optional second point)
	int		target_sig;				// target sig
	ship_subsys *subsys;				// subsys its being fired from
	beam		*next, *prev;			// link list stuff
	vec3d	local_fire_postion;		// offset from the center of the object (for fighter beams only)
	int		framecount;				// how many frames the beam has been active
	int		flags;					// see BF_* defines 	
	float		current_width_factor;		// applied to width due to shrink or grow

	// beam info	
	int		warmup_stamp;			// timestamp for "warming up"
	int		warmdown_stamp;		// timestamp for "warming down"
	BeamType		type;				// the type of beam, direct-fire, slashing, etc
	float		life_left;				// in seconds
	float		life_total;				// total life	
	// this vector has very special meaning. BEFORE performing collision checks, it basically implies a "direction". meaning
	// the vector between it and last_start is where the beam will be aiming. AFTER performing collision checks, it is the
	// literal world collision point on the object (or meaningless, if we hit nothing). The function beam_move_all_pre() is
	// responsible for then setting it up pre-collision time
	vec3d	last_shot;				
	vec3d	last_start;				
	int		shot_index;				// for type D beam weapons
	float	beam_glow_frame;		// what frame a beam glow animation is on
	float	beam_section_frame[MAX_BEAM_SECTIONS];	// what frame a beam section animation is on

	// recent collisions
	beam_collision r_collisions[MAX_FRAME_COLLISIONS];					// recent collisions
	int r_collision_count;														// # of recent collisions

	// collision info for this frame
	beam_collision f_collisions[MAX_FRAME_COLLISIONS];					// collisions for the current frame
	int f_collision_count;														// # of collisions we recorded this frame

	// looping sound info, HANDLE
	sound_handle beam_sound_loop; // invalid if none

	// team 
	char		team;

	float range;
	float damage_threshold;

	// exactly how the beam will behave. by passing this is multiplayer from server to client, we can ensure that
	// everything looks the same
	beam_info binfo;
	int bank;

	int firingpoint;
	float		beam_collide_width;
	float		beam_light_width;

	float u_offset_local;

	bool rotates;					// type 5s only, determines whether it rotates
	float type5_rot_speed;          // how fast it rotates if it does

	WeaponState weapon_state;

	modular_curves_entry_instance modular_curves_instance;
} beam;

extern std::array<beam, MAX_BEAMS> Beams;				// all beams
extern int Beam_count;

#define BEAM_INDEX(beam)			(int)((beam) - Beams.data())

// ------------------------------------------------------------------------------------------------
// BEAM WEAPON FUNCTIONS
//

// ---------------
// the next functions are probably the only ones anyone should care about calling. the rest require somewhat detailed knowledge of beam weapons

// fire a beam, returns objnum on success. the innards of the code handle all the rest, foo
int beam_fire(beam_fire_info *fire_info);

// fire a targeting beam, returns objnum on success. a much much simplified version of a beam weapon
// targeting lasers last _one_ frame. For a continuous stream - they must be created every frame.
// this allows it to work smoothly in multiplayer (detect "trigger down". every frame just create a targeting laser firing straight out of the
// object. this way you get all the advantages of nice rendering and collisions).
// NOTE : only references beam_info_index and shooter
int beam_fire_targeting(fighter_beam_fire_info *fire_info);

// return an object index of the guy who's firing this beam
int beam_get_parent(const object *bm);

// return weapon_info_index of beam
int beam_get_weapon_info_index(const object *bm);

// given a beam object, get the # of collisions which happened during the last collision check (typically, last frame)
int beam_get_num_collisions(int objnum);

// stuff collision info, returns 1 on success
int beam_get_collision(int objnum, int num, int *collision_objnum, mc_info **cinfo);
// ---------------

// init at game startup
void beam_init();

// initialize beam weapons for this level
void beam_level_init();

// shutdown beam weapons for this level
void beam_level_close();

// collide a beam with a ship, returns 1 if we can ignore all future collisions between the 2 objects
int beam_collide_ship(obj_pair *pair);

// collide a beam with an asteroid, returns 1 if we can ignore all future collisions between the 2 objects
int beam_collide_asteroid(obj_pair *pair);

// collide a beam with a missile, returns 1 if we can ignore all future collisions between the 2 objects
int beam_collide_missile(obj_pair *pair);

// collide a beam with debris, returns 1 if we can ignore all future collisions between the 2 objects
int beam_collide_debris(obj_pair *pair);

// pre-move (before collision checking - but AFTER ALL OTHER OBJECTS HAVE BEEN MOVED)
void beam_move_all_pre();

// post-collision time processing for beams
void beam_move_all_post();

// render all beam weapons
void beam_render_all();

// delete all active beams
void beam_delete_all();

// early-out function for when adding object collision pairs, return 1 if the pair should be ignored
int beam_collide_early_out(object *a, object *b);

// pause all looping beam sounds
void beam_pause_sounds();

// unpause looping beam sounds
void beam_unpause_sounds();

void beam_calc_facing_pts(vec3d *top, vec3d *bot, vec3d *fvec, vec3d *pos, float w, float z_add);

// return the amount of damage which should be applied to a ship. basically, filters friendly fire damage 
float beam_get_ship_damage(beam *b, object *objp, vec3d* hitpos = nullptr);



#endif
