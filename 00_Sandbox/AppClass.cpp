#include "AppClass.h"
using namespace Simplex;

void Application::InitVariables(void)
{
	srand(time(NULL));

	m_sProgrammer = "Team Shepherd";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
        vector3(0.0f, 28.5f, 40.0f), // Position
        vector3(0.0f, 0.0f, 15.8f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityMngr = MyEntityManager::GetInstance(); //Initialize the entity manager
	//m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "wolves");
	//m_pEntityMngr->UsePhysicsSolver();

	for (int i = 0; i < 20; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "sheep_" + std::to_string(i), 0);
		vector3 v3Position = vector3(rand() % 25 - 10 + 15, 0.0f, rand() % 25 - 10);
		
		//m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.5f)));
		m_pEntityMngr->UsePhysicsSolver(true, "sheep_" + std::to_string(i));
		m_pEntityMngr->SetMass(1.5, "sheep_" + std::to_string(i));
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
	}

	for (int i = 0; i < 4; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "wolves_" + std::to_string(i), 1);
		vector3 v3Position = vector3(rand() % 25 - 10 - 15, 0.0f, rand() % 25 - 10);
		
		//m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.5f)));
		
		m_pEntityMngr->UsePhysicsSolver(true, "wolves_" + std::to_string(i));
		m_pEntityMngr->SetMass(1.5, "wolves_" + std::to_string(i));
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
	}

	//Christian - 4/7
	//Discworld
	m_discworld = new Mesh();
	m_discworld->GenerateCylinder(35.0f, 1.0f, 15, vector3(0.5f, 0.9f, 0.5f));

	//Mike - 4/7
	//Adding player entity to world

	m_pEntityMngr->AddEntity("Minecraft\\Creeper.obj", "Player", -1);
	m_pEntityMngr->UsePhysicsSolver(true, "Player");
	m_pEntityMngr->SetMass(1.5, "Player");

	//Mike 4/17 - Generating Fence Perimeter
	for (int i = 0; i <= 35; i++)
	{
		float angle = 2 * PI * i / 35;
		float x = 33 * sin(angle);
		float y = 33 * cos(angle);

		m_pEntityMngr->AddEntity("Minecraft\\fence_long.obj", "fence_" + std::to_string(i), 2);
		vector3 v3fencePosition = vector3(x, 0.f, y);
		matrix4 m4Rotation = glm::rotate(IDENTITY_M4, angle, glm::vec3(0, 1.0f, 0));
		matrix4 m4fencePosition = glm::translate(v3fencePosition) * m4Rotation;
		m_pEntityMngr->SetModelMatrix(m4fencePosition);
		m_pEntityMngr->GetRigidBody("fence_" + std::to_string(i))->SetModelMatrix(m4fencePosition);
	}

	//Mike - 4/16 Generating Pen
	GeneratePen(m_pEntityMngr);

	
    m_iClock = m_pSystem->GenClock();
    m_pSystem->StartClock(m_iClock);
}

void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();
	m_fDeltaTime = m_pSystem->GetDeltaTime(m_iClock);

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	//Christian - 4/17
	//CameraRotation(); //Disabled for game

	//Mike - 4/7
	//Set model matrix to the player, rotate based of direction from last WASD input (4/14)
	matrix4 m4Rotation = glm::rotate(IDENTITY_M4, glm::radians((float)fDirection), glm::vec3(0,1.0f,0));
	matrix4 mPlayer = glm::translate((m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Player")))->GetPosition()) * m4Rotation;	
	m_pEntityMngr->SetModelMatrix(mPlayer, "Player");

	//Update Entity Manager
	m_pEntityMngr->Update();
	//m_pEntityMngr->WolfPhysics();
	//m_pEntityMngr->ApplyForce(vector3(0.01f,0.0f,0.0f), "wolves_" + std::to_string(1));

	//Add objects to render list
    m_pEntityMngr->AddEntityToRenderList(-1, true);
}

void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
		
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//Christian - 4/7, 4/17 ; Rowan - 4/23, 4/24
	//Render disc
	//m_pCameraMngr->SetPositionTargetAndUpward(vector3(0.0f, 28.5f, 40.0f), vector3(0.0f, 0.0f, 15.8f), vector3(0.0f, 1.0f, 0.0f));
	m_discworld->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(0.0f, -1.0f, 0.0f)));

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}

void Application::GeneratePen(MyEntityManager* m_pInstance)
{
	m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_1", 2);
	vector3 v3fencePosition = vector3(-21.0f, 0, -4.0f);
	matrix4 m4fencePosition = glm::translate(v3fencePosition);
	m_pInstance->SetModelMatrix(m4fencePosition);

	m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_2", 2);
	v3fencePosition = vector3(-14.0f, 0, -4.0f);
	m4fencePosition = glm::translate(v3fencePosition);
	m_pInstance->SetModelMatrix(m4fencePosition);

	m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_3", 2);
	v3fencePosition = vector3(-21.0f, 0, 10.0f);
	m4fencePosition = glm::translate(v3fencePosition);
	m_pInstance->SetModelMatrix(m4fencePosition);

	m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_4", 2);
	v3fencePosition = vector3(-14.0f, 0, 10.0f);
	m4fencePosition = glm::translate(v3fencePosition);
	m_pInstance->SetModelMatrix(m4fencePosition);

	m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_5", 2);
	v3fencePosition = vector3(-22.0f, 0, 2.f);
	m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
	m_pInstance->SetModelMatrix(m4fencePosition);

	m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_6", 2);
	v3fencePosition = vector3(-22.0f, 0, 9.f);
	m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
	m_pInstance->SetModelMatrix(m4fencePosition);

	m_pInstance->AddEntity("Minecraft\\fence.obj", "fence_pen_7", 2);
	v3fencePosition = vector3(-8.0f, 0, 9.5f);
	m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
	m_pInstance->SetModelMatrix(m4fencePosition);

	m_pInstance->AddEntity("Minecraft\\fence.obj", "fence_pen_8", 2);
	v3fencePosition = vector3(-8.0f, 0, -3.f);
	m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
	m_pInstance->SetModelMatrix(m4fencePosition);
}