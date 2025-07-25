/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/




#include "cmdline/cmdline.h"
#include "osapi/osregistry.h"
#include "network/multi.h"
#include "network/multimsgs.h"
#include "network/multi_obj.h"
#include "freespace.h"
#include "network/stand_gui.h"
#include "network/multiutil.h"
#include "network/multi_voice.h"
#include "network/multi_options.h"
#include "network/multi_team.h"
#include "mission/missioncampaign.h"
#include "mission/missionparse.h"
#include "options/Option.h"
#include "parse/parselo.h"
#include "playerman/player.h"
#include "cfile/cfile.h"
#include "network/multi_fstracker.h"

#include <climits>



// ----------------------------------------------------------------------------------
// MULTI OPTIONS DEFINES/VARS
//

// packet codes
#define MULTI_OPTION_SERVER						0				// server update follows
#define MULTI_OPTION_LOCAL							1				// local netplayer options follow
#define MULTI_OPTION_START_GAME					2				// host's start game options on the standalone server
#define MULTI_OPTION_MISSION						3				// host's mission selection stuff on a standalone server

// global options
#define MULTI_CFG_FILE								NOX("multi.cfg")
multi_global_options Multi_options_g;

char Multi_options_proxy[512] = "";
ushort Multi_options_proxy_port = 0;
bool Multi_cfg_missing = true;

auto TogglePXOOption __UNUSED = options::OptionBuilder<bool>("Multi.TogglePXO",
									std::pair<const char*, int>{"PXO", 1383},
									std::pair<const char*, int>{"Whether or not to play games on the local network or on PXO", 1809})
									.category(std::make_pair("Multi", 1828))
									.level(options::ExpertLevel::Beginner)
									.default_val(false)  // false to match FSO defaults, will set to default when loading new pilots anyway
									.bind_to(&Multi_options_g.pxo)
									.importance(10)
									.flags({options::OptionFlags::RetailBuiltinOption})
									.finish();

// These next few options are a little messy because the values are stored in the player file
// rather than a global. The Change_Listener runs on game start and when the option is changed.
// The "initial" paramter is used to skip setting the player flag on game start because Player
// hasn't been initialized yet. On Player Load the static local globals are set based on the
// player flags and the options will read/save those as necessary. When an option is changed
// any time after startup the Change_Listener will set the player flags. We don't need to manually
// save the player flags because the player file already does that. So after the local globals
// and player flags are sync'd up at game start then we are good to go for the entire runtime.

static bool BroadcastGamesLocally = false;

static bool local_broadcast_change(bool val, bool initial)
{
	if (initial) {
		return false;
	} else {
		if (!val) {
			Player->m_local_options.flags &= ~(MLO_FLAG_LOCAL_BROADCAST);
		} else {
			Player->m_local_options.flags |= MLO_FLAG_LOCAL_BROADCAST;
		}
		BroadcastGamesLocally = (Player->m_local_options.flags & MLO_FLAG_LOCAL_BROADCAST);
		return true;
	}
}

auto LocalBroadcastOption __UNUSED = options::OptionBuilder<bool>("Multi.LocalBroadcast",
									std::pair<const char*, int>{"Broadcast Locally", 1387},
									std::pair<const char*, int>{"Whether or not to broadcast games on the local network", 1808})
									.category(std::make_pair("Multi", 1828))
									.level(options::ExpertLevel::Beginner)
									.default_val(true) // true to match FSO defaults, will set to default when loading new pilots anyway
                                    .bind_to(&BroadcastGamesLocally)
									.change_listener(local_broadcast_change)
									.importance(10)
									.flags({options::OptionFlags::RetailBuiltinOption})
									.finish();

static bool AlwaysFlushCache = false;

static bool flush_cache_change(bool val, bool initial)
{
	if (initial) {
		return false;
	} else {
		if (!val) {
			Player->m_local_options.flags &= ~(MLO_FLAG_FLUSH_CACHE);
		} else {
			Player->m_local_options.flags |= MLO_FLAG_FLUSH_CACHE;
		}
		AlwaysFlushCache = (Player->m_local_options.flags & MLO_FLAG_FLUSH_CACHE);
		return true;
	}
}

