#include "MyEntity.h"
#include "Grid.h"
using namespace Simplex;
std::map<String, MyEntity*> MyEntity::m_IDMap;

//  Accessors
Simplex::MySolver* Simplex::MyEntity::GetSolver(void) { return m_pSolver; }
bool Simplex::MyEntity::HasThisRigidBody(MyRigidBody* a_pRigidBody) { return m_pRigidBody == a_pRigidBody; }
Simplex::MyRigidBody::PRigidBody* Simplex::MyEntity::GetColliderArray(void) { return m_pRigidBody->GetColliderArray(); }
uint Simplex::MyEntity::GetCollidingCount(void) { return m_pRigidBody->GetCollidingCount(); }
matrix4 Simplex::MyEntity::GetModelMatrix(void) { return m_m4ToWorld; }

void Simplex::MyEntity::SetModelMatrix(matrix4 a_m4ToWorld)
{
    if (!m_bInMemory || m_m4ToWorld == a_m4ToWorld)
        return;

    m_m4ToWorld = a_m4ToWorld;
    m_pModel->SetModelMatrix(m_m4ToWorld);
    m_pRigidBody->SetModelMatrix(m_m4ToWorld);

    if (m_bUsePhysicsSolver)
    {
        //experimental way of calculating a matrix components
        glm::vec3 scale;
        glm::quat orientation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(m_m4ToWorld, scale, orientation, translation, skew, perspective);
		
        //TODO: this is a hot fix the orientation inverses itself without this using the decompose
        matrix4 temp = glm::translate(translation) * ToMatrix4(orientation) * glm::scale(scale);
        glm::decompose(temp, scale, orientation, translation, skew, perspective);
        //....

        m_pSolver->SetPosition(translation);
        m_pSolver->SetOrientation(orientation);
        m_pSolver->SetSize(scale);
    }
}

String Simplex::MyEntity::GetDirection()
{
	return direction;
}

void Simplex::MyEntity::SetDirection(String newDir) { direction = newDir; }

Model* Simplex::MyEntity::GetModel(void) { return m_pModel; }
MyRigidBody* Simplex::MyEntity::GetRigidBody(void) { return m_pRigidBody; }
bool Simplex::MyEntity::IsInitialized(void) { return m_bInMemory; }
String Simplex::MyEntity::GetUniqueID(void) { return m_sUniqueID; }
void Simplex::MyEntity::SetAxisVisible(bool a_bSetAxis) { m_bSetAxis = a_bSetAxis; }
void Simplex::MyEntity::SetPosition(vector3 a_v3Position) { if (m_pSolver) m_pSolver->SetPosition(a_v3Position); }

Simplex::vector3 Simplex::MyEntity::GetPosition(void)
{
    if (m_pSolver != nullptr)
        return m_pSolver->GetPosition();
    return vector3();
}

void Simplex::MyEntity::SetVelocity(vector3 a_v3Velocity) { if (m_pSolver) m_pSolver->SetVelocity(a_v3Velocity); }
Simplex::vector3 Simplex::MyEntity::GetVelocity(void)
{
    if (m_pSolver != nullptr)
        return m_pSolver->GetVelocity();
    return vector3();
}

