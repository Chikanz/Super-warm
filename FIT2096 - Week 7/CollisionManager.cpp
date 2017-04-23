#include "CollisionManager.h"
#include <iostream>

CollisionManager::CollisionManager(Player* player, GameObject* dummyPlayer, std::vector<GameObject*>* sceneObjects)
{
	m_player = player; //Player
	m_playerObj = dummyPlayer;

	m_sceneObjects = sceneObjects; //Scene objects (level geometry, ammo boxes, rubies)

	// Clear our arrays to 0 (NULL)
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	memset(m_previousCollisions, 0, sizeof(m_previousCollisions));

	m_nextCurrentCollisionSlot = 0;
}

void CollisionManager::CheckCollisions()
{	
	PlayerToScene(); //Player needs a special loop because it's not a game object
	SceneToScene();

	// Move all current collisions into previous
	memcpy(m_previousCollisions, m_currentCollisions, sizeof(m_currentCollisions));

	// Clear out current collisions
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	
	// Now current collisions is empty, we'll start adding from the start again
	m_nextCurrentCollisionSlot = 0;

}

bool CollisionManager::ArrayContainsCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second)
{
	// See if these two GameObjects appear one after the other in specified collisions array
	// Stop one before length so we don't overrun as we'll be checking two elements per iteration
	for (int i = 0; i < MAX_ALLOWED_COLLISIONS - 1; i += 2)
	{
		if ((arrayToSearch[i] == first && arrayToSearch[i + 1] == second) ||
			arrayToSearch[i] == second && arrayToSearch[i + 1] == first)
		{
			// Found them!
			return true;
		}
	}

	// These objects were not colliding last frame
	return false;
}

void CollisionManager::AddCollision(GameObject* first, GameObject* second)
{	
	// Add the two colliding objects to the current collisions array
	// We keep track of the next free slot so no searching is required
	m_currentCollisions[m_nextCurrentCollisionSlot] = first;
	m_currentCollisions[m_nextCurrentCollisionSlot + 1] = second;
	
	m_nextCurrentCollisionSlot += 2;
}

void CollisionManager::PlayerToScene()
{
	for (unsigned int i = 0; i < m_sceneObjects->size(); i++)
	{
		// Don't need to store pointer to these objects again but favouring clarity
		// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
		GameObject* sceneObj = (*m_sceneObjects)[i];		
		
		CBoundingBox PlayerBounds = m_player->GetBounds();
		CBoundingBox sceneObjBounds = sceneObj->GetBounds();
		

		// Are they colliding this frame?
		bool isColliding = CheckCollision(PlayerBounds, sceneObjBounds);

		// Were they colliding last frame?
		bool wasColliding = ArrayContainsCollision(m_previousCollisions, m_playerObj, sceneObj);

		if (isColliding)
		{
			// Register the collision
			AddCollision(m_playerObj, sceneObj);

			if (wasColliding)
			{
				// We are colliding this frame and we were also colliding last frame - that's a collision stay
				// Tell the item box a kart has collided with it (we could pass it the actual kart too if we like)
				sceneObj->OnCollisionStay(m_playerObj);
				m_player->OnCollisionStay(sceneObj);
			}
			else
			{
				// We are colliding this frame and we weren't last frame - that's a collision enter
				sceneObj->OnCollisionEnter(m_playerObj); 
				m_player->OnCollisionEnter(sceneObj);
			}
		}
		else
		{
			if (wasColliding)
			{
				// We aren't colliding this frame but we were last frame - that's a collision exit
				sceneObj->OnCollisionExit(m_playerObj); 
				m_player->OnCollisionExit(sceneObj);
			}
		}
	}
}


void CollisionManager::SceneToScene()
{
	// We'll check each kart against every other kart
	// Note this is not overly efficient, both in readability and runtime performance

	for (int i = 0; i < m_sceneObjects->size(); i++)
	{
		for (int j = 0; j < m_sceneObjects->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			GameObject* firstObj = (*m_sceneObjects)[i];
			GameObject* secondObj = (*m_sceneObjects)[j];

			// Don't compare a kart to itself
			// Comparing memory addresses - beauty of pointers
			if (firstObj != secondObj)
			{
				CBoundingBox firstBounds = firstObj->GetBounds();
				CBoundingBox secondBounds = secondObj->GetBounds();

				// Do we already know about a collision between these two karts
				bool alreadyHandled = ArrayContainsCollision(m_currentCollisions, firstObj, secondObj);

				if (!alreadyHandled)
				{
					// Are they colliding this frame?
					bool isColliding = CheckCollision(firstBounds, secondBounds);

					// Were they colliding last frame?
					bool wasColliding = ArrayContainsCollision(m_previousCollisions, firstObj, secondObj);

					if (isColliding)
					{
						// Register the collision
						AddCollision(firstObj, secondObj);

						if (wasColliding)
						{
							// We are colliding this frame and we were also colliding last frame - that's a collision stay
							// Tell both karts so they can respond. Also give each one the other kart.
							firstObj->OnCollisionStay(secondObj);
							secondObj->OnCollisionStay(firstObj);
						}
						else
						{
							// We are colliding this frame and we weren't last frame - that's a collision enter
							firstObj->OnCollisionEnter(secondObj);
							secondObj->OnCollisionEnter(firstObj);
						}
					}
					else
					{
						if (wasColliding)
						{
							// We aren't colliding this frame but we were last frame - that's a collision exit
							firstObj->OnCollisionExit(secondObj);
							secondObj->OnCollisionExit(firstObj);
						}
					}
				}
			}
		}
	}
}

