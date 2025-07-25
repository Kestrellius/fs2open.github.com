/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/




#ifdef _WIN32
 #include <winsock2.h>	// required to prevent winsock 1.1 being used
 #include <direct.h>
 #include <io.h>
 #include <psapi.h>
#ifndef _MINGW
 #include <crtdbg.h>
#endif // !_MINGW
#else
#ifdef APPLE_APP
 #include <sys/types.h>
 #include <libproc.h>
#endif
 #include <unistd.h>
 #include <sys/stat.h>
#endif

#include "globalincs/alphacolors.h"
#include "globalincs/crashdump.h"
#include "globalincs/mspdb_callstack.h"
#include "globalincs/version.h"

#include "SDLGraphicsOperations.h"
#include "freespace.h"
#include "freespaceresource.h"
#include "levelpaging.h"

#include "anim/animplay.h"
#include "asteroid/asteroid.h"
#include "autopilot/autopilot.h"
#include "bmpman/bmpman.h"
#include "cfile/cfile.h"
#include "cheats_table/cheats_table.h"
#include "cmdline/cmdline.h"
#include "cmeasure/cmeasure.h"
#include "cutscene/cutscenes.h"
#include "cutscene/movie.h"
#include "executor/global_executors.h"
#include "cutscene/player.h"
#include "debris/debris.h"
#include "debugconsole/console.h"
#include "decals/decals.h"
#include "events/events.h"
#include "exceptionhandler/exceptionhandler.h"
#include "fireball/fireballs.h"
#include "gamehelp/contexthelp.h"
#include "gamehelp/gameplayhelp.h"
#include "gamesequence/gamesequence.h"
#include "gamesnd/eventmusic.h"
#include "gamesnd/gamesnd.h"
#include "graphics/debug_sphere.h"
#include "graphics/font.h"
#include "graphics/light.h"
#include "graphics/matrix.h"
#include "graphics/openxr.h"
#include "graphics/shadows.h"
#include "headtracking/headtracking.h"
#include "hud/hud.h"
#include "hud/hudconfig.h"
#include "hud/hudescort.h"
#include "hud/hudlock.h"
#include "hud/hudmessage.h"
#include "hud/hudparse.h"
#include "hud/hudshield.h"
#include "hud/hudsquadmsg.h"
#include "hud/hudtargetbox.h"
#include "iff_defs/iff_defs.h"
#include "io/cursor.h"
#include "io/joy.h"
#include "io/joy_ff.h"
#include "io/key.h"
#include "io/mouse.h"
#include "io/timer.h"
#include "jumpnode/jumpnode.h"
#include "lab/labv2.h"
#include "libs/discord/discord.h"
#include "libs/ffmpeg/FFmpeg.h"
#include "lighting/lighting.h"
#include "lighting/lighting_profiles.h"
#include "localization/localize.h"
#include "math/staticrand.h"
#include "math/curve.h"
#include "menuui/barracks.h"
#include "menuui/credits.h"
#include "menuui/mainhallmenu.h"
#include "menuui/optionsmenu.h"
#include "menuui/playermenu.h"
#include "menuui/readyroom.h"
#include "menuui/snazzyui.h"
#include "menuui/techmenu.h"
#include "menuui/trainingmenu.h"
#include "mission/missionbriefcommon.h"
#include "mission/missioncampaign.h"
#include "mission/missiongoals.h"
#include "mission/missionhotkey.h"
#include "mission/missionload.h"
#include "mission/missionlog.h"
#include "mission/missionmessage.h"
#include "mission/missionparse.h"
#include "mission/missiontraining.h"
#include "missionui/fictionviewer.h"
#include "missionui/missionbrief.h"
#include "missionui/missioncmdbrief.h"
#include "missionui/missiondebrief.h"
#include "missionui/missionloopbrief.h"
#include "missionui/missionpause.h"
#include "missionui/missionscreencommon.h"
#include "missionui/missionshipchoice.h"
#include "missionui/missionweaponchoice.h"
#include "missionui/redalert.h"
#include "mod_table/mod_table.h"
#include "model/modelreplace.h"
#include "nebula/neb.h"
#include "nebula/neblightning.h"
#include "nebula/volumetrics.h"
#include "network/multi.h"
#include "network/multi_dogfight.h"
#include "network/multi_endgame.h"
#include "network/multi_fstracker.h"
#include "network/multi_ingame.h"
#include "network/multi_interpolate.h"
#include "network/multi_log.h"
#include "network/multi_pause.h"
#include "network/multi_pxo.h"
#include "network/multi_rate.h"
#include "network/multi_respawn.h"
#include "network/multi_turret_manager.h"
#include "network/multi_voice.h"
#include "network/multimsgs.h"
#include "network/multiteamselect.h"
#include "network/multiui.h"
#include "network/multiutil.h"
#include "network/stand_gui.h"
#include "object/objcollide.h"
#include "object/objectsnd.h"
#include "object/waypoint.h"
#include "observer/observer.h"
#include "options/default_settings_table.h"
#include "options/Ingame_Options.h"
#include "options/Option.h"
#include "options/OptionsManager.h"
#include "osapi/osapi.h"
#include "osapi/osregistry.h"
#include "parse/encrypt.h"
#include "parse/generic_log.h"
#include "parse/parselo.h"
#include "parse/sexp.h"
#include "parse/sexp/sexp_lookup.h"
#include "particle/ParticleManager.h"
#include "particle/particle.h"
#include "pilotfile/pilotfile.h"
#include "playerman/managepilot.h"
#include "playerman/player.h"
#include "popup/popup.h"
#include "popup/popupdead.h"
#include "radar/radar.h"
#include "radar/radarsetup.h"
#include "render/3d.h"
#include "render/batching.h"
#include "scpui/rocket_ui.h"
#include "scripting/api/objs/gamestate.h"
#include "scripting/api/objs/camera.h"
#include "scripting/global_hooks.h"
#include "scripting/hook_api.h"
#include "scripting/scripting.h"
#include "ship/afterburner.h"
#include "ship/awacs.h"
#include "ship/ship.h"
#include "ship/shipcontrails.h"
#include "ship/shipfx.h"
#include "ship/shiphit.h"
#include "sound/audiostr.h"
#include "sound/ds.h"
#include "sound/fsspeech.h"
#include "sound/sound.h"
#include "sound/voicerec.h"
#include "starfield/starfield.h"
#include "starfield/supernova.h"
#include "stats/medals.h"
#include "stats/stats.h"
#include "tracing/Monitor.h"
#include "tracing/tracing.h"
#include "utils/Random.h"
#include "utils/threading.h"
#include "weapon/beam.h"
#include "weapon/emp.h"
#include "weapon/flak.h"
#include "weapon/muzzleflash.h"
#include "weapon/shockwave.h"
#include "weapon/weapon.h"


#include <SDL.h>
#include <SDL_main.h>

#include <cinttypes>
#include <stdexcept>

#include "imgui.h"

#ifdef WIN32
// According to AMD and NV, these _should_ force their drivers into high-performance mode
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

#ifdef NDEBUG
#ifdef FRED
#error macro FRED is defined when trying to build release FreeSpace.  Please undefine FRED macro in build settings
#endif
#endif


//	Revision history.
//	Full version:
//    1.00.04	5/26/98	MWA -- going final (12 pm)
//    1.00.03	5/26/98	MWA -- going final (3 am)
//    1.00.02	5/25/98	MWA -- going final
//    1.00.01	5/25/98	MWA -- going final
//		0.90		5/21/98	MWA -- getting ready for final.
//		0.10		4/9/98.  Set by MK.
//
//	OEM version:
//		1.00		5/28/98	AL.	First release to Interplay QA.

void game_reset_view_clip();
void game_reset_shade_frame();
void game_post_level_init();
void game_do_frame(bool set_frametime = true);
void game_time_level_init();
void game_time_level_close();
void game_show_framerate();			// draws framerate in lower right corner

struct big_expl_flash {
	float max_flash_intensity;	// max intensity
	float cur_flash_intensity;	// cur intensity
	TIMESTAMP	flash_start;		// start time
};

#define FRAME_FILTER 16

#define DEFAULT_SKILL_LEVEL	1
int	Game_skill_level = DEFAULT_SKILL_LEVEL;

static SCP_string skill_level_display(int value)
{
	return SCP_string(Skill_level_names(value, true));
}

static void parse_skill_func()
{
	int value;
	stuff_int(&value);

	value -= 1; // Parse 1-5 for the skill levels but convert to our internal 0-4
	CLAMP(value, 0, 4);

	Game_skill_level = value;
}

static auto GameSkillOption __UNUSED = options::OptionBuilder<int>("Game.SkillLevel",
                     std::pair<const char*, int>{"Skill Level", 1284},
                     std::pair<const char*, int>{"The skill level for the game.", 1700})
                     .category(std::make_pair("Game", 1824))
                     .range(0, 4)
                     .level(options::ExpertLevel::Beginner)
                     .default_func([]() { return DEFAULT_SKILL_LEVEL; })
                     .bind_to(&Game_skill_level)
                     .display(skill_level_display)
                     .importance(1)
                     .flags({options::OptionFlags::RetailBuiltinOption})
                     .parser(parse_skill_func)
                     .finish();

bool Screenshake_enabled = true;

static void parse_screenshake_func()
{
	bool enabled;
	stuff_boolean(&enabled);
	Screenshake_enabled = enabled;
}

auto ScreenShakeOption = options::OptionBuilder<bool>("Graphics.ScreenShake",
                     std::pair<const char*, int>{"Screen Shudder Effect", 1812}, // do xstr
                     std::pair<const char*, int>{"Toggles the screen shake effect for weapons, afterburners, and shockwaves", 1813})
                     .category(std::make_pair("Graphics", 1825))
                     .default_func([]() { return Screenshake_enabled; })
                     .level(options::ExpertLevel::Advanced)
                     .importance(55)
                     .bind_to(&Screenshake_enabled)
                     .parser(parse_screenshake_func)
                     .finish();

bool Allow_unfocused_pause = true;

static SCP_string unfocused_pause_display(bool mode) { return mode ? XSTR("Yes", 1394) : XSTR("No", 1395); }

static void parse_unfocused_pause_func()
{
	bool enabled;
	stuff_boolean(&enabled);
	Allow_unfocused_pause = enabled;
}

auto UnfocusedPauseOption = options::OptionBuilder<bool>("Game.UnfocusedPause",
                     std::pair<const char*, int>{"Pause If Unfocused", 1814}, // do xstr
                     std::pair<const char*, int>{"Whether or not the game automatically pauses if it loses focus", 1815})
                     .category(std::make_pair("Game", 1824))
                     .default_func([]() { return Allow_unfocused_pause; })
                     .level(options::ExpertLevel::Advanced)
                     .display(unfocused_pause_display) 
                     .importance(55)
                     .bind_to(&Allow_unfocused_pause)
                     .parser(parse_unfocused_pause_func)
                     .finish();

#define EXE_FNAME			("fs2.exe")

// JAS: Code for warphole camera.
// Needs to be cleaned up.
float Warpout_time = 0.0f;
int Warpout_forced = 0;		// Set if this is a forced warpout that cannot be cancelled.
sound_handle Warpout_sound = sound_handle::invalid();
int Use_joy_mouse = 0;
int Use_palette_flash = 1;
#ifndef NDEBUG
int Use_fullscreen_at_startup = 0;
#endif
int Show_area_effect = 0;
object	*Last_view_target = nullptr;

int dogfight_blown = 0;

int	frame_int = -1;
float frametimes[FRAME_FILTER];
float frametotal = 0.0f;
float flRealframetime;
float flFrametime;
fix FrametimeOverall = 0;

#ifndef NDEBUG
	int	Show_framerate = 1;
#else 
	int	Show_framerate = 0;
#endif

int	Framerate_cap = 120;

// for the model page in system
extern void model_page_in_start();

int	Show_cpu = 0;
int	Show_target_debug_info = 0;
int	Show_target_weapons = 0;
#ifndef NDEBUG
static int Show_player_pos = 0;		// debug console command to show player world pos on HUD
#endif

int Debug_octant = -1;

fix Game_time_compression = F1_0;
fix Desired_time_compression = Game_time_compression;
fix Time_compression_change_rate = 0;
bool Time_compression_locked = false; //Can the user change time with shift- controls?

// auto-lang stuff
int detect_lang();

// if the ships.tbl the player has is valid
int Game_ships_tbl_valid = 0;

// if the weapons.tbl the player has is valid
int Game_weapons_tbl_valid = 0;

int Test_begin = 0;
extern int	Player_attacking_enabled;
int Show_net_stats;

bool Pre_player_entry = false;

int	Fred_running = 0;
bool running_unittests = false;

// required for hudtarget... kinda dumb, but meh
char Fred_alt_names[MAX_SHIPS][NAME_LENGTH+1];
char Fred_callsigns[MAX_SHIPS][NAME_LENGTH+1];

char Game_current_mission_filename[MAX_FILENAME_LEN];
int game_single_step = 0;
int last_single_step=0;

int game_zbuffer = 1;

extern void ssm_init();
extern void ssm_level_init();
extern void ssm_process();

// static variable to contain the time this version was built
// commented out for now until
// I figure out how to get the username into the file
//LOCAL char freespace_build_time[] = "Compiled on:"__DATE__" "__TIME__" by "__USER__;

// amount of time to wait after the player has died before we display the death died popup
#define PLAYER_DIED_POPUP_WAIT		2500
UI_TIMESTAMP Player_died_popup_wait;

UI_TIMESTAMP Multi_ping_timestamp;