static SCP_string flush_cache_display(bool mode) { return mode ? XSTR("Before Game", 1401) : XSTR("Never", 1400); }

auto FlushCacheOption __UNUSED = options::OptionBuilder<bool>("Multi.FlushCache",
									std::pair<const char*, int>{"Flush Cache", 1399},
									std::pair<const char*, int>{"Whether or not flush the multidata cache before games", 1810})
									.category(std::make_pair("Multi", 1828))
									.level(options::ExpertLevel::Beginner)
                                    .flags({options::OptionFlags::ForceMultiValueSelection, options::OptionFlags::RetailBuiltinOption})
                                    .display(flush_cache_display) 
									.default_val(false)
                                    .bind_to(&AlwaysFlushCache)
									.change_listener(flush_cache_change)
									.importance(10)
									.finish();

static bool CacheMissionsToMultidata = false;

static bool transfer_missions_change(bool val, bool initial)
{
	if (initial) {
		return false;
	} else {
		if (!val) {
			Player->m_local_options.flags &= ~(MLO_FLAG_XFER_MULTIDATA);
		} else {
			Player->m_local_options.flags |= MLO_FLAG_XFER_MULTIDATA;
		}
		CacheMissionsToMultidata = (Player->m_local_options.flags & MLO_FLAG_XFER_MULTIDATA);
		return true;
	}
}

static SCP_string transfer_missions_display(bool mode) { return mode ? XSTR("/multidata", 1397) : XSTR("/missions", 1398); }

auto TransferMissionsOption __UNUSED = options::OptionBuilder<bool>("Multi.TransferMissions",
									std::pair<const char*, int>{"Transfer Missions", 1396},
									std::pair<const char*, int>{"What appdata folder to save missions to", 1811})
									.category(std::make_pair("Multi", 1828))
									.level(options::ExpertLevel::Beginner)
                                    .flags({options::OptionFlags::ForceMultiValueSelection, options::OptionFlags::RetailBuiltinOption})
                                    .display(transfer_missions_display) 
									.default_val(false)
                                    .bind_to(&CacheMissionsToMultidata)
									.change_listener(transfer_missions_change)
									.importance(10)
									.finish();

// ----------------------------------------------------------------------------------
// MULTI OPTIONS FUNCTIONS
//

