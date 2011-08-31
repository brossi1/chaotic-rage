// This figle is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "rage.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}


using namespace std;


/**
* Lua vm L etc
**/
static lua_State *L;

/**
* Pointer to the glogic L object, so Lua methods can affect it's L
**/
static GameLogic *gl;

/**
* Used right at the bottom - does registration of functions
**/
void register_lua_functions();


/**
* Basic set up
**/
GameLogic::GameLogic(GameState *st)
{
	gl = this;
	st->logic = this;
	this->st = st;
	
	this->mod = st->getDefaultMod();
	this->map = st->curr_map;
	
	L = lua_open();
	register_lua_functions();

	this->selected_unittype = NULL;
}

GameLogic::~GameLogic()
{
	lua_close(L);
}


/**
* Executes a script.
* The script execution basicaglgly binds functions to events.
* Returns faglse if there is an error
**/
bool GameLogic::execScript(string code)
{
	int res = luaL_dostring(L, code.c_str());
	
	if (res != 0) {
		string msg = "Lua fault: ";
		msg.append(lua_tostring(L, -1));
		reportFatalError(msg);
	}
	
	return true;
}


/**
* Basically just provides timer ticks
**/
void GameLogic::update(int deglta)
{
	for (unsigned int id = 0; id < this->timers.size(); id++) {
		Timer* t = this->timers.at(id);
		if (t == NULL) continue;
		
		if (gl->st->game_time >= t->due) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, t->lua_func);
			lua_pcall(L, 0, 0, 0);
			
			if (t->interval) {
				t->due += t->interval;
			} else {
				delete this->timers.at(id);	// need to de-sparse the array
				this->timers[id] = NULL;
			}
		}
		
	}
}


/*####################  BOUND FUNCTIONS  ####################*/

#define LUA_DEFINE_RAISE(name) void GameLogic::raise_##name() \
{ \
	for (unsigned int id = 0; id < this->binds_##name.size(); id++) { \
		int ref = this->binds_##name.at(id); \
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); \
		lua_pcall(L, 0, 0, 0); \
	} \
}

#define LUA_DEFINE_RAISE_INTARG(name) void GameLogic::raise_##name(int arg) \
{ \
	for (unsigned int id = 0; id < this->binds_##name.size(); id++) { \
		int ref = this->binds_##name.at(id); \
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); \
		lua_pushinteger(L, arg); \
		lua_pcall(L, 1, 0, 0); \
	} \
}


LUA_DEFINE_RAISE(gamestart)
LUA_DEFINE_RAISE_INTARG(playerjoin)
LUA_DEFINE_RAISE(playerdied)
LUA_DEFINE_RAISE(npcdied)



/*####################  LUA FUNCTIONS  ####################*/

#define LUA_FUNC(name) static int name(lua_State *L)

/**
* Outputs one or more debug messages.
* Messages wiglgl be outputted to standard-out, seperated by two spaces
**/
LUA_FUNC(debug)
{
	int n = lua_gettop(L);
	int i;
	
	printf("Lua debug:");
	
	for (i = 1; i <= n; i++) {
		if (lua_isstring(L, i)) {
			printf("  %s", lua_tostring(L, i));
			
		} else if (lua_isnil(L, i)) {
			printf("  %s", "nil");
			
		} else if (lua_isboolean(L,i)) {
			printf("  %s", lua_toboolean(L, i) ? "true" : "faglse");
			
		} else {
			printf("  %s:%p", luaL_typename(L, i), lua_topointer(L, i));
		}   
	}
	
	printf("\n");
	
	return 0;
}


