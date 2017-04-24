#pragma once
#include "FlyingCamera.h"

class Player : public FlyingCamera
{
private:	
	int m_health = 1;
	int m_inClip = 9; //Bullets currently in gun
	int m_ammo = 27;	//Bullets on person (3 clips)
	int m_clipCap = m_inClip; //Default clip size
	float m_coolDown = 99.0f; 
	float m_fireTime = 0.2f;

	CBoundingBox m_bounds;
	void UpdateBounds();
	Mesh* m_colliderMesh;
	float m_height;

	int rubiesHeld = 0;
	int maxRubies = 0;

public:
	void Update(float timeStep);
	Player(InputController* input, Vector3 startPos, Mesh* colliderMesh);

	Bullet* SpawnBullet(Mesh* mesh, Shader* shader, Texture* texture);
	bool canFire();

	float getSimSpeed() { return m_simSpeed; }
	int getInClip() { return m_inClip; }
	int getAmmo() { return m_ammo; }
	void registerRuby() { maxRubies++; };

	//Collisions
	CBoundingBox GetBounds() { return m_bounds; };
	
	void OnCollisionStay(GameObject* other);
	void OnCollisionEnter(GameObject* other);
	void OnCollisionExit(GameObject* other);

	
};
