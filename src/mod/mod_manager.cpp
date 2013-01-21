// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"

using namespace std;



/**
* Load the main mod and user mods.
* Run while the intro is being displayed, on a background thread.
**/
void loadMods(GameState *st)
{
	// Load main mod
	Mod * mod = new Mod(st, "data/cr");
	if (! mod->load()) {
		reportFatalError("Unable to load data module 'cr'.");
	}
	st->mm->addMod(mod);

	// TODO: Support for dynamic loading!
	// Load australia day mod
	mod = new Mod(st, "data/australia_day");
	if (! mod->load()) {
		reportFatalError("Unable to load data module 'australia_day'.");
	}
	st->mm->addMod(mod);

	// Load user mods
	vector<string> * userfiles = getUserModFilenames();
	for (unsigned int i = 0; i < userfiles->size(); i++) {
		mod = new Mod(st, userfiles->at(i));
		if (! mod->load()) {
			reportFatalError("Unable to load data module '" + userfiles->at(i) + "'.");
		}
		st->mm->addMod(mod);
	}
}


/**
* Inits the mod-manager.
**/
ModManager::ModManager(GameState * st)
{
	this->st = st;
	st->mm = this;

	this->mods = new vector<Mod*>();
}


/**
* Add a mod to the list
**/
void ModManager::addMod(Mod * mod)
{
	this->mods->push_back(mod);
}


/**
* Get a mod via name
**/
Mod * ModManager::getMod(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = this->mods->size() - 1; i >= 0; --i) {
		if (this->mods->at(i)->name.compare(name) == 0) return this->mods->at(i);
	}
	
	reportFatalError("Data module is not loaded: " + name);
	return NULL;
}

/**
* Get the 'default' mod
**/
Mod * ModManager::getDefaultMod()
{
	return this->mods->at(0);
}


/**
* Get a list of all loaded mods
* Don't forget to delete() the result
**/
vector<string> * ModManager::getLoadedMods()
{
	vector<string> * out = new vector<string>();
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		out->push_back(mod->name);
	}
	return out;
}


/**
* Get a list of all available mods
* Don't forget to delete() the result
**/
vector<string> * ModManager::getAvailMods()
{
	vector<string> * out = new vector<string>();
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		out->push_back(mod->name);
	}
	return out;
}


/**
* Reload the attrs for all mods.
* Development/Debugging feature only.
**/
bool ModManager::reloadAttrs()
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		if (! mod->reloadAttrs()) return false;
	}
	return true;
}


/**
* Gets an animmodel by name
**/
AIType * ModManager::getAIType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		AIType *et = mod->getAIType(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets an animmodel by name
**/
AnimModel * ModManager::getAnimModel(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		AnimModel *et = mod->getAnimModel(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets an floor type by name
**/
VehicleType * ModManager::getVehicleType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		VehicleType *et = mod->getVehicleType(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets an floor type by name
**/
FloorType * ModManager::getFloorType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		FloorType *et = mod->getFloorType(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets a gametype by name
**/
GameType * ModManager::getGameType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		GameType *et = mod->getGameType(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Returns a start and end iterator for getting all gametypes
**/
vector<GameType*> * ModManager::getAllGameTypes()
{
	vector<GameType*>::iterator start, end;
	vector<GameType*> * gt;
	
	gt = new vector<GameType*>();
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		mod->getAllGameTypes(&start, &end);
		gt->insert(gt->end(), start, end);
	}
	
	return gt;
}


/**
* Gets a object type by name
**/
ObjectType * ModManager::getObjectType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		ObjectType *et = mod->getObjectType(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets a unit type by name
**/
UnitType * ModManager::getUnitType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		UnitType *et = mod->getUnitType(name);
		if (et) return et;
	}
	
	return NULL;
}

/**
* Returns a start and end iterator for getting all unit types
**/
vector<UnitType*> * ModManager::getAllUnitTypes()
{
	vector<UnitType*>::iterator start, end;
	vector<UnitType*> * ut;
	
	ut = new vector<UnitType*>();
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		mod->getAllUnitTypes(&start, &end);
		ut->insert(ut->end(), start, end);
	}
	
	return ut;
}


/**
* Gets a song by name
**/
Song * ModManager::getSong(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		Song *et = mod->getSong(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets a random song
**/
Song * ModManager::getRandomSong()
{
	Mod *mod = this->mods->at(getRandom(0, mods->size() - 1));
	return mod->getRandomSong();
}


/**
* Gets a sound by name
**/
Sound * ModManager::getSound(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		Sound *et = mod->getSound(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets a wall type by name
**/
WallType * ModManager::getWallType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		WallType *et = mod->getWallType(name);
		if (et) return et;
	}
	
	return NULL;
}


/**
* Gets a weapon type by name
**/
WeaponType * ModManager::getWeaponType(string name)
{
	if (name.empty()) return NULL;
	
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		WeaponType *et = mod->getWeaponType(name);
		if (et) return et;
	}
	
	return NULL;
}