void Simplex::MyEntity::SetMass(float a_fMass) { if (m_pSolver) m_pSolver->SetMass(a_fMass); }
float Simplex::MyEntity::GetMass(void)
{
    if (m_pSolver != nullptr)
        return m_pSolver->GetMass();
    return 1.0f;
}
//  MyEntity
void Simplex::MyEntity::Init(void)
{
    m_pMeshMngr = MeshManager::GetInstance();
    m_bInMemory = false;
    m_bSetAxis = false;
    m_pModel = nullptr;
    m_pRigidBody = nullptr;
    m_DimensionArray = nullptr;
    m_m4ToWorld = IDENTITY_M4;
    m_sUniqueID = "";
    m_nDimensionCount = 0;
    m_bUsePhysicsSolver = false;
    m_pSolver = nullptr;
}
void Simplex::MyEntity::Swap(MyEntity& other)
{
    m_bInMemory = false;
    std::swap(m_pModel, other.m_pModel);
    std::swap(m_pRigidBody, other.m_pRigidBody);
    std::swap(m_m4ToWorld, other.m_m4ToWorld);
    std::swap(m_pMeshMngr, other.m_pMeshMngr);
    std::swap(m_bInMemory, other.m_bInMemory);
    std::swap(m_sUniqueID, other.m_sUniqueID);
    std::swap(m_bSetAxis, other.m_bSetAxis);
    std::swap(m_nDimensionCount, other.m_nDimensionCount);
    std::swap(m_DimensionArray, other.m_DimensionArray);
    std::swap(m_pSolver, other.m_pSolver);
}
void Simplex::MyEntity::Release(void)
{
    m_pMeshMngr = nullptr;
    //it is not the job of the entity to release the model, 
    //it is for the mesh manager to do so.
    m_pModel = nullptr;
    if (m_DimensionArray)
    {
        delete[] m_DimensionArray;
        m_DimensionArray = nullptr;
    }
    SafeDelete(m_pRigidBody);
    SafeDelete(m_pSolver);
    m_IDMap.erase(m_sUniqueID);
}
//The big 3
Simplex::MyEntity::MyEntity(String a_sFileName, String a_sUniqueID, uint a_nEntityType)
{
    Init();
    m_pModel = new Model();
    m_pModel->Load(a_sFileName);
    //if the model is loaded
    if (m_pModel->GetName() != "")
    {
        GenUniqueID(a_sUniqueID);
        m_sUniqueID = a_sUniqueID;
        m_IDMap[a_sUniqueID] = this;
        m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList()); //generate a rigid body
        m_bInMemory = true; //mark this entity as viable
    }
    m_pSolver = new MySolver();
	m_nEntityType = a_nEntityType;
}
Simplex::MyEntity::MyEntity(MyEntity const& other)
{
    m_bInMemory = other.m_bInMemory;
    m_pModel = other.m_pModel;
    //generate a new rigid body we do not share the same rigid body as we do the model
    m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList());
    m_m4ToWorld = other.m_m4ToWorld;
    m_pMeshMngr = other.m_pMeshMngr;
    m_sUniqueID = other.m_sUniqueID;
    m_bSetAxis = other.m_bSetAxis;
    m_nDimensionCount = other.m_nDimensionCount;
    m_DimensionArray = other.m_DimensionArray;
    m_pSolver = new MySolver(*other.m_pSolver);
}
MyEntity& Simplex::MyEntity::operator=(MyEntity const& other)
{
    if (this != &other)
    {
        Release();
        Init();
        MyEntity temp(other);
        Swap(temp);
    }
    return *this;
}
MyEntity::~MyEntity() { Release(); }
//--- Methods
void Simplex::MyEntity::AddToRenderList(bool a_bDrawRigidBody)
{
    //if not in memory return
    if (!m_bInMemory)
        return;

    //draw model
    m_pModel->AddToRenderList();

    //draw rigid body
    if (a_bDrawRigidBody)
        m_pRigidBody->AddToRenderList();

    if (m_bSetAxis)
        m_pMeshMngr->AddAxisToRenderList(m_m4ToWorld);
}
MyEntity* Simplex::MyEntity::GetEntity(String a_sUniqueID)
{
    //look the entity based on the unique id
    auto entity = m_IDMap.find(a_sUniqueID);
    //if not found return nullptr, if found return it
    return entity == m_IDMap.end() ? nullptr : entity->second;
}
void Simplex::MyEntity::GenUniqueID(String& a_sUniqueID)
{
    static uint index = 0;
    String sName = a_sUniqueID;
    MyEntity* pEntity = GetEntity(a_sUniqueID);
    //while MyEntity exists keep changing name
    while (pEntity)
    {
        a_sUniqueID = sName + "_" + std::to_string(index);
        index++;
        pEntity = GetEntity(a_sUniqueID);
    }
    return;
}
void Simplex::MyEntity::AddDimension(uint a_uDimension)
{
    //we need to check that this dimension is not already allocated in the list
    if (IsInDimension(a_uDimension))
        return;//it is, so there is no need to add

    //insert the entry
    uint* pTemp;
    pTemp = new uint[m_nDimensionCount + 1];
    if (m_DimensionArray)
    {
        memcpy(pTemp, m_DimensionArray, sizeof(uint) * m_nDimensionCount);
        delete[] m_DimensionArray;
        m_DimensionArray = nullptr;
    }
    pTemp[m_nDimensionCount] = a_uDimension;
    m_DimensionArray = pTemp;

    ++m_nDimensionCount;

    SortDimensions();
}
void Simplex::MyEntity::RemoveDimension(uint a_uDimension)
{
    //if there are no dimensions return
    if (m_nDimensionCount == 0)
        return;

    //we look one by one if its the one wanted
    for (uint i = 0; i < m_nDimensionCount; i++)
    {
        if (m_DimensionArray[i] == a_uDimension)
        {
            //if it is, then we swap it with the last one and then we pop
            std::swap(m_DimensionArray[i], m_DimensionArray[m_nDimensionCount - 1]);
            uint* pTemp;
            pTemp = new uint[m_nDimensionCount - 1];
            if (m_DimensionArray)
            {
                memcpy(pTemp, m_DimensionArray, sizeof(uint) * (m_nDimensionCount - 1));
                delete[] m_DimensionArray;
                m_DimensionArray = nullptr;
            }
            m_DimensionArray = pTemp;

            --m_nDimensionCount;
            SortDimensions();
            return;
        }
    }
}
void Simplex::MyEntity::ClearDimensionSet(void)
{
    if (m_DimensionArray)
    {
        delete[] m_DimensionArray;
        m_DimensionArray = nullptr;
    }
    m_nDimensionCount = 0;
}
bool Simplex::MyEntity::IsInDimension(uint a_uDimension)
{
    //see if the entry is in the set
    for (uint i = 0; i < m_nDimensionCount; i++)
    {
        if (m_DimensionArray[i] == a_uDimension)
            return true;
    }
    return false;
}
bool Simplex::MyEntity::SharesDimension(MyEntity* const a_pOther)
{

    //special case: if there are no dimensions on either MyEntity
    //then they live in the special global dimension
    if (0 == m_nDimensionCount)
    {
        //if no spatial optimization all cases should fall here as every 
        //entity is by default, under the special global dimension only
        if (0 == a_pOther->m_nDimensionCount)
            return true;
    }

    //for each dimension on both Entities we check if there is a common dimension
    for (uint i = 0; i < m_nDimensionCount; ++i)
    {
        for (uint j = 0; j < a_pOther->m_nDimensionCount; j++)
        {
            if (m_DimensionArray[i] == a_pOther->m_DimensionArray[j])
                return true; //as soon as we find one we know they share dimensionality
        }
    }

    //could not find a common dimension
    return false;
}