// load in the config file
#define NEXT_TOKEN()						do { tok = strtok(NULL, "\n"); if(tok != NULL){ drop_leading_white_space(tok); drop_trailing_white_space(tok); } } while(false);
#define SETTING(s)						( !stricmp(tok, s) )
void multi_options_read_config()
{
	CFILE *in;
	char str[512];
	char *tok = NULL;

	// set default value for the global multi options
	Multi_options_g.reset();

	ushort forced_port = (ushort)os_config_read_uint(NULL, "ForcePort", 0);
	Multi_options_g.port = (Cmdline_network_port >= 0) ? (ushort)Cmdline_network_port : forced_port == 0 ? (ushort)DEFAULT_GAME_PORT : forced_port;
	Multi_options_g.log = (Cmdline_multi_log) ? 1 : 0;

	// read in the config file
	in = cfopen(MULTI_CFG_FILE, "rt", CF_TYPE_DATA);
	
	// if we failed to open the config file, user default settings
	if (in == NULL) {
		nprintf(("Network","Failed to open network config file, using default settings\n"));		
	} else {
		Multi_cfg_missing = false;

		while ( !cfeof(in) ) {
			// read in the game info
			memset(str, 0, 512);
			cfgets(str, 512, in);

			// parse the first line
			tok = strtok(str, " \t");

			// check the token
			if (tok != NULL) {
				drop_leading_white_space(tok);
				drop_trailing_white_space(tok);			
			} else {
				continue;
			}		

			// all possible options

			// only standalone cares about the following options
			if (Is_standalone) {
				// setup PXO mode
				if ( SETTING("+pxo") ) {
					Multi_options_g.pxo = true;
					// force protocol to TCP
					Multi_options_g.protocol = NET_TCP;
					NEXT_TOKEN();
					if (tok != NULL) {
						strncpy(Multi_fs_tracker_channel, tok, MAX_PATH-1);
					}
				} else
				// set the standalone server's permanent name
				if	( SETTING("+name") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						strncpy(Multi_options_g.std_pname, tok, STD_NAME_LEN);
					}
				} else
				// standalone won't allow voice transmission
				if ( SETTING("+no_voice") ) {
					Multi_options_g.std_voice = 0;
				} else
				// set the max # of players on the standalone
				if ( SETTING("+max_players") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						if ( !((atoi(tok) < 1) || (atoi(tok) > MAX_PLAYERS)) ) {
							Multi_options_g.std_max_players = atoi(tok);
						}
					}
				} else
				// ban a player
				if ( SETTING("+ban") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						std_add_ban(tok);
					}
				} else
				// set the standalone host password
				if ( SETTING("+passwd") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						strncpy(Multi_options_g.std_passwd, tok, STD_PASSWD_LEN);
#ifdef _WIN32
						// yuck
						extern HWND Multi_std_host_passwd;
						SetWindowText(Multi_std_host_passwd, Multi_options_g.std_passwd);
#else
						// TODO: get password ?
						// argh, gonna have to figure out how to do this - mharris 07/07/2002
#endif
					}
				} else
				// set standalone to low updates
				if ( SETTING("+low_update") ) {
					Multi_options_g.std_datarate = OBJ_UPDATE_LOW;
				} else
				// set standalone to medium updates
				if ( SETTING("+med_update") ) {
					Multi_options_g.std_datarate = OBJ_UPDATE_MEDIUM;
				} else
				// set standalone to high updates
				if ( SETTING("+high_update") ) {
					Multi_options_g.std_datarate = OBJ_UPDATE_HIGH;
				} else
				// set standalone to high updates
				if ( SETTING("+lan_update") ) {
					Multi_options_g.std_datarate = OBJ_UPDATE_LAN;
				} else
				if ( SETTING("+webui_root") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						Multi_options_g.webuiRootDirectory = SCP_string(tok);
					}
				} else
				if ( SETTING("+webapi_username") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						Multi_options_g.webapiUsername = SCP_string(tok);
					}
				} else
				if ( SETTING("+webapi_password") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						Multi_options_g.webapiPassword = SCP_string(tok);
					}
				} else
				if ( SETTING("+webapi_server_port") ) {
					NEXT_TOKEN();
					if (tok != NULL) {
						long int result = strtol(tok, NULL, 10);
						if(result <= 0 || result > USHRT_MAX) {
							mprintf(("ERROR: Invalid or out of range webapi_server_port '%s' specified in multi.cfg, must be integer between 1024 and %i.\n", tok, USHRT_MAX));
						}
						else if(result < 1024) {
							mprintf(("ERROR: webapi_server_port '%ld' in multi.cfg is too low, must be between 1024 and %d.\n", result, USHRT_MAX));
						}
						else {
							mprintf(("Using webapi_server_port '%ld' from multi.cfg.\n", result));
							Multi_options_g.webapiPort = (ushort) result;
						}
					}
				}
			}

			// ... common to all modes ...

			// ip addr of user tracker
			if ( SETTING("+user_server") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					strcpy_s(Multi_options_g.user_tracker_ip, tok);
				}
			} else
			// ip addr of game tracker
			if ( SETTING("+game_server") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					strcpy_s(Multi_options_g.game_tracker_ip, tok);
				}
			} else
			// ip addr of pxo chat server
			if ( SETTING("+chat_server") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					strcpy_s(Multi_options_g.pxo_ip, tok);
				}
			} else
			// url of pilot rankings page
			if ( SETTING("+rank_url") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					strcpy_s(Multi_options_g.pxo_rank_url, tok);
				}
			} else
			// url of pxo account create page
			if ( SETTING("+create_url") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					strcpy_s(Multi_options_g.pxo_create_url, tok);
				}
			} else
			// url of pxo account verify page
			if ( SETTING("+verify_url") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					strcpy_s(Multi_options_g.pxo_verify_url, tok);
				}
			} else
			// url of pxo banners
			if ( SETTING("+banner_url") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					strcpy_s(Multi_options_g.pxo_banner_url, tok);
				}
			} else
			// set the max datarate for high updates
			if ( SETTING("+datarate") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					if ( atoi(tok) >= 4000 ) {
						Multi_options_g.datarate_cap = atoi(tok);
					}
				}			
			} else
			// get the proxy server
			if ( SETTING("+http_proxy") ) {
				NEXT_TOKEN();
				if (tok != NULL) {
					char *ip = strtok(tok, ":");

					if (ip != NULL) {
						strcpy_s(Multi_options_proxy, ip);
					}

					ip = strtok(NULL, "");

					if (ip != NULL) {
						Multi_options_proxy_port = (ushort)atoi(ip);
					} else {
						strcpy_s(Multi_options_proxy, "");
					}
				}
			}
		}

		// close the config file
		cfclose(in);
		in = NULL;
	}

	// sanitize config options for PXO
	multi_fs_tracker_verify_options();

