#include "player.h"
#include <iostream>
#include "MathsHelper.h"
#include "AmmoBox.h"
#include <algorithm>

//Oh yes
Player::Player(InputController* input, Vector3 startPos, Mesh* enemyMesh)
	: FlyingCamera(input, startPos),
	GameObject(enemyMesh, nullptr, nullptr, startPos)
{
	m_height = startPos.y;
	m_colliderMesh = enemyMesh;	

	UpdateBounds();
}

Bullet* Player::SpawnBullet(Mesh* mesh, Shader* shader, Texture* texture)
{
	Bullet* b = new Bullet(m_tag, mesh, shader, texture, m_lookAtTarget);
	//b->SetXRotation(m_pitch);
	b->SetYRotation(m_heading);
	return b;
}

bool Player::canFire()
{
	if (m_coolDown >= m_fireTime && m_inClip > 0 && m_gun)
	{
		m_coolDown = 0;
		m_inClip -= 1;
		return true;
	}

	return false;
}

void Player::OnCollisionStay(GameObject* other)
{

}

void Player::OnCollisionEnter(GameObject* other)
{
	if (other->GetTag() == "Level")
	{
		ApplyForce(-localForwardXZ * 0.5f); //Make shift physics
	}	

	if (other->GetTag() == "Bullet")
	{
		hurtTimer = 0; //Trigger hurt animation
		m_health -= 1;

		if(m_health <= 0)
		{
			ShowCursor(true);
			MessageBox(0, "Game over!", "uh oh!", 0);
			PostQuitMessage(0);
		}
	}
}

void Player::OnCollisionExit(GameObject* other)
{

}

void Player::GrabGun(Gun* g)
{
	m_gun = g;
	g->SetOwner(this);
	m_inClip = m_clipCap;
}

float Player::GetHurtAlpha()
{
	if(hurtTimer < hurtDuration)
	{
		return  MathsHelper::RemapRange(hurtTimer, 0.0f, hurtDuration, 1.0f, 0.0f);		
	}
	return 0;
}

void Player::UpdateBounds()
{
	Vector3 meshMin = m_colliderMesh->GetMin();
	meshMin.y -= m_height;
	m_bounds = CBoundingBox(m_position + meshMin, m_position + m_colliderMesh->GetMax());
	outerBounds = CBoundingBox(m_position + meshMin - (meshMin * 0.5f), m_position + m_colliderMesh->GetMax() + (m_colliderMesh->GetMax() * 0.5f));
}


void Player::Update(float timestep)
{
	//Shaky gun!
	FlyingCamera::Update(timestep);
}

void Player::Update(float timestep, float simSpeed) //Call update from flying cam
{
	//Hurt animation
	if (hurtTimer < hurtDuration)
	{
		hurtTimer += timestep * getSimSpeed();
	}
	m_position.y = 1.5f;

	//Update bounds
	UpdateBounds();

	//Shooty cooldown
	m_coolDown += timestep * getSimSpeed(); //Update cooldown in relation to simTime

	m_position = m_camPosition;
	m_rotY = m_heading;
	m_rotX = m_pitch;
	m_world = Matrix::CreateScale(m_scaleX, m_scaleY, m_scaleZ) * lookAtRotation * Matrix::CreateTranslation(m_position);


	if(m_input->GetMouseUp(1))
	{
		if(m_gun)
		{
			m_gun->RemoveOwner();
			m_gun = nullptr;
		}
	}
	
}

void Player::Render(Direct3D* renderer, Camera* cam) //Nothing to render!
{

}
