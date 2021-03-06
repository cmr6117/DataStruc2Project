#include "AppClass.h"

using namespace Simplex;

Grid* TheGrid = nullptr;
uint numSheep = 0;

// Rowan - 4/24
enum class EntityTypes { Sheep, Wolf, Fence, Player };

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

    float DiscRadius = 35.f;
    TheGrid = new Grid(DiscRadius);

    m_pEntityMngr = MyEntityManager::GetInstance(); //Initialize the entity manager

	//Spawn in 20 sheep to start
    for (int i = 0; i < 20; i++)
    {
		SpawnRandomSheep();
    }

	//Spawn in 4 wolves
    for (int i = 0; i < 4; i++)
    {
        m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "wolves_" + std::to_string(i), (uint)EntityTypes::Wolf);
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
    m_discworld->GenerateCylinder(DiscRadius, 1.0f, 15, vector3(0.5f, 0.9f, 0.5f));

    //Mike - 4/7
    //Adding player entity to world
    m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Player", (uint)EntityTypes::Player);
    m_pEntityMngr->UsePhysicsSolver(true, "Player");
    m_pEntityMngr->SetMass(1.0, "Player");

    //Mike 4/17 - Generating Fence Perimeter
    for (int i = 0; i < 35; i++)
    {
        float angle = 2 * PI * i / 35;
        float x = 33 * sin(angle);
        float y = 33 * cos(angle);

        m_pEntityMngr->AddEntity("Minecraft\\fence_long.obj", "fence_" + std::to_string(i), (uint)EntityTypes::Fence);
        vector3 v3fencePosition = vector3(x, 0.f, y);
        matrix4 m4Rotation = glm::rotate(IDENTITY_M4, angle, glm::vec3(0, 1.0f, 0));
		//m_pEntityMngr->SetPosition(v3fencePosition, "fence" + std::to_string(i));
        matrix4 m4fencePosition = glm::translate(v3fencePosition) * m4Rotation;
        m_pEntityMngr->SetModelMatrix(m4fencePosition);
        //m_pEntityMngr->GetRigidBody("fence_" + std::to_string(i))->SetModelMatrix(m4fencePosition);
    }

    //Mike - 4/16 Generating Pen
    GeneratePen(m_pEntityMngr);


    m_iClock = m_pSystem->GenClock();
    m_pSystem->StartClock(m_iClock);

	//Pass in grid to all starting entities
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		m_pEntityMngr->GetEntity(i)->SetGrid(TheGrid);
		m_pEntityMngr->GetEntity(i)->AssignToCell(); //PAssigning a cell initially ensures all fences get assigned, but they won't be called each update
	}
}