#ifndef _WIN32
	if (Is_standalone) {
		std_configLoaded(&Multi_options_g);
	}
#endif
}

// set netgame defaults 
// NOTE : should be used when creating a newpilot
void multi_options_set_netgame_defaults(multi_server_options *options)
{
	// any player can do squadmate messaging
	options->squad_set = MSO_SQUAD_ANY;

	// only the host can end the game
	options->endgame_set = MSO_END_HOST;

	// allow ingame file xfer and custom pilot pix
	options->flags = (MSO_FLAG_INGAME_XFER | MSO_FLAG_ACCEPT_PIX);

	// set the default time limit to be -1 (no limit)
	options->mission_time_limit = fl2f(-1.0f);

	// set the default max kills for a mission
	options->kill_limit = 9999;

	// set the default # of respawns
	options->respawn = 2;

	// set the default # of max observers
	options->max_observers = 2;

	// set the default netgame qos
	options->voice_qos = 10;

	// set the default token timeout
	options->voice_token_wait = 2000;				// he must wait 2 seconds between voice gets

	// set the default max voice record time
	options->voice_record_time = 5000;

	// Set default skill level to medium
	options->skill_level = NUM_SKILL_LEVELS / 2;
}

// set local netplayer defaults
// NOTE : should be used when creating a newpilot
void multi_options_set_local_defaults(multi_local_options *options)
{
	// accept pix by default and broadcast on the local subnet
	options->flags = (MLO_FLAG_ACCEPT_PIX | MLO_FLAG_LOCAL_BROADCAST);	

	// set the object update level based on the type of network connection specified by the mod
	options->obj_update_level = Default_multi_object_update_level;
}

// fill in the passed netgame options struct with the data from my player file data (only host/server should do this)
void multi_options_netgame_load(multi_server_options *options)
{
	if(options != NULL){
		memcpy(options,&Player->m_server_options,sizeof(multi_server_options));
	}	
}

// fill in the passed local options struct with the data from my player file data (all machines except standalone should do this)
void multi_options_local_load(multi_local_options *options, net_player *pxo_pl)
{
	if(options != NULL){
		memcpy(options,&Player->m_local_options,sizeof(multi_local_options));
	}

	// stuff pxo squad info
	if(pxo_pl != NULL){
		strcpy_s(pxo_pl->p_info.pxo_squad_name, Multi_tracker_squad_name);		
	}
}

// fill out the in-game options local globals using the player data
void multi_options_init_globals()
{
	BroadcastGamesLocally = (Player->m_local_options.flags & MLO_FLAG_LOCAL_BROADCAST);
	AlwaysFlushCache = (Player->m_local_options.flags & MLO_FLAG_FLUSH_CACHE);
	CacheMissionsToMultidata = (Player->m_local_options.flags & MLO_FLAG_XFER_MULTIDATA);
}

