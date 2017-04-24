#include "AmmoBox.h"

AmmoBox::AmmoBox(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position) : 
GameObject(mesh, shader, texture, position)
{
	m_tag = "ammo";
	updateBounds();
}

void AmmoBox::Update(float timeStep, float simTime)
{
	
	updateBounds();
}