void Application::Update(void)
{
    //Update the system so it knows how much time has passed since the last call
    m_pSystem->Update();
    m_fDeltaTime = m_pSystem->GetDeltaTime(m_iClock);

    //Is the arcball active?
    ArcBall();

    // Rowan - 4/24
    // TODO: Toggle between entity manager collision logic, and grid logic based on whether or not the grid is active
    // TODO: Somehow find a way to include things properly so AppClassControls can handle this, and AppClass.h can have member declarations
    // TODO: Show in GUI
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
    {
        if (!m_bToggledGrid) {
            TheGrid->Active = !TheGrid->Active;

            if (TheGrid->Active)
                std::cout << "\nGrid is active";
            else
                std::cout << "\nGrid is inactive";

            m_bToggledGrid = true;
        }
    }
    else
    {
        m_bToggledGrid = false;
    }

    //Mike - 4/7
    //Set model matrix to the player, rotate based of direction from last WASD input (4/14)
    matrix4 m4Rotation = glm::rotate(IDENTITY_M4, glm::radians((float)fDirection), glm::vec3(0, 1.0f, 0));
    matrix4 mPlayer = glm::translate((m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Player")))->GetPosition());
    m_pEntityMngr->SetModelMatrix(mPlayer, "Player");

    // Rowan - 4/24
    // clear collisions
    m_pEntityMngr->ClearCollisions(); 

    // use grid collisions if applicable before moving entities
    if (TheGrid->Active)
        TheGrid->Update();

    // update entity movement stuff
    // if the grid is false, EntityManager will use standard unoptimized collisions before moving entities
    m_pEntityMngr->Update(TheGrid->Active);

    //Add objects to render list
    m_pEntityMngr->AddEntityToRenderList(-1, true);

	m_iSheepSaved = m_pEntityMngr->GetSheepRescued();
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

//Generates fence in fixed position
void Application::GeneratePen(MyEntityManager* m_pInstance)
{
    m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_1", (uint)EntityTypes::Fence);
    vector3 v3fencePosition = vector3(-21.0f, 0, -4.0f);
	m_pEntityMngr->SetPosition(v3fencePosition, "fence_pen_1");
    matrix4 m4fencePosition = glm::translate(v3fencePosition);
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_1")->SetModelMatrix(m4fencePosition);

    m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_2", (uint)EntityTypes::Fence);
    v3fencePosition = vector3(-14.0f, 0, -4.0f);
    m4fencePosition = glm::translate(v3fencePosition);
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_2")->SetModelMatrix(m4fencePosition);

    m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_3", (uint)EntityTypes::Fence);
    v3fencePosition = vector3(-21.0f, 0, 10.0f);
	m_pEntityMngr->SetPosition(v3fencePosition, "fence_pen_3");
    m4fencePosition = glm::translate(v3fencePosition);
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_3")->SetModelMatrix(m4fencePosition);

    m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_4", (uint)EntityTypes::Fence);
    v3fencePosition = vector3(-14.0f, 0, 10.0f);
    m4fencePosition = glm::translate(v3fencePosition);
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_4")->SetModelMatrix(m4fencePosition);

    m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_5", (uint)EntityTypes::Fence);
    v3fencePosition = vector3(-22.0f, 0, 2.f);
	m_pEntityMngr->SetPosition(v3fencePosition, "fence_pen_5");
    m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_5")->SetModelMatrix(m4fencePosition);

    m_pInstance->AddEntity("Minecraft\\fence_long.obj", "fence_pen_6", (uint)EntityTypes::Fence);
    v3fencePosition = vector3(-22.0f, 0, 9.f);
    m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_6")->SetModelMatrix(m4fencePosition);

    m_pInstance->AddEntity("Minecraft\\fence.obj", "fence_pen_7", (uint)EntityTypes::Fence);
    v3fencePosition = vector3(-8.0f, 0, 9.5f);
	m_pEntityMngr->SetPosition(v3fencePosition, "fence_pen_7");
    m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_7")->SetModelMatrix(m4fencePosition);

    m_pInstance->AddEntity("Minecraft\\fence.obj", "fence_pen_8", (uint)EntityTypes::Fence);
    v3fencePosition = vector3(-8.0f, 0, -3.f);
    m4fencePosition = glm::translate(v3fencePosition) * glm::rotate(IDENTITY_M4, glm::radians(90.f), glm::vec3(0, 1.0f, 0));
    m_pInstance->SetModelMatrix(m4fencePosition);
	m_pEntityMngr->GetRigidBody("fence_pen_8")->SetModelMatrix(m4fencePosition);
}

//Spawns a sheep in a random position
void Simplex::Application::SpawnRandomSheep(void)
{
	//Add entity, generate random pos
	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "sheep_" + std::to_string(numSheep), (uint)EntityTypes::Sheep);
	vector3 v3Position = vector3(rand() % 25 - 10 + 15, 0.0f, rand() % 25 - 10);

	m_pEntityMngr->UsePhysicsSolver(true, "sheep_" + std::to_string(numSheep));
	m_pEntityMngr->SetMass(1.5, "sheep_" + std::to_string(numSheep));
	matrix4 m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	//Pass in grid to the entity, it will always be the most recent entity added
	m_pEntityMngr->GetEntity((m_pEntityMngr->GetEntityCount()-1))->SetGrid(TheGrid);

	//increment num of sheep so we can name them accordingly
	numSheep++;
	m_pEntityMngr->SetSheepNum(numSheep);
}