// add data from a multi_server_options struct
void add_server_options(ubyte *data, int *size, const multi_server_options *mso)
{
	int packet_size = *size;

	// misc settings and flags
	ADD_DATA(mso->squad_set);
	ADD_DATA(mso->endgame_set);
	ADD_INT(mso->flags);

	// default respawn count
	ADD_UINT(mso->respawn);

	// default max # of observers
	ADD_DATA(mso->max_observers);

	// default skill level
	ADD_DATA(mso->skill_level);

	// voice settings
	ADD_DATA(mso->voice_qos);
	ADD_INT(mso->voice_token_wait);
	ADD_INT(mso->voice_record_time);

	// time limit
	ADD_INT(mso->mission_time_limit);

	// kill limit
	ADD_INT(mso->kill_limit);

	*size = packet_size;
}

// add data from a multi_local_options struct
void add_local_options(ubyte *data, int *size, const multi_local_options *mlo)
{
	int packet_size = *size;

	ADD_INT(mlo->flags);
	ADD_INT(mlo->obj_update_level);

	*size = packet_size;
}

// get data from multi_server_options struct
void get_server_options(ubyte *data, int *size, multi_server_options *mso)
{
	int offset = *size;

	// misc settings and flags
	GET_DATA(mso->squad_set);
	GET_DATA(mso->endgame_set);
	GET_INT(mso->flags);

	// default respawn count
	GET_UINT(mso->respawn);

	// default max # of observers
	GET_DATA(mso->max_observers);

	// default skill level
	GET_DATA(mso->skill_level);

	// voice settings
	GET_DATA(mso->voice_qos);
	GET_INT(mso->voice_token_wait);
	GET_INT(mso->voice_record_time);

	// time limit
	GET_INT(mso->mission_time_limit);

	// kill limit
	GET_INT(mso->kill_limit);

	*size = offset;
}

// get data from multi_local_options struct
void get_local_options(ubyte *data, int *size, multi_local_options *mlo)
{
	int offset = *size;

	GET_INT(mlo->flags);
	GET_INT(mlo->obj_update_level);

	*size = offset;
}

// update everyone on the current netgame options
void multi_options_update_netgame()
{
	ubyte data[MAX_PACKET_SIZE],code;
	int packet_size = 0;
	
	Assert(Net_player->flags & NETINFO_FLAG_GAME_HOST);

	// build the header and add the opcode
	BUILD_HEADER(OPTIONS_UPDATE);
	code = MULTI_OPTION_SERVER;
	ADD_DATA(code);

	// add the netgame options
	add_server_options(data, &packet_size, &Netgame.options);

	// send the packet
	if(Net_player->flags & NETINFO_FLAG_AM_MASTER){
		multi_io_send_to_all_reliable(data, packet_size);
	} else {
		multi_io_send_reliable(Net_player, data, packet_size);
	}
}

// update everyone with my local settings
void multi_options_update_local()
{
	ubyte data[MAX_PACKET_SIZE],code;
	int packet_size = 0;
	
	// if i'm the server, don't do anything
	if(Net_player->flags & NETINFO_FLAG_AM_MASTER){
		return;
	}

	// build the header and add the opcode
	BUILD_HEADER(OPTIONS_UPDATE);
	code = MULTI_OPTION_LOCAL;
	ADD_DATA(code);

	// add the netgame options
	add_local_options(data, &packet_size, &Net_player->p_info.options);

	// send the packet		
	multi_io_send_reliable(Net_player, data, packet_size);
}

