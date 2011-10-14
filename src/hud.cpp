// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



HUD::HUD(GameState *st)
{
	st->hud = this;
	this->st = st;
	
	this->weapon_menu = false;
	this->spawn_menu = false;
}


void HUD::showSpawnMenu()
{
	spawn_menu = true;
	this->st->reset_mouse = false;
	SDL_ShowCursor(SDL_ENABLE);
}

void HUD::hideSpawnMenu()
{
	spawn_menu = false;
	
	this->st->reset_mouse = true;
	SDL_ShowCursor(SDL_DISABLE);
}

void HUD::addAlertMessage(string text)
{
	AlertMessage *msg = new AlertMessage();
	msg->text = text;
	msg->remove_time = this->st->game_time + 5000;
	this->msgs.push_front(msg);
}

void HUD::addAlertMessage(string text1, string text2)
{
	AlertMessage *msg = new AlertMessage();
	msg->text = text1.append(text2);
	msg->remove_time = this->st->game_time + 5000;
	this->msgs.push_front(msg);
}


/**
* Add a data table to the HUD
**/
int HUD::addDataTable(int x, int y, int cols, int rows)
{
	DataTable * dt;
	
	dt = new DataTable();
	dt->x = x;
	dt->y = y;
	dt->cols = cols;
	dt->rows = rows;
	dt->visible = true;
	
	for (int i = 0; i < (cols * rows); i++) {
		dt->data.push_back("");
	}
	
	this->tables.push_back(dt);
	
	return this->tables.size() - 1;
}

/**
* Set a value of a data table
**/
void HUD::setDataValue(int table_id, int col, int row, string val)
{
	DataTable * dt;
	dt = this->tables.at(table_id);
	dt->data[(row * dt->cols) + col] = val;
}

/**
* Remove a data table
**/
void HUD::removeDataTable(int table_id)
{
	DataTable * dt;
	dt = this->tables.at(table_id);
	dt->visible = false;
}

/**
* Remove a data table
**/
void HUD::removeAllDataTables()
{
	this->tables.clear();
}


/**
* Used for filtering
**/
/*static bool MessageEraser(AlertMessage *e)
{
	return (e->remove_time > st->game_time);
}*/


/**
* Render the heads up display
**/
void HUD::render(RenderOpenGL * render)
{
	if (this->spawn_menu) {
		// TODO: menu for spawning
		for (int i = 0; i <= 1; i++) {
			UnitType *uc = st->getDefaultMod()->getUnitType(i);
			UnitTypeState *ucs = uc->getState(UNIT_STATE_STATIC);
			if (! uc->playable) continue;
			
			glPushMatrix();
			glTranslatef(100 + i * 100, 500, 0);
			glRotatef(90, 1, 0, 0);
			glRotatef(180, 0, 0, 1);
			
			AnimPlay * play = new AnimPlay(ucs->model);
			((RenderOpenGL*) render)->renderAnimPlay(play, 0);
			delete(play);
			
			glPopMatrix();
		}
		
		
	} else if (this->weapon_menu && this->st->local_players[0]) {
		// Weapon menu
		SDL_Rect r = {100, 100, 125, 125};
		unsigned int i, num = this->st->local_players[0]->getNumWeapons();
		
		for (i = 0; i < num; i++) {
			WeaponType *wt = this->st->local_players[0]->getWeaponTypeAt(i);
			
			render->renderText(wt->title, r.x, r.y);

			if (i == this->st->local_players[0]->getCurrentWeaponID()) {
				render->renderText(">", r.x - 25, r.y);
			}
			
			r.y += 30;
		}
		
		
	} else {
		// Alert messages
		int y = 1000;
		for (list<AlertMessage*>::iterator it = this->msgs.begin(); it != this->msgs.end(); it++) {
			AlertMessage *msg = (*it);
			
			if (msg->remove_time < st->game_time) {
				// TODO: Remove the message
				continue;
			}
			
			y -= 20;
			render->renderText(msg->text, 20, y);
		}
		
		// Data tables
		for (unsigned int i = 0; i < this->tables.size(); i++) {
			DataTable *dt = this->tables[i];
			if (! dt->visible) continue;
			
			for (int col = 0; col < dt->cols; col++) {
				for (int row = 0; row < dt->rows; row++) {
					render->renderText(dt->data[(row * dt->cols) + col], dt->x + (100 * col), dt->y + (20 * row));
				}
			}
		}
		
		if (st->local_players[0] != NULL) {
			int val;
			
			val = st->local_players[0]->getMagazine();
			if (val >= 0) {
				char buf[50];
				sprintf(buf, "%i", val);
				render->renderText(buf, 50, 50);
			} else if (val == -2) {
				render->renderText("relodn!", 50, 50);
			}
			
			val = st->local_players[0]->getBelt();
			if (val >= 0) {
				char buf[50];
				sprintf(buf, "%i", val);
				render->renderText(buf, 150, 50);
			}
			
			val = st->local_players[0]->getHealth();
			if (val >= 0) {
				char buf[50];
				sprintf(buf, "%i", val);
				render->renderText(buf, 50, 80);
			}
		}
	}
}


/**
* Handles events.
* Called before the main event processing occurs.
* Return EVENT_BUBBLE to bubble the event, EVENT_PREVENT to prevent additional processing
**/
int HUD::handleEvent(SDL_Event *event)
{
	if (event->type == SDL_KEYUP) {
		if (event->key.keysym.sym == SDLK_LSHIFT) {
			// SHIFT key
			st->hud->weapon_menu = ! st->hud->weapon_menu;
			return HUD::EVENT_PREVENT;

		} else if (event->key.keysym.sym == SDLK_UP) {
			this->st->local_players[0]->setWeapon(this->st->local_players[0]->getPrevWeaponID());
			this->st->local_players[1]->setWeapon(this->st->local_players[1]->getPrevWeaponID());
			return HUD::EVENT_PREVENT;
			
		}  else if (event->key.keysym.sym == SDLK_DOWN) {
			this->st->local_players[0]->setWeapon(this->st->local_players[0]->getNextWeaponID());
			this->st->local_players[1]->setWeapon(this->st->local_players[1]->getNextWeaponID());
			return HUD::EVENT_PREVENT;
			
		}

	} else if (event->type == SDL_MOUSEBUTTONDOWN) {
		// MOUSE BUTTON + SCROLL
		if (event->button.button == SDL_BUTTON_WHEELUP) {
			if (! this->weapon_menu) this->weapon_menu = true;
			if (this->st->local_players[0]) this->st->local_players[0]->setWeapon(this->st->local_players[0]->getPrevWeaponID());
			if (this->st->local_players[1]) this->st->local_players[1]->setWeapon(this->st->local_players[1]->getPrevWeaponID());
			return HUD::EVENT_PREVENT;
			
		} else if (event->button.button == SDL_BUTTON_WHEELDOWN) {
			if (! this->weapon_menu) this->weapon_menu = true;
			if (this->st->local_players[0]) this->st->local_players[0]->setWeapon(this->st->local_players[0]->getNextWeaponID());
			if (this->st->local_players[1]) this->st->local_players[1]->setWeapon(this->st->local_players[1]->getNextWeaponID());
			return HUD::EVENT_PREVENT;
			
		} else {
			this->weapon_menu = false;
			return HUD::EVENT_BUBBLE;
		}
	}
	
	return HUD::EVENT_BUBBLE;
}