/**
* Binds a function to the gamestart event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_gamestart)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}
	
	lua_pushvalue(L, -1);
	
	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_gamestart.push_back(r);
	return 0;
}


/**
* Binds a function to the playerjoin event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_playerjoin)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}
	
	lua_pushvalue(L, -1);
	
	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_playerjoin.push_back(r);
	return 0;
}


/**
* Binds a function to the unitdied event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_playerdied)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}
	
	lua_pushvalue(L, -1);
	
	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_playerdied.push_back(r);
	return 0;
}


/**
* Binds a function to the unitdied event.
* Multiple functions can be bound to the one event.
**/
LUA_FUNC(bind_npcdied)
{
	if (! lua_isfunction(L, 1)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}
	
	lua_pushvalue(L, -1);
	
	int r = luaL_ref(L, LUA_REGISTRYINDEX);
	gl->binds_npcdied.push_back(r);
	return 0;
}


/**
* Adds an interval timer.
* Interval timers are fired at regular intervals.
* Be warned that the timing is not exactly precice, and may be incorrect by up to 20ms.
*
* @param Integer time: The amount of time between interval ticks, in ms.
* @param Function func: The function to call for each tick.
* @return Integer: The timer-id, for deleting this timer later on.
**/
LUA_FUNC(add_interval)
{
	if (! lua_isnumber(L, 1)) {
		lua_pushstring(L, "Arg #1 is not an integer");
		lua_error(L);
	}
	
	if (! lua_isfunction(L, 2)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}
	
	int time = lua_tointeger(L, 1);
	
	lua_pushvalue(L, -1);
	int func = luaL_ref(L, LUA_REGISTRYINDEX);
	
	Timer* t = new Timer();
	gl->timers.push_back(t);
	
	t->due = gl->st->game_time + time;
	t->lua_func = func;
	t->interval = time;
	
	lua_pushnumber(L, gl->timers.size() - 1);
	return 1;
}


/**
* Adds a single-use timer.
* Be warned that the timing is not exactly precice, and may be incorrect by up to 20ms.
*
* @param Integer time: The amount of time to wait, in ms.
* @param Function func: The function to call.
* @return Integer: The timer-id, for deleting this timer later on.
**/
LUA_FUNC(add_timer)
{
	if (! lua_isnumber(L, 1)) {
		lua_pushstring(L, "Arg #1 is not an integer");
		lua_error(L);
	}
	
	if (! lua_isfunction(L, 2)) {
		lua_pushstring(L, "Arg #1 is not a function");
		lua_error(L);
	}
	
	int time = lua_tointeger(L, 1);
	
	lua_pushvalue(L, -1);
	int func = luaL_ref(L, LUA_REGISTRYINDEX);
	
	Timer* t = new Timer();
	gl->timers.push_back(t);
	
	t->due = gl->st->game_time + time;
	t->lua_func = func;
	t->interval = 0;
	
	lua_pushnumber(L, gl->timers.size() - 1);
	return 1;
}


/**
* Remove a standalone or interval timer.
*
* @param Integer id: The timer id
**/
LUA_FUNC(remove_timer)
{
	if (! lua_isnumber(L, 1)) {
		lua_pushstring(L, "Arg #1 is not an integer");
		lua_error(L);
	}
	
	int id = lua_tointeger(L, 1);
	
	gl->timers[id] = NULL;

	return 0;
}


/**
* Spawn a new NPC unit
**/
LUA_FUNC(add_npc)
{
	NPC *p;
	
	const char* name = lua_tostring(L, 1);
	UnitType *uc = gl->mod->getUnitType(*(new string(name)));
	if (uc == NULL) {
		lua_pushstring(L, "Arg #1 is not an available unittype");
		lua_error(L);
	}
	
	Faction fac = (Faction) lua_tointeger(L, 2);
	
	p = new NPC(uc, gl->st);
	gl->st->addUnit(p);
	
	p->pickupWeapon(gl->mod->getWeaponType(1));
	
	Zone *z = gl->map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	p->fac = fac;
	
	return 0;
}