// update the standalone with the settings I have picked at the "start game" screen
void multi_options_update_start_game(netgame_info *ng)
{
	ubyte data[MAX_PACKET_SIZE],code;
	int packet_size = 0;

	// should be a host on a standalone
	Assert((Net_player->flags & NETINFO_FLAG_GAME_HOST) && !(Net_player->flags & NETINFO_FLAG_AM_MASTER));

	// build the header
	BUILD_HEADER(OPTIONS_UPDATE);
	code = MULTI_OPTION_START_GAME;
	ADD_DATA(code);

	// add the start game options
	ADD_STRING(ng->name);
	ADD_INT(ng->mode);
	ADD_INT(ng->security);

	// add mode-specific data
	switch(ng->mode){
	case NG_MODE_PASSWORD:
		ADD_STRING(ng->passwd);
		break;
	case NG_MODE_RANK_ABOVE:
	case NG_MODE_RANK_BELOW:
		ADD_INT(ng->rank_base);
		break;
	}

	// send to the standalone server	
	multi_io_send_reliable(Net_player, data, packet_size);
}

// update the standalone with the mission settings I have picked (mission filename, etc)
void multi_options_update_mission(netgame_info *ng, int campaign_mode)
{
	ubyte data[MAX_PACKET_SIZE],code;
	int packet_size = 0;

	// should be a host on a standalone
	Assert((Net_player->flags & NETINFO_FLAG_GAME_HOST) && !(Net_player->flags & NETINFO_FLAG_AM_MASTER));

	// build the header
	BUILD_HEADER(OPTIONS_UPDATE);
	code = MULTI_OPTION_MISSION;
	ADD_DATA(code);

	// type (coop or team vs. team)
	ADD_INT(ng->type_flags);

	// respawns
	ADD_UINT(ng->respawn);

	// add the mission/campaign filename
	code = (ubyte)campaign_mode;
	ADD_DATA(code);
	if(campaign_mode){
		ADD_STRING(ng->campaign_name);
	} else {
		ADD_STRING(ng->mission_name);
	}

	// send to the server	
	multi_io_send_reliable(Net_player, data, packet_size);
}


// ----------------------------------------------------------------------------------
// MULTI OPTIONS FUNCTIONS
//