bool Simplex::MyEntity::IsColliding(MyEntity* const other)
{
    //if not in memory return
    if (!m_bInMemory || !other->m_bInMemory)
        return true;

    //if the entities are not living in the same dimension
    //they are not colliding
    if (!SharesDimension(other))
        return false;

    return m_pRigidBody->IsColliding(other->GetRigidBody());
}

// RW 4/13
bool Simplex::MyEntity::IsInRadius(MyEntity* const other)
{
    vector3 otherPos = other->GetRigidBody()->GetCenterGlobal();
    vector3 ourPos = GetRigidBody()->GetCenterGlobal();

    // get the distance between other and us
    float magnitude = glm::distance(otherPos, ourPos);

    if (magnitude < m_fRadiusRepel)
    {
        // other is inside our repel radius, TODO: we move away from them based on depth
        m_fRadiusRepelDepth = m_fRadiusRepel - magnitude;
        return true;
    }

    return false;
}

void Simplex::MyEntity::ClearCollisionList(void)
{
    m_pRigidBody->ClearCollidingList();
    m_pSolver->SetIsColliding(false);
}
void Simplex::MyEntity::SortDimensions(void)
{
    std::sort(m_DimensionArray, m_DimensionArray + m_nDimensionCount);
}
void Simplex::MyEntity::ApplyForce(vector3 a_v3Force)
{
    m_pSolver->ApplyForce(a_v3Force);
}
void Simplex::MyEntity::Update(float getMax)
{
    if (m_bUsePhysicsSolver)
    {
        if (m_pRigidBody->GetCollidingCount() > 0)
            m_pSolver->SetIsColliding(true);

        m_pSolver->Update(getMax);

        //SetModelMatrix(glm::translate(m_pSolver->GetPosition()) * glm::scale(m_pSolver->GetSize()));
        quaternion temp = m_pSolver->GetOrientation();
        SetModelMatrix(glm::translate(m_pSolver->GetPosition()) * ToMatrix4(temp) *  glm::scale(m_pSolver->GetSize()));
    }

	if (activeGrid->Active)
	{
		AssignToCell();
	}

	quaternion temp = m_pSolver->GetOrientation();
	GetRigidBody()->SetModelMatrix(glm::translate(m_pSolver->GetPosition()) * ToMatrix4(temp) *  glm::scale(m_pSolver->GetSize()));
}
void Simplex::MyEntity::ResolveCollision(MyEntity* a_pOther)
{
    if (m_bUsePhysicsSolver)
    {
        m_pSolver->ResolveCollision(a_pOther->GetSolver());
    }

	//If colliding with fence, must ensure entity cannot pass it, 
	//but don't trigger if fences collide with other fences
	if (a_pOther->m_sUniqueID.find("fence") != std::string::npos && !(m_sUniqueID.find("fence") != std::string::npos))
	{
		float dirToCenter = glm::distance(GetPosition(), ZERO_V3) * 10;

		//Fence Pen Collision
		if (a_pOther->m_sUniqueID.find("pen") != std::string::npos && !(m_sUniqueID.find("pen") != std::string::npos))
		{
			vector3 fenceForce = ZERO_V3;

			if (GetDirection().find("North") != std::string::npos)
			{
				fenceForce.z += dirToCenter;
			}
			if (GetDirection().find("South") != std::string::npos)
			{
				fenceForce.z -= dirToCenter;
			}
			if (GetDirection().find("East") != std::string::npos)
			{
				fenceForce.x -= dirToCenter;
			}
			if (GetDirection().find("West") != std::string::npos)
			{
				fenceForce.x += dirToCenter;
			}

			SetDirection(""); // clear direction
			ApplyForce(fenceForce);
		}
		//Fence Perimeter Collision
		else
		{
			if (GetPosition().x < 0)
				ApplyForce(vector3(dirToCenter, 0.0f, 0.0f));
			else
				ApplyForce(vector3(-dirToCenter, 0.0f, 0.0f));

			if (GetPosition().z < 0)
				ApplyForce(vector3(0.0f, 0.0f, dirToCenter));
			else
				ApplyForce(vector3(0.0f, 0.0f, -dirToCenter));
		}
	}

	//Sheep pushback from player and wolf
	if ((a_pOther->m_sUniqueID.find("Player") != std::string::npos) && (m_sUniqueID.find("sheep") != std::string::npos) || 
		(a_pOther->m_sUniqueID.find("wolves") != std::string::npos) && (m_sUniqueID.find("sheep") != std::string::npos))
	{
		//If the sheep isn't in the pen, proceed with physics calculation
		if (!IsInPen())
		{
			vector3 entityPos = a_pOther->GetPosition();
			vector3 directionVec = GetPosition() - entityPos;
			directionVec.y = 0.0f;
			String currentDir = "";
			
			//normalize the direction vector distanceBetween/m_playerRadius
			directionVec = glm::normalize(directionVec);

			//Set a direction for use in fence collision
			if (directionVec.z > 0) { currentDir += "South"; }
			else { currentDir += "North"; }
			if (directionVec.x > 0) { currentDir += "East"; }
			else { currentDir += "West"; }

			SetDirection(currentDir);
			step = 0.3f;

			//apply the force
			ApplyForce(directionVec);
			Update(0.05f);
		}
	}
}
void Simplex::MyEntity::UsePhysicsSolver(bool a_bUse)
{
    m_bUsePhysicsSolver = a_bUse;
}

