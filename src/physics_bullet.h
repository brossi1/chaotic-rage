// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <btBulletDynamicsCommon.h>
#include "rage.h"

using namespace std;



#define BIT(x) (1<<(x))
enum CollisionGroup {
	CG_NOTHING = 0,           // Nothing!
	CG_TERRAIN = BIT(0),      // The ground surface
	CG_DEBRIS = BIT(1),       // Debris, dead stuff, etc
	CG_STATIC = BIT(2),       // Walls and other statics
	CG_DYNAMIC = BIT(3),      // Objects and other dynamics
	CG_UNIT = BIT(4),         // Units (btKinematicCharacterController)
	CG_VEHICLE = BIT(5),      // Vehicles (btRaycastVehicle)
};


/**
* Simple wrapper around the "Bullet" physics library
**/
class PhysicsBullet
{
	private:
		GameState * st;
		
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*>* collisionShapes;
		
		btRigidBody* groundRigidBody;
		
		// Used by the add functions to set the right collision mask.
		map<int, int> masks;

	public:
		PhysicsBullet(GameState * st);
		~PhysicsBullet();
		virtual void init();
		virtual void preGame();
		virtual void postGame();
		
	public:
		btRigidBody* addRigidBody(btCollisionShape* colShape, float mass, float x, float y, float z, CollisionGroup group);
		btRigidBody* addRigidBody(btCollisionShape* colShape, float m, btDefaultMotionState* motionState, CollisionGroup group);
		void addRigidBody(btRigidBody* body, CollisionGroup group);
		void addCollisionObject(btCollisionObject* obj, CollisionGroup group);
		void addAction(btActionInterface* action);
		void addVehicle(btRaycastVehicle* vehicle);
		void markDead(btRigidBody* body);
		void remRigidBody(btRigidBody* body);
		void delRigidBody(btRigidBody* body);
		
		void stepTime(int ms);
		void doCollisions();
		
		btDiscreteDynamicsWorld* getWorld();
		
		static void QuaternionToEulerXYZ(const btQuaternion &quat, btVector3 &euler);
		static float QuaternionToYaw(const btQuaternion &quat);
		btVector3 spawnLocation(float x, float z, float height);
};