// process an incoming multi options packet
void multi_options_process_packet(unsigned char *data, header *hinfo)
{
	ubyte code;	
	multi_local_options bogus;
	int idx,player_index;
	char str[255];
	int offset = HEADER_LENGTH;

	// find out who is sending this data	
	player_index = find_player_index(hinfo->id);

	if (player_index < 0) {
		nprintf(("Network", "Received packet from unknown player!\n"));
		return;
	}

	// get the packet code
	GET_DATA(code);
	switch(code){
	// get the start game options
	case MULTI_OPTION_START_GAME:
		Assert(Game_mode & GM_STANDALONE_SERVER);

		// get the netgame name
		GET_STRING(Netgame.name);		

		// get the netgame mode
		GET_INT(Netgame.mode);

		// get the security #
		GET_INT(Netgame.security);

		// get mode specific data
		switch(Netgame.mode){
		case NG_MODE_PASSWORD:
			GET_STRING(Netgame.passwd);
			break;
		case NG_MODE_RANK_ABOVE:
		case NG_MODE_RANK_BELOW:
			GET_INT(Netgame.rank_base);
			break;
		}

		// update standalone stuff
		std_connect_set_gamename(Netgame.name);
		std_multi_update_netgame_info_controls();
		break;

	// get mission choice options
	case MULTI_OPTION_MISSION: {
		netgame_info ng;
		SCP_string title;
		int campaign_type,max_players;

		Assert(Game_mode & GM_STANDALONE_SERVER);

		// coop or team vs. team mode
		GET_INT(ng.type_flags);
		if((ng.type_flags & NG_TYPE_TEAM) && !(Netgame.type_flags & NG_TYPE_TEAM)){
			multi_team_reset();
		}
		// if squad war was switched on
		if((ng.type_flags & NG_TYPE_SW) && !(Netgame.type_flags & NG_TYPE_SW)){
			mprintf(("STANDALONE TURNED ON SQUAD WAR!!\n"));
		}
		Netgame.type_flags = ng.type_flags;

		// new respawn count
		GET_UINT(Netgame.respawn);

		// name string
		memset(str,0,255);

		GET_DATA(code);
		// campaign mode
		if(code){
			GET_STRING(ng.campaign_name);

			// set the netgame max players here if the filename has changed
			if(strcmp(Netgame.campaign_name,ng.campaign_name) != 0){				
				if(!mission_campaign_get_info(ng.campaign_name,title,&campaign_type,&max_players)){
					Netgame.max_players = 0;
				} else {
					Netgame.max_players = max_players;
				}

				strcpy_s(Netgame.campaign_name,ng.campaign_name);
			}

			Netgame.campaign_mode = 1;

			// put brackets around the campaign name
			if(Game_mode & GM_STANDALONE_SERVER){
				strcpy_s(str,"(");
				strcat_s(str,Netgame.campaign_name);
				strcat_s(str,")");
				std_multi_set_standalone_mission_name(str);
			}
		}
		// non-campaign mode
		else {
			GET_STRING(ng.mission_name);

			if(strcmp(Netgame.mission_name,ng.mission_name) != 0){
				if(strlen(ng.mission_name)){
					Netgame.max_players = mission_parse_get_multi_mission_info( ng.mission_name );
				} else {
					// setting this to -1 will prevent us from being seen on the network
					Netgame.max_players = -1;				
				}
				strcpy_s(Netgame.mission_name,ng.mission_name);
				strcpy_s(Game_current_mission_filename,Netgame.mission_name);				
			}			

			Netgame.campaign_mode = 0;
            
			// set the mission name
			if(Game_mode & GM_STANDALONE_SERVER){
				std_multi_set_standalone_mission_name(Netgame.mission_name);			
			}
		}

		send_netgame_update_packet();	   
		break;
	}

	// get the netgame options
	case MULTI_OPTION_SERVER:		
		get_server_options(data, &offset, &Netgame.options);

		// if we're a standalone set for no sound, do so here
		if((Game_mode & GM_STANDALONE_SERVER) && !Multi_options_g.std_voice){
			Netgame.options.flags |= MSO_FLAG_NO_VOICE;
		} else {
			// maybe update the quality of sound
			multi_voice_maybe_update_vars(Netgame.options.voice_qos,Netgame.options.voice_record_time);
		}

		// set the skill level
		Game_skill_level = Netgame.options.skill_level;		

		if((Game_mode & GM_STANDALONE_SERVER) && !(Game_mode & GM_CAMPAIGN_MODE)){
			Netgame.respawn = Netgame.options.respawn;
		}

		// if we have the "temp closed" flag toggle
		if(Netgame.options.flags & MLO_FLAG_TEMP_CLOSED){
			Netgame.flags ^= NG_FLAG_TEMP_CLOSED;
		}
		Netgame.options.flags &= ~(MLO_FLAG_TEMP_CLOSED);

		// if i'm the standalone server, I should rebroadcast to all other players
		if(Game_mode & GM_STANDALONE_SERVER){
			for(idx=0;idx<MAX_PLAYERS;idx++){
				if(MULTI_CONNECTED(Net_players[idx]) && (Net_player != &Net_players[idx]) && (&Net_players[idx] != &Net_players[player_index]) ){
					multi_io_send_reliable(&Net_players[idx], data, offset);
				}
			}

			send_netgame_update_packet();
		}
		break;
	
	// local netplayer options
	case MULTI_OPTION_LOCAL:
		if(player_index == -1){
			get_local_options(data, &offset, &bogus);
		} else {
			get_local_options(data, &offset, &Net_players[player_index].p_info.options);

			//If the client has sent an object update higher than that which the server allows, reset it
			if (Net_player->flags & NETINFO_FLAG_AM_MASTER) {
				if (Net_players[player_index].p_info.options.obj_update_level > Cmdline_objupd) {
					Net_players[player_index].p_info.options.obj_update_level = Cmdline_objupd;
				}
			}
		}		
		break;
	}
	PACKET_SET_SIZE();
}