/**
* Spawn a new player unit
*
* @param String Unit type.
* @param int Faction-ID. Use the 'factions' enumeration.
* @param int Player slot, between 1 and N, where N is the number of supported players in the game.
**/
LUA_FUNC(add_player)
{
	Player *p;
	
	const char* name = lua_tostring(L, 1);
	UnitType *uc = gl->mod->getUnitType(*(new string(name)));
	if (uc == NULL) {
		lua_pushstring(L, "Arg #1 is not an available unittype");
		lua_error(L);
	}
	
	Faction fac = (Faction) lua_tointeger(L, 2);
	
	int slot = (Faction) lua_tointeger(L, 3);
	
	p = new Player(uc, gl->st);
	p->fac = fac;
	p->slot = slot;
	
	p->pickupWeapon(gl->mod->getWeaponType(0));
	p->pickupWeapon(gl->mod->getWeaponType(1));
	p->pickupWeapon(gl->mod->getWeaponType(2));
	
	Zone *z = gl->map->getSpawnZone(fac);
	if (z == NULL) {
		cerr << "Map does not have any spawnpoints\n";
		exit(1);
	}
	p->x = z->getRandomX();
	p->y = z->getRandomY();
	
	gl->st->addUnit(p);
	
	if (gl->st->curr_slot == slot) {
		gl->st->curr_player = p;
		if (gl->st->hud) gl->st->hud->hideSpawnMenu();
	}
	
	return 0;
}


/**
* Display an alert message
**/
LUA_FUNC(show_alert_message)
{
	const char* ctext = lua_tostring(L, 1);
	string text = (*(new string(ctext)));
	if (text.empty()) {
		lua_pushstring(L, "Arg #1 is not a string");
		lua_error(L);
	}
	
	if (gl->st->hud) gl->st->hud->addAlertMessage(text);
	
	return 0;
}


/**
* 
*
* @return String: The currently selected unit type
**/
LUA_FUNC(get_selected_unittype)
{
	lua_pushstring(L, gl->selected_unittype->name.c_str());
	return 1;
}


/**
* 
*
* @return String: The currently selected unit type
**/
LUA_FUNC(random)
{
	int min = lua_tointeger(L, 1);
	int max = lua_tointeger(L, 2);
	
	lua_pushinteger(L, getRandom(min,max));
	
	return 1;
}



/**
* For function binding
**/
#define LUA_REG(name) lua_register(L, #name, name)

/**
* Register aglgl of the Lua functions glisted above
**/
void register_lua_functions()
{
	LUA_REG(debug);
	LUA_REG(bind_gamestart);
	LUA_REG(bind_playerjoin);
	LUA_REG(bind_playerdied);
	LUA_REG(bind_npcdied);
	LUA_REG(add_interval);
	LUA_REG(add_timer);
	LUA_REG(remove_timer);
	LUA_REG(add_npc);
	LUA_REG(add_player);
	LUA_REG(show_alert_message);
	LUA_REG(get_selected_unittype);
	LUA_REG(random);
	
	
	// Factions constants table
	lua_createtable(L,0,0);
	lua_pushnumber(L, FACTION_TEAM1); lua_setfield(L, -2, "team1");
	lua_pushnumber(L, FACTION_TEAM2); lua_setfield(L, -2, "team2");
	lua_pushnumber(L, FACTION_TEAM3); lua_setfield(L, -2, "team3");
	lua_pushnumber(L, FACTION_TEAM4); lua_setfield(L, -2, "team4");
	lua_pushnumber(L, FACTION_TEAM5); lua_setfield(L, -2, "team5");
	lua_pushnumber(L, FACTION_TEAM6); lua_setfield(L, -2, "team6");
	lua_pushnumber(L, FACTION_TEAM7); lua_setfield(L, -2, "team7");
	lua_pushnumber(L, FACTION_TEAM8); lua_setfield(L, -2, "team8");
	lua_pushnumber(L, FACTION_INDIVIDUAL); lua_setfield(L, -2, "individual");
	lua_pushnumber(L, FACTION_COMMON); lua_setfield(L, -2, "common");
	lua_setglobal(L, "factions");
}