// builtin mission list stuff
int Game_builtin_mission_count = 92;
fs_builtin_mission Game_builtin_mission_list[MAX_BUILTIN_MISSIONS] = {
	// single player campaign
	{ "freespace2.fc2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN_FILE),					"" },
			
	// act 1
	{ "sm1-01.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-02.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-03.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-04.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-05.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-06.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-07.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-08.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-09.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "sm1-10.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "loop1-1.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "loop1-2.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "loop1-3.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "training-1.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "training-2.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "training-3.fs2",				(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "tsm-104.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "tsm-105.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},
	{ "tsm-106.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_2	},

	// act 2
	{ "sm2-01.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-02.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-03.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-04.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-05.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-06.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-07.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-08.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-09.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm2-10.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},

	// act 3
	{ "sm3-01.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-02.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-03.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-04.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-05.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-06.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-07.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-08.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-09.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "sm3-10.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},
	{ "loop2-1.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},		
	{ "loop2-2.fs2",					(FSB_FROM_VOLITION | FSB_CAMPAIGN),							FS_CDROM_VOLUME_3	},

	// multiplayer missions

	// gauntlet
	{ "g-shi.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "g-ter.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "g-vas.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},

	// coop
	{ "m-01.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "m-02.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "m-03.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "m-04.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},		

	// dogfight
	{ "mdh-01.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-02.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-03.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-04.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-05.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-06.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-07.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-08.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdh-09.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-01.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-02.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-03.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-04.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-05.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-06.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-07.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-08.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdl-09.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-01.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-02.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-03.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-04.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-05.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-06.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-07.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-08.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "mdm-09.fs2",					(FSB_FROM_VOLITION | FSB_MULTI),								""						},		
	{ "osdog.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},

	// TvT		
	{ "mt-01.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-02.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-03.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-04.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-05.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-06.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-07.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-08.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-09.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},
	{ "mt-10.fs2",						(FSB_FROM_VOLITION | FSB_MULTI),								""						},				

	// campaign
	{ "templar.fc2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN_FILE),					"" },
	{ "templar-01.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
	{ "templar-02.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
	{ "templar-03.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
	{ "templar-04.fs2",				(FSB_FROM_VOLITION | FSB_MULTI | FSB_CAMPAIGN),			""						},				
};


// Internal function prototypes
void game_do_training_checks();
void game_shutdown();
void game_show_event_debug(float frametime);
void game_event_debug_init();
void game_frame(bool paused = false);
void game_start_subspace_ambient_sound();
void game_stop_subspace_ambient_sound();
void verify_ships_tbl();
void verify_weapons_tbl();
void game_title_screen_display();
void game_title_screen_close();

// loading background filenames
static const char *Game_loading_bground_fname[GR_NUM_RESOLUTIONS] = {
	"LoadingBG",		// GR_640
	"2_LoadingBG"		// GR_1024
};


static const char *Game_loading_ani_fname[GR_NUM_RESOLUTIONS] = {
	"Loading",		// GR_640
	"2_Loading"		// GR_1024
};

static const char *Game_title_screen_fname[GR_NUM_RESOLUTIONS] = {
	"PreLoad",
	"2_PreLoad"
};

static const char *Game_logo_screen_fname[GR_NUM_RESOLUTIONS] = {
	"PreLoadLogo",
	"2_PreLoadLogo"
};

// for title screens
static int Game_title_bitmap = -1;
static int Game_title_logo = -1;

// How much RAM is on this machine. Set in WinMain
uint64_t FreeSpace_total_ram = 0;

// game flash stuff
float Game_flash_red = 0.0f;
float Game_flash_green = 0.0f;
float Game_flash_blue = 0.0f;
float Sun_spot = 0.0f;
big_expl_flash Big_expl_flash = {0.0f, 0.0f, TIMESTAMP::invalid()};

// game shudder stuff (in ms)
bool Game_shudder_perpetual = false;
bool Game_shudder_everywhere = false;
TIMESTAMP Game_shudder_time = TIMESTAMP::invalid();
int Game_shudder_total = 0;
float Game_shudder_intensity = 0.0f;			// should be between 0.0 and 100.0

// EAX stuff
sound_env Game_sound_env;
sound_env Game_default_sound_env = { EAX_ENVIRONMENT_BATHROOM, 0.2f, 0.2f, 1.0f };

const fs_builtin_mission *game_find_builtin_mission(const char *filename)
{
	// look through all existing builtin missions
	for(int idx=0; idx<Game_builtin_mission_count; idx++){
		if(!stricmp(Game_builtin_mission_list[idx].filename, filename)){
			return &Game_builtin_mission_list[idx];
		}
	}

	// didn't find it
	return nullptr;
}

int game_get_default_skill_level()
{
	return DEFAULT_SKILL_LEVEL;
}

// Resets the flash
void game_flash_reset()
{
	Game_flash_red = 0.0f;
	Game_flash_green = 0.0f;
	Game_flash_blue = 0.0f;
	Sun_spot = 0.0f;
	Big_expl_flash.max_flash_intensity = 0.0f;
	Big_expl_flash.cur_flash_intensity = 0.0f;
	Big_expl_flash.flash_start = TIMESTAMP::invalid();
}

extern float Framerate;

/**
 * Adds a flash effect.  
 *
 * These can be positive or negative. The range will get capped at around -1 to 1, so stick 
 * with a range like that.
 *
 * @param r red colour value
 * @param g green colour value
 * @param b blue colour value
 */
void game_flash( float r, float g, float b )
{
	Game_flash_red += r;
	Game_flash_green += g;
	Game_flash_blue += b;

	if ( Game_flash_red < -1.0f )	{
		Game_flash_red = -1.0f;
	} else if ( Game_flash_red > 1.0f )	{
		Game_flash_red = 1.0f;
	}

	if ( Game_flash_green < -1.0f )	{
		Game_flash_green = -1.0f;
	} else if ( Game_flash_green > 1.0f )	{
		Game_flash_green = 1.0f;
	}

	if ( Game_flash_blue < -1.0f )	{
		Game_flash_blue = -1.0f;
	} else if ( Game_flash_blue > 1.0f )	{
		Game_flash_blue = 1.0f;
	}

}

/**
 * Adds a flash for Big Ship explosions
 * @param flash flash intensity. Range capped from 0 to 1.
 */
void big_explosion_flash(float flash)
{
	CLAMP(flash, 0.0f, 1.0f);

	Big_expl_flash.flash_start = TIMESTAMP::immediate();
	Big_expl_flash.max_flash_intensity = flash;
	Big_expl_flash.cur_flash_intensity = 0.0f;
}

//	Amount to diminish palette towards normal, per second.
#define	DIMINISH_RATE	0.75f
#define	SUN_DIMINISH_RATE	6.00f

int Sun_drew = 0;

float sn_glare_scale = 1.7f;
DCF(sn_glare, "Sets the sun glare scale (Default is 1.7)")
{
	dc_stuff_float(&sn_glare_scale);
}

float Supernova_last_glare = 0.0f;

void game_sunspot_process(float frametime)
{
	TRACE_SCOPE(tracing::SunspotProcess);
	float Sun_spot_goal = 0.0f;

	int supernova_sun_idx = 0;
	int supernova_light_idx = light_find_for_sun(supernova_sun_idx);

	// supernova
	auto sn_stage = supernova_stage();
	if (sn_stage != SUPERNOVA_STAGE::NONE && supernova_light_idx >= 0) {
		// sunspot differently based on supernova stage
		switch (sn_stage) {
		// this case is only here to make gcc happy - apparently it doesn't know we already checked for it
		case SUPERNOVA_STAGE::NONE:
			UNREACHABLE("The SUPERNOVA_STAGE::NONE case should have already been handled");
			break;

		// approaching. player still in control
		case SUPERNOVA_STAGE::STARTED:
		case SUPERNOVA_STAGE::CLOSE:
			float pct;
			pct = supernova_sunspot_pct();

			vec3d light_dir;				
			light_get_global_dir(&light_dir, supernova_light_idx);
			float dot;
			dot = vm_vec_dot( &light_dir, &Eye_matrix.vec.fvec );
			
			if(dot >= 0.0f){
				// scale it some more
				dot = dot * (0.5f + (pct * 0.5f));
				dot += 0.05f;					

				Sun_spot_goal += (dot * sn_glare_scale);
			}

			// draw the sun glow
			if ( !shipfx_eye_in_shadow( &Eye_position, Viewer_obj, supernova_light_idx ) )	{
				// draw the glow for this sun
				stars_draw_sun_glow(supernova_sun_idx);
			}

			Supernova_last_glare = Sun_spot_goal;
			break;

		// camera cut. player not in control. note : at this point camera starts out facing the sun. so we can go nice and bright
		case SUPERNOVA_STAGE::HIT:
		case SUPERNOVA_STAGE::TOOLTIME:
			Sun_spot_goal = 0.9f;
			Sun_spot_goal += supernova_sunspot_pct() * 0.1f;

			if(Sun_spot_goal > 1.0f){
				Sun_spot_goal = 1.0f;
			}

			Sun_spot_goal *= sn_glare_scale;
			Supernova_last_glare = Sun_spot_goal;
			break;		

		// fade to white. display dead popup
		case SUPERNOVA_STAGE::DEAD1:
		case SUPERNOVA_STAGE::DEAD2:
			Supernova_last_glare += (2.0f * flFrametime);
			if(Supernova_last_glare > 2.0f){
				Supernova_last_glare = 2.0f;
			}

			Sun_spot_goal = Supernova_last_glare;
			break;
		}
	
		Sun_drew = 0;				
	} else {
		Sun_spot_goal = 0.0f;
		if ( Sun_drew )	{
			// check sunspots for all suns
			int n_lights = light_get_global_count();

			// check
			for(int light_idx=0; light_idx<n_lights; light_idx++)	{
				bool in_shadow = shipfx_eye_in_shadow(&Eye_position, Viewer_obj, light_idx);

				if (!in_shadow) {
					vec3d light_dir;				
					light_get_global_dir(&light_dir, light_idx);

					//only do sunglare stuff if this light source has one
					if (light_has_glare(light_idx))	{
						float dot = vm_vec_dot( &light_dir, &Eye_matrix.vec.fvec )*0.5f+0.5f;
						Sun_spot_goal += (float)pow(dot,85.0f);
					}

					// draw the glow for this sun
					int sun_idx = light_get_sun_index(light_idx);
					if (sun_idx >= 0) {
						stars_draw_sun_glow(sun_idx);
					}
				}
			}

			Sun_drew = 0;
		}
	}

	float dec_amount = frametime*SUN_DIMINISH_RATE;

	if ( Sun_spot < Sun_spot_goal )	{
		Sun_spot += dec_amount;
		if ( Sun_spot > Sun_spot_goal )	{
			Sun_spot = Sun_spot_goal;
		}
	} else if ( Sun_spot > Sun_spot_goal )	{
		Sun_spot -= dec_amount;
		if ( Sun_spot < Sun_spot_goal )	{
			Sun_spot = Sun_spot_goal;
		}
	}
}


/**
 * Call once a frame to diminish the flash effect to 0.
 * @param frametime Period over which to dimish at ::DIMINISH_RATE
 */
static void game_flash_diminish(float frametime)
{
	float dec_amount = frametime*DIMINISH_RATE;

	if ( Game_flash_red > 0.0f ) {
		Game_flash_red -= dec_amount;		
		if ( Game_flash_red < 0.0f )
			Game_flash_red = 0.0f;
	} else {
		Game_flash_red += dec_amount;		
		if ( Game_flash_red > 0.0f )
			Game_flash_red = 0.0f;
	} 

	if ( Game_flash_green > 0.0f ) {
		Game_flash_green -= dec_amount;		
		if ( Game_flash_green < 0.0f )
			Game_flash_green = 0.0f;
	} else {
		Game_flash_green += dec_amount;		
		if ( Game_flash_green > 0.0f )
			Game_flash_green = 0.0f;
	} 

	if ( Game_flash_blue > 0.0f ) {
		Game_flash_blue -= dec_amount;		
		if ( Game_flash_blue < 0.0f )
			Game_flash_blue = 0.0f;
	} else {
		Game_flash_blue += dec_amount;		
		if ( Game_flash_blue > 0.0f )
			Game_flash_blue = 0.0f;
	} 

	// update big_explosion_cur_flash
	if (Big_expl_flash.flash_start.isValid()) {
#define	TIME_UP		1500
#define	TIME_DOWN	2500
		int duration = TIME_UP + TIME_DOWN;
		int time = timestamp_until(Big_expl_flash.flash_start);
		if (time > -duration) {
			time = -time;
			if (time < TIME_UP) {
				Big_expl_flash.cur_flash_intensity = Big_expl_flash.max_flash_intensity * time / (float)TIME_UP;
			}
			else {
				time -= TIME_UP;
				Big_expl_flash.cur_flash_intensity = Big_expl_flash.max_flash_intensity * ((float)TIME_DOWN - time) / (float)TIME_DOWN;
			}
		}
	} else {
		Big_expl_flash.cur_flash_intensity = 0.0f;
	}
	
	if ( Use_palette_flash )	{
		int r,g,b;

		// Change the 200 to change the color range of colors.
		r = fl2i( Game_flash_red*128.0f );  
		g = fl2i( Game_flash_green*128.0f );   
		b = fl2i( Game_flash_blue*128.0f );  

		if ( Sun_spot > 0.0f && gr_sunglare_enabled() && !gr_lightshafts_enabled()) {
			r += fl2i(Sun_spot*128.0f);
			g += fl2i(Sun_spot*128.0f);
			b += fl2i(Sun_spot*128.0f);
		}

		if ( Big_expl_flash.cur_flash_intensity  > 0.0f ) {
			r += fl2i(Big_expl_flash.cur_flash_intensity*128.0f);
			g += fl2i(Big_expl_flash.cur_flash_intensity*128.0f);
			b += fl2i(Big_expl_flash.cur_flash_intensity*128.0f);
		}

		if ( r < 0 ) r = 0; else if ( r > 255 ) r = 255;
		if ( g < 0 ) g = 0; else if ( g > 255 ) g = 255;
		if ( b < 0 ) b = 0; else if ( b > 255 ) b = 255;

		if ( (r!=0) || (g!=0) || (b!=0) ) {
			gr_flash( r, g, b );
		}
	}
	
}


void game_level_close()
{
	if (scripting::hooks::OnMissionAboutToEndHook->isActive())
	{
		scripting::hooks::OnMissionAboutToEndHook->run();
	}

	//WMC - this is actually pretty damn dangerous, but I don't want a modder
	//to accidentally use an override here without realizing it.
	if (!scripting::hooks::OnMissionEndHook->isActive() || !scripting::hooks::OnMissionEndHook->isOverride())
	{
		// save player-persistent variables and containers
		mission_campaign_save_on_close_variables();	// Goober5000
		mission_campaign_save_on_close_containers(); // jg18

		// De-Initialize the game subsystems
		obj_delete_all();
		obj_reset_colliders();
		multi_interpolate_clear_all(); // object related
		sexp_music_close();	// Goober5000
		event_music_level_close();
		game_stop_looped_sounds();
		snd_stop_all();
		obj_snd_level_close();					// uninit object-linked persistant sounds
		gamesnd_unload_gameplay_sounds();	// unload gameplay sounds from memory
		anim_level_close();						// stop and clean up any anim instances
		message_mission_shutdown();			// called after anim_level_close() to make sure instances are clear
		shockwave_level_close();
		fireball_close();	
		shield_hit_close();
		asteroid_level_close();
		jumpnode_level_close();
		waypoint_level_close();
		flak_level_close();						// unload flak stuff
		neb2_level_close();						// shutdown gaseous nebula stuff
		volumetrics_level_close();
		ct_level_close();
		beam_level_close();
		mission_brief_common_reset();		// close out parsed briefing/mission stuff
		cam_close();
		subtitles_close();
		animation::ModelAnimationSet::stopAnimations();
		particle::ParticleManager::get()->clearSources();
		particle::close();
		trail_level_close();
		ship_close_cockpit_displays(Player_ship);
		hud_level_close();
		hud_escort_clear_all();
		model_instance_free_all();
		batch_render_close();

		// be sure to not only reset the time but the lock as well
		set_time_compression(1.0f, 0.0f);
		lock_time_compression(false);

		audiostream_unpause_all();
		snd_aav_init();

		gr_set_ambient_light(120, 120, 120);

		stars_level_close();

		multi_close_oo_and_ship_tracker();
		Multi_Turret_Manager.reset(); // Cyborg, this can safely be done after everything else.  At some point, I'll probably consolidate these.

		Pilot.save_savefile();

		// Cybor17 - also, undo cheats.
		extern bool Weapon_energy_cheat;
		Weapon_energy_cheat = false;

		game_time_level_close();

		if (Game_mode & GM_STANDALONE_SERVER) {
			model_free_all();			// Free all existing models if standalone server

			// clean out interp data as it's better to allocate as needed here instead
			// of letting it use a bunch of memory
			extern void model_deallocate_interp_data();
			model_deallocate_interp_data();

			extern void model_collide_free_point_list();
			model_collide_free_point_list();
		}
	}
	else
	{
		Error(LOCATION, "Scripting Mission End override is not fully supported yet.");
	}

	if (scripting::hooks::OnMissionEndHook->isActive())
	{
		scripting::hooks::OnMissionEndHook->run();
	}
}

time_t load_gl_init;
uint load_post_level_init;
extern bool Cmdline_reuse_rng_seed;
extern uint Cmdline_rng_seed;
/**
 * Intializes game stuff.  
 *
 * @return 0 on failure, 1 on success
 */
void game_level_init()
{
	game_busy( NOX("** starting game_level_init() **") );
	load_gl_init = time(nullptr);

	// seed the random number generator in multiplayer
	if ( Game_mode & GM_MULTIPLAYER ) {
		// seed the generator from the netgame security flags -- ensures that all players in
		// multiplayer will have the same random number sequence (with static rand functions)
		Random::seed( Netgame.security );

		// semirand function needs to get re-initted every time in multiplayer
		init_semirand();
	} else if(Cmdline_reuse_rng_seed) {
		Random::seed( Cmdline_rng_seed );
	}

	Framecount = 0;
	game_reset_view_clip();
	game_reset_shade_frame();

	Key_normal_game = (Game_mode & GM_NORMAL);
	Cheats_enabled = 0;
	CheatUsed = "";

	Game_shudder_time = TIMESTAMP::invalid();

	Perspective_locked = false;
	Slew_locked = false;

	// reset the geometry map and distortion map batcher, this should to be done pretty soon in this mission load process (though it's not required)
	batch_reset();

	// Initialize the game subsystems
	game_time_level_init();

	Multi_ping_timestamp = UI_TIMESTAMP::invalid();

	obj_init();						// Must be inited before the other systems

	if ( !(Game_mode & GM_STANDALONE_SERVER) ) {
		model_page_in_start();		// mark any existing models as unused but don't unload them yet
		mprintf(( "Beginning level bitmap paging...\n" ));
		bm_page_in_start();
	} else {
		model_free_all();			// Free all existing models if standalone server
	}

	mission_brief_common_init();		// Free all existing briefing/debriefing text
	weapon_level_init();

	NavSystem_Init();				// zero out the nav system

	ai_level_init();				//	Call this before ship_init() because it reads ai.tbl.
	multi_init_oo_and_ship_tracker();	// Inits/resets multiplayer ship tracking system.  Has to be done before creating any ships.
	ship_level_init();
	player_level_init();
	shipfx_flash_init();			// Init the ship gun flash system.
	game_flash_reset();			// Reset the flash effect
	particle::init();				// Reset the particle system
	fireball_init();
	debris_init();
	shield_hit_init();				//	Initialize system for showing shield hits

	mission_goals_and_events_init();
	mission_log_init();
	messages_init();
	obj_snd_level_init();					// init object-linked persistant sounds
	anim_level_init();
	shockwave_level_init();
	afterburner_level_init();
	scoring_level_init( &Player->stats );
	key_level_init();
	asteroid_level_init();
	control_config_clear_used_status();
	collide_ship_ship_sounds_init();

	Pre_player_entry = true;		//	Means the player has not yet entered.
	Entry_delay_time = 0;			//	Could get overwritten in mission read.

	observer_init();
	flak_level_init();				// initialize flak - bitmaps, etc
	ct_level_init();				// initialize ships contrails, etc
	awacs_level_init();				// initialize AWACS
	beam_level_init();				// initialize beam weapons
	ssm_level_init();	
	supernova_level_init();
	cam_init();
	snd_aav_init();

	// multiplayer dogfight hack
	dogfight_blown = 0;

	shipfx_engine_wash_level_init();

	stars_pre_level_init();
	neb2_level_init();
	nebl_level_init();

	Last_view_target = nullptr;

	// campaign wasn't ended
	Campaign_ending_via_supernova = 0;

	load_gl_init = (time(nullptr) - load_gl_init);
	mprintf(("Game_level_init took %ld seconds\n", load_gl_init));

	//WMC - Init multi players for level
	if (Game_mode & GM_MULTIPLAYER && Player != nullptr) {
		Player->flags |= PLAYER_FLAGS_IS_MULTI;

		// clear multiplayer stats
		init_multiplayer_stats();
	}
}

/**
 * Called when a mission is over -- does server specific stuff.
 */
void freespace_stop_mission()
{	
	game_level_close();
	Game_mode &= ~GM_IN_MISSION;
}

/**
 * Called at frame interval to process networking stuff
 */
void game_do_networking()
{
	Assert( Net_player != nullptr );
	if (!(Game_mode & GM_MULTIPLAYER)){
		return;
	}

	// see if this player should be reading/writing data.  Bit is set when at join
	// screen onward until quits back to main menu.
	if ( !(Net_player->flags & NETINFO_FLAG_DO_NETWORKING) ){
		return;
	}

	if(gameseq_get_state()!=GS_STATE_MULTI_PAUSED){
		multi_do_frame();
	} else {
		multi_pause_do_frame();
	}	
}

// An estimate as to how high the count passed to game_loading_callback will go.
// This is just a guess, it seems to always be about the same.   The count is
// proportional to the code being executed, not the time, so this works good
// for a bar, assuming the code does about the same thing each time you
// load a level.   You can find this value by looking at the return value
// of game_busy_callback(nullptr), which I conveniently print out to the
// debug output window with the '=== ENDING LOAD ==' stuff.   
#define COUNT_ESTIMATE 719

int Game_loading_callback_inited = 0;
int Game_loading_background = -1;
generic_anim Game_loading_ani;

static int Game_loading_ani_coords[GR_NUM_RESOLUTIONS][2] = {
	{
		63, 316  // GR_640
	},
	{
		101, 505	// GR_1024
	}
};

#ifndef NDEBUG
extern char Processing_filename[MAX_PATH_LEN];
static int busy_shader_created = 0;
shader busy_shader;
#endif
static int framenum;

/**
 * This gets called 10x per second and count is the number of times ::game_busy() has been 
 * called since the current callback function was set.
 */
void game_loading_callback(int count)
{
	game_do_networking();

	Assert( Game_loading_callback_inited==1 );
	Assertion( Game_loading_ani.num_frames > 0, "Load Screen animation %s not found, or corrupted. Needs to be an animation with at least 1 frame.", Game_loading_ani.filename );

	int do_flip = 0;

	int new_framenum = bm_get_anim_frame(Game_loading_ani.first_frame, static_cast<float>(count), static_cast<float>(COUNT_ESTIMATE));
	// retail incremented the frame number by one, essentially skipping the 1st frame except for single-frame anims
	if (Game_loading_ani.num_frames > 1 && new_framenum < Game_loading_ani.num_frames-1) {
		new_framenum++;
	}

	//make sure we always run forwards - graphical hack
	if(new_framenum > framenum)
		framenum = new_framenum;

	if ( Game_loading_ani.num_frames > 0 )	{
		GR_MAYBE_CLEAR_RES(Game_loading_background);
		if ( Game_loading_background > -1 )	{
			gr_set_bitmap( Game_loading_background );
			gr_bitmap(0,0,GR_RESIZE_MENU);
		}

		gr_set_bitmap( Game_loading_ani.first_frame + framenum );
		gr_bitmap(Game_loading_ani_coords[gr_screen.res][0],Game_loading_ani_coords[gr_screen.res][1], GR_RESIZE_MENU);

		do_flip = 1;
	}

#ifndef NDEBUG
	// print the current filename being processed by game_busy(), the shader here is a quick hack
	// since the background isn't always drawn so we can't clear the text away from the previous
	// filename. the shader is completely opaque to hide the old text. must easier and faster than
	// redrawing the entire screen every flip - taylor
	if (!busy_shader_created) {
		gr_create_shader(&busy_shader, 5, 5, 5, 255);
		busy_shader_created = 1;
	}

	if (Processing_filename[0] != '\0') {
		gr_set_shader(&busy_shader);
		gr_shade(0, 0, gr_screen.max_w_unscaled, 17, GR_RESIZE_MENU); // make sure it goes across the entire width

		gr_set_color_fast(&Color_white);
		gr_string(5, 5, Processing_filename, GR_RESIZE_MENU);

		do_flip = 1;
		memset( Processing_filename, 0, MAX_PATH_LEN );
	}
#endif

	auto progress = static_cast<float>(count) / static_cast<float>(COUNT_ESTIMATE);
	CLAMP(progress, 0.0f, 1.0f);

	if (scripting::hooks::OnLoadScreen->run(scripting::hook_param_list(scripting::hook_param("Progress", 'f', progress))) > 0) {
		// At least one script exeuted so we probably need to do a flip now
		do_flip = 1;
	}

	os_defer_events_on_load_screen();

	if (do_flip)
		gr_flip();
}

void game_loading_callback_init()
{
	Assert( Game_loading_callback_inited==0 );

	Game_loading_background = bm_load(The_mission.loading_screen[gr_screen.res]);
	
	if (Game_loading_background < 0)
		Game_loading_background = bm_load(Game_loading_bground_fname[gr_screen.res]);

	generic_anim_init(&Game_loading_ani, Game_loading_ani_fname[gr_screen.res]);
	generic_anim_load(&Game_loading_ani);
	Assertion( Game_loading_ani.num_frames > 0, "Load Screen animation %s not found, or corrupted. Needs to be an animation with at least 1 frame.", Game_loading_ani.filename );

	Game_loading_callback_inited = 1;
	io::mouse::CursorManager::get()->showCursor(false);
	framenum = 0;
	game_busy_callback( game_loading_callback, (COUNT_ESTIMATE/Game_loading_ani.num_frames)+1 );


}

void game_loading_callback_close()
{
	Assert( Game_loading_callback_inited==1 );

	// Make sure bar shows all the way over.
	game_loading_callback(COUNT_ESTIMATE);
	
	int real_count __UNUSED = game_busy_callback( nullptr );
	io::mouse::CursorManager::get()->showCursor(true);

	Game_loading_callback_inited = 0;
	
#ifndef NDEBUG
	mprintf(( "=================== ENDING LOAD ================\n" ));
	mprintf(( "Real count = %d,  Estimated count = %d\n", real_count, COUNT_ESTIMATE ));
	mprintf(( "================================================\n" ));
#else
	// to remove warnings in release build
	real_count = 0;
#endif

	generic_anim_unload(&Game_loading_ani);

	bm_release( Game_loading_background );
	common_free_interface_palette();		// restore game palette
	Game_loading_background = -1;

	font::set_font(font::FONT1);

	// Mission loading during multi doesn't always set a new state event
	// so this hook will let Lua scripts know we've finished no matter what
	// game type is currently being played.
	if (scripting::hooks::OnLoadComplete->isActive()) {
		scripting::hooks::OnLoadComplete->run();
	}
}

/**
 * Update the sound environment (ie change EAX settings based on proximity to large ships)
 */
void game_maybe_update_sound_environment()
{
	// do nothing for now
}

/**
 * Assign the sound environment for the game, based on the current mission
 */
void game_assign_sound_environment()
{
	if (The_mission.sound_environment.id >= 0) {
		Game_sound_env = The_mission.sound_environment;
	} else if (SND_ENV_DEFAULT > 0) {
		sound_env_get(&Game_sound_env, SND_ENV_DEFAULT);
	} else {
		Game_sound_env = Game_default_sound_env;
	}
}

/**
 * Function which gets called before actually entering the mission.
 */
void freespace_mission_load_stuff()
{
	// called if we're not on a freespace dedicated (non rendering, no pilot) server
	// IE : we _don't_ want to load any sounds or bitmap/texture info on this machine.
	if(!(Game_mode & GM_STANDALONE_SERVER)){	
	
		mprintf(( "=================== STARTING LEVEL DATA LOAD ==================\n" ));

		game_busy( NOX("** setting up event music **") );
		event_music_level_start(-1);	// preloads the first 2 seconds for each event music track

		game_busy( NOX("** unloading interface sounds **") );
		gamesnd_unload_interface_sounds();		// unload interface sounds from memory

		{
			TRACE_SCOPE(tracing::PreloadMissionSounds);

			game_busy( NOX("** preloading common game sounds **") );
			gamesnd_preload_common_sounds();			// load in sounds that are expected to play

			game_busy( NOX("** preloading gameplay sounds **") );
			gamesnd_load_gameplay_sounds();			// preload in gameplay sounds if wanted

			game_busy( NOX("** assigning sound environment for mission **") );
			ship_assign_sound_all();	// assign engine sounds to ships
			game_assign_sound_environment();	 // assign the sound environment for this mission
		}

		obj_merge_created_list();

		if (!(Game_mode & GM_MULTIPLAYER)) {
			// call function in missionparse.cpp to fixup player/ai stuff.
			game_busy( NOX("** fixing up player/ai stuff **") );
			mission_parse_fixup_players();
		}

		// Load in all the bitmaps for this level
		level_page_in();

		game_busy( NOX("** finished with level_page_in() **") );

		if(Game_loading_callback_inited) {
			game_loading_callback_close();
		}
	} 
	// the only thing we need to call on the standalone for now.
	else {
		obj_merge_created_list();

		// Load in all the bitmaps for this level
		level_page_in();
	}
}

/**
 * Called after mission is loaded.
 *
 * Because player isn't created until after mission loads, some things must get initted after the level loads
 */
void game_post_level_init()
{
	TRACE_SCOPE(tracing::LoadPostMissionLoad);

	HUD_init();
	hud_setup_escort_list();
	mission_hotkey_set_defaults();	// set up the default hotkeys (from mission file)

	stars_post_level_init();	

	// While trying to track down the nebula bug I encountered a cool effect -
	// comment this out to fly a mission in a void. Maybe we should develop this
	// into a full effect or something, because it is seriously cool.
	neb2_post_level_init(The_mission.flags[Mission::Mission_Flags::Neb2_fog_color_override]);

	// Initialize decal system
	decals::initializeMission();

#ifndef NDEBUG
	game_event_debug_init();
#endif

	training_mission_init();
	asteroid_create_all();

	// set ambient light for level
	gr_set_ambient_light(The_mission.ambient_light_level & 0xff, 
							(The_mission.ambient_light_level >> 8) & 0xff,
							(The_mission.ambient_light_level >> 16) & 0xff);

	// If this is a red alert mission in campaign mode, bash status
	if ( (Game_mode & GM_CAMPAIGN_MODE) && red_alert_mission() ) {
		red_alert_bash_ship_status();
	}

	freespace_mission_load_stuff();

	mission_process_alt_types();

	if (scripting::hooks::OnMissionStart->isActive()) {
		// HACK: That scripting hook should be in mission so GM_IN_MISSION has to be set
		Game_mode |= GM_IN_MISSION;
		scripting::hooks::OnMissionStart->run(scripting::hook_param_list(
			scripting::hook_param("Player", 'o', Player_obj, Player_obj != nullptr)
		));
		Game_mode &= ~GM_IN_MISSION;
	}
}

/**
 * Tells the server to load the mission and initialize structures
 */
bool game_start_mission()
{
	mprintf(( "=================== STARTING LEVEL LOAD ==================\n" ));

	int s1 __UNUSED = timer_get_milliseconds();

	// clear post processing settings
	gr_post_process_set_defaults();

	get_mission_info(Game_current_mission_filename, &The_mission, false);

	if ( !(Game_mode & GM_STANDALONE_SERVER) )
		game_loading_callback_init();

	game_level_init();
	
	if (Game_mode & GM_MULTIPLAYER) {
		Player->flags |= PLAYER_FLAGS_IS_MULTI;

		// clear multiplayer stats
		init_multiplayer_stats();
	}

	game_busy( NOX("** starting mission_load() **") );
	bool load_success = mission_load(Game_current_mission_filename);

	// free up memory from parsing the mission
	stop_parse();

	if ( !load_success ) {
		if ( !(Game_mode & GM_MULTIPLAYER) ) {
			// the version will have been assigned before loading was aborted
			if (!gameversion::check_at_least(The_mission.required_fso_version)) {
				popup(PF_BODY_BIG | PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("This mission requires FSO version %s", 1671), format_version(The_mission.required_fso_version, true).c_str());
			}
			// standard load failure
			else {
				popup(PF_BODY_BIG | PF_USE_AFFIRMATIVE_ICON, 1, POPUP_OK, XSTR("Attempt to load the mission failed", 169));
			}
			gameseq_post_event(GS_EVENT_MAIN_MENU);
		} else {
			multi_quit_game(PROMPT_NONE, MULTI_END_NOTIFY_NONE, MULTI_END_ERROR_LOAD_FAIL);
		}

		if ( !(Game_mode & GM_STANDALONE_SERVER) ) {
			game_loading_callback_close();
		}

		game_level_close();

		return false;
	}

	// Since we just freed up memory, now is probably a good time to run the Lua garbage collector.
	// This is also after the preliminary checks are complete but before things start getting paged in.
	auto L = Script_system.GetLuaSession();
	if (L != nullptr)
	{
		game_busy(NOX("** cleaning up Lua objects **"));
		lua_gc(L, LUA_GCCOLLECT, 0);
	}

	game_busy( NOX("** starting game_post_level_init() **") );
	load_post_level_init = (uint) time(nullptr);
	game_post_level_init();
	load_post_level_init = (uint) (time(nullptr) - load_post_level_init);

#ifndef NDEBUG
	{
		void Do_model_timings_test();
		Do_model_timings_test();	
	}
#endif

	bm_print_bitmaps();

	// init some common team select stuff now
	if (Game_mode & GM_MULTIPLAYER) {
		multi_ts_common_level_init();
	}

	int e1 __UNUSED = timer_get_milliseconds();

	mprintf(("Level load took %f seconds.\n", (e1 - s1) / 1000.0f ));
	return true;
}

int Interface_framerate = 0;
#ifndef NDEBUG

DCF_BOOL( mouse_control, Use_mouse_to_fly )
DCF_BOOL( show_framerate, Show_framerate )
DCF_BOOL( show_target_debug_info, Show_target_debug_info )
DCF_BOOL( show_target_weapons, Show_target_weapons )
DCF_BOOL( lead_target_cheat, Players[Player_num].lead_target_cheat )
DCF_BOOL( sound, Sound_enabled )
DCF_BOOL( zbuffer, game_zbuffer )
DCF_BOOL( show_shield_mesh, Show_shield_mesh)
DCF_BOOL( player_attacking, Player_attacking_enabled )
DCF_BOOL( show_waypoints, Show_waypoints )
DCF_BOOL( show_area_effect, Show_area_effect )
DCF_BOOL( show_net_stats, Show_net_stats )
DCF_BOOL( log, Log_debug_output_to_file )
extern int Training_message_method;
DCF_BOOL( training_msg_method, Training_message_method )
DCF_BOOL( show_player_pos, Show_player_pos )
DCF_BOOL(i_framerate, Interface_framerate )
DCF_BOOL( show_bmpman_usage, Cmdline_bmpman_usage )

DCF(warp, "Tests warpin effect")
{
	if (dc_optional_string_either("help", "--help")) {
		dc_printf( "Params: bool warpin, string Target = ""\n  Warps in if true, out if false. Player is target unless specific ship is specified\n" );
		return;
	} // Else, process command

	// TODO: Provide status flag

	bool warpin;
	char target[MAX_NAME_LEN];
	int idx = -1;

	dc_stuff_boolean(&warpin);
	if (dc_maybe_stuff_string_white(target, MAX_NAME_LEN)) {
		idx = ship_name_lookup(target);
	}	// Else, default target to player
	
	if (idx < 0) {
		// Player is target
		if (Player_ai->target_objnum > -1) {
			if(warpin) {
				shipfx_warpin_start(&Objects[Player_ai->target_objnum]);
			} else {
				shipfx_warpout_start(&Objects[Player_ai->target_objnum]);
			}
		}
	} else {
		// Non-player is targer
		if (warpin) {
			shipfx_warpin_start(&Objects[Ships[idx].objnum]);
		} else {
			shipfx_warpout_start(&Objects[Ships[idx].objnum]);
		}
	}
	
}

DCF(show_cpu,"Toggles showing cpu usage")
{
	bool process = true;

	if (dc_optional_string_either("help", "--help")) {
		dc_printf( "Usage: (optional) bool Show_cpu\n If true, Show_cpu is set and Show_mem is cleared.  If false, then Show_cpu is cleared.  If nothing passed, then toggle.\n" );
		process = false;
	}
	
	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		dc_printf("Show_cpu is %s\n", (Show_cpu ? "TRUE" : "FALSE"));
		process = false;
	}

	if (!process) {
		// Help and/or status was given, so don't process the command
		return;
	} // Else, process the command

	if (!dc_maybe_stuff_boolean(&Show_cpu)) {
		// Nothing passed, so toggle
		Show_cpu = !Show_cpu;
	}	// Else, value was set/cleared by user
}

#endif

DCF(use_joy_mouse,"Makes joystick move mouse cursor")
{
	bool process = true;

	if (dc_optional_string_either("help", "--help")) {
		dc_printf("Usage: use_joy_mouse [bool]\nSets use_joy_mouse to true or false.  If nothing passed, then toggles it.\n");
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		dc_printf("use_joy_mouse is %s\n", (Use_joy_mouse ? "TRUE" : "FALSE"));
		process = false;
	}

	if (!process) {
		return;
	}

	if(!dc_maybe_stuff_boolean(&Use_joy_mouse)) {
		// Nothing passed, so toggle
		Use_joy_mouse = !Use_joy_mouse;
	} // Else, value was set/cleared by user

	os_config_write_uint( nullptr, NOX("JoystickMovesCursor"), Use_joy_mouse );
}

DCF_BOOL(palette_flash, Use_palette_flash);

int Use_low_mem = 0;

DCF(low_mem,"Uses low memory settings regardless of RAM")
{
	bool process = true;

	if (dc_optional_string_either("help", "--help")) {
		dc_printf("Usage: low_mem [bool]\nSets low_mem to true or false.  If nothing passed, then toggles it.\n");
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		dc_printf("low_mem is %s\n", (Use_low_mem ? "TRUE" : "FALSE"));
		process = false;
	}

	if (!process) {
		return;
	}

	if (!dc_maybe_stuff_boolean(&Use_low_mem)) {
		// Nothing passed, so toggle
		Use_low_mem = !Use_low_mem;
	} // Else, value was set/cleared by user

	os_config_write_uint( nullptr, NOX("LowMem"), Use_low_mem );
}


#ifndef NDEBUG

DCF(force_fullscreen, "Forces game to startup in fullscreen mode")
{
	bool process = true;
	if (dc_optional_string_either("help", "--help")) {
		dc_printf("Usage: low_mem [bool]\nSets low_mem to true or false.  If nothing passed, then toggles it.\n");
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		dc_printf("low_mem is %s\n", (Use_fullscreen_at_startup ? "TRUE" : "FALSE"));
		process = false;
	}

	if (!process) {
		return;
	}

	if (dc_maybe_stuff_boolean(&Use_fullscreen_at_startup)) {
		// Nothing passed, so toggle
		Use_fullscreen_at_startup = !Use_fullscreen_at_startup;
	} // Else, value was set/cleared by user

	os_config_write_uint( nullptr, NOX("ForceFullscreen"), Use_fullscreen_at_startup );
}
#endif

int	Framerate_delay = 0;

#ifdef FS2_VOICER
// This is really awful but thank the guys of X11 for naming something "Window"
#	include "SDL_syswm.h" // For SDL_SysWMinfo
#endif

/**
 * Game initialisation
 */
void game_init()
{
	int s1 __UNUSED, e1 __UNUSED;
	const char *ptr;
	char whee[MAX_PATH_LEN];

	Game_current_mission_filename[0] = 0;

	// seed the random number generator
	Random::seed(static_cast<unsigned int>(time(nullptr)));

	ImGui::CreateContext();

	Framerate_delay = 0;

#ifndef NDEBUG
	load_filter_info();
#endif

	// encrypt stuff
	encrypt_init();

	// Initialize the timer before the os
	timer_init();

	if (LoggingEnabled) {
		outwnd_init();
	}

	// Cyborg17 -- This section used to be in game_main(), but in a section before outwnd_init()
	if (Is_standalone){
		nprintf(("Network", "Standalone running\n"));
	}

	// init os stuff next
	os_init( Osreg_class_name, Window_title.c_str(), Osreg_app_name );

	threading::init_task_pool();

#ifndef NDEBUG
	mprintf(("FreeSpace 2 Open version: %s\n", FS_VERSION_FULL));

	extern void cmdline_debug_print_cmdline();
	cmdline_debug_print_cmdline();
#endif

	memset(whee, 0, sizeof(whee));

	_getcwd(whee, MAX_PATH_LEN-1);

	strcat_s(whee, DIR_SEPARATOR_STR);
	strcat_s(whee, EXE_FNAME);

	//Initialize the libraries
	s1 = timer_get_milliseconds();

	if ( cfile_init(whee, nullptr) ) {			// initialize before calling any cfopen stuff!!!
		exit(1);
	}

	e1 = timer_get_milliseconds();

	mod_table_init();		// load in all the mod dependent settings

	// Must be run after mod table is parsed so we know the value of Using_in_game_options
	// Must be run before everything else inits so we can init those with the correct
	// default values or user preferences
 	if (Using_in_game_options) {
		default_settings_init(); // load in current preferred default options values, if any
		options::OptionsManager::instance()->loadInitialValues();
	}

	// initialize localization module. Make sure this is done AFTER initialzing OS.
	lcl_init( detect_lang() );	
	lcl_xstr_init();


	if (Is_standalone) {
		// force off some cmdlines if they are on
		Cmdline_spec = 0;
		Cmdline_glow = 0;
		Cmdline_env = 0;
		Cmdline_height = 0;
		Cmdline_normal = 0;
		Cmdline_voice_recognition = 0;
		Cmdline_freespace_no_sound = 1;
		Cmdline_freespace_no_music = 1;
		Cmdline_NoFPSCap = 0;
		Cmdline_load_all_weapons = 0;

		// Force some ingame options to off
		Use_3D_shockwaves = false;
		options::OptionsManager::instance()->set_ingame_binary_option("Graphics.3DShockwaves", false);

		// now init the standalone server code
		std_init_standalone();
	}

	// verify that he has a valid ships.tbl (will Game_ships_tbl_valid if so)
	verify_ships_tbl();

	// verify that he has a valid weapons.tbl
	verify_weapons_tbl();


	Use_joy_mouse = 0;
	Use_low_mem = os_config_read_uint( nullptr, NOX("LowMem"), 0 );

#ifndef NDEBUG
	Use_fullscreen_at_startup = os_config_read_uint( nullptr, NOX("ForceFullscreen"), 1 );
#endif

	// change FPS cap if told to do so (for those who can't use vsync or where vsync isn't enough)
	uint max_fps = 0;
	if ( (max_fps = os_config_read_uint(nullptr, NOX("MaxFPS"), 0)) != 0 ) {
		if ( (max_fps > 15) && (max_fps < 120) ) {
			Framerate_cap = (int)max_fps;
		}
	}

	Asteroids_enabled = 1;		

/////////////////////////////
// SOUND INIT START
/////////////////////////////

	if ( !Is_standalone ) {
		snd_init();
	}

	if(!fsspeech_init()) {
		mprintf(("Failed to init speech\n"));
	}

/////////////////////////////
// SOUND INIT END
/////////////////////////////

	std::unique_ptr<SDLGraphicsOperations> sdlGraphicsOperations;
	if (!Is_standalone) {
		// Standalone mode doesn't require graphics operations
		sdlGraphicsOperations.reset(new SDLGraphicsOperations());
	}

	int graphics_api = GR_DEFAULT;
	if (Cmdline_vulkan)
		graphics_api = GR_VULKAN;

	if (!gr_init(std::move(sdlGraphicsOperations), graphics_api)) {
		os::dialogs::Message(os::dialogs::MESSAGEBOX_ERROR, "Error initializing graphics!");
		exit(1);
		return;
	}

	if (LoggingEnabled && Cmdline_debug_window) {
		outwnd_debug_window_init();
	}

	// This needs to happen after graphics initialization
	tracing::init();

// Karajorma - Moved here from the sound init code cause otherwise windows complains
#ifdef FS2_VOICER
	if(Cmdline_voice_recognition)
	{
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version); // initialize info structure with SDL version info

		bool voiceRectOn = false;
		if(SDL_GetWindowWMInfo(os::getSDLMainWindow(), &info)) { // the call returns true on success
			// success
			voiceRectOn = VOICEREC_init(info.info.win.window, WM_RECOEVENT, GRAMMARID1, IDR_CMD_CFG);
		} else {
			// call failed
			mprintf(( "Couldn't get window information: %s\n", SDL_GetError() ));
		}
	
		if(voiceRectOn == false)
		{
			os::dialogs::Message(os::dialogs::MESSAGEBOX_ERROR, "Failed to init voice rec!");
		}
	}

#endif

	font::init();					// loads up all fonts
	font::checkFontOptions();
	
	// add title screen
	game_title_screen_display();
	
	// attempt to load up master tracker registry info (login and password)
	Multi_tracker_id = -1;		

	// pxo login and password
	ptr = os_config_read_string(NOX("PXO"),NOX("Login"),nullptr);
	if(ptr == nullptr){
		nprintf(("Network","Error reading in PXO login data\n"));
		strcpy_s(Multi_tracker_login,"");
	} else {		
		strcpy_s(Multi_tracker_login,ptr);
	}
	ptr = os_config_read_string(NOX("PXO"),NOX("Password"),nullptr);
	if(ptr == nullptr){
		nprintf(("Network","Error reading PXO password\n"));
		strcpy_s(Multi_tracker_passwd,"");
	} else {		
		strcpy_s(Multi_tracker_passwd,ptr);
	}	

	// pxo squad name and password
	ptr = os_config_read_string(NOX("PXO"),NOX("SquadName"),nullptr);
	if(ptr == nullptr){
		nprintf(("Network","Error reading in PXO squad name\n"));
		strcpy_s(Multi_tracker_squad_name, "");
	} else {		
		strcpy_s(Multi_tracker_squad_name, ptr);
	}

	// If less than 48MB of RAM, use low memory model.
	if (
#ifdef _WIN32
		(FreeSpace_total_ram < 48*1024*1024) ||
#endif
		Use_low_mem )	{
		mprintf(( "Using normal memory settings...\n" ));
		bm_set_low_mem(1);		// Use every other frame of bitmaps
	} else {
		mprintf(( "Using high memory settings...\n" ));
		bm_set_low_mem(0);		// Use all frames of bitmaps
	}
	
	curves_init();
	particle::ParticleManager::init();

	gamesnd_parse_soundstbl(true);	// Goober5000 - this needs to be split
	iff_init();						// Goober5000 - this must be done even before species_defs :p
	species_init();					// Load up the species defs - this needs to be done FIRST -- Kazan
	gamesnd_parse_soundstbl(false);

	brief_icons_init();

	hud_init_comm_orders();	// Goober5000

	control_config_common_init();				// sets up localization stuff in the control config

	rank_init();
	traitor_init();
	medals_init();

	cutscene_init();
	key_init();
	mouse_init();

	gameseq_init();

	multi_init();	

	// start up the mission logfile
	logfile_init(LOGFILE_EVENT_LOG);
	log_string(LOGFILE_EVENT_LOG,"FS2_Open Mission Log - Opened \n\n", 1);

	// standalone's don't use the joystick and it seems to sometimes cause them to not get shutdown properly
	if(!Is_standalone){
		io::joystick::init();
	}

	player_controls_init();
	model_init();	
	virtual_pof_init();

	event_music_init();

	// initialize alpha colors
	// CommanderDJ: try with colors.tbl first, then use the old way if that doesn't work
	alpha_colors_init();

	decals::initialize();

	// fireballs need to be parsed before parsing ships.
	// they don't need to be inited (data loaded etc.) until the mission starts
	fireball_parse_tbl();

	animation::ModelAnimationParseHelper::parseTables();

	// Initialize SEXPs. Must happen before ship init for LuaAI
	sexp_startup();

	obj_init();
	collide_init();
	mflash_game_init();	
	armor_init();
	ai_init();
	ai_profiles_init();		// Goober5000
	weapon_init();
	glowpoint_init();
	ship_init();						// read in ships.tbl	

	player_init();	
	mission_campaign_init();		// load in the default campaign	
	anim_init();
	context_help_init();			
	techroom_intel_init();			// parse species.tbl, load intel info  
	hud_positions_init();		//Setup hud positions
	
	// initialize psnet
	psnet_init(Multi_options_g.port);						// initialize the networking code

	asteroid_init();
	mission_brief_common_init();	// Mark all the briefing structures as empty.

	neb2_init();						// fullneb stuff
	nebl_init();
	stars_init();
	ssm_init();	
	player_tips_init();				// helpful tips
	beam_init();

	cheat_table_init();

	lighting_profiles::load_profiles();

	// load the list of pilot pic filenames (for barracks and pilot select popup quick reference)
	pilot_load_pic_list();	
	pilot_load_squad_pic_list();

	if (!Is_standalone) {
		// Load the default cursor and enable it
		io::mouse::Cursor* cursor = io::mouse::CursorManager::get()->loadCursor("cursor", true);
		if (cursor) {
			io::mouse::CursorManager::get()->setCurrentCursor(cursor);
		}
	}

	if(!Cmdline_reparse_mainhall)
	{
		main_hall_table_init();
	}

	Viewer_mode = 0;

	// Now that all data has been loaded, post-process anything from game_settings before we initialize scripting
	mod_table_post_process();

	// Note: Avoid calling any non-script functions after this line and before OnGameInit->run(), lest they run before scripting has completely initialized.
	script_init();			//WMC

	// Do this before the initial scripting hook runs in case that hook does something with the UI
	scpui::initialize();

	Script_system.RunInitFunctions();
	Scripting_game_init_run = true;	// set this immediately before OnGameInit so that OnGameInit *itself* will run
	if (scripting::hooks::OnGameInit->isActive()) {
		scripting::hooks::OnGameInit->run();
	}
	//Technically after the splash screen, but the best we can do these days. Since the override is hard-deprecated, we don't need to check it.
	if (scripting::hooks::OnSplashScreen->isActive()) {
		scripting::hooks::OnSplashScreen->run();
	}

	// This calls os_poll() so it should be placed after script initialization.
	game_title_screen_close();

	// A non-deprecated hook that runs after the splash screen has faded out.
	if (scripting::hooks::OnSplashEnd->isActive()) {
		scripting::hooks::OnSplashEnd->run();
	}

	// convert old pilot files (if they need it)
	convert_pilot_files();

	if ( !Is_standalone ) {
#ifdef WITH_FFMPEG
		libs::ffmpeg::initialize();
#endif
		if (Discord_presence) {
			libs::discord::init();
		}
	}

	nprintf(("General", "Ships.tbl is : %s\n", Game_ships_tbl_valid ? "VALID" : "INVALID!!!!"));
	nprintf(("General", "Weapons.tbl is : %s\n", Game_weapons_tbl_valid ? "VALID" : "INVALID!!!!"));

	mprintf(("cfile_init() took %d\n", e1 - s1));

	options::OptionsManager::instance()->printValues();

	// if we are done initializing, start showing the cursor
	io::mouse::CursorManager::get()->showCursor(true);

	mouse_set_pos(gr_screen.max_w / 2, gr_screen.max_h / 2);
}

char transfer_text[128];

float	Start_time = 0.0f;

float Framerate = 0.0f;

#ifndef NDEBUG
float Timing_total = 0.0f;
float Timing_render2 = 0.0f;
float Timing_render3 = 0.0f;
float Timing_flip = 0.0f;
float Timing_clear = 0.0f;
#endif

MONITOR(NumPolysDrawn)
MONITOR(NumPolys)
MONITOR(NumVerts)
MONITOR(BmpUsed)
MONITOR(BmpNew)

void game_get_framerate()
{	
	if (frame_int == -1) {
		for (int i = 0; i < FRAME_FILTER; i++)
			frametimes[i] = 0.0f;

		frametotal = 0.0f;
		frame_int = 0;
	}

	frametotal -= frametimes[frame_int];
	frametotal += flRealframetime;
	frametimes[frame_int] = flRealframetime;
	frame_int = (frame_int + 1 ) % FRAME_FILTER;

	if (frametotal != 0.0f) {
		if (Framecount >= FRAME_FILTER)
			Framerate = FRAME_FILTER / frametotal;
		else
			Framerate = Framecount / frametotal;
	}

	Framecount++;
}

/**
 * Show FPS within game
 */
void game_show_framerate()
{	
	int line_height = gr_get_font_height() + 1;

	float cur_time = f2fl(timer_get_approx_seconds());
	if (cur_time - Start_time > 30.0f) {
		mprintf(("%i frames executed in %7.3f seconds, %7.3f frames per second.\n", Framecount, cur_time - Start_time, Framecount/(cur_time - Start_time)));
		Start_time += 1000.0f;
	}

#ifdef WMC
	//WMC - this code spits out the target of all turrets
	if ( (Player_ai->target_objnum != -1) && (Objects[Player_ai->target_objnum].type == OBJ_SHIP) ) {
		//Debug crap
		int t = 0;
		ship_subsys	*pss;

		gr_set_color_fast(&HUD_color_debug);

		object *objp = &Objects[Player_ai->target_objnum];
		for ( pss = GET_FIRST(&shipp->subsys_list); pss !=END_OF_LIST(&shipp->subsys_list); pss = GET_NEXT(pss) ) {
			if (pss->system_info->type == SUBSYSTEM_TURRET) {
				if(pss->turret_enemy_objnum == -1)
					gr_printf_no_resize(gr_screen.center_offset_x + 10, gr_screen.center_offset_y + (t*line_height), "Turret %d: <None>", t);
				else if (Objects[pss->turret_enemy_objnum].type == OBJ_SHIP)
					gr_printf_no_resize(gr_screen.center_offset_x + 10, gr_screen.center_offset_y + (t*line_height), "Turret %d: %s", t, Ships[Objects[pss->turret_enemy_objnum].instance].ship_name);
				else
					gr_printf_no_resize(gr_screen.center_offset_x + 10, gr_screen.center_offset_y + (t*line_height), "Turret %d: <Object %d>", t, pss->turret_enemy_objnum);

				t++;
			}
		}
	}
#endif

	if ((Show_framerate && HUD_draw) || Cmdline_frame_profile || Cmdline_bmpman_usage) {

		gr_set_color_fast(&HUD_color_debug);

		if (Cmdline_frame_profile) {
			// Split frame profile into two columns if necessary to avoid losing trace data
			int fp_start_y = gr_screen.center_offset_y + 100 + line_height;
			int fp_line_limit = (gr_screen.max_h - fp_start_y) / line_height;
			size_t fp_column_break = 0;
			auto fp_trace_str = tracing::get_frame_profile_output();

			for (int i = 0; i < fp_line_limit && fp_column_break < fp_trace_str.length(); i++) {
				fp_column_break = fp_trace_str.find_first_of('\n', fp_column_break+1);
			}

			gr_string(gr_screen.center_offset_x + 20, fp_start_y, fp_trace_str.substr(0,fp_column_break).c_str(), GR_RESIZE_NONE);

			if (fp_column_break < fp_trace_str.length()) {
				gr_string(gr_screen.max_w / 2, fp_start_y, fp_trace_str.substr(fp_column_break, fp_trace_str.npos).c_str(), GR_RESIZE_NONE);
			}
		}

		if (Show_framerate) {
			if (frametotal != 0.0f)
				gr_printf_no_resize( gr_screen.center_offset_x + 20, gr_screen.center_offset_y + 100, "FPS: %0.1f", Framerate );
			else
				gr_string( gr_screen.center_offset_x + 20, gr_screen.center_offset_y + 100, "FPS: ?", GR_RESIZE_NONE );
		}

		if (Cmdline_bmpman_usage) {
			gr_printf_no_resize( gr_screen.center_offset_x + 20, gr_screen.center_offset_y + 100 - line_height, "BMPMAN: %d/%d", bmpman_count_bitmaps(), bmpman_count_available_slots() );
		}
	}

	// possibly show control checking info
	control_check_indicate();

#ifdef _WIN32
	if (Cmdline_show_stats && HUD_draw) {
		int sx,sy;
		sx = gr_screen.center_offset_x + 20;
		sy = gr_screen.center_offset_y + 100 + (line_height * 2);

		SCP_string mem_buffer;

		PROCESS_MEMORY_COUNTERS_EX process_stats;
		process_stats.cb = sizeof(process_stats);

		if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&process_stats), sizeof(process_stats))) {
			sprintf(mem_buffer, "Private Usage: " SIZE_T_ARG " Meg", static_cast<size_t>(process_stats.PrivateUsage) / 1024 / 1024);
			gr_string(sx, sy, mem_buffer.c_str(), GR_RESIZE_NONE);
			sy += line_height;

			sprintf(mem_buffer, "Working set size: " SIZE_T_ARG " Meg", static_cast<size_t>(process_stats.WorkingSetSize) / 1024 / 1024);
			gr_string(sx, sy, mem_buffer.c_str(), GR_RESIZE_NONE);
			sy += line_height;
			sy += line_height;
		}

		MEMORYSTATUSEX mem_stats;
		mem_stats.dwLength = sizeof(mem_stats);
		if (GlobalMemoryStatusEx(&mem_stats)) {
			sprintf(mem_buffer, "Physical Free: %" PRIu64 " / %" PRIu64 " Meg", mem_stats.ullAvailPhys / 1024 / 1024, mem_stats.ullTotalPhys / 1024 / 1024);
			gr_string(sx, sy, mem_buffer.c_str(), GR_RESIZE_NONE);
			sy += line_height;

			sprintf(mem_buffer, "Pagefile Free: %" PRIu64 " / %" PRIu64 " Meg", mem_stats.ullAvailPageFile / 1024 / 1024, mem_stats.ullTotalPageFile / 1024 / 1024);
			gr_string(sx, sy, mem_buffer.c_str(), GR_RESIZE_NONE);
			sy += line_height;

			sprintf(mem_buffer, "Virtual Free:  %" PRIu64 " / %" PRIu64 " Meg", mem_stats.ullAvailVirtual / 1024 / 1024, mem_stats.ullTotalVirtual / 1024 / 1024);
			gr_string(sx, sy, mem_buffer.c_str(), GR_RESIZE_NONE);
		}
	}
#endif

#ifndef NDEBUG
	if ( Show_cpu == 1 ) {
		
		int sx = gr_screen.center_offset_x + gr_screen.center_w - 154;
		int sy = gr_screen.center_offset_y + 15;

		gr_set_color_fast(&HUD_color_debug);

		gr_printf_no_resize( sx, sy, NOX("DMA: %s"), transfer_text );
		sy += line_height;
		gr_printf_no_resize( sx, sy, NOX("POLYP: %d"), modelstats_num_polys );
		sy += line_height;
		gr_printf_no_resize( sx, sy, NOX("POLYD: %d"), modelstats_num_polys_drawn );
		sy += line_height;
		gr_printf_no_resize( sx, sy, NOX("VERTS: %d"), modelstats_num_verts );
		sy += line_height;

		{

			extern int Num_pairs;		// Number of object pairs that were checked.
			gr_printf_no_resize( sx, sy, NOX("PAIRS: %d"), Num_pairs );
			sy += line_height;

			extern int Num_pairs_checked;	// What percent of object pairs were checked.
			gr_printf_no_resize( sx, sy, NOX("FVI: %d"), Num_pairs_checked );
			sy += line_height;
			Num_pairs_checked = 0;

		}

		gr_printf_no_resize( sx, sy, NOX("Snds: %d"), snd_num_playing() );
		sy += line_height;

		if ( Timing_total > 0.01f )	{
			gr_printf_no_resize(  sx, sy, NOX("CLEAR: %.0f%%"), Timing_clear*100.0f/Timing_total );
			sy += line_height;
			gr_printf_no_resize( sx, sy, NOX("REND2D: %.0f%%"), Timing_render2*100.0f/Timing_total );
			sy += line_height;
			gr_printf_no_resize( sx, sy, NOX("REND3D: %.0f%%"), Timing_render3*100.0f/Timing_total );
			sy += line_height;
			gr_printf_no_resize( sx, sy, NOX("FLIP: %.0f%%"), Timing_flip*100.0f/Timing_total );
			sy += line_height;
			gr_printf_no_resize( sx, sy, NOX("GAME: %.0f%%"), (Timing_total-(Timing_render2+Timing_render3+Timing_flip+Timing_clear))*100.0f/Timing_total );
			sy += line_height;
		}
	}

	if ( Show_player_pos ) {
		int sx = gr_screen.center_offset_x + 320;
		int sy = gr_screen.center_offset_y + 100;
		gr_printf_no_resize(sx, sy, NOX("Player Pos: (%d,%d,%d)"), fl2i(Player_obj->pos.xyz.x), fl2i(Player_obj->pos.xyz.y), fl2i(Player_obj->pos.xyz.z));
	}

	MONITOR_INC(NumPolys, modelstats_num_polys);
	MONITOR_INC(NumPolysDrawn, modelstats_num_polys_drawn );
	MONITOR_INC(NumVerts, modelstats_num_verts );

	modelstats_num_polys = 0;
	modelstats_num_polys_drawn = 0;
	modelstats_num_verts = 0;
	modelstats_num_sortnorms = 0;
#endif
}

void game_show_eye_pos(camid cid)
{
	if ( !Cmdline_show_pos )
		return;

	if(!cid.isValid())
		return;

	camera *cam = cid.getCamera();
	vec3d cam_pos = vmd_zero_vector;
	matrix cam_orient = vmd_identity_matrix;
	cam->get_info(&cam_pos, &cam_orient);

	//Do stuff
	int font_height = 2*gr_get_font_height();
	angles rot_angles;

	gr_set_color_fast(&HUD_color_debug);

	//Position
	gr_printf_no_resize(gr_screen.center_offset_x + 20, gr_screen.center_offset_y + 100 - font_height, "X:%f Y:%f Z:%f", cam_pos.xyz.x, cam_pos.xyz.y, cam_pos.xyz.z);
	font_height -= font_height/2;

	//Orientation
	vm_extract_angles_matrix(&rot_angles, &cam_orient);
	rot_angles.p *= (180/PI);
	rot_angles.b *= (180/PI);
	rot_angles.h *= (180/PI);
	gr_printf_no_resize(gr_screen.center_offset_x + 20, gr_screen.center_offset_y + 100 - font_height, "Xr:%f Yr:%f Zr:%f", rot_angles.p, rot_angles.b, rot_angles.h);
}

void game_show_standalone_framerate()
{
	float frame_rate=30.0f;
	if ( frame_int == -1 )	{
		for (float& frametime : frametimes) {
			frametime = 0.0f;
		}
		frametotal = 0.0f;
		frame_int = 0;
	}
	frametotal -= frametimes[frame_int];
	frametotal += flRealframetime;
	frametimes[frame_int] = flRealframetime;
	frame_int = (frame_int + 1 ) % FRAME_FILTER;

	if ( frametotal != 0.0 )	{
		if ( Framecount >= FRAME_FILTER ){
			frame_rate = FRAME_FILTER / frametotal;
		} else {
			frame_rate = Framecount / frametotal;
		}
	}
	std_set_standalone_fps(frame_rate);
	Framecount++;
}

/**
 * Show the time remaining in a mission.  Used only when the end-mission sexpression is used
 *
 * mission_end_time is a global from missionparse.cpp that contains the mission time at which the
 * mission should end (in fixed seconds).  There is code in missionparse.cpp which actually handles
 * checking how much time is left.
 */
void game_show_time_left()
{
	if (Mission_end_time == -1)
		return;

	int diff = f2i(Mission_end_time - Missiontime);
	// be sure to bash to 0.  diff could be negative on frame that we quit mission
	if (diff < 0)
		diff = 0;

	hud_set_default_color();
	gr_printf_no_resize( gr_screen.center_offset_x + 5, gr_screen.center_offset_y + 40, XSTR( "Mission time remaining: %d seconds", 179), diff );
}

//========================================================================================
//=================== NEW DEBUG CONSOLE COMMANDS TO REPLACE OLD DEBUG PAUSE MENU =========
//========================================================================================

#ifndef NDEBUG

DCF(ai_pause,"Pauses ai")
{
	bool process = true;
	
	if (dc_optional_string_either("help", "--help")) {
		dc_printf( "Usage: ai_paused [bool]\nSets ai_paused to true or false.  If nothing passed, then toggles it.\n" );
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		dc_printf( "ai_paused is %s\n", (ai_paused?"TRUE":"FALSE") );
		process = false;
	}

	if (!process) {
		return;
	}

	if (!dc_maybe_stuff_boolean(&ai_paused)) {
		ai_paused = !ai_paused;
	}

	if (ai_paused) {
		obj_init_all_ships_physics();
	}
}

DCF(single_step,"Enables single step mode.")
{
	bool process = true;
	
	if (dc_optional_string_either("help", "--help")) {
		dc_printf( "Usage: game_single_step [bool]\nEnables or disables single-step mode.  If nothing passed, then toggles it.\nSingle-step mode will freeze the game, and will advance frame by frame with each key press\n");
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		dc_printf( "ai_paused is %s\n", (game_single_step ? "TRUE" : "FALSE") );
		process = false;
	}

	if (!process) {
		return;
	}

	if (!dc_maybe_stuff_boolean(&game_single_step)) {
		game_single_step = !game_single_step;
	}

	last_single_step = 0;	// Make so single step waits a frame before stepping
}

DCF_BOOL(physics_pause, physics_paused)
DCF_BOOL(ai_rendering, Ai_render_debug_flag)
DCF_BOOL(ai_firing, Ai_firing_enabled )

// Create some simple aliases to these commands...
debug_command dc_s("s","shortcut for single_step",dcf_single_step);
debug_command dc_p("p","shortcut for physics_pause", dcf_physics_pause );
debug_command dc_r("r","shortcut for ai_rendering", dcf_ai_rendering );
debug_command dc_f("f","shortcut for ai_firing", dcf_ai_firing);
debug_command dc_a("a","shortcut for ai_pause", dcf_ai_pause);
#endif

//========================================================================================
//========================================================================================


void game_training_pause_do()
{
	int key = game_check_key();
	if (key > 0){
		gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
	}

	gr_flip();
}


void game_increase_skill_level()
{
	Game_skill_level++;
	if (Game_skill_level >= NUM_SKILL_LEVELS){
		Game_skill_level = 0;
	}
}

int View_percent = 100;


DCF(view, "Sets the percent of the 3d view to render.")
{
	bool process = true;
	int value;

	if (dc_optional_string_either("help", "--help")) {
		dc_printf("Usage: view [n]\nwhere n is percent of view to show (5-100).\n");
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		dc_printf("View is set to %d%%\n", View_percent );
		process = false;
	}

	if (!process) {
		return;
	}

	dc_stuff_int(&value);
	if ( (value >= 5 ) && (value <= 100) ) {
		View_percent = value;
	} else {
		dc_printf("Error: Outside legal range [5 - 100]");
	}
}


/**
 * Set the clip region for the 3d rendering window
 */
void game_reset_view_clip()
{
	Cutscene_bar_flags = CUB_NONE;
	Cutscene_delta_time = 1.0f;
	Cutscene_bars_progress = 1.0f;
}

void game_set_view_clip(float  /*frametime*/)
{
	if ((Game_mode & GM_DEAD) || (supernova_stage() >= SUPERNOVA_STAGE::HIT))
	{
		// Set the clip region for the letterbox "dead view"
		int yborder = gr_screen.max_h/4;

		if (g3_in_frame() == 0) {
			// Ensure that the bars are black
			gr_set_color(0,0,0);
			gr_set_bitmap(0); // Valathil - Don't ask me why this has to be here but otherwise the black bars don't draw
			gr_rect(0, 0, gr_screen.max_w, yborder, GR_RESIZE_NONE);
			gr_rect(0, gr_screen.max_h-yborder, gr_screen.max_w, yborder, GR_RESIZE_NONE);
		} else {
			//	Numeric constants encouraged by J "pig farmer" S, who shall remain semi-anonymous.
			// J.S. I've changed my ways!! See the new "no constants" code!!!
			gr_set_clip(0, yborder, gr_screen.max_w, gr_screen.max_h - yborder*2, GR_RESIZE_NONE );	
		}
	}
	else {
		// Set the clip region for normal view
		if ( View_percent >= 100 )	{
			gr_reset_clip();
		} else {
			int xborder, yborder;

			if ( View_percent < 5 )	{
				View_percent = 5;
			}

			float fp = i2fl(View_percent)/100.0f;
			int fi = fl2i(fl_sqrt(fp)*100.0f);
			if ( fi > 100 ) fi=100;
			
			xborder = ( gr_screen.max_w*(100-fi) )/200;
			yborder = ( gr_screen.max_h*(100-fi) )/200;

			gr_set_clip(xborder, yborder, gr_screen.max_w-xborder*2,gr_screen.max_h-yborder*2, GR_RESIZE_NONE );
		}
	}
}

extern int Tool_enabled;
int tst = 0;
time_t tst_time = 0;
int tst_big = 0;
vec3d tst_pos;
int tst_bitmap = -1;
float tst_x, tst_y;
float tst_offset, tst_offset_total;
int tst_mode;
TIMESTAMP tst_stamp;
void game_tst_frame_pre()
{
	// start tst
	if(tst == 3){
		tst = 0;

		// screen position
		vertex v;
		g3_rotate_vertex(&v, &tst_pos);
		g3_project_vertex(&v);	
	
		// offscreen
		if(!(
				(v.screen.xyw.x >= 0)
				&& (v.screen.xyw.x <= gr_screen.max_w)
				&& (v.screen.xyw.y >= 0)
				&& (v.screen.xyw.y <= gr_screen.max_h)
			))
		{
			return;
		}	

		// big ship? always tst
		if(tst_big){
			// within 3000 meters
			if( vm_vec_dist_quick(&tst_pos, &Eye_position) <= 3000.0f){
				tst = 2;				
			}
		} else {			
			// within 300 meters
			if( (vm_vec_dist_quick(&tst_pos, &Eye_position) <= 300.0f) && ((tst_time == 0) || ((time(nullptr) - tst_time) >= 10)) ){
				tst = 2;				
			} 
		}			
	}

}
void game_tst_frame()
{
	int left = 0;

	if(!Tool_enabled){
		return;
	}
	
	// setup tst
	if(tst == 2){		
		tst_time = time(nullptr);

		// load the tst bitmap		
		switch(Random::next(4)){
		case 0:			
			tst_bitmap = bm_load("ig_jim");
			left = 1;
			mprintf(("TST 0\n"));
			break;

		case 1:
			tst_bitmap = bm_load("ig_kan");
			left = 0;
			mprintf(("TST 1\n"));
			break;

		case 2:
			tst_bitmap = bm_load("ig_jim");
			left = 1;
			mprintf(("TST 2\n"));
			break;
			
		default:			
			tst_bitmap = bm_load("ig_kan");
			left = 0;
			mprintf(("TST 3\n"));
			break;
		}

		if(tst_bitmap < 0){
			tst = 0;
			return;
		}		

		// get the tst bitmap dimensions
		int w, h;
		bm_get_info(tst_bitmap, &w, &h);

		// tst y
		tst_y = frand_range(0.0f, (float)gr_screen.max_h - h);

		snd_play(gamesnd_get_interface_sound(InterfaceSounds::VASUDAN_BUP));

		// tst x and direction
		tst_mode = 0;
		if(left){
			tst_x = (float)-w;
			tst_offset_total = (float)w;
			tst_offset = (float)w;
		} else {
			tst_x = (float)gr_screen.max_w;
			tst_offset_total = (float)-w;
			tst_offset = (float)w;
		}

		tst = 1;
	}

	// run tst
	if(tst == 1){
		float diff = (tst_offset_total / 0.5f) * flFrametime;

		// move the bitmap
		if(tst_mode == 0){
			tst_x += diff;
			
			tst_offset -= fl_abs(diff);
		} else if(tst_mode == 2){
			tst_x -= diff;
			
			tst_offset -= fl_abs(diff);
		}

		// draw the bitmap
		gr_set_bitmap(tst_bitmap);
		gr_bitmap((int)tst_x, (int)tst_y, GR_RESIZE_NONE);

		if(tst_mode == 1){
			if(timestamp_elapsed_safe(tst_stamp, 1100)){
				tst_mode = 2;
			}
		} else {
			// if we passed the switch point
			if(tst_offset <= 0.0f){
				// switch modes
				switch(tst_mode){
				case 0:
					tst_mode = 1;
					tst_stamp = _timestamp(1000);
					tst_offset = fl_abs(tst_offset_total);
					break;				

				case 2:				
					tst = 0;
					return;
				}
			}				
		}
	}
}
void game_tst_mark(const object *objp, const ship *shipp)
{
	ship_info *sip;	

	if(!Tool_enabled){
		return;
	}

	// bogus
	if((objp == nullptr) || (shipp == nullptr) || (shipp->ship_info_index < 0) || (shipp->ship_info_index >= ship_info_size())){
		return;
	}
	sip = &Ship_info[shipp->ship_info_index];

	// already tst
	if(tst){
		return;
	}

	tst_pos = objp->pos;
	if(sip->is_big_or_huge()){
		tst_big = 1;
	}
	tst = 3;
}

extern void render_shields();

void player_repair_frame(float frametime)
{
	if(MULTIPLAYER_MASTER){
		for(auto & np : Net_players){

			if(MULTI_CONNECTED(np) && (Net_player != nullptr)
					&& (Net_player->player_id != np.player_id)
					&& (np.m_player != nullptr)
					&& (np.m_player->objnum >= 0)
					&& (np.m_player->objnum < MAX_OBJECTS)){

				// don't rearm/repair if the player is dead or dying/departing
				if ( !NETPLAYER_IS_DEAD((&np)) && !(Ships[Objects[np.m_player->objnum].instance].is_dying_or_departing()) ) {
					ai_do_repair_frame(&Objects[np.m_player->objnum],&Ai_info[Ships[Objects[np.m_player->objnum].instance].ai_index],frametime);
				}
			}
		}
	}	

	if ( (Player_obj != nullptr) && (Player_obj->type == OBJ_SHIP) && !(Game_mode & GM_STANDALONE_SERVER) && (Player_ship != nullptr) && !(Player_ship->flags[Ship::Ship_Flags::Dying]) ) {
		ai_do_repair_frame(Player_obj, &Ai_info[Ships[Player_obj->instance].ai_index], frametime);
	}
}

#define NUM_FRAMES_TEST		300
#define NUM_MIXED_SOUNDS	16
void do_timing_test(float frame_time)
{
	static int framecount = 0;
	static int test_running = 0;
	static float test_time = 0.0f;

	static sound_handle snds[NUM_MIXED_SOUNDS];

	if ( test_running ) {
		framecount++;
		test_time += frame_time;
		if ( framecount >= NUM_FRAMES_TEST ) {
			test_running = 0;
			nprintf(("General", "%d frames took %.3f seconds\n", NUM_FRAMES_TEST, test_time));
			for (auto& snd : snds)
				snd_stop(snd);
		}
	}

	if ( Test_begin == 1 ) {
		framecount = 0;
		test_running = 1;
		test_time = 0.0f;
		Test_begin = 0;

		// start looping digital sounds
		for (int i = 0; i < NUM_MIXED_SOUNDS; i++ )
			snds[i] = snd_play_looping( gamesnd_get_game_sound(gamesnd_id(i)), 0.0f, -1, -1);
	}
}

DCF(fov, "Change the field of view of the main camera")
{
	camera *cam = Main_camera.getCamera();
	bool process = true;
	float value;

	if (dc_optional_string_either("help", "--help")) {
		dc_printf( "Usage: fov [factor]\nFactor is the zoom factor btwn .25 and 1.25\nNo parameter resets it to default.\n" );
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		if(cam == nullptr) {
			dc_printf("Camera unavailable.");
		} else {
			dc_printf("Zoom factor set to %6.3f (original = 0.5, John = 0.75)\n", g3_get_hfov(cam->get_fov()));
		}

		process = false;
	}

	if ((cam == nullptr) || (!process)) {
		return;
	}

	if (!dc_maybe_stuff_float(&value)) {
		// No value passed, use default
		cam->set_fov(VIEWER_ZOOM_DEFAULT);
	} else {
		// Value passed, Clamp it to valid values
		if (value < 0.25f) {
			value = 0.25f;
			dc_printf("Zoom factor clamped to 0.25\n");
		} else if (value > 1.25f) {
			value = 1.25f;
			dc_printf("Zoom factor clamped to 1.25\n");
		} else {
			dc_printf("Zoom factor set to %6.3f\n", value);
		}

		cam->set_fov(value);
	}
}

DCF(framerate_cap, "Sets the framerate cap")
{
	bool process = true;

	if (dc_optional_string_either("help", "--help")) {
		dc_printf("Usage: framerate_cap [n]\nwhere n is the frames per second to cap framerate at.\n");
		dc_printf("If n is 0 or omitted, then the framerate cap is removed\n");
		dc_printf("[n] must be from 1 to 120.\n");
		process = false;
	}

	if (dc_optional_string_either("status", "--status") || dc_optional_string_either("?", "--?")) {
		if ( Framerate_cap ) {
			dc_printf("Framerate cap is set to %d fps\n", Framerate_cap );
		} else {
			dc_printf("There is no framerate cap currently active.\n");
		}

		process = false;
	}

	if (!process) {
		return;
	}

	if (!dc_maybe_stuff_int(&Framerate_cap)) {
		Framerate_cap = 0;
	}

	if ((Framerate_cap < 0) || (Framerate_cap > 120)) {
		dc_printf( "Illegal value for framerate cap. (Must be from 1-120) \n");
		Framerate_cap = 0;
	}

	if (Framerate_cap == 0) {
		dc_printf("Framerate cap disabled");
	} else {
		dc_printf("Framerate cap is set to %d fps\n", Framerate_cap );
	}
}

#define	MIN_DIST_TO_DEAD_CAMERA		50.0f
int Show_viewing_from_self = 0;

void say_view_target()
{
	object	*view_target;

	if ((Viewer_mode & VM_OTHER_SHIP) && (Player_ai->target_objnum != -1))
		view_target = &Objects[Player_ai->target_objnum];
	else
		view_target = Player_obj;

	if (Game_mode & GM_DEAD) {
		if (Player_ai->target_objnum != -1)
			view_target = &Objects[Player_ai->target_objnum];
	}

	if (!(Game_mode & GM_DEAD_DIED) && ((Game_mode & (GM_DEAD_BLEW_UP)) || ((Last_view_target != nullptr) && (Last_view_target != view_target)))) {
		if (view_target != Player_obj){

			const char *view_target_name = "";
			switch(Objects[Player_ai->target_objnum].type) {
			case OBJ_SHIP:
				if (Ships[Objects[Player_ai->target_objnum].instance].flags[Ship::Ship_Flags::Hide_ship_name]) {
					view_target_name = "targeted ship";
				} else {
					view_target_name = Ships[Objects[Player_ai->target_objnum].instance].get_display_name();
				}
				break;
			case OBJ_WEAPON:
				view_target_name = Weapon_info[Weapons[Objects[Player_ai->target_objnum].instance].weapon_info_index].get_display_name();
				Viewer_mode &= ~VM_OTHER_SHIP;
				break;
			case OBJ_JUMP_NODE: {
				view_target_name = XSTR("jump node", 184);
				Viewer_mode &= ~VM_OTHER_SHIP;
				break;
				}
			case OBJ_DEBRIS: {
				view_target_name = "Debris";
				Viewer_mode &= ~VM_OTHER_SHIP;
				break;
				}

			default:
				UNREACHABLE("Trying to view an invalid object!");
				break;
			}

			if ( view_target_name[0] ) {
				hud_set_iff_color(&Objects[Player_ai->target_objnum], 1);
				HUD_fixed_printf(0.0f, gr_screen.current_color, XSTR( "Viewing %s%s\n", 185), (Viewer_mode & VM_OTHER_SHIP) ? XSTR( "from ", 186) : "", view_target_name);
				Show_viewing_from_self = 1;
			}
		} else {
			color col;
			gr_init_color(&col, 0, 255, 0);
			if((Game_mode & GM_MULTIPLAYER) && (Net_player->flags & NETINFO_FLAG_OBSERVER) && (Player_obj->type == OBJ_OBSERVER)){
				HUD_fixed_printf(2.0f, col, "%s", XSTR( "Viewing from observer\n", 187));
				Show_viewing_from_self = 1;
			} else {
				if (Show_viewing_from_self)
					HUD_fixed_printf(2.0f, col, "%s", XSTR( "Viewing from self\n", 188));
			}
		}
	}

	Last_view_target = view_target;
}


float Game_hit_x = 0.0f;
float Game_hit_y = 0.0f;

// Reset at the beginning of each frame
void game_whack_reset()
{
	Game_hit_x = 0.0f;
	Game_hit_y = 0.0f;
}

// Apply a 2d whack to the player
void game_whack_apply( float x, float y )
{
	// Do some force feedback
	joy_ff_play_dir_effect(x * 80.0f, y * 80.0f);

	// Move the eye 
	Game_hit_x += x;
	Game_hit_y += y;
}

// call to apply a "shudder"
void game_shudder_apply(int time, float intensity, bool perpetual, bool everywhere)
{
	Game_shudder_perpetual = perpetual;
	Game_shudder_everywhere = everywhere;
	Game_shudder_time = _timestamp(time);
	Game_shudder_total = time;
	Game_shudder_intensity = intensity;
}

float get_shake(float intensity, int decay_time, int max_decay_time)
{
	int r = Random::next();

	float shake = intensity * (float)(r-Random::HALF_MAX_VALUE) * Random::INV_F_MAX_VALUE;
	
	if (decay_time >= 0 && max_decay_time > 0) {
		shake *= (0.5f - fl_abs(0.5f - (float) decay_time / (float) max_decay_time));
	}

	return shake;
}

bool is_screenshake_enabled()
{
	if (Game_mode & GM_MULTIPLAYER) {
		return true;
	} else {
		return ScreenShakeOption->getValue();
	}
}

#define FF_SCALE	10000
extern int Wash_on;
extern float sn_shudder;
void apply_view_shake(matrix *eye_orient)
{
	bool do_global_shudder = false;
	angles tangles;
	tangles.p = 0.0f;
	tangles.h = 0.0f;
	tangles.b = 0.0f;

	bool do_hud_shudder = (Viewer_obj == Player_obj);

	if ((Viewer_mode & VM_CHASE) && Apply_shudder_to_chase_view)
		do_hud_shudder = true;

	// do shakes that only affect the HUD (unless disabled by some combination of game type and settings)
	if (do_hud_shudder && is_screenshake_enabled()) {
		physics_info *pi = &Player_obj->phys_info;

		// Make eye shake due to afterburner
		if (!timestamp_elapsed(pi->afterburner_decay)) {
			tangles.p += get_shake(0.07f, timestamp_until(pi->afterburner_decay), ABURN_DECAY_TIME);
			tangles.h += get_shake(0.07f, timestamp_until(pi->afterburner_decay), ABURN_DECAY_TIME);
		}

		// Make eye shake due to engine wash
		if (Player_obj->type == OBJ_SHIP && (Ships[Player_obj->instance].wash_intensity > 0) && Wash_on ) {
			float wash_intensity = Ships[Player_obj->instance].wash_intensity * Ship_info[Ships[Player_obj->instance].ship_info_index].ship_shudder_modifier;
	
			tangles.p += get_shake(0.07f * wash_intensity, -1, 0);
			tangles.h += get_shake(0.07f * wash_intensity, -1, 0);
	
			// play the force feedback effect
			vec3d rand_vec;
			vm_vec_rand_vec_quick(&rand_vec);
			joy_ff_play_dir_effect(FF_SCALE * wash_intensity * rand_vec.xyz.x, FF_SCALE * wash_intensity * rand_vec.xyz.y);
		}

		do_global_shudder = true;
	}
	// do shakes that affect external cameras
	else {
		// Make eye shake due to supernova
		if (supernova_camera_cut()) {
			float cut_pct = supernova_sunspot_pct();
			tangles.p += get_shake(0.07f * cut_pct * sn_shudder, -1, 0);
			tangles.h += get_shake(0.07f * cut_pct * sn_shudder, -1, 0);
		}

		if (Game_shudder_everywhere) {
			do_global_shudder = true;
		}
	}

	if (do_global_shudder) {	
		// Make eye shake due to shuddering
		if (Game_shudder_perpetual) {
			tangles.p += get_shake(Game_shudder_intensity * 0.005f, -1, 0);
			tangles.h += get_shake(Game_shudder_intensity * 0.005f, -1, 0);
		} else if (Game_shudder_time.isValid()) {
			if (timestamp_elapsed(Game_shudder_time)) {
				Game_shudder_time = TIMESTAMP::invalid();
			} else {
				tangles.p += get_shake(Game_shudder_intensity * 0.005f, timestamp_until(Game_shudder_time), Game_shudder_total);
				tangles.h += get_shake(Game_shudder_intensity * 0.005f, timestamp_until(Game_shudder_time), Game_shudder_total);
			}
		}
	}

	// maybe bail
	if (fl_near_zero(tangles.p) && fl_near_zero(tangles.h) && fl_near_zero(tangles.b))
		return;

	matrix	tm, tm2;
	vm_angles_2_matrix(&tm, &tangles);
	Assert(vm_vec_mag(&tm.vec.fvec) > 0.0f);
	Assert(vm_vec_mag(&tm.vec.rvec) > 0.0f);
	Assert(vm_vec_mag(&tm.vec.uvec) > 0.0f);
	vm_matrix_x_matrix(&tm2, eye_orient, &tm);
	*eye_orient = tm2;
}

//	Player's velocity just before he blew up.  Used to keep camera target moving.
vec3d	Dead_player_last_vel = { { { 1.0f, 1.0f, 1.0f } } };

int Scripting_didnt_draw_hud = 1;

extern vec3d Dead_camera_pos;

//	Set eye_pos and eye_orient based on view mode.
camid game_render_frame_setup()
{
	if(!Main_camera.isValid())
	{
		Main_camera = cam_create("Main camera");
	}
	camera *main_cam = Main_camera.getCamera();
	if(main_cam == nullptr)
	{
		Error(LOCATION, "Unable to generate main camera");
		return camid();
	}

	vec3d	eye_pos, eye_fvec;
	matrix	eye_orient = vmd_identity_matrix;

	static int last_Viewer_mode = 0;
	static int last_Game_mode = 0;
	static fov_t last_FOV = Sexp_fov;

	bool fov_changed = ((Sexp_fov > 0.0f) && (!std::holds_alternative<float>(last_FOV) || std::get<float>(last_FOV) != Sexp_fov));

	//First, make sure we take into account 2D Missions.
	//These replace the normal player in-cockpit view with a topdown view.
	if(The_mission.flags[Mission::Mission_Flags::Mission_2d])
	{
		if(!(Viewer_mode & ~VM_CAMERA_LOCKED))
		{
			Viewer_mode = VM_TOPDOWN;
		}
	}

	// This code is supposed to detect camera "cuts"... like going between
	// different views.
	bool camera_cut = false;
	if ( (last_Viewer_mode != Viewer_mode)
		|| (last_Game_mode != Game_mode)
		|| (fov_changed)
		|| (Viewer_mode & VM_FREECAMERA))	{
		//mprintf(( "************** Camera cut! ************\n" ));
		last_Viewer_mode = Viewer_mode;
		last_Game_mode = Game_mode;
		last_FOV = main_cam->get_fov();

		// Camera moved.  Tell stars & debris to not do blurring.
		stars_camera_cut();	
		camera_cut = true;
	}

	say_view_target();

	if ( Viewer_mode & VM_PADLOCK_ANY ) {
		player_display_padlock_view();
	}

	if (Game_mode & GM_DEAD) {
		vec3d	vec_to_deader, view_pos;
		float		dist;

		Viewer_mode |= VM_DEAD_VIEW;

		if (Player_ai->target_objnum != -1) {
			int view_from_player = 1;

			if (Viewer_mode & VM_OTHER_SHIP) {
				//	View from target.
				Viewer_obj = &Objects[Player_ai->target_objnum];
				object_get_eye( &eye_pos, &eye_orient, Viewer_obj );
				view_from_player = 0;
			}

			if(Viewer_obj)
				Script_system.SetHookObject("Viewer", Viewer_obj);
			else
				Script_system.RemHookVar("Viewer");

			if ( view_from_player ) {
				//	View target from player ship.
				Viewer_obj = nullptr;
				eye_pos = Player_obj->pos;
				vm_vec_normalized_dir(&eye_fvec, &Objects[Player_ai->target_objnum].pos, &eye_pos);
				vm_vector_2_matrix_norm(&eye_orient, &eye_fvec, nullptr, nullptr);
				//rtn_cid = ship_get_followtarget_eye( Player_obj );
			}
		} else {
			dist = vm_vec_normalized_dir(&vec_to_deader, &Player_obj->pos, &Dead_camera_pos);
			
			if (dist < MIN_DIST_TO_DEAD_CAMERA)
				dist += flFrametime * 16.0f;

			vm_vec_scale(&vec_to_deader, -dist);
			vm_vec_add(&Dead_camera_pos, &Player_obj->pos, &vec_to_deader);
			
			view_pos = Player_obj->pos;

			if (!(Game_mode & GM_DEAD_BLEW_UP)) {
				Viewer_mode &= ~(VM_EXTERNAL | VM_CHASE);
				vm_vec_scale_add2(&Dead_camera_pos, &Original_vec_to_deader, 25.0f * flFrametime);
				Dead_player_last_vel = Player_obj->phys_info.vel;
				//nprintf(("AI", "Player death roll vel = %7.3f %7.3f %7.3f\n", Player_obj->phys_info.vel.x, Player_obj->phys_info.vel.y, Player_obj->phys_info.vel.z));
			} else if (Player_ai->target_objnum != -1) {
				view_pos = Objects[Player_ai->target_objnum].pos;
			} else {
				//	Make camera follow explosion, but gradually slow down.
				vm_vec_scale_add2(&Player_obj->pos, &Dead_player_last_vel, flFrametime);
				view_pos = Player_obj->pos;
				vm_vec_scale(&Dead_player_last_vel, 0.99f);
				vm_vec_scale_add2(&Dead_camera_pos, &Original_vec_to_deader, MIN(25.0f, vm_vec_mag_quick(&Dead_player_last_vel)) * flFrametime);
			}

			eye_pos = Dead_camera_pos;

			vm_vec_normalized_dir(&eye_fvec, &Player_obj->pos, &eye_pos);
			vm_vector_2_matrix_norm(&eye_orient, &eye_fvec, nullptr, nullptr);
			Viewer_obj = nullptr;
		}
	} 

	// if supernova shockwave
	if(supernova_camera_cut()){
		// no viewer obj
		Viewer_obj = nullptr;

		// call it dead view
		Viewer_mode |= VM_DEAD_VIEW;

		// set eye pos and orient
		//rtn_cid = supernova_set_view();
		supernova_get_eye(&eye_pos, &eye_orient);
	} else {	
		//	If already blown up, these other modes can override.
		if (!(Game_mode & (GM_DEAD | GM_DEAD_BLEW_UP))) {
			Viewer_mode &= ~VM_DEAD_VIEW;

			if(!(Viewer_mode & VM_FREECAMERA))
				Viewer_obj = Player_obj;
 
			if (Viewer_mode & VM_OTHER_SHIP) {
				if (Player_ai->target_objnum != -1){
					Viewer_obj = &Objects[Player_ai->target_objnum];
				} else {
					Viewer_mode &= ~VM_OTHER_SHIP;
				}
			}

			if(Viewer_mode & VM_FREECAMERA) {
				Viewer_obj = nullptr;

				if (scripting::hooks::OnCameraSetUpHook->isActive()) {
					scripting::hooks::OnCameraSetUpHook->run(scripting::hook_param_list(
						scripting::hook_param("Camera", 'o', scripting::api::l_Camera.Set(cam_get_current()))));
				}

				return cam_get_current();
			} else if (Viewer_mode & VM_EXTERNAL) {
				matrix	tm, tm2;

				vm_angles_2_matrix(&tm2, &Viewer_external_info.angles);
				vm_matrix_x_matrix(&tm, &Viewer_obj->orient, &tm2);

				Viewer_external_info.current_distance = cam_get_bbox_dist(Viewer_obj, Viewer_external_info.preferred_distance, &tm2);

				vm_vec_scale_add(&eye_pos, &Viewer_obj->pos, &tm.vec.fvec, Viewer_external_info.current_distance);

				vm_vec_normalized_dir(&eye_fvec, &Viewer_obj->pos, &eye_pos);
				vm_vector_2_matrix_norm(&eye_orient, &eye_fvec, &Viewer_obj->orient.vec.uvec, nullptr);
				Viewer_obj = nullptr;

				//	Modify the orientation based on head orientation.
				compute_slew_matrix(&eye_orient, &Viewer_slew_angles);

			} else if ( Viewer_mode & VM_CHASE ) {
				if (Viewer_obj->type != OBJ_SHIP)
					object_get_eye(&eye_pos, &eye_orient, Viewer_obj);
				else {
					vec3d aim_pt;

					vec3d tmp_up;
					matrix eyemat;
					object_get_eye(&tmp_up, &eyemat, Viewer_obj, false);	// slew is computed at the bottom of the block

					eye_pos = Viewer_obj->pos;

					// get a point far in front of the ship to point the camera at
					vm_vec_copy_scale(&aim_pt, &Viewer_obj->orient.vec.fvec, Viewer_obj->radius * 100.0f);
					vm_vec_add2(&aim_pt, &Viewer_obj->pos);

					vec3d chase_view_offset = Ship_info[Ships[Viewer_obj->instance].ship_info_index].chase_view_offset;
					if (IS_VEC_NULL(&chase_view_offset)) {
						if (Viewer_obj == Player_obj)
							chase_view_offset = vm_vec_new(0.0f, 0.625f * Viewer_obj->radius, -2.125f * Viewer_obj->radius);
						else
							chase_view_offset = vm_vec_new(0.0f, 0.75f * Viewer_obj->radius, -2.5f * Viewer_obj->radius);
					}

					// position the camera based on the offset and external camera distance
					vec3d rotated_chase_view_offset;
					vm_vec_unrotate(&rotated_chase_view_offset, &chase_view_offset, &Viewer_obj->orient);
					vm_vec_add2(&eye_pos, &rotated_chase_view_offset);
					vm_vec_scale_add2(&eye_pos, &eyemat.vec.fvec, -Viewer_chase_info.distance);
					vm_vec_scale_add2(&eye_pos, &eyemat.vec.uvec, 0.35f * Viewer_chase_info.distance);

					vec3d old_pos;
					main_cam->get_info(&old_pos, nullptr);
					bool hyperspace = (Player_ship->flags[Ship::Ship_Flags::Depart_warp] && Warp_params[Player_ship->warpout_params_index].warp_type == WT_HYPERSPACE);
					if (camera_cut || hyperspace)
						old_pos = eye_pos;

					// "push" the camera backwards in the direction of its old position based on acceleration to to make it
					// feel like the camera is trying to keep up with the ship (based on the rigidity value)
					vec3d velocity_comp = Viewer_obj->phys_info.vel;
					vm_vec_scale_add2(&velocity_comp, &Viewer_obj->phys_info.acceleration, (-5.0f / (vm_vec_mag(&Viewer_obj->phys_info.rotvel) * 2.0f + 1)) * flFrametime);
					vm_vec_scale_add2(&old_pos, &velocity_comp, flFrametime);
					vec3d eye_mov = eye_pos - old_pos;
					eye_mov *= exp(-Ship_info[Ships[Viewer_obj->instance].ship_info_index].chase_view_rigidity * flFrametime);
					eye_pos -= eye_mov;

					vm_vec_normalized_dir(&eye_fvec, &aim_pt, &eye_pos);

					// JAS: I added the following code because if you slew up using
					// Descent-style physics, tmp_dir and Viewer_obj->orient.vec.uvec are
					// equal, which causes a zero-length vector in the vm_vector_2_matrix
					// call because the up and the forward vector are the same.   I fixed
					// it by adding in a fraction of the right vector all the time to the
					// up vector.
					tmp_up = eyemat.vec.uvec;
					vm_vec_scale_add2(&tmp_up, &eyemat.vec.rvec, 0.00001f);

					vm_vector_2_matrix(&eye_orient, &eye_fvec, &tmp_up, nullptr);
					Viewer_obj = nullptr;

					//	Modify the orientation based on head orientation.
					compute_slew_matrix(&eye_orient, &Viewer_slew_angles);
				}
			} else if ( Viewer_mode & VM_WARP_CHASE ) {
					Warp_camera.get_info(&eye_pos, nullptr);

					ship * shipp = &Ships[Player_obj->instance];

					vec3d warp_pos = Player_obj->pos;
					shipp->warpout_effect->getWarpPosition(&warp_pos);
					vm_vec_normalized_dir(&eye_fvec, &warp_pos, &eye_pos);
					vm_vector_2_matrix_norm(&eye_orient, &eye_fvec, &Player_obj->orient.vec.uvec, nullptr);
					Viewer_obj = nullptr;
			} else if (Viewer_mode & VM_TOPDOWN) {
					angles rot_angles = { PI_2, 0.0f, 0.0f };
					bool position_override = false;
					if(Viewer_obj->type == OBJ_SHIP) {
						ship_info *sip = &Ship_info[Ships[Viewer_obj->instance].ship_info_index];
						if(sip->topdown_offset_def) {
							eye_pos.xyz.x = Viewer_obj->pos.xyz.x + sip->topdown_offset.xyz.x;
							eye_pos.xyz.y = Viewer_obj->pos.xyz.y + sip->topdown_offset.xyz.y;
							eye_pos.xyz.z = Viewer_obj->pos.xyz.z + sip->topdown_offset.xyz.z;
							position_override = true;
						}
					}
					if(!position_override) {
						eye_pos.xyz.x = Viewer_obj->pos.xyz.x;
						eye_pos.xyz.y = Viewer_obj->pos.xyz.y + Viewer_obj->radius * 25.0f;
						eye_pos.xyz.z = Viewer_obj->pos.xyz.z;
					}
					vm_angles_2_matrix(&eye_orient, &rot_angles);
					Viewer_obj = nullptr;
			} else {
				// get an eye position for the player object
				object_get_eye( &eye_pos, &eye_orient, Viewer_obj );
			}
		}
	}

	main_cam->set_position(&eye_pos);
	main_cam->set_rotation(&eye_orient);

	if (scripting::hooks::OnCameraSetUpHook->isActive())	{
		scripting::hooks::OnCameraSetUpHook->run(scripting::hook_param_list(
			scripting::hook_param("Camera", 'o', scripting::api::l_Camera.Set(Main_camera))));
	}

	// setup neb2 rendering
	neb2_render_setup(Main_camera);

	return Main_camera;
}

#ifndef NDEBUG
extern void ai_debug_render_stuff();
#endif

int Game_subspace_effect = 0;
DCF_BOOL( subspace, Game_subspace_effect )

void clip_frame_view();

// Does everything needed to render a frame
extern SCP_vector<object*> effect_ships;
extern SCP_vector<object*> transparent_objects;
void game_render_frame( camid cid, const vec3d* offset, const matrix* rot_offset, const fov_t* fov_override)
{
	GR_DEBUG_SCOPE("Main Frame");
	TRACE_SCOPE(tracing::RenderMainFrame);

	g3_start_frame(game_zbuffer);

	camera *cam = cid.getCamera();

	if(cam != nullptr)
	{
		vec3d eye_pos;
		matrix eye_orient;

		//Get current camera info
		cam->get_info(&eye_pos, &eye_orient);
		if (offset != nullptr) {
			vec3d offset_local;
			vm_vec_unrotate(&offset_local, offset, &eye_orient);
			eye_pos += offset_local;
		}
		if (rot_offset != nullptr) {
			//matrix m;
			//vm_copy_transpose(&m, rot_offset);
			eye_orient = *rot_offset * eye_orient;
		}

		//Handle jitter if not cutscene camera
		if( !(Viewer_mode & VM_FREECAMERA) ) {
			apply_view_shake(&eye_orient);
			cam->set_rotation(&eye_orient);		// this was added in 57fbb21c; see Mantis 1743; needed to make the target indicator shake
		}
		//If cutscene camera, handle jitter differently
		else if ( Game_shudder_everywhere ) {
			apply_view_shake(&eye_orient);		// the camera rotation shouldn't be modified for cutscenes or it will produce feedback
		}

		//Maybe override FOV from SEXP
		if(Sexp_fov <= 0.0f)
			g3_set_view_matrix(&eye_pos, &eye_orient, fov_override ? *fov_override : cam->get_fov());
		else
			g3_set_view_matrix(&eye_pos, &eye_orient, fov_override ? *fov_override : Sexp_fov);
	}
	else
	{
		g3_set_view_matrix(&vmd_zero_vector, &vmd_identity_matrix, fov_override ? *fov_override : VIEWER_ZOOM_DEFAULT);
	}

	if (!(Game_mode & GM_LAB)) {
		HUD_set_offsets();
	}

	// for multiplayer clients, call code in Shield.cpp to set up the Shield_hit array.  Have to
	// do this becaues of the disjointed nature of this system (in terms of setup and execution).
	// must be done before ships are rendered
	if ( MULTIPLAYER_CLIENT ) {
		shield_point_multi_setup();
	}

	// this needs to happen after g3_start_frame() and before the primary projection and view matrix is setup
	if ( Cmdline_env ) {
		stars_setup_environment_mapping(cid);
	}
	gr_zbuffer_clear(TRUE);

	gr_scene_texture_begin();

	neb2_render_setup(cid);

	gr_set_proj_matrix(Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
	gr_set_view_matrix(&Eye_position, &Eye_matrix);

	if ( Game_subspace_effect )	{
		stars_draw(0,0,0,1,0);
	} else {
		stars_draw(1,1,1,0,0);
	}

	shadows_render_all(Proj_fov, &Eye_matrix, &Eye_position);
	obj_render_queue_all();

	render_shields();

	if (!Trail_render_override) trail_render_all();						// render missilie trails after everything else.
	particle::render_all();					// render particles after everything else.
	

	beam_render_all();						// render all beam weapons

	// render nebula lightning
	nebl_render_all();

	// render local player nebula
	neb2_render_poofs();

	batching_render_all(false);

	gr_copy_effect_texture();

	// render all ships with shader effects on them
	SCP_vector<object*>::iterator obji = effect_ships.begin();
	for(;obji != effect_ships.end();++obji)
	{
		obj_render(*obji);
	}
	effect_ships.clear();

	// render distortions after the effect framebuffer is copied.
	batching_render_all(true);

	Shadow_override = true;

	//Draw the viewer 'cause we didn't before.
	//This is currently seperate to facilitate deferred rendering on different view/proj matrices and with different settings
	if (Viewer_obj && Viewer_obj->type == OBJ_SHIP && Viewer_obj->instance >= 0) {
		gr_end_proj_matrix();
		gr_end_view_matrix();

		gr_override_fog(true);
		GR_DEBUG_SCOPE("Render Cockpit");
		ship_render_player_ship(Viewer_obj, offset, rot_offset, fov_override);
		gr_override_fog(false);

		gr_set_proj_matrix(Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
		gr_set_view_matrix(&Eye_position, &Eye_matrix);
	}

#ifndef NDEBUG
	debug_sphere::render();
	ai_debug_render_stuff();
	extern void snd_spew_debug_info();
	snd_spew_debug_info();
#endif

	// Do the sunspot
	game_sunspot_process(flFrametime);

	gr_end_proj_matrix();
	gr_end_view_matrix();

	Shadow_override = false;
	//================ END OF 3D RENDERING STUFF ====================

	gr_scene_texture_end();

	extern int Multi_display_netinfo;
	if(Multi_display_netinfo){
		extern void multi_display_netinfo();
		multi_display_netinfo();
	}	

	game_tst_frame_pre();

#ifndef NDEBUG
	do_timing_test(flFrametime);

	extern int OO_update_index;	
	multi_rate_display(OO_update_index, gr_screen.center_offset_x + 375, gr_screen.center_offset_y);

#endif
	
	g3_end_frame();
}

//	Flip the page and time how long it took.
void game_flip_page_and_time_it()
{
	tracing::async::end(tracing::MainFrame, tracing::MainFrameScope);
	tracing::async::begin(tracing::MainFrame, tracing::MainFrameScope);

	fix t1 = timer_get_fixed_seconds();
	gr_flip();
	fix t2 = timer_get_fixed_seconds();
	fix d = t2 - t1;
	int t = (gr_screen.max_w*gr_screen.max_h*gr_screen.bytes_per_pixel)/1024;
	sprintf( transfer_text, NOX("%d MB/s"), (int)fixmuldiv(t,65,d) );
}

void game_simulation_frame()
{
	TRACE_SCOPE(tracing::Simulation);

	//Do camera stuff
	//This is for the warpout cam
	if ( Player->control_mode != PCM_NORMAL )
		Warp_camera.do_frame(flFrametime);

	//Do ingame cutscenes stuff
	if(!Time_compression_locked)
	{
		cam_do_frame(flFrametime);
	}
	else
	{
		cam_do_frame(flRealframetime);
	}

	// blow ships up in multiplayer dogfight
	if( MULTIPLAYER_MASTER && (Net_player != nullptr) && (Netgame.type_flags & NG_TYPE_DOGFIGHT) && (f2fl(Missiontime) >= 2.0f) && !dogfight_blown){
		// blow up all non-player ships
		for (auto moveup: list_range(&Ship_obj_list)){
			if (Objects[moveup->objnum].flags[Object::Object_Flags::Should_be_dead])
				continue;

			// bogus
			if((moveup->objnum < 0) || (moveup->objnum >= MAX_OBJECTS) || (Objects[moveup->objnum].type != OBJ_SHIP) || (Objects[moveup->objnum].instance < 0) || (Objects[moveup->objnum].instance >= MAX_SHIPS) || (Ships[Objects[moveup->objnum].instance].ship_info_index < 0) || (Ships[Objects[moveup->objnum].instance].ship_info_index >= ship_info_size())){
				continue;
			}

			auto shipp = &Ships[Objects[moveup->objnum].instance];
			auto sip = &Ship_info[shipp->ship_info_index];

			// only blow up small ships			
			if((sip->is_small_ship()) && (multi_find_player_by_object(&Objects[moveup->objnum]) < 0) && (shipp->team == Iff_traitor) && (Objects[moveup->objnum].net_signature != STANDALONE_SHIP_SIG) ){							
				// function to simply explode a ship where it is currently at
				ship_self_destruct( &Objects[moveup->objnum] );					
			}
		}

		dogfight_blown = 1;
	}

	// process AWACS stuff - do this first thing
	awacs_process();

	//Do autopilot stuff
	NavSystem_Do();

	// single player, set Player hits_this_frame to 0
	if ( !(Game_mode & GM_MULTIPLAYER) && Player ) {
		Player->damage_this_burst -= (flFrametime * MAX_BURST_DAMAGE  / (0.001f * BURST_DURATION));
		Player->damage_this_burst = MAX(Player->damage_this_burst, 0.0f);
	}

	// supernova
	supernova_process();
	if (supernova_stage() >= SUPERNOVA_STAGE::DEAD1) {
		return;
	}

	// fire targeting lasers now so that 
	// 1 - created this frame
	// 2 - collide this frame
	// 3 - render this frame
	// 4 - ignored and deleted next frame
	// the basic idea being that because it con be confusing to deal with them on a multi-frame basis, they are only valid for
	// frame
	ship_process_targeting_lasers();	

	// do this here so that it works for multiplayer
	if ( Viewer_obj ) {
		// get viewer direction
		int viewer_direction = PHYSICS_VIEWER_REAR;

		if(Viewer_mode == 0){
			viewer_direction = PHYSICS_VIEWER_FRONT;
		}
		if(Viewer_mode & VM_PADLOCK_UP){
			viewer_direction = PHYSICS_VIEWER_UP;
		}
		else if(Viewer_mode & VM_PADLOCK_REAR){
			viewer_direction = PHYSICS_VIEWER_REAR;
		} 
		else if(Viewer_mode & VM_PADLOCK_LEFT){
			viewer_direction = PHYSICS_VIEWER_LEFT;
		} 
		else if(Viewer_mode & VM_PADLOCK_RIGHT){
			viewer_direction = PHYSICS_VIEWER_RIGHT;
		}

		physics_set_viewer( &Viewer_obj->phys_info, viewer_direction );
	} else {
		physics_set_viewer( &Objects[Player->objnum].phys_info, PHYSICS_VIEWER_FRONT );
	}

	// evaluate mission departures and arrivals before we process all objects.
	if ( !(Game_mode & GM_MULTIPLAYER) || (MULTIPLAYER_MASTER && !multi_endgame_ending()) ) {

		// we don't want to evaluate mission stuff when any ingame joiner in multiplayer is receiving
		// ships/wing packets.
		if ( !((Game_mode & GM_MULTIPLAYER) && (Netgame.flags & NG_FLAG_INGAME_JOINING_CRITICAL))) {
			mission_parse_eval_stuff();
		}

		// if we're an observer, move ourselves separately from the standard physics
		if((Game_mode & GM_MULTIPLAYER) && (Net_player->flags & NETINFO_FLAG_OBSERVER)){
			obj_observer_move(flFrametime);
		}
		
		// move all the objects now
		obj_move_all(flFrametime);

		game_do_training_checks();

		mission_eval_goals();
	}

	// always check training objectives, even in multiplayer missions. we need to do this so that the directives gauge works properly on clients
	training_check_objectives();
	
	// do all interpolation now
	if ( MULTIPLAYER_CLIENT && !multi_endgame_ending() && !(Netgame.flags & NG_FLAG_SERVER_LOST)) {
		// client side processing of warping in effect stages
		multi_do_client_warp(flFrametime);     
	
		// client side movement of an observer
		if((Net_player->flags & NETINFO_FLAG_OBSERVER) || (Player_obj->type == OBJ_OBSERVER)){
			obj_observer_move(flFrametime);   
		}

		// move all objects - does interpolation now as well
		obj_move_all(flFrametime);
	}

	// only process the message queue when the player is "in" the game
	if ( !Pre_player_entry ){
		message_queue_process();				// process any messages send to the player
	}

	message_maybe_distort();				// maybe distort incoming message if comms damaged
	player_repair_frame(flFrametime);	//	AI objects get repaired in ai_process, called from move code...deal with player.
	player_process_pending_praise();		// maybe send off a delayed praise message to the player
	player_maybe_play_all_alone_msg();	// maybe tell the player he is all alone	

	if(!(Game_mode & GM_STANDALONE_SERVER)){		
		if ( !Pre_player_entry ){
			emp_process_local();

			hud_update_frame(flFrametime);						// update hud systems
		}

		if (!physics_paused)	{
			// Move particle system
			particle::move_all(flFrametime);
			particle::ParticleManager::get()->doFrame(flFrametime);

			// Move missile trails
			trail_move_all(flFrametime);

			// Flash the gun flashes
			shipfx_flash_do_frame(flFrametime);			

			shockwave_move_all(flFrametime);	// update all the shockwaves
		}

		// subspace missile strikes
		ssm_process();

		if ( !Pre_player_entry ){
			obj_snd_do_frame();						// update the object-linked persistant sounds

			game_maybe_update_sound_environment();
			snd_update_listener(&Eye_position, &Player_obj->phys_info.vel, &Eye_matrix);

			if ( Game_subspace_effect ) {
				game_start_subspace_ambient_sound();
			}
		}
	}

	// Kick off externally injected operations after the simulation step has finished
	executor::OnSimulationExecutor->process();
	scripting::hooks::OnSimulation->run();
}

// Maybe render and process the dead-popup
void game_maybe_do_dead_popup(float frametime)
{
	if ( popupdead_is_active() ) {
		int leave_popup=1;
		int choice = popupdead_do_frame(frametime);

		if ( Game_mode & GM_NORMAL ) {
			switch(choice) {
			case 0:
				gameseq_post_event(GS_EVENT_ENTER_GAME);
				break;

			case 1:
				gameseq_post_event(GS_EVENT_END_GAME);
				break;

			case 2:
				gameseq_post_event(GS_EVENT_START_GAME);
				break;

			// this should only happen during a red alert mission
			case 3:			
				Assertion(The_mission.flags[Mission::Mission_Flags::Red_alert], "FSO tried to replay the previous mission without first being in a Red_alert mission.");
				if (The_mission.flags[Mission::Mission_Flags::Red_alert])
				{
					// choose the previous mission
					mission_campaign_previous_mission();
				}

				gameseq_post_event(GS_EVENT_START_GAME);
				break;

			default:
				leave_popup=0;
				break;
			}
		} else {
			switch( choice ) {

			case POPUPDEAD_DO_MAIN_HALL:
				if ( !multi_quit_game(PROMPT_ALL) ) {
					leave_popup = 0;

					// reset the previous choice so this bit isn't called again next frame
					extern int Popupdead_choice;
					Popupdead_choice = -1;
				}
				break;

			case POPUPDEAD_DO_RESPAWN:				
				multi_respawn_normal();
				event_music_player_respawn();
				break;

			case POPUPDEAD_DO_OBSERVER:
				multi_respawn_observer();
				event_music_player_respawn_as_observer();
				break;

			default:
				leave_popup = 0;
				break;
			}
		}

		if ( leave_popup ) {
			popupdead_close(true);
		}
	}
}

// returns true if player is actually in a game_play stats
bool game_actually_playing()
{
	int state;

	state = gameseq_get_state();
	return (state == GS_STATE_GAME_PLAY) || (state == GS_STATE_DEATH_DIED) || (state == GS_STATE_DEATH_BLEW_UP);
}

void game_render_hud(camid cid, const fov_t* fov_override = nullptr)
{
	gr_reset_clip();

	//Since we render things twice in VR mode, frametime needs to be halved for HUD, as the HUD uses the frametime to advance ANI's and crucially, the missile lock...
	float frametime = openxr_enabled() ? flFrametime * 0.5f : flFrametime;

	if(cid.isValid()) {
		g3_start_frame(0);		// 0 = turn zbuffering off
		g3_set_view( cid.getCamera() );

		if (fov_override)
			g3_set_fov(*fov_override);

		hud_render_preprocess(frametime);

		g3_end_frame();
	}

	// main HUD rendering function
	hud_render_all(frametime);

	// Diminish the palette effect
	game_flash_diminish(flFrametime);
}

//100% blackness
void game_reset_shade_frame()
{
	Fade_type = FadeType::FI_NONE;
	gr_create_shader(&Viewer_shader, 0, 0, 0, 0);
}

void game_shade_frame(float  /*frametime*/)
{
	// only do frame shade if we are actually in a game play state
	if ( !game_actually_playing() ) {
		return;
	}

	GR_DEBUG_SCOPE("Shade frame");

	if (Fade_type != FadeType::FI_NONE) {
		Assertion(Fade_start_timestamp.isValid(), "When Fade_type is set, Fade_start_timestamp must be valid!");
		Assertion(Fade_end_timestamp.isValid(), "When Fade_type is set, Fade_end_timestamp must be valid!");
		Assertion(timestamp_compare(Fade_end_timestamp, Fade_start_timestamp) > 0, "Fade_end_timestamp must be after Fade_start_timestamp!");

		if (timestamp_elapsed(Fade_start_timestamp)) {
			int startAlpha = 0;
			int endAlpha = 0;

			if (Fade_type == FadeType::FI_FADEOUT) {
				endAlpha = 255;
			} else if (Fade_type == FadeType::FI_FADEIN) {
				startAlpha = 255;
			}

			int alpha = 0;

			if (!timestamp_elapsed(Fade_end_timestamp)) {
				int duration = timestamp_get_delta(Fade_start_timestamp, Fade_end_timestamp);
				int elapsed = timestamp_since(Fade_start_timestamp);

				alpha = fl2i( (float)startAlpha + (((float)endAlpha - (float)startAlpha) / (float)duration) * (float)elapsed );
			} else {
				//Fade finished
				Fade_type = FadeType::FI_NONE;
				Fade_start_timestamp = TIMESTAMP::invalid();
				Fade_end_timestamp = TIMESTAMP::invalid();

				alpha = endAlpha;
			}

			Viewer_shader.c = (ubyte)alpha;
		}
	}

	gr_flash_alpha(Viewer_shader.r, Viewer_shader.g, Viewer_shader.b, Viewer_shader.c);
}

const static int CUTSCENE_BAR_DIVISOR = 8;
void bars_do_frame(float frametime)
{
	if((Cutscene_bar_flags & CUB_GRADUAL) && Cutscene_bars_progress < 1.0f)
	{
		//Determine how far along we are
		Assert(Cutscene_delta_time > 0.0f);

		Cutscene_bars_progress += frametime / Cutscene_delta_time;
		if(Cutscene_bars_progress >= 1.0f)
		{
			//Reset this stuff
			Cutscene_delta_time = 1.0f;
			Cutscene_bars_progress = 1.0f;
		}

		//Figure out where the bars should be
		int yborder;
		if(Cutscene_bar_flags & CUB_CUTSCENE)
			yborder = fl2i(Cutscene_bars_progress*(gr_screen.max_h/CUTSCENE_BAR_DIVISOR));
		else
			yborder = gr_screen.max_h/CUTSCENE_BAR_DIVISOR - fl2i(Cutscene_bars_progress*(gr_screen.max_h/CUTSCENE_BAR_DIVISOR));

		if (g3_in_frame() == 0) {
			//Set rectangles
			gr_set_color(0,0,0);
			gr_set_bitmap(0); // Valathil - Don't ask me why this has to be here but otherwise the black bars don't draw
			gr_rect(0, 0, gr_screen.max_w, yborder, GR_RESIZE_NONE);
			gr_rect(0, gr_screen.max_h-yborder, gr_screen.max_w, yborder, GR_RESIZE_NONE);
		} else {
			//Set clipping
			gr_reset_clip();
			gr_set_clip(0, yborder, gr_screen.max_w, gr_screen.max_h - yborder*2, GR_RESIZE_NONE );
		}
	}
	else if(Cutscene_bar_flags & CUB_CUTSCENE)
	{
		int yborder = gr_screen.max_h/CUTSCENE_BAR_DIVISOR;

		if (g3_in_frame() == 0) {
			gr_set_color(0,0,0);
			gr_set_bitmap(0); // Valathil - Don't ask me why this has to be here but otherwise the black bars don't draw
			gr_rect(0, 0, gr_screen.max_w, yborder, GR_RESIZE_NONE);
			gr_rect(0, gr_screen.max_h-yborder, gr_screen.max_w, yborder, GR_RESIZE_NONE);
		} else {
			gr_reset_clip();
			gr_set_clip(0, yborder, gr_screen.max_w, gr_screen.max_h - (yborder*2), GR_RESIZE_NONE );
		}
	}
}

void clip_frame_view() {
	if(!Time_compression_locked) {
		// Player is dead
		game_set_view_clip(flFrametime);

		// Cutscene bars
		bars_do_frame(flRealframetime);
	} else {
		// Player is dead
		game_set_view_clip(flRealframetime);

		// Cutscene bars
		bars_do_frame(flRealframetime);
	}
}

//WMC - This does stuff like fading in and out and subtitles. Special FX?
//Basically stuff you need rendered after everything else (including HUD)
void game_render_post_frame()
{
	float frametime = flFrametime;
	if(Time_compression_locked)
	{
		frametime = flRealframetime;
	}

	subtitles_do_frame(frametime);
	game_shade_frame(frametime);
	subtitles_do_frame_post_shaded(frametime);
}

#ifndef NDEBUG
#define DEBUG_GET_TIME(x)	{ x = timer_get_fixed_seconds(); }
#define DEBUG_TIMER_SIG fix& clear_time2, fix& render3_time1, fix& render3_time2, fix& render2_time1, fix& render2_time2, fix& flip_time1, fix& flip_time2,
#define DEBUG_TIMER_CALL_CLEAN clear_time2, render3_time1, render3_time2, render2_time1, render2_time2, flip_time1, flip_time2
#define DEBUG_TIMER_CALL DEBUG_TIMER_CALL_CLEAN ,
#else
#define DEBUG_GET_TIME(x)
#define DEBUG_TIMER_SIG
#define DEBUG_TIMER_CALL_CLEAN
#define DEBUG_TIMER_CALL
#endif

void game_do_full_frame(DEBUG_TIMER_SIG const vec3d* offset = nullptr, const matrix* rot_offset = nullptr, const fov_t* fov_override = nullptr) {
	// clear the screen to black
	gr_reset_clip();
	if ((Game_detail_flags & DETAIL_FLAG_CLEAR)) {
		gr_clear();
	}

	DEBUG_GET_TIME(clear_time2)
	DEBUG_GET_TIME(render3_time1)

	camid cid = game_render_frame_setup();

	game_render_frame(cid, offset, rot_offset, fov_override);

	//Cutscene bars
	clip_frame_view();

	// save the eye position and orientation
	if (Game_mode & GM_MULTIPLAYER) {
		cid.getCamera()->get_info(&Net_player->s_info.eye_pos, &Net_player->s_info.eye_orient);
	}

	Scripting_didnt_draw_hud = 1;
	auto scripting_param_list = scripting::hook_param_list(
		scripting::hook_param("Self", 'o', Viewer_obj),
		scripting::hook_param("Player", 'o', Player_obj)
	);

	if (scripting::hooks::OnHudDraw->isActive()) {
		Scripting_didnt_draw_hud = scripting::hooks::OnHudDraw->isOverride(scripting::hooks::ObjectDrawConditions{ Viewer_obj }, scripting_param_list) ? 0 : 1;
	}

	if (Scripting_didnt_draw_hud) {
		GR_DEBUG_SCOPE("Render HUD");
		TRACE_SCOPE(tracing::RenderHUD);

		game_render_hud(cid, fov_override);
	}
	HUD_reset_clip();

	if ((Game_detail_flags & DETAIL_FLAG_HUD) && (!(Game_mode & GM_MULTIPLAYER) || ((Game_mode & GM_MULTIPLAYER) && !(Net_player->flags & NETINFO_FLAG_OBSERVER)))) {
		anim_render_all(0, flFrametime);
	}

	if (!(Viewer_mode & (VM_EXTERNAL | VM_DEAD_VIEW | VM_WARP_CHASE | VM_PADLOCK_ANY)))
	{
		TRACE_SCOPE(tracing::RenderHUDHook);

		// see also hu.isOnHudDrawCalled()
		if (scripting::hooks::OnHudDraw->isActive()) {
			if (fov_override)
				g3_set_fov(*fov_override);

			scripting::hooks::OnHudDraw->run(scripting::hooks::ObjectDrawConditions{ Viewer_obj }, scripting_param_list);
		}
	}

	// check to see if we should display the death died popup
	if (Game_mode & GM_DEAD_BLEW_UP) {
		if (Game_mode & GM_MULTIPLAYER) {
			// catch the situation where we're supposed to be warping out on this transition
			if (Net_player->flags & NETINFO_FLAG_WARPING_OUT) {
				multi_handle_sudden_mission_end();
				send_debrief_event();
			}
			else if (Player_died_popup_wait.isValid() && ui_timestamp_elapsed(Player_died_popup_wait)) {
				Player_died_popup_wait = UI_TIMESTAMP::invalid();
				popupdead_start();
			}
		}
		else {
			if (Player_died_popup_wait.isValid() && ui_timestamp_elapsed(Player_died_popup_wait)) {
				Player_died_popup_wait = UI_TIMESTAMP::invalid();
				popupdead_start();
			}
		}
	}

	DEBUG_GET_TIME(render3_time2)
	DEBUG_GET_TIME(render2_time1)

	gr_reset_clip();
	game_get_framerate();
	game_show_framerate();
	game_show_eye_pos(cid);

	game_show_time_left();

	gr_reset_clip();
	game_render_post_frame();

	game_tst_frame();

	DEBUG_GET_TIME(render2_time2)

	// maybe render and process the dead popup
	game_maybe_do_dead_popup(flFrametime);

	// If a regular popup is active, don't flip (popup code flips)
	if (!popup_running_state()) {
		if (fov_override)
			g3_set_fov(*fov_override);

		DEBUG_GET_TIME(flip_time1)
		game_flip_page_and_time_it();
		DEBUG_GET_TIME(flip_time2)
	}
}

void game_frame(bool paused)
{
#ifndef NDEBUG
	fix total_time1, total_time2;
	fix render2_time1=0, render2_time2=0;
	fix render3_time1=0, render3_time2=0;
	fix flip_time1=0, flip_time2=0;
	fix clear_time1=0, clear_time2=0;
#endif

#ifndef NDEBUG
	if (Framerate_delay) {
		int	start_time = timer_get_milliseconds();
		while (timer_get_milliseconds() < start_time + Framerate_delay)
			;
	}
#endif
	// start timing frame
	TRACE_SCOPE(tracing::MainFrame);

	if(Player_obj)
		Script_system.SetHookObject("Player", Player_obj);
	else
		Script_system.RemHookVar("Player");

	DEBUG_GET_TIME( total_time1 )

	if(paused)
	{
		// Reset the lights here or they just keep on increasing
		light_reset();
	}
	else
	{
		// var to hold which state we are in
		bool actually_playing = game_actually_playing();

		if ((!(Game_mode & GM_MULTIPLAYER)) || ((Game_mode & GM_MULTIPLAYER) && !(Net_player->flags & NETINFO_FLAG_OBSERVER))) {
			if (!(Game_mode & GM_STANDALONE_SERVER)){
				Assert( OBJ_INDEX(Player_obj) >= 0 );
			}
		}
	
		if (Pre_player_entry && Missiontime > Entry_delay_time) {
			Pre_player_entry = false;
			event_music_set_start_delay();
		}

		//	Note: These are done even before the player enters, else buffers can overflow.
		if (! (Game_mode & GM_STANDALONE_SERVER)){
			radar_frame_init();
		}

		shield_frame_init();

		if ( !Pre_player_entry && actually_playing ) {
			if (! (Game_mode & GM_STANDALONE_SERVER) ) {

				if( (!popup_running_state()) && (!popupdead_is_active()) ){
					game_process_keys();
					read_player_controls( Player_obj, flFrametime);
				}
				
				// if we're not the master, we may have to send the server-critical ship status button_info bits
				if (MULTIPLAYER_CLIENT && !(Net_player->flags & NETINFO_FLAG_OBSERVER)){
					multi_maybe_send_ship_status();
				}
			}
		}
	
		// Reset the whack stuff
		game_whack_reset();
	
		// These two lines must be outside of Pre_player_entry code,
		// otherwise too many lights are added.
		light_reset();
	
		if ((Game_mode & GM_MULTIPLAYER) && (Netgame.game_state == NETGAME_STATE_SERVER_TRANSFER)){
			return;
		}
		
		game_simulation_frame();
		
		// if not actually in a game play state, then return.  This condition could only be true in 
		// a multiplayer game.
		if (!actually_playing ) {
			Assert( Game_mode & GM_MULTIPLAYER );
			return;
		}

	}

	if (!Pre_player_entry) {
		if (! (Game_mode & GM_STANDALONE_SERVER)) {
			DEBUG_GET_TIME( clear_time1 )
			if (!openxr_enabled()) {
				game_do_full_frame(DEBUG_TIMER_CALL_CLEAN);
			}
			else {
				// This is a VR frame. Essentially like a normal frame, but we need some additional cleanup
				// 1. Make sure both eyes have identical random for stuff like screenshakes
				// 2. Save and restore anything that makes a visual delta to the last frame (currently only retail stars)
				// 3. Make sure we don't allow the rendering to mess up the lighting data
				extern std::unique_ptr<star[]> Stars, Stars_XRBuffer;
				int random = util::Random::next();

				SCP_vector<light> Lights_copy = Lights, Static_light_copy = Static_light;

				const auto& pose = openxr_start_stereo_frame();

				util::Random::seed(random);
				game_do_full_frame(DEBUG_TIMER_CALL &pose.eyes[0].offset, &pose.eyes[0].orientation, &pose.eyes[0].zoom);
				std::swap(Stars, Stars_XRBuffer);

				Lights = std::move(Lights_copy);
				Static_light = std::move(Static_light_copy);

				util::Random::seed(random);
				game_do_full_frame(DEBUG_TIMER_CALL &pose.eyes[1].offset, &pose.eyes[1].orientation, &pose.eyes[1].zoom);
				std::swap(Stars, Stars_XRBuffer);
			}
		} else {
			game_show_standalone_framerate();
		}
	}

	red_alert_maybe_move_to_next_mission();

	asteroid_frame();

	// process lightning (nebula only)
	nebl_process();

	if (Cmdline_frame_profile) {
		tracing::frame_profile_process_frame();
	}

	DEBUG_GET_TIME( total_time2 )

#ifndef NDEBUG
	// Got some timing numbers
	Timing_total = f2fl( total_time2 - total_time1 ) * 1000.0f;
	Timing_clear = f2fl( clear_time2 - clear_time1 ) * 1000.0f;
	Timing_render2 = f2fl( render2_time2- render2_time1 ) * 1000.0f;
	Timing_render3 = f2fl( render3_time2- render3_time1 ) * 1000.0f;
	Timing_flip = f2fl( flip_time2 - flip_time1 ) * 1000.0f;
#endif

}

#define	MAX_FRAMETIME	(F1_0/4)		// Frametime gets saturated at this.  Changed by MK on 11/1/97.
												//	Some bug was causing Frametime to always get saturated at 2.0 seconds after the player
												//	died.  This resulted in screwed up death sequences.

fix Last_time = 0;						// The absolute time of game at end of last frame (beginning of this frame)
fix Last_delta_time = 0;				// While game is paused, this keeps track of how much elapsed in the frame before paused.
TIMESTAMP Last_frame_timestamp = TIMESTAMP::invalid();
UI_TIMESTAMP Last_frame_ui_timestamp = UI_TIMESTAMP::invalid();
static bool Time_paused = false;

void game_time_level_init()
{
	// stop time while we're loading so that timestamps set on initialization will all be consistent, among other things
	game_stop_time();

	Missiontime = 0;
	timestamp_start_mission();
}

void game_time_level_close()
{
	// resume time so that other things, e.g. main hall, can use timestamps
	game_start_time();
}

void game_stop_time(bool by_os_focus)
{
	if (!Time_paused) {
		fix time;
		time = timer_get_fixed_seconds();
		// Save how much time progressed so far in the frame so we can
		// use it when we unpause.
		Last_delta_time = time - Last_time;		

		if (Last_delta_time < 0) {
			Last_delta_time = 0;
		}
	}
	Time_paused = true;

	// Stop the timer_tick stuff...
	// We always want to 'sudo' the change, unless this is caused by the focus, because we want the game to have priority in that case
	timestamp_pause(!by_os_focus);
}

bool game_time_is_stopped()
{
	return Time_paused;
}

void game_start_time(bool by_os_focus)
{
	if (Time_paused) {
		fix time;
		time = timer_get_fixed_seconds();
		// Take current time, and set it backwards to account for time
		// that the frame already executed, so that timer_get_fixed_seconds() - Last_time
		// will be correct when it goes to calculate the frametime next
		// frame.
		Last_time = time - Last_delta_time;		
	}
	Time_paused = false;

	// Restore the timer_tick stuff...
	// We always want to 'sudo' the change, unless this is caused by the focus, because we want the game to have priority in that case
	timestamp_unpause(!by_os_focus);
}

void lock_time_compression(bool is_locked)
{
	Time_compression_locked = is_locked;
}

void change_time_compression(float multiplier)
{
	Desired_time_compression = fl2f( f2fl(Game_time_compression) * multiplier );
	Time_compression_change_rate = 0;
}

void set_time_compression(float compression, float change_time)
{
	Desired_time_compression = fl2f(compression);

	if(change_time <= 0.0f)
	{
		Time_compression_change_rate = 0;
		return;
	}

	Time_compression_change_rate = fl2f( f2fl(Desired_time_compression - Game_time_compression) / change_time );
}

void game_set_frametime(int state)
{
	fix thistime;
	float frame_cap_diff;
	bool do_pre_player_skip = false;

	// sync all timestamps across the entire frame
	timer_start_frame();

	thistime = timer_get_fixed_seconds();

	if ( Last_time == 0 )	
		Frametime = F1_0 / 30;
	else
		Frametime = thistime - Last_time;

#ifndef NDEBUG
	fix	debug_frametime = Frametime;	//	Just used to display frametime.
#endif

	//	If player hasn't entered mission yet, make frame take 1/4 second.
	if ((Pre_player_entry) && (state == GS_STATE_GAME_PLAY)) {
		Frametime = F1_0/4;
		do_pre_player_skip = true;
	}

	Assertion( Framerate_cap > 0, "Framerate cap %d is too low. Needs to be a positive, non-zero number", Framerate_cap );

	// Cap the framerate so it doesn't get too high.
	if (!Cmdline_NoFPSCap)
	{
		fix cap;

		cap = F1_0/Framerate_cap;
		if (Frametime < cap) {
			thistime = cap - Frametime;
			os_sleep(static_cast<int>(f2fl(thistime) * 1000.0f));
			Frametime = cap;
			thistime = timer_get_fixed_seconds();
		}
	}

	if((Game_mode & GM_STANDALONE_SERVER) && 
		(f2fl(Frametime) < ((float)1.0/(float)Multi_options_g.std_framecap))){

		frame_cap_diff = ((float)1.0/(float)Multi_options_g.std_framecap) - f2fl(Frametime);		
		os_sleep(static_cast<int>(frame_cap_diff*1000)); 				
		
		thistime += fl2f((frame_cap_diff));		

		Frametime = thistime - Last_time;
   }

	// If framerate is too low, cap it.
	if (Frametime > MAX_FRAMETIME)	{
#ifndef NDEBUG
		mprintf(("Frame %2i too long!!: frametime = %.3f (%.3f)\n", Framecount, f2fl(Frametime), f2fl(debug_frametime)));
#endif
		Frametime = MAX_FRAMETIME;
	}

	flRealframetime = f2fl(Frametime);

	//Handle changes in time compression
	if(Game_time_compression != Desired_time_compression)
	{
		if (Time_compression_change_rate == 0)
		{
			Game_time_compression = Desired_time_compression;
		}
		else
		{
			bool ascending = Desired_time_compression > Game_time_compression;

			Game_time_compression += fixmul(Time_compression_change_rate, Frametime);

			if((ascending && Game_time_compression > Desired_time_compression)
				|| (!ascending && Game_time_compression < Desired_time_compression))
			{
				Game_time_compression = Desired_time_compression;
			}
		}

		timestamp_update_time_compression();
	}

	Frametime = fixmul(Frametime, Game_time_compression);

	if (Frametime <= 0)
	{
		// If the Frametime is zero or below due to Game_time_compression, set
		// the Frametime to 1 (1/65536 of a second).
		Frametime = 1;
	}

	Last_time = thistime;

	// Unlike Last_frame_ui_timestamp, it's probably ok to leave this here for the following reasons:
	// 1) Frametime-related values have always been updated in this specific function
	// 2) The only time this function is not called during a frame is when the game is paused, which is a state
	//    wherein timestamps don't need to be updated anyway (the internal Timestamp_paused_at_counter variable
	//    only needs to be set when the pause starts)
	// 3) The only place Last_frame_timestamp is used is for checking control key or button presses, and in that
	//    situation there is no check for duration, only whether the timestamp has changed
	Last_frame_timestamp = _timestamp();

	flFrametime = f2fl(Frametime);

	// before the player enters the mission, we blitz through time
	if (do_pre_player_skip)
		timestamp_adjust_seconds(flRealframetime, TIMER_DIRECTION::FORWARD);

	// wrap overall frametime if needed
	if ( FrametimeOverall > (INT_MAX - F1_0) )
		FrametimeOverall = 0;

	FrametimeOverall += Frametime;
}

fix game_get_overall_frametime()
{
	return FrametimeOverall;
}

void game_update_missiontime()
{
	Missiontime = timestamp_get_mission_time();
}

void game_do_frame(bool set_frametime)
{
	if (set_frametime) {
		game_set_frametime(GS_STATE_GAME_PLAY);
	}

	game_update_missiontime();

	if (Game_mode & GM_STANDALONE_SERVER) {
		std_multi_set_standalone_missiontime(f2fl(Missiontime));
	}

	if ( game_single_step && (last_single_step == game_single_step) ) {
		os_set_title( NOX("SINGLE STEP MODE (Pause exits, any other key steps)") );
		while( key_checkch() == 0 )
			os_sleep(10);
		os_set_title( XSTR( "FreeSpace", 171) );
		Last_time = timer_get_fixed_seconds();
	}

	last_single_step = game_single_step;

	game_frame();
}

void multi_maybe_do_frame()
{
	if ( (Game_mode & GM_MULTIPLAYER) && (Game_mode & GM_IN_MISSION) && !Multi_pause_status){
		game_do_frame(false);
	}
}

int Joymouse_button_status = 0;

// Flush all input devices
void game_flush()
{
	key_flush();
	mouse_flush();
	snazzy_flush();

	Joymouse_button_status = 0;
}

// function for multiplayer only which calls game_do_state_common() when running the
// debug console
void game_do_dc_networking()
{
	Assert( Game_mode & GM_MULTIPLAYER );

	game_do_state_common( gameseq_get_state() );
}

// Call this whenever in a loop, or when you need to check for a keystroke.
int game_check_key()
{
	int k = game_poll();

	// convert keypad enter to normal enter
	if ((k & KEY_MASK) == KEY_PADENTER)
		k = (k & ~KEY_MASK) | KEY_ENTER;

	return k;
}

bool pause_if_unfocused()
{
	if (Using_in_game_options) {
		return UnfocusedPauseOption->getValue();
	} else {
		return !Cmdline_no_unfocus_pause;
	}
}

// same as game_check_key(), except this is used while actually in the game.  Since there
// generally are differences between game control keys and general UI keys, makes sense to
// have seperate functions for each case.  If you are not checking a game control while in a
// mission, you should probably be using game_check_key() instead.
int game_poll()
{
	if (pause_if_unfocused())
	{
		// If we're in a single player game, pause it.  
		// Cyborg17 - Multiplayer *must not* have its time affected by being in the background.
		// otherwise, ship interpolation will become inaccurate.
		if (!os_foreground() && !(Game_mode & GM_MULTIPLAYER)) {
			game_stop_time(true);
			os_sleep(1);
			game_start_time(true);
			if ((gameseq_get_state() == GS_STATE_GAME_PLAY) && (!popup_active()) && (!popupdead_is_active())) {
				game_process_pause_key();

			}
		}
	}

	int k = key_inkey();

	// Move the mouse cursor with the joystick. Currently uses Joystick0
	if (os_foreground() && !io::mouse::CursorManager::get()->isCursorShown() && (Use_joy_mouse))	{
		// Move the mouse cursor with the joystick
		int mx, my;

		int raw_axis[2];

		joystick_read_raw_axis(CID_JOY0, 2, raw_axis);

		int jx = joy_get_scaled_reading(raw_axis[0]);
		int jy = joy_get_scaled_reading(raw_axis[1]);

		int dx = fl2i(f2fl(jx)*flFrametime*500.0f);
		int dy = fl2i(f2fl(jy)*flFrametime*500.0f);

		if ( dx || dy )	{
			mouse_get_real_pos( &mx, &my );
			mouse_set_pos( mx+dx, my+dy );
		}

		int j = joy_down(CC_bind(CID_JOY0, 0));
		int m = mouse_down(MOUSE_LEFT_BUTTON);

		if ( j != Joymouse_button_status )	{
			Joymouse_button_status = j;
			if ( j && (!m) )	{
				mouse_mark_button( MOUSE_LEFT_BUTTON, 1 );
			} else if ( (!j) && (m) )	{
				mouse_mark_button( MOUSE_LEFT_BUTTON, 0 );
			}
		}
	}

	// if we should be ignoring keys because of some multiplayer situations
	if((Game_mode & GM_MULTIPLAYER) && multi_ignore_controls(k)){
		return 0;
	}

	int state = gameseq_get_state();

	// If a popup is running, don't process all the Fn keys
	if( popup_active()) {
		if (state != GS_STATE_DEATH_BLEW_UP) {
			return k;
		}
	}

	switch (k) {
		case KEY_F1:
			launch_context_help();
			k = 0;
			break;

		case KEY_F2:
			// don't allow f2 while warping out in multiplayer
			if((Game_mode & GM_MULTIPLAYER) && (Net_player != nullptr) && (Net_player->flags & NETINFO_FLAG_WARPING_OUT)){
				break;
			}

			switch (state) {
				case GS_STATE_INITIAL_PLAYER_SELECT:
				case GS_STATE_OPTIONS_MENU:
				case GS_STATE_HUD_CONFIG:
				case GS_STATE_CONTROL_CONFIG:
				case GS_STATE_DEATH_DIED:
//				case GS_STATE_DEATH_BLEW_UP:	// 	DEATH_BLEW_UP might be okay but do not comment out DEATH_DIED as otherwise no clean up is performed on the dead ship
				case GS_STATE_VIEW_MEDALS:
					break;

				default:
					gameseq_post_event(GS_EVENT_OPTIONS_MENU);
					k = 0;
					break;
			}

			break;

			// hotkey selection screen -- only valid from briefing and beyond.
		case KEY_F3:	
			if ( (state == GS_STATE_TEAM_SELECT) || (state == GS_STATE_BRIEFING) || (state == GS_STATE_SHIP_SELECT) || (state == GS_STATE_WEAPON_SELECT) || (state == GS_STATE_GAME_PLAY) || (state == GS_STATE_GAME_PAUSED) ) {
				gameseq_post_event( GS_EVENT_HOTKEY_SCREEN );
				k = 0;
			}
			break;

		case KEY_DEBUGGED + KEY_F3:
			gameseq_post_event( GS_EVENT_TOGGLE_FULLSCREEN );
			break;

		case KEY_F4:
			if(Game_mode & GM_MULTIPLAYER){
				if((state == GS_STATE_GAME_PLAY) || (state == GS_STATE_MULTI_PAUSED)){
					gameseq_post_event( GS_EVENT_MISSION_LOG_SCROLLBACK );
					k = 0;
				} 
			} else {
				if ((state == GS_STATE_GAME_PLAY) || (state == GS_STATE_DEATH_DIED) || (state == GS_STATE_DEATH_BLEW_UP) || (state == GS_STATE_GAME_PAUSED) ) {
					gameseq_post_event( GS_EVENT_MISSION_LOG_SCROLLBACK );
					k = 0;
				}
			}
			break;

		case KEY_ESC | KEY_SHIFTED:
			// make sure to quit properly out of multiplayer
			if(Game_mode & GM_MULTIPLAYER){
				multi_quit_game(PROMPT_NONE);
			}

			gameseq_post_event( GS_EVENT_QUIT_GAME );
			k = 0;

			break;

		case KEY_DEBUGGED + KEY_P:			
			break;			

		case KEY_PRINT_SCRN: 
			{
				static int counter = os_config_read_uint(nullptr, "ScreenshotNum", 0);
				char tmp_name[MAX_FILENAME_LEN];

				// we could probably go with .3 here for 1,000 shots but people really need to clean out
				// their directories better than that so it's 100 for now.
				sprintf( tmp_name, NOX("screen%.4i"), counter );
				counter++;

				// we've got two character precision so we can only have 100 shots at a time, reset if needed
				//Now we have four digit precision :) -WMC
				if (counter > 9999)
				{
					//This should pop up a dialogue or something ingame.
					Warning(LOCATION, "Screenshot count has reached max of 9999. Resetting to 0.");
					counter = 0;
				}

				mprintf(( "Dumping screen to '%s'\n", tmp_name ));
				gr_print_screen(tmp_name);

				os_config_write_uint(nullptr, "ScreenshotNum", counter);
			}

			k = 0;
			break;

		case KEY_SHIFTED | KEY_ENTER: {

#if !defined(NDEBUG)

			if ( Game_mode & GM_NORMAL ){
				game_stop_time();
			}

			// if we're in multiplayer mode, do some special networking
			if(Game_mode & GM_MULTIPLAYER){
				debug_console(game_do_dc_networking);
			} else {				
				debug_console();
			}

			game_flush();

			if ( Game_mode & GM_NORMAL )
				game_start_time();

#endif

			break;
		}
	}

	return k;
}

void os_close()
{
	gameseq_post_event(GS_EVENT_QUIT_GAME);
}

// All code to process events.   This is the only place
// that you should change the state of the game.
void game_process_event( int current_state, int event )
{
	mprintf(("Got event %s (%d) in state %s (%d)\n", GS_event_text[event], event, GS_state_text[current_state], current_state));

	switch (event) {
		case GS_EVENT_SIMULATOR_ROOM:
			gameseq_set_state(GS_STATE_SIMULATOR_ROOM);
			break;

		case GS_EVENT_MAIN_MENU:
			gameseq_set_state(GS_STATE_MAIN_MENU);		
			break;

		case GS_EVENT_OPTIONS_MENU:
			gameseq_push_state( GS_STATE_OPTIONS_MENU );
			break;

		case GS_EVENT_BARRACKS_MENU:
			gameseq_set_state(GS_STATE_BARRACKS_MENU);		
			break;

		case GS_EVENT_TECH_MENU:
			gameseq_set_state(GS_STATE_TECH_MENU);		
			break;
		case GS_EVENT_LAB:
			gameseq_push_state(GS_STATE_LAB);
			break;
		case GS_EVENT_TRAINING_MENU:
			gameseq_set_state(GS_STATE_TRAINING_MENU);		
			break;

		case GS_EVENT_START_GAME:
			Select_default_ship = 0;			
			gameseq_set_state(GS_STATE_START_GAME);
			break;

		case GS_EVENT_START_GAME_QUICK:
			Select_default_ship = 1;
			gameseq_post_event(GS_EVENT_ENTER_GAME);
			break;

		case GS_EVENT_CMD_BRIEF:
			gameseq_set_state(GS_STATE_CMD_BRIEF);
			break;

		case GS_EVENT_RED_ALERT:
			gameseq_set_state(GS_STATE_RED_ALERT);
			break;

		case GS_EVENT_START_BRIEFING:
			gameseq_set_state(GS_STATE_BRIEFING);
			break;

		case GS_EVENT_DEBRIEF:
			// did we end the campaign in the main freespace 2 single player campaign?
			// (specifically, did we successfully jump out when the supernova was in progress
			// and the campaign was ending?)
			if (Campaign_ending_via_supernova && (Game_mode & GM_CAMPAIGN_MODE)/* && !stricmp(Campaign.filename, "freespace2")*/) {
				gameseq_post_event(GS_EVENT_END_CAMPAIGN);
			} else {
				gameseq_set_state(GS_STATE_DEBRIEF);
			}
			break;

		case GS_EVENT_SHIP_SELECTION:
			gameseq_set_state( GS_STATE_SHIP_SELECT );
			break;

		case GS_EVENT_WEAPON_SELECTION:
			gameseq_set_state( GS_STATE_WEAPON_SELECT );
			break;

		case GS_EVENT_ENTER_GAME:
			if (Game_mode & GM_MULTIPLAYER) {
				// if we're respawning, make sure we change the view mode so that the hud shows up
				if (current_state == GS_STATE_DEATH_BLEW_UP) {
					Viewer_mode = 0;
				}

				gameseq_set_state(GS_STATE_GAME_PLAY);
			} else {
				gameseq_set_state(GS_STATE_GAME_PLAY, 1);
			}

			// clear multiplayer button info
			extern button_info Multi_ship_status_bi;
			memset(&Multi_ship_status_bi, 0, sizeof(button_info));

			// Make hv.Player available in "On Gameplay Start" hook -zookeeper
			scripting::hooks::OnGameplayStart->run(scripting::hook_param_list(
				scripting::hook_param("Player", 'o', Player_obj, Player_obj != nullptr)
			));

			Start_time = f2fl(timer_get_approx_seconds());
			mprintf(("Entering game at time = %7.3f\n", Start_time));

			openxr_start_mission();
			break;

		case GS_EVENT_END_GAME:
			if ( (current_state == GS_STATE_GAME_PLAY) || (current_state == GS_STATE_DEATH_DIED) ||
				(current_state == GS_STATE_DEATH_BLEW_UP) ||	(current_state == GS_STATE_DEBRIEF) || (current_state == GS_STATE_MULTI_DOGFIGHT_DEBRIEF)) {
					gameseq_set_state(GS_STATE_MAIN_MENU);

			} else
				Error(LOCATION,"FSO reached an end game state from an invalid state: %d", current_state);

			break;

		case GS_EVENT_QUIT_GAME:
			main_hall_stop_music(true);
			main_hall_stop_ambient();
			gameseq_set_state(GS_STATE_QUIT_GAME);
			break;

		case GS_EVENT_GAMEPLAY_HELP:
			gameseq_push_state( GS_STATE_GAMEPLAY_HELP );
			break;

		case GS_EVENT_PAUSE_GAME:
			gameseq_push_state(GS_STATE_GAME_PAUSED);
			break;

		case GS_EVENT_DEBUG_PAUSE_GAME:
			gameseq_push_state(GS_STATE_DEBUG_PAUSED);
			break;

		case GS_EVENT_TRAINING_PAUSE:
			gameseq_push_state(GS_STATE_TRAINING_PAUSED);
			break;

		case GS_EVENT_PREVIOUS_STATE:
			gameseq_pop_state();
			break;

		case GS_EVENT_LOAD_MISSION_MENU:
			gameseq_set_state(GS_STATE_LOAD_MISSION_MENU);
			break;

		case GS_EVENT_MISSION_LOG_SCROLLBACK:
			gameseq_push_state( GS_STATE_MISSION_LOG_SCROLLBACK );
			break;

		case GS_EVENT_HUD_CONFIG:
			gameseq_push_state( GS_STATE_HUD_CONFIG );
			break;

		case GS_EVENT_CONTROL_CONFIG:
			gameseq_push_state( GS_STATE_CONTROL_CONFIG );
			break;	

		case GS_EVENT_DEATH_DIED:
			gameseq_set_state( GS_STATE_DEATH_DIED );
			break;

		case GS_EVENT_DEATH_BLEW_UP:
			if (  current_state == GS_STATE_DEATH_DIED )	{
				gameseq_set_state( GS_STATE_DEATH_BLEW_UP );
				event_music_player_death();	
			} else {
				mprintf(( "Ignoring GS_EVENT_DEATH_BLEW_UP because we're in state %d\n", current_state ));
			}
			break;

		case GS_EVENT_NEW_CAMPAIGN:
			if (!mission_load_up_campaign()){
				readyroom_continue_campaign();
			}
			break;

		case GS_EVENT_CAMPAIGN_CHEAT:
			if (!mission_load_up_campaign()){
				readyroom_continue_campaign();
			}
			break;

		case GS_EVENT_CAMPAIGN_ROOM:
			gameseq_set_state(GS_STATE_CAMPAIGN_ROOM);
			break;

		case GS_EVENT_CREDITS:
			gameseq_set_state( GS_STATE_CREDITS );
			break;

		case GS_EVENT_VIEW_MEDALS:
			gameseq_push_state( GS_STATE_VIEW_MEDALS );
			break;

		case GS_EVENT_SHOW_GOALS:
			gameseq_push_state( GS_STATE_SHOW_GOALS );	// use push_state() since we might get to this screen through a variety of states
			break;

		case GS_EVENT_HOTKEY_SCREEN:
			gameseq_push_state( GS_STATE_HOTKEY_SCREEN );	// use push_state() since we might get to this screen through a variety of states
			break;
		
	// multiplayer stuff follow these comments
		case GS_EVENT_PXO:
			gameseq_set_state( GS_STATE_PXO );
			break;

		case GS_EVENT_PXO_HELP:
			gameseq_set_state( GS_STATE_PXO_HELP );
			break;

		case GS_EVENT_MULTI_JOIN_GAME:
			gameseq_set_state( GS_STATE_MULTI_JOIN_GAME );
			break;

		case GS_EVENT_MULTI_HOST_SETUP:
			gameseq_set_state( GS_STATE_MULTI_HOST_SETUP );
			break;

		case GS_EVENT_MULTI_CLIENT_SETUP:
			gameseq_set_state( GS_STATE_MULTI_CLIENT_SETUP );
			break;

		case GS_EVENT_GOTO_VIEW_CUTSCENES_SCREEN:
			gameseq_set_state(GS_STATE_VIEW_CUTSCENES);
			break;

		case GS_EVENT_MULTI_STD_WAIT:
			gameseq_set_state( GS_STATE_MULTI_STD_WAIT );
			break;

		case GS_EVENT_STANDALONE_MAIN:
			gameseq_set_state( GS_STATE_STANDALONE_MAIN );
			break;   

		case GS_EVENT_MULTI_PAUSE:
			gameseq_push_state( GS_STATE_MULTI_PAUSED );
			break;			

		case GS_EVENT_INGAME_PRE_JOIN:
			gameseq_set_state( GS_STATE_INGAME_PRE_JOIN );
			break;
		
		case GS_EVENT_EVENT_DEBUG:
			gameseq_push_state(GS_STATE_EVENT_DEBUG);
			break;

		// Start a warpout where player automatically goes 70 no matter what
		// and can't cancel out of it.
		case GS_EVENT_PLAYER_WARPOUT_START_FORCED:
			Warpout_forced = 1;							// If non-zero, bash the player to speed and go through effect

			// Same code as in GS_EVENT_PLAYER_WARPOUT_START only ignores current mode
			Player->saved_viewer_mode = Viewer_mode;
			Player->control_mode = PCM_WARPOUT_STAGE1;
			Warpout_sound = snd_play(gamesnd_get_game_sound(GameSounds::PLAYER_WARP_OUT));
			Warpout_time = 0.0f;			// Start timer!
			break;

		case GS_EVENT_PLAYER_WARPOUT_START:
			if ( Player->control_mode != PCM_NORMAL )	{
				mprintf(( "Player isn't in normal mode; cannot warp out.\n" ));
			} else {
				Player->saved_viewer_mode = Viewer_mode;
				Player->control_mode = PCM_WARPOUT_STAGE1;
				Warpout_sound = snd_play(gamesnd_get_game_sound(GameSounds::PLAYER_WARP_OUT));
				Warpout_time = 0.0f;			// Start timer!
				Warpout_forced = 0;				// If non-zero, bash the player to speed and go through effect
			}
			break;

		case GS_EVENT_PLAYER_WARPOUT_STOP:
			if ( Player->control_mode != PCM_NORMAL )	{
				if ( !Warpout_forced )	{		// cannot cancel forced warpout
					Player->control_mode = PCM_NORMAL;
					Viewer_mode = Player->saved_viewer_mode;
					hud_subspace_notify_abort();
					mprintf(( "Player put back to normal mode.\n" ));
				    if (Warpout_sound.isValid()) {
					    snd_stop( Warpout_sound );
					    Warpout_sound = sound_handle::invalid();
				    }
				}
			}
			break;

		case GS_EVENT_PLAYER_WARPOUT_DONE_STAGE1:		// player ship got up to speed
			if ( Player->control_mode != PCM_WARPOUT_STAGE1 )	{
				gameseq_post_event( GS_EVENT_PLAYER_WARPOUT_STOP );
				mprintf(( "Player put back to normal mode, because of invalid sequence in stage1.\n" ));
			} else {
				mprintf(( "Hit target speed.  Starting warp effect and moving to stage 2!\n" ));
				shipfx_warpout_start( Player_obj );
				Player->control_mode = PCM_WARPOUT_STAGE2;

				if (!(The_mission.ai_profile->flags[AI::Profile_Flags::No_warp_camera])) {
					Player->saved_viewer_mode = Viewer_mode;
					Viewer_mode |= VM_WARP_CHASE;
					Warp_camera = warp_camera(Player_obj);
				}
			}
			break;

		case GS_EVENT_PLAYER_WARPOUT_DONE_STAGE2:		// player ship got into the warp effect
			if ( Player->control_mode != PCM_WARPOUT_STAGE2 )	{
				gameseq_post_event( GS_EVENT_PLAYER_WARPOUT_STOP );
				mprintf(( "Player put back to normal mode, because of invalid sequence in stage2.\n" ));
			} else {
				mprintf(( "Hit warp effect.  Moving to stage 3!\n" ));
				Player->control_mode = PCM_WARPOUT_STAGE3;
			}
			break;

		case GS_EVENT_PLAYER_WARPOUT_DONE:	// player ship got through the warp effect
			mprintf(( "Player warped out.  Going to debriefing!\n" ));
			Player->control_mode = PCM_NORMAL;
			Viewer_mode = Player->saved_viewer_mode;
		    Warpout_sound        = sound_handle::invalid();

		    send_debrief_event();
			break;

		case GS_EVENT_STANDALONE_POSTGAME:
			gameseq_set_state(GS_STATE_STANDALONE_POSTGAME);
			break;

		case GS_EVENT_INITIAL_PLAYER_SELECT:
			gameseq_set_state(GS_STATE_INITIAL_PLAYER_SELECT);
			break;

		case GS_EVENT_GAME_INIT:
			// see if the command line option has been set to use the last pilot, and act accordingly
			if( player_select_get_last_pilot() ) {	
				// always enter the main menu -- do the automatic network startup stuff elsewhere
				// so that we still have valid checks for networking modes, etc.
				gameseq_set_state(GS_STATE_MAIN_MENU);
			} else {
				gameseq_set_state(GS_STATE_INITIAL_PLAYER_SELECT);
			}
			break;

		case GS_EVENT_MULTI_MISSION_SYNC:
			gameseq_set_state(GS_STATE_MULTI_MISSION_SYNC);
			break;		

		case GS_EVENT_MULTI_START_GAME:
			gameseq_set_state(GS_STATE_MULTI_START_GAME);
			break;

		case GS_EVENT_MULTI_HOST_OPTIONS:
			gameseq_set_state(GS_STATE_MULTI_HOST_OPTIONS);
			break;

		case GS_EVENT_MULTI_DOGFIGHT_DEBRIEF:
			gameseq_set_state(GS_STATE_MULTI_DOGFIGHT_DEBRIEF);
			break;

		case GS_EVENT_TEAM_SELECT:
			gameseq_set_state(GS_STATE_TEAM_SELECT);
			break;

		case GS_EVENT_END_CAMPAIGN:			
			gameseq_set_state(GS_STATE_END_OF_CAMPAIGN);
			break;		

		case GS_EVENT_LOOP_BRIEF:
			gameseq_set_state(GS_STATE_LOOP_BRIEF);
			break;

		case GS_EVENT_FICTION_VIEWER:
			gameseq_set_state(GS_STATE_FICTION_VIEWER);
			break;

		case GS_EVENT_SCRIPTING:
			gameseq_set_state(GS_STATE_SCRIPTING);
			break;

		case GS_EVENT_SCRIPTING_MISSION:
			gameseq_push_state(GS_STATE_SCRIPTING_MISSION);
			break;

		case GS_EVENT_INGAME_OPTIONS:
			gameseq_push_state(GS_STATE_INGAME_OPTIONS);
			break;

		default:
			Error(LOCATION, "FSO does not have a valid game state to set. It tried to set %d", event);
			break;
	}
}

static bool going_to_briefing_state(int new_state)
{
	switch (new_state) {
		case GS_STATE_TEAM_SELECT:
		case GS_STATE_SHIP_SELECT:
		case GS_STATE_WEAPON_SELECT:
		case GS_STATE_OPTIONS_MENU:
		case GS_STATE_GAMEPLAY_HELP:
		case GS_STATE_HOTKEY_SCREEN:
		case GS_STATE_MULTI_MISSION_SYNC:
		case GS_STATE_BRIEFING:
		case GS_STATE_CMD_BRIEF:
		case GS_STATE_RED_ALERT:
		case GS_STATE_FICTION_VIEWER:
			return true;
	}

	return false;
}

static bool going_to_menu_state(int new_state)
{
	switch (new_state) {
		case GS_STATE_MAIN_MENU:
		case GS_STATE_PXO:
		case GS_STATE_MULTI_JOIN_GAME:
		case GS_STATE_STANDALONE_MAIN:
		case GS_STATE_QUIT_GAME:
			return true;
	}

	return false;
}

// Called when a state is being left.
// The current state is still at old_state, but as soon as
// this function leaves, then the current state will become
// new state.     You should never try to change the state
// in here... if you think you need to, you probably really
// need to post an event, not change the state.
void game_leave_state( int old_state, int new_state )
{
	events::GameLeaveState(old_state, new_state);

	int end_mission = 1;

	switch (new_state) {
		case GS_STATE_GAME_PAUSED:
		case GS_STATE_DEBUG_PAUSED:
		case GS_STATE_OPTIONS_MENU:
		case GS_STATE_CONTROL_CONFIG:		
		case GS_STATE_MISSION_LOG_SCROLLBACK:
		case GS_STATE_DEATH_DIED:
		case GS_STATE_SHOW_GOALS:
		case GS_STATE_HOTKEY_SCREEN:		
		case GS_STATE_MULTI_PAUSED:
		case GS_STATE_TRAINING_PAUSED:
		case GS_STATE_EVENT_DEBUG:				
		case GS_STATE_GAMEPLAY_HELP:
		case GS_STATE_LAB:
		case GS_STATE_SCRIPTING_MISSION:
		case GS_STATE_INGAME_OPTIONS:
			end_mission = 0;  // these events shouldn't end a mission
			break;
	}

	// This is kind of a hack but it ensures options are logged even if scripting calls for a state change with an override active
	if (old_state == GS_STATE_OPTIONS_MENU) {
			if (new_state != GS_STATE_CONTROL_CONFIG && new_state != GS_STATE_HUD_CONFIG) {
				// print the options settings again to log any player-made changes
				options::OptionsManager::instance()->printValues();
			}
	}

	if (scripting::hooks::OnStateAboutToEndHook->isActive() || scripting::hooks::OnStateEndHook->isActive())
	{
		auto script_param_list = scripting::hook_param_list(
			scripting::hook_param("OldState", 'o', scripting::api::l_GameState.Set(scripting::api::gamestate_h(old_state))),
			scripting::hook_param("NewState", 'o', scripting::api::l_GameState.Set(scripting::api::gamestate_h(new_state))));

		if (scripting::hooks::OnStateAboutToEndHook->isActive())
			scripting::hooks::OnStateAboutToEndHook->run(script_param_list);

		if (scripting::hooks::OnStateEndHook->isActive() && scripting::hooks::OnStateEndHook->isOverride(script_param_list))
		{
			scripting::hooks::OnStateEndHook->run(std::move(script_param_list));
			return;
		}
	}

	//WMC - Clear scripting bitmaps
	Script_system.UnloadImages();

	switch (old_state) {
		case GS_STATE_BRIEFING:
			brief_stop_voices();

			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();

				if ( going_to_menu_state(new_state) ) {
					freespace_stop_mission();
				}
			}

			// COMMAND LINE OPTION
			if (Cmdline_multi_stream_chat_to_file){
				cfwrite_string(NOX("-------------------------------------------\n"),Multi_chat_stream);
				cfclose(Multi_chat_stream);
			}
			break;

		case GS_STATE_DEBRIEF:
			if ( (new_state != GS_STATE_VIEW_MEDALS) && (new_state != GS_STATE_OPTIONS_MENU) ) {
				debrief_close();				
				fsspeech_stop();
				common_maybe_play_cutscene(MOVIE_POST_DEBRIEF);
			}
			break;

		case GS_STATE_MULTI_DOGFIGHT_DEBRIEF:
			multi_df_debrief_close();
			break;

		case GS_STATE_LOAD_MISSION_MENU:
			mission_load_menu_close();
			break;

		case GS_STATE_SIMULATOR_ROOM:
			sim_room_close();
			break;

		case GS_STATE_CAMPAIGN_ROOM:
			campaign_room_close();
			break;

		case GS_STATE_CMD_BRIEF:
			if (new_state == GS_STATE_OPTIONS_MENU) {
				cmd_brief_hold();
			} else {
				cmd_brief_close();

				if ( !going_to_briefing_state(new_state) ) {
					common_select_close();

					if ( going_to_menu_state(new_state) ) {
						freespace_stop_mission();
					}
				}
			}
			break;

		case GS_STATE_RED_ALERT:
			red_alert_close();

			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();

				if ( going_to_menu_state(new_state) ) {
					freespace_stop_mission();
				}
			}
			break;

		case GS_STATE_SHIP_SELECT:
			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();

				if ( going_to_menu_state(new_state) ) {
					freespace_stop_mission();
				}
			}
			break;

		case GS_STATE_WEAPON_SELECT:
			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();

				if ( going_to_menu_state(new_state) ) {
					freespace_stop_mission();
				}
			}
			break;

		case GS_STATE_TEAM_SELECT:
			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();

				if ( going_to_menu_state(new_state) ) {
					freespace_stop_mission();
				}
			}
			break;

		case GS_STATE_MAIN_MENU:
			main_hall_close();
			break;

		case GS_STATE_OPTIONS_MENU:
			if(new_state == GS_STATE_MULTI_JOIN_GAME){
				multi_join_clear_game_list();
			}

			options_menu_close();

			// can't use going_to_briefing_state() here since it doesn't catch sub-optionmenu states
			// it's basically just to catch the exit game button
			if ( going_to_menu_state(new_state) ) {
				common_select_close();
			}

			if (new_state != GS_STATE_CONTROL_CONFIG && new_state != GS_STATE_HUD_CONFIG) {
				// unpause all sounds, since we could be headed back to the game
				// only unpause if we're in-mission; we could also be in the main hall
				if (Game_mode & GM_IN_MISSION) {
					weapon_unpause_sounds();
					audiostream_unpause_all();
					message_resume_all();

					// multi doesn't pause here so time keeps going
					if ( !(Game_mode & GM_MULTIPLAYER) ) {
						game_start_time();
					}
				}
			}
			break;

		case GS_STATE_BARRACKS_MENU:
			if(new_state != GS_STATE_VIEW_MEDALS){
				barracks_close();
			}
			break;

		case GS_STATE_MISSION_LOG_SCROLLBACK:
			hud_scrollback_close();
			break;

		case GS_STATE_TRAINING_MENU:
			training_menu_close();
			break;

		case GS_STATE_GAME_PLAY:
			tracing::async::end(tracing::MainFrame, tracing::MainFrameScope);

			if ( !(Game_mode & GM_STANDALONE_SERVER) ) {
				player_save_target_and_weapon_link_prefs();
				game_stop_looped_sounds();
			}

			sound_env_disable();
			joy_ff_stop_effects();

			// stop game time under certain conditions
			if ( end_mission || 
				((Game_mode & GM_NORMAL) && (new_state != GS_STATE_DEATH_DIED) && (new_state != GS_STATE_DEATH_BLEW_UP)) ||
				((Game_mode & GM_MULTIPLAYER) && (new_state == GS_STATE_MULTI_PAUSED)) ){
				game_stop_time();
			}

			if (end_mission) {
				// when in multiplayer and going back to the main menu, send a leave game packet
				// right away (before calling stop mission).  stop_mission was taking to long to
				// close mission down and I want people to get notified ASAP.
				if ( (Game_mode & GM_MULTIPLAYER) && (new_state == GS_STATE_MAIN_MENU) ){
					multi_quit_game(PROMPT_NONE);
				}

				freespace_stop_mission();

				// when going straight from the mission into the pxo state, make sure to clear the mission.
				if (new_state == GS_STATE_PXO) {
					gr_clear();
					gr_flip();
					gr_clear();
					gr_flip();
				}

				if (Cmdline_benchmark_mode) {
					gameseq_post_event( GS_EVENT_QUIT_GAME );
				}
			}
			io::mouse::CursorManager::get()->showCursor(true);
			break;

		case GS_STATE_TECH_MENU:
			techroom_close();
			break;

		case GS_STATE_TRAINING_PAUSED:
			Training_num_lines = 0;
			// fall through to GS_STATE_GAME_PAUSED
			FALLTHROUGH;

		case GS_STATE_GAME_PAUSED:
			game_start_time();
			if ( end_mission ) {
				pause_close();
			}
			break;

		case GS_STATE_DEBUG_PAUSED:
			#ifndef NDEBUG
				game_start_time();
				pause_debug_close();
			#endif
			break;

		case GS_STATE_HUD_CONFIG:
			hud_config_close();
			break;

		case GS_STATE_PXO:
			if (new_state != GS_STATE_PXO_HELP) {
				multi_pxo_close();
			}
			break;

		case GS_STATE_PXO_HELP:
			multi_pxo_help_close();
			break;

		// join/start a game
		case GS_STATE_MULTI_JOIN_GAME:
			if(new_state != GS_STATE_OPTIONS_MENU){
				multi_join_game_close();
			}
			break;

		case GS_STATE_MULTI_HOST_SETUP:
		case GS_STATE_MULTI_CLIENT_SETUP:
			// if this is just the host going into the options screen, don't do anything
			if((new_state == GS_STATE_MULTI_HOST_OPTIONS) || (new_state == GS_STATE_OPTIONS_MENU)){
				break;
			}

			// close down the proper state
			if(old_state == GS_STATE_MULTI_HOST_SETUP){
				multi_create_game_close();
			} else {
				multi_game_client_setup_close();
			}

			// COMMAND LINE OPTION
			if (Cmdline_multi_stream_chat_to_file){
				if( (new_state != GS_STATE_TEAM_SELECT) && (Multi_chat_stream!=nullptr) ) {
					cfwrite_string(NOX("-------------------------------------------\n"),Multi_chat_stream);
					cfclose(Multi_chat_stream);
				}
			}			
			break;

		case GS_STATE_CONTROL_CONFIG:
			control_config_close();
			break;

		case GS_STATE_DEATH_DIED:
		case GS_STATE_DEATH_BLEW_UP:
			if (end_mission) {
				if (old_state == GS_STATE_DEATH_DIED) {
					Game_mode &= ~GM_DEAD_DIED;
				} else if (old_state == GS_STATE_DEATH_BLEW_UP) {
					Game_mode &= ~GM_DEAD_BLEW_UP;
				}

				if (new_state != GS_STATE_DEATH_BLEW_UP) {
					if ( !(Game_mode & GM_MULTIPLAYER) ) {
						popupdead_close();			// it's usually closed by this point, but not always (e.g. if end-mission is called)
						freespace_stop_mission();
					} else {
						// early end while respawning or blowing up in a multiplayer game
						// if we are not respawing as an observer or as a player, our new state will not
						// be gameplay state.
						if ( (new_state != GS_STATE_GAME_PLAY) && (new_state != GS_STATE_MULTI_PAUSED) ) {
							game_stop_time();
							popupdead_close();
							freespace_stop_mission();
						}
					}
				}
			}
			break;

		case GS_STATE_CREDITS:
			credits_close();
			main_hall_start_music();
			break;

		case GS_STATE_VIEW_MEDALS:
			medal_main_close();
			break;

		case GS_STATE_SHOW_GOALS:
			mission_show_goals_close();
			break;

		case GS_STATE_HOTKEY_SCREEN:
			if ( new_state != GS_STATE_OPTIONS_MENU ) {
				mission_hotkey_close();
			}
			break;

		case GS_STATE_MULTI_MISSION_SYNC:
			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();
			}

			// if we're moving into the options menu we don't need to do anything else
			if(new_state == GS_STATE_OPTIONS_MENU){	
				break;
			}

			Assert( Game_mode & GM_MULTIPLAYER );
			multi_sync_close();

			if ( new_state == GS_STATE_GAME_PLAY ){
				// palette_restore_palette();

				// change a couple of flags to indicate our state!!!
				Net_player->state = NETPLAYER_STATE_IN_MISSION;
				send_netplayer_update_packet();

				// set the game mode
				Game_mode |= GM_IN_MISSION;

				// any other state needs to close out the mission, because we're not going in to gameplay.
			} else if ( going_to_menu_state(new_state) ) {
				freespace_stop_mission();
			}

			main_hall_stop_music(true);
			main_hall_stop_ambient();
			break;		
   
		case GS_STATE_VIEW_CUTSCENES:
			cutscenes_screen_close();
			break;

		case GS_STATE_MULTI_STD_WAIT:
			multi_standalone_wait_close();
			break;

		case GS_STATE_STANDALONE_MAIN:			
			standalone_main_close();
			if(new_state == GS_STATE_MULTI_STD_WAIT){		
				init_multiplayer_stats();										
			}			
			break;

		case GS_STATE_STANDALONE_POSTGAME:
			multi_standalone_postgame_close();
			break;

		case GS_STATE_MULTI_PAUSED:
			multi_pause_close(end_mission);
			break;			

		case GS_STATE_INGAME_PRE_JOIN:
			multi_ingame_select_close();
			break;

		case GS_STATE_INITIAL_PLAYER_SELECT:			
			player_select_close();			
			break;		

		case GS_STATE_MULTI_START_GAME:
			multi_start_game_close();
			break;

		case GS_STATE_MULTI_HOST_OPTIONS:
			multi_host_options_close();
			break;				

		case GS_STATE_END_OF_CAMPAIGN:
			mission_campaign_end_close();
			break;

		case GS_STATE_LOOP_BRIEF:
			loop_brief_close();
			break;

		case GS_STATE_FICTION_VIEWER:
			fiction_viewer_close();

			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();

				if ( going_to_menu_state(new_state) ) {
					freespace_stop_mission();
				}
			}
			break;

		case GS_STATE_LAB:
			lab_close();
			// restore default cursor and enable it --wookieejedi
			if (!Is_standalone) {
				io::mouse::Cursor* cursor = io::mouse::CursorManager::get()->loadCursor("cursor", true);
				if (cursor) {
					io::mouse::CursorManager::get()->setCurrentCursor(cursor);
				}
			}
			break;

		case GS_STATE_SCRIPTING:
		case GS_STATE_SCRIPTING_MISSION:
			// this can happen because scripting can be done in odd places.
			if ( !going_to_briefing_state(new_state) ) {
				common_select_close();

				if ( going_to_menu_state(new_state) ) {
					freespace_stop_mission();
				}
			}
			scripting_state_close();
			break;

		case GS_STATE_INGAME_OPTIONS:
			ingame_options_close();
			// restore default cursor and enable it --wookieejedi
			if (!Is_standalone) {
				io::mouse::Cursor* cursor = io::mouse::CursorManager::get()->loadCursor("cursor", true);
				if (cursor) {
					io::mouse::CursorManager::get()->setCurrentCursor(cursor);
				}
			}
			break;
	}

	//WMC - Now run scripting stuff
	if (scripting::hooks::OnStateEndHook->isActive())
	{
		auto script_param_list = scripting::hook_param_list(
			scripting::hook_param("OldState", 'o', scripting::api::l_GameState.Set(scripting::api::gamestate_h(old_state))),
			scripting::hook_param("NewState", 'o', scripting::api::l_GameState.Set(scripting::api::gamestate_h(new_state))));

		scripting::hooks::OnStateEndHook->run(std::move(script_param_list));
	}
}

// variable used for automatic netgame starting/joining
int Main_hall_netgame_started = 0;

// Called when a state is being entered.
// The current state is set to the state we're entering at
// this point, and old_state is set to the state we're coming
// from.    You should never try to change the state
// in here... if you think you need to, you probably really
// need to post an event, not change the state.

void game_enter_state( int old_state, int new_state )
{
	events::GameEnterState(old_state, new_state);

	using namespace scripting::api;

	auto script_param_list = scripting::hook_param_list(
		scripting::hook_param("OldState", 'o', l_GameState.Set(gamestate_h(old_state))),
		scripting::hook_param("NewState", 'o', l_GameState.Set(gamestate_h(new_state))));

	if(scripting::hooks::OnStateStart->isActive()) {
		if (scripting::hooks::OnStateStart->isOverride(script_param_list)) {
			scripting::hooks::OnStateStart->run(script_param_list);
			return;
		}
	}

	switch (new_state) {
		case GS_STATE_MAIN_MENU: {
			// in multiplayer mode, be sure that we are not doing networking anymore.
			if ( Game_mode & GM_MULTIPLAYER ) {
				Assert( Net_player != nullptr );
				Net_player->flags &= ~NETINFO_FLAG_DO_NETWORKING;
			}

			// remove any multiplayer flags from the game mode
			Game_mode &= ~(GM_MULTIPLAYER);

			// set the game_mode based on the type of player
			Assert( Player != nullptr );

			if ( Player->flags & PLAYER_FLAGS_IS_MULTI ){
				Game_mode = GM_MULTIPLAYER;
			} else {
				Game_mode = GM_NORMAL;
			}

			// determine which ship this guy is currently based on
			// if we are seeing the main hall for the first time, allow falling back when the current campaign isn't available
			const auto result = mission_load_up_campaign(old_state == GS_STATE_INITIAL_PLAYER_SELECT);

			// if there was a problem, pass an empty main hall which will set up appropriate defaults
			if (result != 0) {
				main_hall_init("");
			}
			// if we're coming from the end of a campaign, we want to load the first mainhall of the campaign
			else if (Campaign.next_mission == -1) {
				main_hall_init(Campaign.missions[0].main_hall);
			}
			// otherwise load the mainhall for the mission the player's up to
			else {
				main_hall_init(Campaign.missions[Campaign.next_mission].main_hall);
			}

			if ( (Cmdline_start_netgame || (Cmdline_connect_addr != nullptr)) && (!Main_hall_netgame_started) /*&& (Game_mode == GM_MULTIPLAYER)*/) { // DTP added "&& (game_mode == GM_multiplayer)" so that ppl don't get thrown into Multiplayer with a Singleplayer Pilot.
				Main_hall_netgame_started = 1;
				main_hall_do_multi_ready();

				if (Cmdline_start_mission) {
					mprintf(( "Ignoring the -start_mission commandline because it is incompatible with multiplayer.\n"));
					Cmdline_start_mission = nullptr;
				}

			} else if(Cmdline_start_mission) {
				strcpy_s(Game_current_mission_filename, Cmdline_start_mission);
				mprintf(( "Straight to mission '%s'\n", Game_current_mission_filename ));

				// force to singleplayer, because this is a singleplayer only commmandline option
				if (Game_mode & GM_MULTIPLAYER) {
					Game_mode = GM_NORMAL;
					mprintf(( "Forcing to single player mode.  Multiplayer is not compatible with the -start_mission commandline.\n" ));
				}

				gameseq_post_event(GS_EVENT_START_GAME);
				// This stops the mission from loading again when you go back to the hall
				Cmdline_start_mission = nullptr;
			}
			break;
		}

		case GS_STATE_START_GAME:
			main_hall_stop_music(true);
			main_hall_stop_ambient();
			
			if (Game_mode & GM_NORMAL) {
				// this should put us into a new state on failure!
				if (!game_start_mission())
					break;
			}

			// maybe play a movie before the mission
			mission_campaign_maybe_play_movie(CAMPAIGN_MOVIE_PRE_MISSION);

			// determine where to go next
			if (mission_has_fiction()) {
				gameseq_post_event(GS_EVENT_FICTION_VIEWER);
			} else if (mission_has_cmd_brief()) {
				gameseq_post_event(GS_EVENT_CMD_BRIEF);
			} else if (red_alert_mission()) {
				gameseq_post_event(GS_EVENT_RED_ALERT);
			} else {
				gameseq_post_event(GS_EVENT_START_BRIEFING);
			}
			break;

		case GS_STATE_FICTION_VIEWER:
			// init some elements common to ship/weapon select (to be done before cutscene)
			common_select_init();
			common_maybe_play_cutscene(MOVIE_PRE_FICTION); 	
			fiction_viewer_init();
			break;

		case GS_STATE_CMD_BRIEF: {
			if (old_state == GS_STATE_OPTIONS_MENU) {
				cmd_brief_unhold();
			} else {
				// init some elements common to ship/weapon select (to be done before cutscene)
				common_select_init();
				common_maybe_play_cutscene(MOVIE_PRE_CMD_BRIEF); 	
				int team_num = 0;  // team number used as index for which cmd brief to use.
				cmd_brief_init(team_num);
			}
			break;
		}

		case GS_STATE_RED_ALERT:
			// init some elements common to ship/weapon select (to be done before cutscene)
			common_select_init();
			common_maybe_play_cutscene(MOVIE_PRE_BRIEF); 	
			red_alert_init();
			break;

		case GS_STATE_BRIEFING:
			// init some elements common to ship/weapon select (to be done before cutscene)
			common_select_init();

			if ( (old_state != GS_STATE_TEAM_SELECT) && (old_state != GS_STATE_SHIP_SELECT) &&
				 (old_state != GS_STATE_WEAPON_SELECT) && (old_state != GS_STATE_OPTIONS_MENU) &&
				 (old_state != GS_STATE_GAMEPLAY_HELP) )
			{
				common_maybe_play_cutscene(MOVIE_PRE_BRIEF);
			}
			brief_init();
			break;

		case GS_STATE_DEBRIEF:
			game_stop_looped_sounds();
			mission_goal_fail_incomplete();				// fail all incomplete goals before entering debriefing
			hud_config_as_player();
			if ( (old_state != GS_STATE_VIEW_MEDALS) && (old_state != GS_STATE_OPTIONS_MENU) ){
				common_maybe_play_cutscene(MOVIE_PRE_DEBRIEF); 	
				debrief_init();
			}
			break;

		case GS_STATE_MULTI_DOGFIGHT_DEBRIEF:
			multi_df_debrief_init();
			break;

		case GS_STATE_LOAD_MISSION_MENU:
			mission_load_menu_init();
			break;

		case GS_STATE_SIMULATOR_ROOM:
			sim_room_init();
			break;

		case GS_STATE_CAMPAIGN_ROOM:
			campaign_room_init();
			break;

		case GS_STATE_SHIP_SELECT:
			ship_select_init();
			break;

		case GS_STATE_WEAPON_SELECT:
			weapon_select_init();
			break;

		case GS_STATE_TEAM_SELECT:		
			multi_ts_init();
			break;

		case GS_STATE_GAME_PAUSED:
			game_stop_time();
			pause_init();
			break;

		case GS_STATE_DEBUG_PAUSED:
		case GS_STATE_TRAINING_PAUSED:
			#ifndef NDEBUG
				game_stop_time();
				pause_debug_init();
			#endif
			break;

		case GS_STATE_OPTIONS_MENU:
			options_menu_init();

			if (old_state != GS_STATE_CONTROL_CONFIG && old_state != GS_STATE_HUD_CONFIG) {
				// pause all sounds, since we could get here through the game
				// only pause if we're in-mission; we could also be in the main hall
				if (Game_mode & GM_IN_MISSION) {
					weapon_pause_sounds();
					audiostream_pause_all();
					message_pause_all();

					// multi doesn't pause here so time needs to keep going
					if ( !(Game_mode & GM_MULTIPLAYER) ) {
						game_stop_time();
					}
				}
			}
			break;
 
		case GS_STATE_GAME_PLAY:

			// maybe play a cutscene
			if((old_state == GS_STATE_BRIEFING) || (old_state == GS_STATE_SHIP_SELECT) || 
			   (old_state == GS_STATE_WEAPON_SELECT) || (old_state == GS_STATE_RED_ALERT) ) {
				common_maybe_play_cutscene(MOVIE_PRE_GAME); 
			}

			/* game could be comming from a restart (rather than first start)
			so make sure that we zero the hud gauge overrides (Sexp_hud_*)
			\sa sexp_hud_display_gauge*/
			if ( (old_state == GS_STATE_GAME_PLAY)
				|| (old_state == GS_STATE_BRIEFING)
				|| (old_state == GS_STATE_DEBRIEF)
				|| (old_state == GS_STATE_SHIP_SELECT)
				|| (old_state == GS_STATE_WEAPON_SELECT)
				|| (old_state == GS_STATE_RED_ALERT) )
			{
				Sexp_hud_display_warpout = 0;
			}

			// Goober5000 - people may not have realized that pausing causes this state to be re-entered
			if ((old_state != GS_STATE_GAME_PAUSED) && (old_state != GS_STATE_MULTI_PAUSED) && (old_state != GS_STATE_MAIN_MENU))
			{
				if ( !Is_standalone )
					radar_mission_init();

				//Set the current hud
				set_current_hud();

				if ( !Is_standalone ) {
					ship_init_cockpit_displays(Player_ship);
				}
			}

			// coming from the gameplay state or the main menu, we might need to load the mission
			if ( (Game_mode & GM_NORMAL) && ((old_state == GS_STATE_MAIN_MENU) || (old_state == GS_STATE_GAME_PLAY) || (old_state == GS_STATE_DEATH_BLEW_UP)) ) {
				if ( !game_start_mission() )		// this should put us into a new state.
					// Failed!!!
					break;
			}

			// if we are coming from the briefing, ship select, weapons loadout, or main menu (in the
			// case of quick start), then do bitmap loads, etc  Don't do any of the loading stuff
			// if we are in multiplayer -- this stuff is all handled in the multi-wait section
			if ( !(Game_mode & GM_MULTIPLAYER) && ((old_state == GS_STATE_BRIEFING) || (old_state == GS_STATE_SHIP_SELECT) ||
				(old_state == GS_STATE_WEAPON_SELECT) || (old_state == GS_STATE_MAIN_MENU) || (old_state == GS_STATE_MULTI_STD_WAIT) || (old_state == GS_STATE_SIMULATOR_ROOM)) ) {
					// JAS: Used to do all paging here.

					#ifndef NDEBUG
					//XSTR:OFF
						HUD_printf("Skill level is set to ** %s **", Skill_level_names(Game_skill_level));
					//XSTR:ON
					#endif

					main_hall_stop_music(true);
					main_hall_stop_ambient();
					event_music_first_pattern();	// start the first pattern
			}

			// special code that restores player ship selection and weapons loadout when doing a quick start
			if ( !(Game_mode & GM_MULTIPLAYER) && ((old_state == GS_STATE_MAIN_MENU) || (old_state == GS_STATE_DEATH_BLEW_UP)  || (old_state == GS_STATE_GAME_PLAY)) ) {
				if ( !stricmp(Player_loadout.filename, Game_current_mission_filename) ) {
					wss_direct_restore_loadout();
				}
			}

			// single-player, quick-start after just died... we need to set weapon linking and kick off the event music
			if (!(Game_mode & GM_MULTIPLAYER) && (old_state == GS_STATE_DEATH_BLEW_UP) ) {
				event_music_first_pattern();	// start the first pattern
			}

			if ( !(Game_mode & GM_STANDALONE_SERVER) && ((old_state != GS_STATE_GAME_PAUSED) && (old_state != GS_STATE_MULTI_PAUSED)) ) {
				event_music_first_pattern();	// start the first pattern
			}
			player_restore_target_and_weapon_link_prefs();

			Game_mode |= GM_IN_MISSION;

#ifndef NDEBUG
			// required to truely make mouse deltas zeroed in debug mouse code
void mouse_force_pos(int x, int y);
			mouse_force_pos(gr_screen.max_w / 2, gr_screen.max_h / 2);
#endif

			game_flush();

			// only start time if in single player, or coming from multi wait state
			if (
					(
						(Game_mode & GM_NORMAL) && 
						(old_state != GS_STATE_VIEW_CUTSCENES)
					) || (
						(Game_mode & GM_MULTIPLAYER) && (
							(old_state == GS_STATE_MULTI_PAUSED) ||
							(old_state == GS_STATE_MULTI_MISSION_SYNC) ||
							(old_state == GS_STATE_INGAME_PRE_JOIN)
						)
					)
				) {
					game_start_time();
			}

			// when coming from the multi paused state, reset the timestamps
			if ( (Game_mode & GM_MULTIPLAYER) && (old_state == GS_STATE_MULTI_PAUSED) ){
				multi_reset_timestamps();
			}
	
			// under certain circumstances, the server should reset the object update rate limiting stuff
			if( MULTIPLAYER_MASTER && ((old_state == GS_STATE_MULTI_PAUSED) || (old_state == GS_STATE_MULTI_MISSION_SYNC)) ){
				
				// reinitialize the rate limiting system for all clients
				multi_oo_rate_init_all();
			}

			// multiplayer clients should always re-initialize their control info rate limiting system			
			if(MULTIPLAYER_CLIENT){
				multi_oo_rate_init_all();
			}
			
			// reset ping times
			if(Game_mode & GM_MULTIPLAYER){
				multi_ping_reset_players();
			}

			Game_subspace_effect = 0;
			if (The_mission.flags[Mission::Mission_Flags::Subspace]) {
				Game_subspace_effect = 1;
			}

			sound_env_set(&Game_sound_env);
			joy_ff_mission_init(Ship_info[Player_ship->ship_info_index].rotation_time);

			// clear multiplayer button info
			extern button_info Multi_ship_status_bi;
			memset(&Multi_ship_status_bi, 0, sizeof(button_info));
			
			io::mouse::CursorManager::get()->showCursor(false, true);

			tracing::async::begin(tracing::MainFrame, tracing::MainFrameScope);
			break;

		case GS_STATE_HUD_CONFIG:
			hud_config_init();
			break;

		case GS_STATE_PXO:
			if (old_state != GS_STATE_PXO_HELP) {
				multi_pxo_init( 0 );
			}
			break;

		case GS_STATE_PXO_HELP:
			multi_pxo_help_init();
			break;

		case GS_STATE_MULTI_JOIN_GAME:
			multi_join_clear_game_list();

			if (old_state != GS_STATE_OPTIONS_MENU) {
				multi_join_game_init();
			}

			break;

		case GS_STATE_MULTI_HOST_SETUP:		
			// don't reinitialize if we're coming back from the host options screen
			if ((old_state != GS_STATE_MULTI_HOST_OPTIONS) && (old_state != GS_STATE_OPTIONS_MENU)) {
				multi_create_game_init();
			}

			break;

		case GS_STATE_MULTI_CLIENT_SETUP:		
			if (old_state != GS_STATE_OPTIONS_MENU) {
				multi_game_client_setup_init();
			}

			break;

		case GS_STATE_CONTROL_CONFIG:
			control_config_init();
			break;

		case GS_STATE_TECH_MENU:
			techroom_init();
			break;

		case GS_STATE_BARRACKS_MENU:
			if(old_state != GS_STATE_VIEW_MEDALS){
				barracks_init();
			}
			break;

		case GS_STATE_MISSION_LOG_SCROLLBACK:
			hud_scrollback_init();
			break;

		case GS_STATE_DEATH_DIED:
			if(!(Game_mode & GM_MULTIPLAYER)){
				player_show_death_message();
			}
			Game_mode |= GM_DEAD_DIED;
			break;

		case GS_STATE_DEATH_BLEW_UP:
			if ( !popupdead_is_active() ) {
				Player_ai->target_objnum = -1;
			}

			// stop any local EMP effect
			emp_stop_local();

			Players[Player_num].flags &= ~PLAYER_FLAGS_AUTO_TARGETING;	//	Prevent immediate switch to a hostile ship.
			Game_mode |= GM_DEAD_BLEW_UP;		
			Show_viewing_from_self = 0;

			// timestamp how long we should wait before displaying the died popup
			if ( !popupdead_is_active() ) {
				Player_died_popup_wait = ui_timestamp(PLAYER_DIED_POPUP_WAIT);
			}
			break;

		case GS_STATE_GAMEPLAY_HELP:
			gameplay_help_init();
			break;

		case GS_STATE_CREDITS:
			main_hall_stop_music(true);
			main_hall_stop_ambient();
			credits_init();
			break;

		case GS_STATE_VIEW_MEDALS:
			medal_main_init(Player);
			break;

		case GS_STATE_SHOW_GOALS:
			mission_show_goals_init();
			break;

		case GS_STATE_HOTKEY_SCREEN:
			mission_hotkey_init();
			break;

		case GS_STATE_MULTI_MISSION_SYNC:
			// if we're coming from the options screen, don't do any
			if(old_state == GS_STATE_OPTIONS_MENU){
				break;
			}

			switch(Multi_sync_mode){
			case MULTI_SYNC_PRE_BRIEFING:
				// if moving from game forming to the team select state						
				multi_sync_init();			
				break;
			case MULTI_SYNC_POST_BRIEFING:
				// if moving from briefing into the mission itself			
				multi_sync_init();
			
				// tell everyone that we're now loading data
				Net_player->state = NETPLAYER_STATE_DATA_LOAD;
				send_netplayer_update_packet();

				// JAS: Used to do all paging here!!!!
								
				Net_player->state = NETPLAYER_STATE_WAITING;			
				send_netplayer_update_packet();
				break;
			case MULTI_SYNC_INGAME:
				multi_sync_init();
				break;
			}
			break;		
   
		case GS_STATE_VIEW_CUTSCENES:
			cutscenes_screen_init();
			break;

		case GS_STATE_MULTI_STD_WAIT:
			multi_standalone_wait_init();
			break;

		case GS_STATE_STANDALONE_MAIN:
			// don't initialize if we're coming from one of these 2 states unless there are no 
			// players left (reset situation)
			if((old_state != GS_STATE_STANDALONE_POSTGAME) || multi_endgame_ending()){
				standalone_main_init();
			}
			break;	

		case GS_STATE_STANDALONE_POSTGAME:
			multi_standalone_postgame_init();
			break;

		case GS_STATE_MULTI_PAUSED:
			multi_pause_init();
			break;

		case GS_STATE_INGAME_PRE_JOIN:
			multi_ingame_select_init();
			break;

		case GS_STATE_INITIAL_PLAYER_SELECT:
			player_select_init();
			break;		

		case GS_STATE_MULTI_START_GAME:
			multi_start_game_init();
			break;

		case GS_STATE_MULTI_HOST_OPTIONS:
			multi_host_options_init();
			break;		

		case GS_STATE_END_OF_CAMPAIGN:
			mission_campaign_end_init();
			break;		

		case GS_STATE_LOOP_BRIEF:
			loop_brief_init();
			break;

		case GS_STATE_LAB:
			lab_init();
			break;

		case GS_STATE_SCRIPTING:
		case GS_STATE_SCRIPTING_MISSION:
			scripting_state_init();
			break;

		case GS_STATE_INGAME_OPTIONS:
			ingame_options_init();
	} // end switch

	//WMC - now do user scripting stuff
	if (scripting::hooks::OnStateStart->isActive()) {
		scripting::hooks::OnStateStart->run(std::move(script_param_list));
	}
}

// do stuff that may need to be done regardless of state
void game_do_state_common(int state,int no_networking)
{
#ifndef NDEBUG
	// If the frame took more than 5 seconds, assume we're tracing through a debugger.  If timestamps are running, correct the elapsed time.
	if (!Cmdline_slow_frames_ok && !timestamp_is_paused() && Last_frame_ui_timestamp.isValid()) {
		auto delta_timestamp = ui_timestamp_since(Last_frame_ui_timestamp);
		if (delta_timestamp > 5 * MILLISECONDS_PER_SECOND) {
			delta_timestamp -= 20;	// suppose last frame was 50 FPS
			mprintf(("Too much time passed between frames.  Adjusting timestamp by %i milliseconds to compensate\n", delta_timestamp));
			timestamp_adjust_pause_offset(delta_timestamp);
		}
	}
#endif
	Last_frame_ui_timestamp = ui_timestamp();

	io::mouse::CursorManager::doFrame();		// determine if to draw the mouse this frame
	snd_do_frame();								// update sound system
	event_music_do_frame();						// music needs to play across many states

	multi_log_process();	

	if (no_networking) {
		return;
	}

	// maybe do a multiplayer frame based on game mode and state type	
	if (Game_mode & GM_MULTIPLAYER) {
		switch (state) {
			case GS_STATE_OPTIONS_MENU:
			case GS_STATE_GAMEPLAY_HELP:
			case GS_STATE_HOTKEY_SCREEN:
			case GS_STATE_HUD_CONFIG:
			case GS_STATE_CONTROL_CONFIG:
			case GS_STATE_MISSION_LOG_SCROLLBACK:
			case GS_STATE_SHOW_GOALS:
			case GS_STATE_VIEW_CUTSCENES:
			case GS_STATE_EVENT_DEBUG:
				multi_maybe_do_frame();
				break;
		}
		
		game_do_networking();
	}
}

// Called once a frame.
// You should never try to change the state
// in here... if you think you need to, you probably really
// need to post an event, not change the state.
int Game_do_state_should_skip = 0;
void game_do_state(int state)
{
	// always lets the do_state_common() function determine if the state should be skipped
	Game_do_state_should_skip = 0;

	// legal to set the should skip state anywhere in this function
	game_do_state_common(state); // do stuff that may need to be done regardless of state

	if (Game_do_state_should_skip) {
		return;
	}

	if (OnFrameHook->isActive() && OnFrameHook->isOverride()) {
		game_set_frametime(state);

		game_check_key();

		gr_clear();
		gr_flip(); // Does state hook automagically
		return;
	}

	switch (state) {
	case GS_STATE_MAIN_MENU:
			game_set_frametime(GS_STATE_MAIN_MENU);
			main_hall_do(flFrametime);
			break;

		case GS_STATE_OPTIONS_MENU:
			game_set_frametime(GS_STATE_OPTIONS_MENU);
			options_menu_do_frame(flFrametime);
			break;

		case GS_STATE_BARRACKS_MENU:
			game_set_frametime(GS_STATE_BARRACKS_MENU);
			barracks_do_frame(flFrametime);
			break;

		case GS_STATE_TRAINING_MENU:
			game_set_frametime(GS_STATE_TRAINING_MENU);
			training_menu_do_frame(flFrametime);
			break;

		case GS_STATE_TECH_MENU:
			game_set_frametime(GS_STATE_TECH_MENU);
			techroom_do_frame(flFrametime);
			break;

		case GS_STATE_GAMEPLAY_HELP:
			game_set_frametime(GS_STATE_GAMEPLAY_HELP);
			gameplay_help_do_frame(flFrametime);
			break;

		case GS_STATE_GAME_PLAY:	// do stuff that should be done during gameplay
			game_do_frame();
			break;

		case GS_STATE_GAME_PAUSED:

			if(pause_get_type() == PAUSE_TYPE_VIEWER)	{

				read_player_controls( Player_obj, flFrametime);
				game_frame(true);
			}
				
			pause_do();
			break;

		case GS_STATE_DEBUG_PAUSED:
			#ifndef NDEBUG
				game_set_frametime(GS_STATE_DEBUG_PAUSED);
				pause_debug_do();
			#endif
			break;

		case GS_STATE_TRAINING_PAUSED:
			game_training_pause_do();
			break;

		case GS_STATE_LOAD_MISSION_MENU:
			game_set_frametime(GS_STATE_LOAD_MISSION_MENU);
			mission_load_menu_do();
			break;
		
		case GS_STATE_BRIEFING:
			game_set_frametime(GS_STATE_BRIEFING);
			brief_do_frame(flFrametime);
			break;

		case GS_STATE_DEBRIEF:
			game_set_frametime(GS_STATE_DEBRIEF);
			debrief_do_frame(flFrametime);
			break;

		case GS_STATE_MULTI_DOGFIGHT_DEBRIEF:
			game_set_frametime(GS_STATE_MULTI_DOGFIGHT_DEBRIEF);
			multi_df_debrief_do();
			break;

		case GS_STATE_SHIP_SELECT:
			game_set_frametime(GS_STATE_SHIP_SELECT);
			ship_select_do(flFrametime);
			break;

		case GS_STATE_WEAPON_SELECT:
			game_set_frametime(GS_STATE_WEAPON_SELECT);
			weapon_select_do(flFrametime);
			break;

		case GS_STATE_MISSION_LOG_SCROLLBACK:
			game_set_frametime(GS_STATE_MISSION_LOG_SCROLLBACK);
			hud_scrollback_do_frame(flFrametime);
			break;

		case GS_STATE_HUD_CONFIG:
			game_set_frametime(GS_STATE_HUD_CONFIG);
			hud_config_do_frame(flFrametime);
			break;

		case GS_STATE_PXO:
			game_set_frametime(GS_STATE_PXO);
			multi_pxo_do();
			break;

		case GS_STATE_PXO_HELP:
			game_set_frametime(GS_STATE_PXO_HELP);
			multi_pxo_help_do();
			break;

		case GS_STATE_MULTI_JOIN_GAME:
			game_set_frametime(GS_STATE_MULTI_JOIN_GAME);
			multi_join_game_do_frame();
			break;

		case GS_STATE_MULTI_HOST_SETUP:
			game_set_frametime(GS_STATE_MULTI_HOST_SETUP);
			multi_create_game_do();
			break;

		case GS_STATE_MULTI_CLIENT_SETUP:
			game_set_frametime(GS_STATE_MULTI_CLIENT_SETUP);
			multi_game_client_setup_do_frame();
			break;

		case GS_STATE_CONTROL_CONFIG:
			game_set_frametime(GS_STATE_CONTROL_CONFIG);
			control_config_do_frame(flFrametime);
			break;	

		case GS_STATE_DEATH_DIED:
			game_do_frame();			
			break;

		case GS_STATE_DEATH_BLEW_UP:
			game_do_frame();
			break;

		case GS_STATE_SIMULATOR_ROOM:
			game_set_frametime(GS_STATE_SIMULATOR_ROOM);
			sim_room_do_frame(flFrametime);
			break;

		case GS_STATE_CAMPAIGN_ROOM:
			game_set_frametime(GS_STATE_CAMPAIGN_ROOM);
			campaign_room_do_frame(flFrametime);
			break;

		case GS_STATE_RED_ALERT:
			game_set_frametime(GS_STATE_RED_ALERT);
			red_alert_do_frame(flFrametime);
			break;

		case GS_STATE_CMD_BRIEF:
			game_set_frametime(GS_STATE_CMD_BRIEF);
			cmd_brief_do_frame(flFrametime);
			break;

		case GS_STATE_CREDITS:
			game_set_frametime(GS_STATE_CREDITS);
			credits_do_frame(flFrametime);
			break;

		case GS_STATE_VIEW_MEDALS:
			game_set_frametime(GS_STATE_VIEW_MEDALS);
			medal_main_do();
			break;

		case GS_STATE_SHOW_GOALS:
			game_set_frametime(GS_STATE_SHOW_GOALS);
			mission_show_goals_do_frame(flFrametime);
			break;

		case GS_STATE_HOTKEY_SCREEN:
			game_set_frametime(GS_STATE_HOTKEY_SCREEN);
			mission_hotkey_do_frame(flFrametime);
			break;	
   
		case GS_STATE_VIEW_CUTSCENES:
			game_set_frametime(GS_STATE_VIEW_CUTSCENES);
			cutscenes_screen_do_frame();
			break;

		case GS_STATE_MULTI_STD_WAIT:
			game_set_frametime(GS_STATE_STANDALONE_MAIN);
			multi_standalone_wait_do();
			break;

		case GS_STATE_STANDALONE_MAIN:
			game_set_frametime(GS_STATE_STANDALONE_MAIN);
			standalone_main_do();
			break;	

		case GS_STATE_MULTI_PAUSED:
			game_set_frametime(GS_STATE_MULTI_PAUSED);
			multi_pause_do();
			break;

		case GS_STATE_TEAM_SELECT:
			game_set_frametime(GS_STATE_TEAM_SELECT);
			multi_ts_do();
			break;

		case GS_STATE_INGAME_PRE_JOIN:
			game_set_frametime(GS_STATE_INGAME_PRE_JOIN);
			multi_ingame_select_do();
			break;

		case GS_STATE_EVENT_DEBUG:
	#ifndef NDEBUG
			game_set_frametime(GS_STATE_EVENT_DEBUG);
			game_show_event_debug(flFrametime);
	#endif
			break;

		case GS_STATE_STANDALONE_POSTGAME:
			game_set_frametime(GS_STATE_STANDALONE_POSTGAME);
			multi_standalone_postgame_do();
			break;

		case GS_STATE_INITIAL_PLAYER_SELECT:
			game_set_frametime(GS_STATE_INITIAL_PLAYER_SELECT);
			player_select_do();
			break;

		case GS_STATE_MULTI_MISSION_SYNC:
			game_set_frametime(GS_STATE_MULTI_MISSION_SYNC);
			multi_sync_do();
			break;

		case GS_STATE_MULTI_START_GAME:
			game_set_frametime(GS_STATE_MULTI_START_GAME);
			multi_start_game_do();
			break;
		
		case GS_STATE_MULTI_HOST_OPTIONS:
			game_set_frametime(GS_STATE_MULTI_HOST_OPTIONS);
			multi_host_options_do();
			break;

		case GS_STATE_END_OF_CAMPAIGN:
			mission_campaign_end_do();
			break;	

		case GS_STATE_LOOP_BRIEF:
			game_set_frametime(GS_STATE_LOOP_BRIEF);
			loop_brief_do(flFrametime);
			break;

		case GS_STATE_FICTION_VIEWER:
			game_set_frametime(GS_STATE_FICTION_VIEWER);
			fiction_viewer_do_frame(flFrametime);
			break;

		case GS_STATE_LAB:
			game_set_frametime(GS_STATE_LAB);
			lab_do_frame(flFrametime);
			break;

		case GS_STATE_SCRIPTING:
			game_set_frametime(GS_STATE_SCRIPTING);
			scripting_state_do_frame(flFrametime, true);
			break;

		case GS_STATE_SCRIPTING_MISSION:
			game_set_frametime(GS_STATE_SCRIPTING_MISSION);
			scripting_state_do_frame(flFrametime, false);
			break;

		case GS_STATE_INGAME_OPTIONS:
			game_set_frametime(GS_STATE_INGAME_OPTIONS);
			ingame_options_do_frame();

   } // end switch(gs_current_state)

	if (LoggingEnabled && Cmdline_debug_window) {
		// Do a frame for the debug window here since this code is always executed
		outwnd_debug_window_do_frame(flFrametime);
	}
}


#ifdef _WIN32
// return 0 if there is enough RAM to run FreeSpace, otherwise return -1
int game_do_ram_check(uint64_t ram_in_bytes)
{
	if ( ram_in_bytes < 30*1024*1024 )	{
		int allowed_to_run = 1;
		if ( ram_in_bytes < 25*1024*1024 ) {
			allowed_to_run = 0;
		}

		char tmp[1024];
		uint FreeSpace_total_ram_MB;
		FreeSpace_total_ram_MB = (uint)(ram_in_bytes/(1024*1024));

		if ( allowed_to_run ) {

			sprintf( tmp, XSTR( "FreeSpace has detected that you only have %dMB of free memory.\n\nFreeSpace requires at least 32MB of memory to run.  If you think you have more than %dMB of physical memory, ensure that you aren't running SmartDrive (SMARTDRV.EXE).  Any memory allocated to SmartDrive is not usable by applications\n\nPress 'OK' to continue running with less than the minimum required memory\n", 193), FreeSpace_total_ram_MB, FreeSpace_total_ram_MB);

			os::dialogs::Message( os::dialogs::MESSAGEBOX_ERROR, tmp, XSTR( "Not Enough RAM", 194));

		} else {
			sprintf( tmp, XSTR( "FreeSpace has detected that you only have %dMB of free memory.\n\nFreeSpace requires at least 32MB of memory to run.  If you think you have more than %dMB of physical memory, ensure that you aren't running SmartDrive (SMARTDRV.EXE).  Any memory allocated to SmartDrive is not usable by applications\n", 195), FreeSpace_total_ram_MB, FreeSpace_total_ram_MB);
			os::dialogs::Message( os::dialogs::MESSAGEBOX_ERROR, tmp, XSTR( "Not Enough RAM", 194) );
			return -1;
		}
	}

	return 0;
}

#endif // ifdef WIN32

void game_spew_pof_info_sub(int model_num, polymodel *pm, int sm, CFILE *out, int *out_total, int *out_destroyed_total)
{
	int i;
	int sub_total = 0;
	int sub_total_destroyed = 0;
	int total = 0;
	char str[255] = "";		
	
	// get the total for all his children
	for (i=pm->submodel[sm].first_child; i >= 0; i = pm->submodel[i].next_sibling )	{
		game_spew_pof_info_sub(model_num, pm, i, out, &sub_total, &sub_total_destroyed);
	}	

	// find the # of faces for this _individual_ object	
	total = submodel_get_num_polys(model_num, sm);
	if(strstr(pm->submodel[sm].name, "-destroyed")){
		sub_total_destroyed = total;
	}
	
	// write out total
	sprintf(str, "Submodel %s total : %d faces\n", pm->submodel[sm].name, total);
	cfputs(str, out);		

	*out_total += total + sub_total;
	*out_destroyed_total += sub_total_destroyed;
}

#define BAIL()			do { int _idx; for(_idx=0; _idx<num_files; _idx++){ if(pof_list[_idx] != nullptr){vm_free(pof_list[_idx]); pof_list[_idx] = nullptr;}} return;} while(false);

void game_spew_pof_info()
{
	char *pof_list[1000];
	char str[255] = "";

	// get file list
	int num_files = cf_get_file_list(1000, pof_list, CF_TYPE_MODELS, "*.pof");

	// spew info on all the pofs
	if(!num_files){
		return;
	}

	// go
	CFILE* out = cfopen("pofspew.txt", "wt", CF_TYPE_DATA);
	if(out == nullptr){
		BAIL();
	}	
	int counted = 0;
	for(int idx=0; idx<num_files; idx++, counted++){
		sprintf(str, "%s.pof", pof_list[idx]);
		int model_num = model_load(str);
		if(model_num >= 0){
			polymodel *pm = model_get(model_num);

			// if we have a real model
			if(pm != nullptr){
				cfputs(str, out);
				cfputs("\n", out);
				
				// go through and print all raw submodels
				cfputs("RAW\n", out);
				int total = 0;
				int model_total = 0;
				for (int i=0; i<pm->n_models; i++)	{
					total = submodel_get_num_polys(model_num, i);					
					
					model_total += total;
					sprintf(str, "Submodel %s total : %d faces\n", pm->submodel[i].name, total);
					cfputs(str, out);
				}				
				sprintf(str, "Model total %d\n", model_total);				
				cfputs(str, out);				

				// now go through and do it by LOD
				cfputs("BY LOD\n\n", out);				
				for(int i=0; i<pm->n_detail_levels; i++){
					sprintf(str, "LOD %d\n", i);
					cfputs(str, out);

					// submodels
					int root_total = submodel_get_num_polys(model_num, pm->detail[i] );
					total = 0;
					int destroyed_total = 0;
					for (int j=pm->submodel[pm->detail[i]].first_child; j >= 0; j = pm->submodel[j].next_sibling )	{
						game_spew_pof_info_sub(model_num, pm, j, out, &total, &destroyed_total);
					}

					sprintf(str, "Submodel %s total : %d faces\n", pm->submodel[pm->detail[i]].name, root_total);
					cfputs(str, out);

					sprintf(str, "TOTAL: %d\n", total + root_total);					
					cfputs(str, out);
					sprintf(str, "TOTAL not counting destroyed faces %d\n", (total + root_total) - destroyed_total);
					cfputs(str, out);
					sprintf(str, "TOTAL destroyed faces %d\n\n", destroyed_total);
					cfputs(str, out);
				}				
				cfputs("------------------------------------------------------------------------\n\n", out);				
			}
			// Free memory of this model again
			model_unload(model_num);
		}
	}

	cfclose(out);
	model_free_all();
	BAIL();
}

DCF(pofspew, "Spews POF info without shutting down the game")
{
	game_spew_pof_info();
}

/**
* Does some preliminary checks and then enters main event loop.
*
* @returns 0 on a clean exit, or
* @returns 1 on an error
*/
int game_main(int argc, char *argv[])
{
	int state;

#ifdef _WIN32
	// Find out how much RAM is on this machine
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	GlobalMemoryStatusEx(&ms);
	FreeSpace_total_ram = ms.ullTotalPhys;

	if ( game_do_ram_check(FreeSpace_total_ram) == -1 ) {
		return 1;
	}

	if ( ms.ullTotalVirtual < 1024 ) {
		os::dialogs::Message( os::dialogs::MESSAGEBOX_ERROR, XSTR( "FreeSpace requires virtual memory to run.\r\n", 196), XSTR( "No Virtual Memory", 197) );
		return 1;
	}
		
	char *tmp_mem = (char *) vm_malloc(16 * 1024 * 1024);
	if (!tmp_mem) {
		os::dialogs::Message(os::dialogs::MESSAGEBOX_ERROR, XSTR( "Not enough memory to run FreeSpace.\r\nTry closing down some other applications.\r\n", 198), XSTR( "Not Enough Memory", 199));
		return 1;
	}

	vm_free(tmp_mem);
	tmp_mem = nullptr;
#endif // _WIN32


	if ( !parse_cmdline(argc, argv) ) {
		return 1;
	}

	game_init();

	// if networking is unavailable then standalone is useless, so just fail
	if (Is_standalone && !psnet_is_active()) {
		ml_string("Failed to initialize networking! Aborting...");
		game_shutdown();
		return 1;
	}

	if (!Is_standalone && !headtracking::init())
	{
		mprintf(("Headtracking is not enabled...\n"));
	}

	game_stop_time();

	// maybe spew pof stuff
	if (Cmdline_spew_pof_info) {
		game_spew_pof_info();
		game_shutdown();
		return 0;
	}

	// maybe spew VP CRCs, and exit
	if (Cmdline_verify_vps) {
		extern void cfile_spew_pack_file_crcs();
		cfile_spew_pack_file_crcs();
		game_shutdown();
		return 0;
	}

	// This needs to be done after the dynamic SEXP init so that our documentation contains the dynamic sexps
	if (Cmdline_output_sexp_info) {
		output_sexps("sexps.html");
	}

	bool skip_intro = Disable_intro_movie;
	if (scripting::hooks::OnIntroAboutToPlay->isActive()) {
		skip_intro = scripting::hooks::OnIntroAboutToPlay->isOverride();
		scripting::hooks::OnIntroAboutToPlay->run();
	}

	if (!Is_standalone && !skip_intro) {
		movie::play("intro.mve");
	}

	game_start_time();

	if (Is_standalone) {
		gameseq_post_event(GS_EVENT_STANDALONE_MAIN);
	} else {
		gameseq_post_event(GS_EVENT_GAME_INIT);		// start the game rolling -- check for default pilot, or go to the pilot select screen
	}

	while (1) {
		// only important for non THREADED mode
		os_poll();

		events::EngineUpdate();

		state = gameseq_process_events();
		if ( state == GS_STATE_QUIT_GAME ) {
			break;
		}

		// Since tracing is always active this needs to happen in the main loop
		tracing::process_events();
	} 

	game_shutdown();

	return 0;
}

// game_shutdown()
//
// This function is called when FreeSpace terminates normally.  
//
void game_shutdown(void)
{
	events::EngineShutdown();

	headtracking::shutdown();

	fsspeech_deinit();
#ifdef FS2_VOICER
	if(Cmdline_voice_recognition)
	{
		VOICEREC_deinit();
	}
#endif

	// don't ever flip a page on the standalone!
	if(!(Game_mode & GM_STANDALONE_SERVER)){
		gr_reset_clip();
		gr_clear();
		gr_flip();
	}
	
	// if the player has left the "player select" screen and quit the game without actually choosing
	// a player, Player will be nullptr, in which case we shouldn't write the player file out!
	if (!(Game_mode & GM_STANDALONE_SERVER) && (Player!=nullptr) && !Is_standalone){
		Pilot.save_player();
		Pilot.save_savefile();
	}

	particle::ParticleManager::shutdown();
	batching_shutdown();

	// load up common multiplayer icons
	multi_unload_common_icons();
	hud_close();	
	fireball_close();				// free fireball system
	particle::close();			// close out the particle system
	weapon_close();					// free any memory that was allocated for the weapons
	ship_close();					// free any memory that was allocated for the ships

	decals::shutdown();

	io::mouse::CursorManager::shutdown();

	mission_campaign_clear();	// clear out the campaign stuff
	mission_parse_close();		// clear out any extra memory that may be in use by mission parsing
	multi_voice_close();			// close down multiplayer voice (including freeing buffers, etc)
	multi_log_close();
	logfile_close(LOGFILE_EVENT_LOG); // close down the mission log
#ifdef MULTI_USE_LAG
	multi_lag_close();
#endif

	scoring_close();

	stars_close();			// clean out anything used by stars code

	// the menu close functions will unload the bitmaps if they were displayed during the game
	main_hall_close();
	training_menu_close();

	// more fundamental shutdowns begin here ----------

	sexp_shutdown();				// Free SEXP resources

	scpui::shutdown_scripting();	// Free the scripting resources of the new UI first

	Script_system.Clear();			// Everything after this should be done without scripting so we can free those resources here.
									// By this point, all things that hold Lua References must be destroyed (such as Lua SEXPs)

	scpui::shutdown();				// Deinitialize the new UI system, needs to be done after scripting shutdown to make sure the resources were released properly

	control_config_common_close();
	io::joystick::shutdown();

	audiostream_close();
	snd_close();
	event_music_close();
	gamesnd_close();		// close out gamesnd, needs to happen *after* other sounds are closed
	psnet_close();

	obj_shutdown();

	model_free_all();
	bm_unload_all();			// unload/free bitmaps, has to be called *after* model_free_all()!

	tracing::shutdown();

	if (LoggingEnabled) {
		outwnd_debug_window_deinit();
	}

	gr_close();

	if (Is_standalone) {
		std_deinit_standalone();
	}

	ImGui::DestroyContext();

	os_cleanup();

	cfile_close();

	// although the comment in cmdline.cpp said this isn't needed,
	// Valgrind disagrees (quite possibly incorrectly), but this is just cleaner
	if (Cmdline_mod != nullptr) {
		delete[] Cmdline_mod;
		Cmdline_mod = nullptr;
	}

	lcl_xstr_close();

	threading::shut_down_task_pool();
}

// game_stop_looped_sounds()
//
// This function will call the appropriate stop looped sound functions for those
// modules which use looping sounds.  It is not enough just to stop a looping sound
// at the DirectSound level, the game is keeping track of looping sounds, and this
// function is used to inform the game that looping sounds are being halted.
//
void game_stop_looped_sounds()
{
	hud_stop_looped_locking_sounds();
	hud_stop_looped_engine_sounds();
	afterburner_stop_sounds();
	player_stop_looped_sounds();
	obj_snd_stop_all();		// stop all object-linked persistant sounds
	game_stop_subspace_ambient_sound();
	snd_stop(Radar_static_looping);
	Radar_static_looping = sound_handle::invalid();
	snd_stop(Target_static_looping);
	shipfx_stop_engine_wash_sound();
	Target_static_looping = sound_handle::invalid();
}

void game_do_training_checks()
{
	int i, s;
	float d;
	waypoint_list *wplp;

	if (Training_context & TRAINING_CONTEXT_SPEED) {
		s = (int) Player_obj->phys_info.fspeed;
		if ((s >= Training_context_speed_min) && (s <= Training_context_speed_max)) {
			if (!Training_context_speed_set) {
				Training_context_speed_set = 1;
				Training_context_speed_timestamp = _timestamp();
			}

		} else
			Training_context_speed_set = 0;
	}

	if (Training_context & TRAINING_CONTEXT_FLY_PATH) {
		wplp = find_waypoint_list_at_index(Training_context_waypoint_path);
		if (wplp->get_waypoints().size() > (uint) Training_context_goal_waypoint) {
			i = Training_context_goal_waypoint;
			do {
				waypoint *wpt = find_waypoint_at_index(wplp, i);
				Assert(wpt != nullptr);
				d = vm_vec_dist(wpt->get_pos(), &Player_obj->pos);
				if (d <= Training_context_distance) {
					Training_context_at_waypoint = i;
					if (Training_context_goal_waypoint == i) {
						Training_context_goal_waypoint++;
						snd_play(gamesnd_get_game_sound(GameSounds::CARGO_REVEAL), 0.0f);
					}

					break;
				}

				i++;
				if ((uint) i == wplp->get_waypoints().size())
					i = 0;

			} while (i != Training_context_goal_waypoint);
		}
	}

	if ((Players_target == UNINITIALIZED) || (Player_ai->target_objnum != Players_target) || (Player_ai->targeted_subsys != Players_targeted_subsys)) {
		Players_target = Player_ai->target_objnum;
		Players_targeted_subsys = Player_ai->targeted_subsys;
		Players_target_timestamp = _timestamp();
	}
	// following added by Sesquipedalian for is_missile_locked
	if ((Players_mlocked == UNINITIALIZED) || (Player_ai->current_target_is_locked != Players_mlocked)) {
		Players_mlocked = Player_ai->current_target_is_locked;
		Players_mlocked_timestamp = _timestamp();
	}

}

/////////// Following is for event debug view screen

#ifndef NDEBUG

#define EVENT_DEBUG_MAX	5000
#define EVENT_DEBUG_EVENT 0x8000

int Event_debug_index[EVENT_DEBUG_MAX];
int ED_count;

void game_add_event_debug_index(int n, int indent)
{
	if (ED_count < EVENT_DEBUG_MAX)
		Event_debug_index[ED_count++] = n | (indent << 16);
}

void game_add_event_debug_sexp(int n, int indent)
{
	if (n < 0)
		return;

	if (Sexp_nodes[n].first >= 0) {
		game_add_event_debug_sexp(Sexp_nodes[n].first, indent);
		game_add_event_debug_sexp(Sexp_nodes[n].rest, indent);
		return;
	}

	game_add_event_debug_index(n, indent);
	if (Sexp_nodes[n].subtype == SEXP_ATOM_OPERATOR)
		game_add_event_debug_sexp(Sexp_nodes[n].rest, indent + 1);
	else
		game_add_event_debug_sexp(Sexp_nodes[n].rest, indent);
}

void game_event_debug_init()
{
	int e;

	ED_count = 0;
	for (e=0; e<(int)Mission_events.size(); e++) {
		game_add_event_debug_index(e | EVENT_DEBUG_EVENT, 0);
		game_add_event_debug_sexp(Mission_events[e].formula, 1);
	}
}

void game_show_event_debug(float  /*frametime*/) 
{
	char buf[256];
	int i, k, z;
	int font_height, font_width;	
	int y_index, y_max;
	static int scroll_offset = 0;
	
	k = game_check_key();
	if (k)
		switch (k) {
			case KEY_UP:
			case KEY_PAD8:
				scroll_offset--;
				if (scroll_offset < 0)
					scroll_offset = 0;
				break;

			case KEY_DOWN:
			case KEY_PAD2:
				scroll_offset++;
				break;

			case KEY_PAGEUP:
				scroll_offset -= 20;
				if (scroll_offset < 0)
					scroll_offset = 0;
				break;

			case KEY_PAGEDOWN:
				scroll_offset += 20;	// not font-independent, hard-coded since I counted the lines!
				break;

			default:
				gameseq_post_event(GS_EVENT_PREVIOUS_STATE);
				key_flush();
				break;
		} // end switch

	gr_clear();
	gr_set_color_fast(&Color_bright);
	font::set_font(font::FONT1);
	gr_get_string_size(&font_width, nullptr, NOX("EVENT DEBUG VIEW"));
	
	gr_string((gr_screen.clip_width_unscaled - font_width) / 2, gr_screen.center_offset_y + 15, NOX("EVENT DEBUG VIEW"));

	gr_set_color_fast(&Color_normal);
	font::set_font(font::FONT1);
	gr_get_string_size(&font_width, &font_height, NOX("test"));
	y_max = gr_screen.center_offset_y + gr_screen.center_h - font_height - 5;
	y_index = gr_screen.center_offset_y + 45;

	k = scroll_offset;
	while (k < ED_count) {
		if (y_index > y_max)
			break;

		z = Event_debug_index[k];
		if (z & EVENT_DEBUG_EVENT) {
			z &= 0x7fff;
			sprintf(buf, NOX("%s%s (%s) %s%d %d"), (Mission_events[z].flags & MEF_CURRENT) ? NOX("* ") : "",
				Mission_events[z].name.c_str(), Mission_events[z].result ? NOX("True") : NOX("False"),
				(Mission_events[z].chain_delay < 0) ? "" : NOX("x "),
				Mission_events[z].repeat_count, Mission_events[z].interval);

		} else {
			i = (z >> 16) * 3;
			buf[i] = 0;
			while (i--)
				buf[i] = ' ';

			strcat_s(buf, Sexp_nodes[z & 0x7fff].text);
			switch (Sexp_nodes[z & 0x7fff].value) {
				case SEXP_TRUE:
					strcat_s(buf, NOX(" (True)"));
					break;

				case SEXP_FALSE:
					strcat_s(buf, NOX(" (False)"));
					break;

				case SEXP_KNOWN_TRUE:
					strcat_s(buf, NOX(" (Always true)"));
					break;

				case SEXP_KNOWN_FALSE:
					strcat_s(buf, NOX(" (Always false)"));
					break;

				case SEXP_CANT_EVAL:
					strcat_s(buf, NOX(" (Can't eval)"));
					break;

				case SEXP_NAN:
				case SEXP_NAN_FOREVER:
					strcat_s(buf, NOX(" (Not a number)"));
					break;
			}
		}

		gr_printf_no_resize(gr_screen.center_offset_x + 10, y_index, "%s", buf);
		y_index += font_height;
		k++;
	}

	gr_flip();
}

#endif // NDEBUG

#ifndef NDEBUG
FILE * Time_fp;
FILE * Texture_fp;

int Tmap_npixels=0;
int Tmap_num_too_big = 0;
int Num_models_needing_splitting = 0;

void Time_model( int modelnum )
{
	vec3d eye_pos, model_pos;
	matrix eye_orient, model_orient;

	polymodel *pm = model_get( modelnum );

	size_t l = strlen(pm->filename);
	while( (l>0) )	{
		if ( (l == '/') || (l=='\\') || (l==':'))	{
			l++;
			break;
		}
		l--;
	}
	char *pof_file = &pm->filename[l];

	int model_needs_splitting = 0;

	int i;
	for (i=0; i<pm->n_textures; i++ )	{
		char filename[1024];
		ubyte pal[768];
		texture_map *tmap = &pm->maps[i];

		for (auto &texture : tmap->textures)
		{
			int bmp_num = texture.GetOriginalTexture();
			if ( bmp_num > -1 )	{
				bm_get_palette(bmp_num, pal, filename );		
				int w,h;
				bm_get_info( bmp_num,&w, &h );


				if ( (w > 512) || (h > 512) )	{
					fprintf( Texture_fp, "%s\t%s\t%d\t%d\n", pof_file, filename, w, h );
					Tmap_num_too_big++;
					model_needs_splitting++;
				}
			}
		}
	}

	if ( model_needs_splitting )	{
		Num_models_needing_splitting++;
	}
	eye_orient = model_orient = vmd_identity_matrix;
	eye_pos = model_pos = vmd_zero_vector;

	eye_pos.xyz.z = -pm->rad*2.0f;

	vec3d eye_to_model;

	vm_vec_normalized_dir( &eye_to_model, &model_pos, &eye_pos );
	vm_vector_2_matrix_norm( &eye_orient, &eye_to_model, nullptr, nullptr );

	fix t1 = timer_get_fixed_seconds();

	angles ta;
	ta.p = ta.b = ta.h = 0.0f; 
	int framecount = 0;

	Tmap_npixels = 0;

	int bitmaps_used_this_frame, bitmaps_new_this_frame;
		
	bm_get_frame_usage(&bitmaps_used_this_frame,&bitmaps_new_this_frame);

	modelstats_num_polys = modelstats_num_verts = 0;

	while( ta.h < PI2 )	{

		matrix m1;
		vm_angles_2_matrix(&m1, &ta );
		vm_matrix_x_matrix( &model_orient, &vmd_identity_matrix, &m1 );

		gr_reset_clip();

		g3_start_frame(1);

		//WMC - I think I can set this to VIEWER_ZOOM_DEFAULT.
		//If it's not appropriate, use cam_get_current()
		g3_set_view_matrix( &eye_pos, &eye_orient, VIEWER_ZOOM_DEFAULT );	

		model_clear_instance( modelnum );

		model_render_params render_info;
		render_info.set_detail_level_lock(0);
		model_set_detail_level(0);		// use highest detail level
		model_render_immediate( &render_info, modelnum, &model_orient, &model_pos );	//|MR_NO_POLYS );

		g3_end_frame();

		framecount++;
		ta.h += 0.1f;

		int k = key_inkey();
		if ( k == KEY_ESC ) {
			exit(1);
		}
	}

	fix t2 = timer_get_fixed_seconds();

	bm_get_frame_usage(&bitmaps_used_this_frame,&bitmaps_new_this_frame);

	// safety check
	if (framecount > 0) { 
		modelstats_num_polys /= framecount;
		modelstats_num_verts /= framecount;

		Tmap_npixels /=framecount;
	}

	mprintf(( "'%s' is %.2f FPS\n", pof_file, i2fl(framecount)/f2fl(t2-t1) ));
	fprintf( Time_fp, "\"%s\"\t%.0f\t%d\t%d\t%d\t%d\n", pof_file, i2fl(framecount)/f2fl(t2-t1), bitmaps_used_this_frame, modelstats_num_polys, modelstats_num_verts, Tmap_npixels );
}

int Time_models = 0;
DCF_BOOL( time_models, Time_models )

void Do_model_timings_test()
{
	if ( !Time_models ) return;

	mprintf(( "Timing models!\n" ));

	ubyte model_used[MAX_POLYGON_MODELS] = {0};
	int model_id[MAX_POLYGON_MODELS];

	// Load them all
	for (auto & sip : Ship_info) {
		sip.model_num = model_load(sip.pof_file);

		model_used[sip.model_num % MAX_POLYGON_MODELS]++;
		model_id[sip.model_num % MAX_POLYGON_MODELS] = sip.model_num;
	}

	Texture_fp = fopen( NOX("ShipTextures.txt"), "wt" );
	if ( !Texture_fp ) return;

	Time_fp = fopen( NOX("ShipTimings.txt"), "wt" );
	if ( !Time_fp ) return;

	fprintf( Time_fp, "Name\tFPS\tTRAM\tPolys\tVerts\tPixels\n" );

	for (int i=0; i<MAX_POLYGON_MODELS; i++ )	{
		if ( model_used[i] )	{
			Time_model( model_id[i] );
		}
	}
	
	fprintf( Texture_fp, "Number too big: %d\n", Tmap_num_too_big );
	fprintf( Texture_fp, "Number of models needing splitting: %d\n", Num_models_needing_splitting );
	
	fclose(Time_fp);
	fclose(Texture_fp);

	exit(1);
}
#endif

/// Call this function when you want to inform the player that a feature is disabled in this build
void game_feature_disabled_popup()
{
	popup(PF_USE_AFFIRMATIVE_ICON|PF_BODY_BIG, 1, POPUP_OK, XSTR( "Sorry, the requested feature is currently disabled in this build", 1621));
}

/// format the specified time (fixed point) into a nice string
void game_format_time(fix m_time,char *time_str)
{
	char tmp[10];

	float mtime = f2fl(m_time);

	// get the hours, minutes and seconds
	int hours = (int)(mtime / 3600.0f);
	if(hours > 0){
		mtime -= (3600.0f * (float)hours);
	}
	int seconds = (int)mtime%60;
	int minutes = (int)mtime/60;

	// print the hour if necessary
	if(hours > 0){		
		sprintf(time_str,XSTR( "%d:", 201),hours);
		// if there are less than 10 minutes, print a leading 0
		if(minutes < 10){
			strcpy_s(tmp,NOX("0"));
			strcat(time_str,tmp);
		}		
	}	
	
	// print the minutes
	if(hours){
		sprintf(tmp,XSTR( "%d:", 201),minutes);
		strcat(time_str,tmp);
	} else {
		sprintf(time_str,XSTR( "%d:", 201),minutes);
	}

	// print the seconds
	if(seconds < 10){
		strcpy_s(tmp,NOX("0"));
		strcat(time_str,tmp);
	} 
	sprintf(tmp,"%d",seconds);
	strcat(time_str,tmp);
}

// ----------------------------------------------------------------
//
// Subspace Ambient Sound START
//
// ----------------------------------------------------------------

static sound_handle Subspace_ambient_left_channel  = sound_handle::invalid();
static sound_handle Subspace_ambient_right_channel = sound_handle::invalid();

// 
void game_start_subspace_ambient_sound()
{
	if (!Subspace_ambient_left_channel.isValid()) {
		Subspace_ambient_left_channel = snd_play_looping(gamesnd_get_game_sound(GameSounds::SUBSPACE_LEFT_CHANNEL), -1.0f);
	}

	if (!Subspace_ambient_right_channel.isValid()) {
		Subspace_ambient_right_channel = snd_play_looping(gamesnd_get_game_sound(GameSounds::SUBSPACE_RIGHT_CHANNEL), 1.0f);
	}
}

void game_stop_subspace_ambient_sound()
{
	if (Subspace_ambient_left_channel.isValid()) {
		snd_stop(Subspace_ambient_left_channel);
		Subspace_ambient_left_channel = sound_handle::invalid();
	}

	if (Subspace_ambient_right_channel.isValid()) {
		snd_stop(Subspace_ambient_right_channel);
		Subspace_ambient_right_channel = sound_handle::invalid();
	}
}

// ----------------------------------------------------------------
//
// Subspace Ambient Sound END
//
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// Language autodetection stuff
//

// default setting is "-1" to use registry setting with English as fall back
// DO NOT change that default setting here or something uncouth might happen
// in the localization code
int detect_lang()
{
	SCP_string first_font;

	// if the reg is set then let lcl_init() figure out what to do
	if (os_config_read_string( nullptr, NOX("Language"), nullptr ) != nullptr)
		return -1;

	// try and open the file to verify
	font::stuff_first(first_font);
	CFILE *detect = cfopen(const_cast<char*>(first_font.c_str()), "rb");

	// will use default setting if something went wrong
	if (!detect)
		return -1;

	// get the long checksum of the file
    uint file_checksum = 0;
	cfseek(detect, 0, SEEK_SET);
	cf_chksum_long(detect, &file_checksum);
	cfclose(detect);
	detect = nullptr;

	// now compare the checksum/filesize against known #'s
	for (int idx=0; idx < NUM_BUILTIN_LANGUAGES; idx++) {
		if (Lcl_builtin_languages[idx].checksum == (int)file_checksum) {
			mprintf(( "AutoLang: Language auto-detection successful...\n" ));
			return idx;
		}
	}

	// notify if a match was not found, include detected checksum
	mprintf(( "ERROR: Unknown Language Checksum: %i\n", (int)file_checksum ));
	mprintf(( "Using default language settings...\n" ));

	return -1;
}

//
// Eng Auto Lang stuff
// ----------------------------------------------------------------


// ----------------------------------------------------------------
// SHIPS TBL VERIFICATION STUFF
//

// checksums, just keep a list of all valid ones, if it matches any of them, keep it
#define NUM_SHIPS_TBL_CHECKSUMS		1

int Game_ships_tbl_checksums[NUM_SHIPS_TBL_CHECKSUMS] = {
//	-1022810006,					// 1.0 FULL
	-1254285366						// 1.2 FULL (German)
};

void verify_ships_tbl()
{	
	// detect if the packfile exists
	CFILE *detect = cfopen("ships.tbl", "rb");
	Game_ships_tbl_valid = 0;	 
	
	// not mission-disk
	if(!detect){
		Game_ships_tbl_valid = 0;
		return;
	}	

	// get the long checksum of the file
	uint file_checksum = 0;
	cfseek(detect, 0, SEEK_SET);	
	cf_chksum_long(detect, &file_checksum);
	cfclose(detect);
	detect = nullptr;

	// now compare the checksum/filesize against known #'s
	for(int Game_ships_tbl_checksum : Game_ships_tbl_checksums){
		if(Game_ships_tbl_checksum == (int)file_checksum){
			Game_ships_tbl_valid = 1;
			return;
		}
	}
}

DCF(shipspew, "display the checksum for the current ships.tbl")
{
	CFILE *detect = cfopen("ships.tbl", "rb");
	// get the long checksum of the file
	uint file_checksum = 0;
	cfseek(detect, 0, SEEK_SET);	
	cf_chksum_long(detect, &file_checksum);
	cfclose(detect);

	dc_printf("%d", file_checksum);
}

// ----------------------------------------------------------------
// WEAPONS TBL VERIFICATION STUFF
//

// checksums, just keep a list of all valid ones, if it matches any of them, keep it
#define NUM_WEAPONS_TBL_CHECKSUMS		1

int Game_weapons_tbl_checksums[NUM_WEAPONS_TBL_CHECKSUMS] = {
//	399297860,				// 1.0 FULL	
	-553984927				// 1.2 FULL (german)
};

void verify_weapons_tbl()
{
	// detect if the packfile exists
	CFILE *detect = cfopen("weapons.tbl", "rb");
	Game_weapons_tbl_valid = 0;	 
	
	// not mission-disk
	if(!detect){
		Game_weapons_tbl_valid = 0;
		return;
	}	

	// get the long checksum of the file
	uint file_checksum = 0;
	cfseek(detect, 0, SEEK_SET);	
	cf_chksum_long(detect, &file_checksum);
	cfclose(detect);
	detect = nullptr;

	// now compare the checksum/filesize against known #'s
	for(int Game_weapons_tbl_checksum : Game_weapons_tbl_checksums){
		if(Game_weapons_tbl_checksum == (int)file_checksum){
			Game_weapons_tbl_valid = 1;
			return;
		}
	}
}

DCF(wepspew, "display the checksum for the current weapons.tbl")
{
	CFILE *detect = cfopen("weapons.tbl", "rb");
	// get the long checksum of the file
	uint file_checksum = 0;
	cfseek(detect, 0, SEEK_SET);	
	cf_chksum_long(detect, &file_checksum);
	cfclose(detect);

	dc_printf("%d", file_checksum);
}

// if the game is running using hacked data
int game_hacked_data()
{
	if (MULTI_IS_TRACKER_GAME) {
		return multi_fs_tracker_validate_game_data();
	} else {
		// hacked!
		if(!Game_weapons_tbl_valid || !Game_ships_tbl_valid){
			return 1;
		}
	}

    // not hacked
    return 0;
}

void game_title_screen_draw(float alpha)
{
	game_set_frametime(-1); //Set the frame time
	
	// clamp alpha to avoid any funny business because this is a splash screen not a comedy sketch
	CLAMP(alpha, 0.0f, 1.0f);

	gr_clear();
	if (Game_title_bitmap != -1) {
		// set
		gr_set_bitmap(Game_title_bitmap, GR_ALPHABLEND_FILTER, GR_BITBLT_MODE_NORMAL, alpha);

		// get bitmap's width and height
		int width, height;
		bm_get_info(Game_title_bitmap, &width, &height);

		// set the screen scale to the bitmap's dimensions
		gr_set_screen_scale(width, height);

		// draw it in the center of the screen
		gr_bitmap((gr_screen.max_w_unscaled - width) / 2, (gr_screen.max_h_unscaled - height) / 2, GR_RESIZE_MENU);
	}

	if (Game_title_logo != -1) {
		gr_set_bitmap(Game_title_logo, GR_ALPHABLEND_FILTER, GR_BITBLT_MODE_NORMAL, alpha);
			
		// get bitmap's width and height
		int width, height;
		bm_get_info(Game_title_logo, &width, &height);

		// set the screen scale to the screen's dimensions
		gr_set_screen_scale(gr_screen.max_w, gr_screen.max_h);

		if (Splash_logo_center){
			gr_bitmap((gr_screen.max_w_unscaled - width) / 2, (gr_screen.max_h_unscaled - height) / 2, GR_RESIZE_MENU);
		} else {
			gr_bitmap(0, 0, GR_RESIZE_MENU);
		}
	}

		gr_reset_screen_scale();

	// flip
	gr_flip();
	os_poll();
}

void game_title_screen_display()
{
	// anything that shares code between standalone and normal cannot make
	// gr_* calls in standalone mode because all gr_ calls are NULL pointers
	if (Is_standalone) {
		return;
	}
	
	//As the script system isn't available here (it needs tables loaded and stuff), the on Splash Screen hook is removed from here.
	//It is deprecated as of 23.0 (where it was nonfunctional anyways), and will only be called for compatibility once the init has occurred after the splash screen -Lafiel
	
	Game_title_logo = bm_load(Game_logo_screen_fname[gr_screen.res]);

	// Search splash screens for matches
	bool any_exact_match = false;
	bool any_range_match = false;
	for (auto &ss : Splash_screens) {
		if (ss.aspect_ratio_exact != 0.0f) {
			ss.match_exact = fl_equal(ss.aspect_ratio_exact, gr_screen.clip_aspect, 0.01f);
		}
		if (ss.aspect_ratio_min != 0.0f || ss.aspect_ratio_max != 0.0f) {
			bool match = true;
			if (ss.aspect_ratio_min != 0.0f && ss.aspect_ratio_min > gr_screen.clip_aspect) {
				match = false;
			}
			if (ss.aspect_ratio_max != 0.0f && ss.aspect_ratio_max < gr_screen.clip_aspect) {
				match = false;
			}
			ss.match_range = match;
		}
		if (ss.match_exact) {
			any_exact_match = true;
		}
		if (ss.match_range) {
			any_range_match = true;
		}
	}

	// Filter splash screens
	Splash_screens.erase(
		std::remove_if(Splash_screens.begin(), Splash_screens.end(), [any_exact_match, any_range_match](const splash_screen& ss) {
			if (any_exact_match) {
				return !ss.match_exact;
			} else if (any_range_match) {
				return !ss.match_range;
			} else {
				return !ss.is_default;
			}
		}), Splash_screens.end()
	);

	const char* filename;
	if (Splash_screens.size() == 1) {
		filename = Splash_screens[0].filename.c_str();
	} else if (!Splash_screens.empty()) {
		filename = Splash_screens[Random::next((int)Splash_screens.size())].filename.c_str();
	} else {
		filename = Game_title_screen_fname[gr_screen.res];
	}
	mprintf(("Loading %s as splash screen\n", filename));
	Game_title_bitmap = bm_load(filename);

	if (Splash_fade_in_time > 0) {
		float alpha = 0.0f;
		int timer = timer_get_milliseconds() + Splash_fade_in_time;

		while (timer_get_milliseconds() < timer) {
			game_title_screen_draw(alpha); // draw the splash image

			// increment alpha to the next value
			float a_inc = 1.0f / ((Splash_fade_in_time / 1000) / flFrametime);
			alpha += a_inc;
		}
	} else {
		game_title_screen_draw(1.0f);
	}

}

void game_title_screen_close()
{
	// anything that shares code between standalone and normal cannot make
	// gr_* calls in standalone mode because all gr_ calls are NULL pointers
	if (Is_standalone) {
		return;
	}

	if (Splash_fade_out_time > 0) {
		//draw a starting splash image. This also reset the frametime
		//after asset loading messed with it. Helps to keep the fade out
		//nice and smooooooooth, baby.
		game_title_screen_draw(1.0f);

		float alpha = 1.0f;
		int timer = timer_get_milliseconds() + Splash_fade_out_time;

		while (timer_get_milliseconds() < timer) {
			game_title_screen_draw(alpha); // draw the splash image

			// decrement alpha to the next value
			float a_inc = 1.0f / ((Splash_fade_out_time / 1000) / flFrametime);
			alpha -= a_inc;
		}
	}

	if (Game_title_bitmap != -1) {
		bm_release(Game_title_bitmap);
		Game_title_bitmap = -1;
	}

	if (Game_title_logo != -1) {
		bm_release(Game_title_logo);
		Game_title_bitmap = -1;
	}
}

// return true if the game is running with "low memory", which is less than 48MB
bool game_using_low_mem()
{
	if (Use_low_mem == 0) {
		return false;
	} else {
		return true;
	}
}

//  place calls here that need to take effect immediately when the game is
//  minimized.  Called from osapi.cpp 
void game_pause()
{
	// Protection against flipping out -- Kazan
	if (!GameState_Stack_Valid())
		return;

	if (!pause_if_unfocused())
		return;

	if (!(Game_mode & GM_MULTIPLAYER)){
		switch ( gameseq_get_state() )
		{
			case GS_STATE_MAIN_MENU:
				main_hall_pause(); // not an instant shutoff of misc anims and sounds
				break;

			case GS_STATE_BRIEFING:
				brief_pause();
				break;

			case GS_STATE_DEBRIEF:
				debrief_pause();
				break;

			case GS_STATE_CMD_BRIEF:
				cmd_brief_pause();
				break;

			case GS_STATE_RED_ALERT:
				red_alert_voice_pause();
				break;

			// anything that would leave the ambient mainhall sound going
			case GS_STATE_TECH_MENU:
			case GS_STATE_BARRACKS_MENU:
				main_hall_stop_ambient();
				main_hall_stop_music(true); // not an instant shutoff
				break;

			// things that would get music except if they are called while in-mission
			case GS_STATE_OPTIONS_MENU:
			case GS_STATE_HUD_CONFIG:
				if ( !(Game_mode & GM_IN_MISSION) ) {
					main_hall_stop_ambient();
					main_hall_stop_music(true); // not an instant shutoff
				}
				break;

			// only has the ambient sound, no music
			case GS_STATE_INITIAL_PLAYER_SELECT:
				main_hall_stop_ambient();
				break;

			// pause_init is a special case and we don't unpause it ourselves
			case GS_STATE_GAME_PLAY:
				if ( (!popup_active()) && (!popupdead_is_active()) )
					pause_init();
				break;

			case GS_STATE_FICTION_VIEWER:
				fiction_viewer_pause();
				break;

			default:
				audiostream_pause_all();
		}
	}
}

// calls to be executed when the game is restored from minimized or inactive state
void game_unpause()
{
	if (!GameState_Stack_Valid())
		return;

	if (!pause_if_unfocused())
		return;

	// automatically recover from everything but an in-mission pause
	if (!(Game_mode & GM_MULTIPLAYER)) {
		switch ( gameseq_get_state() )
		{
			case GS_STATE_MAIN_MENU:
				main_hall_unpause();
				break;

			case GS_STATE_BRIEFING:
				brief_unpause();
				break;

			case GS_STATE_DEBRIEF:
				debrief_unpause();
				break;

			case GS_STATE_CMD_BRIEF:
				cmd_brief_unpause();
				break;

			case GS_STATE_RED_ALERT:
				red_alert_voice_unpause();
				break;

			// anything that would leave the ambient mainhall sound going
			case GS_STATE_TECH_MENU:
			case GS_STATE_BARRACKS_MENU:
				main_hall_start_ambient();
				main_hall_start_music(); // not an instant shutoff
				break;

			// things that would get music except if they are called while in-mission
			case GS_STATE_OPTIONS_MENU:
			case GS_STATE_HUD_CONFIG:
				if ( !(Game_mode & GM_IN_MISSION) ) {
					main_hall_start_ambient();
					main_hall_start_music(); // not an instant shutoff
				}
				break;

			// only has the ambient sound, no music
			case GS_STATE_INITIAL_PLAYER_SELECT:
				main_hall_start_ambient();
				break;

			// if in a game then do nothing, pause_init() should have been called
			// and will get cleaned up elsewhere
			case GS_STATE_GAME_PLAY:
				break;

			// ditto for if we explicitly paused the game and then minimized it
			case GS_STATE_GAME_PAUSED:
				break;

			case GS_STATE_FICTION_VIEWER:
				fiction_viewer_unpause();
				break;

			default:
				audiostream_unpause_all();
		}
	}
}

#define DONT_CATCH_MAIN_EXCEPTIONS
int main(int argc, char *argv[])
{
	int result = -1;
	Assert(argc > 0);

	crashdump::installCrashHandler();

#ifdef WIN32
	// Don't let more than one instance of FreeSpace run.
	HWND hwnd = FindWindow(NOX("FreeSpaceClass"), nullptr);
	if (hwnd)	{
		SetForegroundWindow(hwnd);
		return 0;
	}

	::CoInitialize(nullptr);

	SCP_mspdbcs_Initialise();

	bool keep_stdout = false;
	char envbuf[2];
	// GetEnvironmentVariable returns the amount of stored characters on success, we're looking for the value "1"
	// so we only care about the case where it successfully read exactly one character (not including the null byte).
	if (GetEnvironmentVariable("FSO_KEEP_STDOUT", envbuf, 2) == 1) {
		keep_stdout = envbuf[0] == '1';
	}

	// Only try redirecting stdout if FSO_KEEP_STDOUT is not set to "1".
	if (!keep_stdout) {
		// If we're being evoked from a console, attach the STDIO streams to it and reopen the streams
		// This is needed because Windows assumes SUBSYSTEM:WINDOWS programs won't need console IO.  Additionally, SDL
		// seems to close or otherwise grab the streams for somthing else.
		if (AttachConsole(ATTACH_PARENT_PROCESS)) {
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}
	}
#else
#ifdef APPLE_APP
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(getppid(), pathbuf, sizeof(pathbuf)) <= 0) {
        Warning(LOCATION, "Could not retrieve parent pidpath!");
    }
    if (strcmp("/sbin/launchd", pathbuf) == 0) {
        // Finder sets the working directory to the root of the drive so we have to get a little creative
        // to find out where on the disk we should be running from for CFILE's sake.
        char *path_name = SDL_GetBasePath();
        chdir(path_name);
        SDL_free(path_name);
    }
#endif
#endif

#if defined(GAME_ERRORLOG_TXT) && defined(_MSC_VER)
	__try {
#elif !defined(DONT_CATCH_MAIN_EXCEPTIONS)
	try {
#endif
		result = game_main(argc, argv);
#if defined(GAME_ERRORLOG_TXT) && defined(_MSC_VER)
	}
	__except (RecordExceptionInfo(GetExceptionInformation(), "FreeSpace 2 Main Thread")) {
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
#elif !defined(DONT_CATCH_MAIN_EXCEPTIONS)
	}
	catch (const std::exception &ex) {
		Error(LOCATION, "Caught std::exception in main(): '%s'!", ex.what());
		fprintf(stderr, "Caught std::exception in main(): '%s'!\n", ex.what());

		result = EXIT_FAILURE;
	}
	catch (...) {
		Error(LOCATION, "Caught exception in main()!");
		fprintf(stderr, "Caught exception in main()!\n");

		result = EXIT_FAILURE;
	}
#endif

#ifdef WIN32
	SCP_mspdbcs_Cleanup();

	::CoUninitialize();

#ifndef __MINGW32__
	_CrtDumpMemoryLeaks();
#endif
#endif

	return result;
}
