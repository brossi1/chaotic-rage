// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "../audio/audio.h"

using namespace std;


class btCollisionShape;

class WallTypeDamage {
	public:
		int health;
		AssimpModel * model;
};

class WallType
{
	public:
		// from data file
		string name;
		CRC32 id;

		bool stretch;
		bool wall;
		WallType * ground_type;

		vector <WallTypeDamage *> damage_models;
		vector <AudioPtr> walk_sounds;

		AssimpModel * model;
		btCollisionShape* col_shape;

		int check_radius;
		float health;

		SpritePtr surf;

	public:
		WallType();
		~WallType();
};


// Config file opts
extern cfg_opt_t walltype_opts [];

// Item loading function handler
WallType* loadItemWallType(cfg_t* cfg_item, Mod* mod);

