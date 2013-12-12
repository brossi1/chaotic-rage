// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <assimp/scene.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../rage.h"
#include "glvao.h"

using namespace std;


/**
* How many bones can be in an model
**/
#define MAX_BONES 32

/**
* How many bone ids/weights can be assigned to a vertex
**/
#define MAX_WEIGHTS 4


class AssimpAnimation;
class AssimpAnimKey;
class AssimpBone;
class AssimpMaterial;
class AssimpMesh;
class AssimpModel;
class AssimpNode;
class AssimpNodeAnim;
class Mod;
class Render3D;


class AssimpBone
{
	public:
		string name;
		glm::mat4 offset;
		AssimpNode* nd;
};

class AssimpFace
{
	public:
		unsigned int a;
		unsigned int b;
		unsigned int c;
};

class AssimpMesh
{
	public:
		GLVAO *vao;
		int numFaces;
		int materialIndex;
		vector<AssimpBone*> bones;
		AssimpNode* nd;
		
		// For the physics bits
		vector<AssimpFace>* faces;
		vector<glm::vec4>* verticies;
		
	public:
		AssimpMesh() : nd(NULL), faces(NULL), verticies(NULL) {}
};

class AssimpMaterial
{
	public:
		SpritePtr diffuse;
		SpritePtr normal;
		
	public:
		AssimpMaterial():
			diffuse(NULL), normal(NULL)
			{}
};

class AssimpNode
{
	public:
		AssimpNode* parent;
		vector<AssimpNode*> children;
		vector<unsigned int> meshes;
		glm::mat4 transform;
		string name;

	public:
		AssimpNode() : parent(NULL) {}
		
		void addChild(AssimpNode* child) {
			this->children.push_back(child);
			child->parent = this;
		}
};

class AssimpAnimKey
{
	public:
		glm::vec3 vec;		// vector for position/scale
		glm::quat quat;		// quaternion for rotation
		float time;
};

class AssimpNodeAnim
{
	public:
		string name;
		vector<AssimpAnimKey> position;
		vector<AssimpAnimKey> rotation;
		vector<AssimpAnimKey> scale;
};

class AssimpAnimation
{
	public:
		string name;
		float duration;
		float ticsPerSec;
		vector<AssimpNodeAnim*> anims;
};


class AssimpModel
{
	friend class RenderOpenGL;
	friend class AnimPlay;
	friend class Map;

	private:
		const struct aiScene* sc;			// TODO: remove this, pass it as a func. arg everywhere instead.
		Uint8* boneIds;
		float* boneWeights;
		btVector3 boundingSize;
		btCollisionShape *shape;
		
	protected:
		Mod *mod;
		string name;
		vector<AssimpMesh*> meshes;
		vector<AssimpMaterial*> materials;
		vector<AssimpAnimation*> animations;
		AssimpNode* rootNode;
		
	public:
		AssimpModel(Mod* mod, string name);
		bool load(Render3D* render, bool meshdata);
		~AssimpModel();
		
	public:
		const struct aiScene* getScene() { return this->sc; }		// TODO: remove this too
		btVector3 getBoundingSize();
		btVector3 getBoundingSizeHE();
		btCollisionShape* getCollisionShape();
		AssimpNode* findNode(string name);
		string getName() { return this->name; }

	private:
		void calcBoundingBox();
		void calcBoundingBoxNode(const aiNode* nd, aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo);

		void loadMeshes(bool opengl);
		void loadMaterials(Render3D* render);
		SpritePtr loadTexture(Render3D* render, aiString path);
		void loadMeshdata(bool update);
		
		void loadNodes();
		AssimpNode* loadNode(aiNode* nd, unsigned int depth);
		AssimpNode* findNode(AssimpNode* nd, string name);
		
		void loadAnimations();
		AssimpAnimation* loadAnimation(const aiAnimation* anim);

		glm::vec3 convVector(aiVector3D in);
		glm::quat convQuaternion(aiQuaternion in);

		void loadBones(const aiMesh* mesh, AssimpMesh* myMesh);
		Uint8* getBoneIds();
		float* getBoneWeights();
		void freeBones();
		void setBoneNodes();
		
		void createCollisionShape();
};

