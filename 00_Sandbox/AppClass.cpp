#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	srand(time(NULL));

	m_sProgrammer = "Team Shepherd";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 5.0f, 25.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)


	m_pEntityMngr = MyEntityManager::GetInstance(); //Initialize the entity manager
	//m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "wolves");
	//m_pEntityMngr->UsePhysicsSolver();

	for (int i = 0; i < 20; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "sheep_" + std::to_string(i));
		vector3 v3Position = vector3(rand() % 25 - 10 + 15, 0.0f, rand() % 25 - 10);
		matrix4 m4Position = glm::translate(v3Position);
		//m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.5f)));
		m_pEntityMngr->SetModelMatrix(m4Position);
		//m_pEntityMngr->UsePhysicsSolver(true);
		//m_pEntityMngr->SetMass(1.5);

		//m_pEntityMngr->SetMass(i+1);
	}

	for (int i = 0; i < 4; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "wolves_" + std::to_string(i));
		vector3 v3Position = vector3(rand() % 25 - 10 - 15, 0.0f, rand() % 25 - 10);
		matrix4 m4Position = glm::translate(v3Position);
		//m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.5f)));
		m_pEntityMngr->SetModelMatrix(m4Position);
		//m_pEntityMngr->UsePhysicsSolver(true);
		//m_pEntityMngr->SetMass(1.5);

		//m_pEntityMngr->SetMass(i+1);
	}

	//Christian - 4/7
	//Discworld
	m_discworld = new Mesh();
	m_discworld->GenerateCylinder(100.0f, 1.0f, 15, vector3(0.5f, 0.9f, 0.5f));

	//Mike - 4/7
	//Adding player entity to world
	m_pEntityMngr->AddEntity("Minecraft\\Creeper.obj", "Player");

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
	
	//Mike - 4/7
	//Set model matrix to the player
	matrix4 mPlayer = glm::translate(m_v3PlayerPos) * IDENTITY_M4;
	m_pEntityMngr->SetModelMatrix(mPlayer, "Player");

	//Update Entity Manager
	m_pEntityMngr->Update();

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

	//Christian - 4/7
	//Render disc
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