void Simplex::MyEntity::SetGrid(Grid * gr)
{
	activeGrid = gr;
}

//Entity determines where to assign itself within the grid structure
void Simplex::MyEntity::AssignToCell()
{
	//entity removes itself from any cells it was in, in preparation of reassignment
	//This was previously the job of RefreshEntities() in CellNode.cpp, but that seemed to slow it down too much
	if (cellIndices.size() != 0)
	{
		for (int i = 0; i < cellIndices.size(); i++)
		{
			activeGrid->CellList[cellIndices[i]]->RemoveLocalEntity(m_sUniqueID);
		}

		cellIndices.clear();
	}
	
	//Find distance from entity to origin of grid
	//Since entities can spread across multiple cells (up to 3), check min, max, and center to ensure that any cell it's in is covered
	vector3 minPos = GetRigidBody()->GetMinGlobal();
	vector3 maxPos = GetRigidBody()->GetMaxGlobal();
	vector3 cenPos = GetRigidBody()->GetCenterGlobal();

	float distXmin = glm::distance(vector3((-activeGrid->WorldRadius), 0.0f, 0.0f), vector3(minPos.x,0.0f,0.0f));
	float distZmin = glm::distance(vector3(0.0f, 0.0f,(-activeGrid->WorldRadius)), vector3(0.0f, 0.0f, minPos.z));
	float cellRowMin = ceil(distXmin / activeGrid->CellSize);
	float cellColMin = ceil(distZmin / activeGrid->CellSize);

	float distXcen = glm::distance(vector3((-activeGrid->WorldRadius), 0.0f, 0.0f), vector3(cenPos.x, 0.0f, 0.0f));
	float distZcen = glm::distance(vector3(0.0f, 0.0f, (-activeGrid->WorldRadius)), vector3(0.0f, 0.0f, cenPos.z));
	float cellRowCen = ceil(distXcen / activeGrid->CellSize);
	float cellColCen = ceil(distZcen / activeGrid->CellSize);

	float distXmax = glm::distance(vector3((-activeGrid->WorldRadius), 0.0f, 0.0f), vector3(maxPos.x, 0.0f, 0.0f));
	float distZmax = glm::distance(vector3(0.0f, 0.0f, (-activeGrid->WorldRadius)), vector3(0.0f, 0.0f, maxPos.z));
	float cellRowMax = ceil(distXmax / activeGrid->CellSize);
	float cellColMax = ceil(distZmax / activeGrid->CellSize);

	int currentRow = 1; 
	int currentColumn = 1; 
	int numCells = activeGrid->NUM_CELLS;

	for (int i = 0; i < activeGrid->CellList.size(); i++)
	{
		//Flag to make sure an entity isn't adding itself to a local list multiple times, in case the different points are in the same cell
		bool alreadyAdded = false;

		// iterate over local column local row until they match row and column, add entity to CellList[i]
		if (currentRow == cellRowMin && currentColumn == cellColMin)
		{
			activeGrid->CellList[i]->AddLocalEntity(this);
			cellIndices.push_back(i);
			alreadyAdded = true;
		}

		if (currentRow == cellRowCen && currentColumn == cellColCen && !alreadyAdded)
		{
			activeGrid->CellList[i]->AddLocalEntity(this);
			cellIndices.push_back(i);
			alreadyAdded = true;
		}

		if (currentRow == cellRowMax && currentColumn == cellColMax && !alreadyAdded)
		{
			activeGrid->CellList[i]->AddLocalEntity(this);
			cellIndices.push_back(i);
			alreadyAdded = true;
		}

		if ((i + 1) % numCells == 0)
		{
			currentRow++;
			currentColumn = 0;
		}

		currentColumn++;
	}
}

//returns true if entity is within the bounds of the pen
bool Simplex::MyEntity::IsInPen()
{
	if (GetPosition().x > -21 && GetPosition().x < -9)
	{
		if (GetPosition().z > -4 && GetPosition().z < 10)
		{
			return true;
		}
	}

	return false;
}
