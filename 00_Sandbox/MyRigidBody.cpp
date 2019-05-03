#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = true;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3CenterL = ZERO_V3;
	m_v3CenterG = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;

	m_uCollidingCount = 0;
	m_CollidingArray = nullptr;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bVisibleBS, other.m_bVisibleBS);
	std::swap(m_bVisibleOBB, other.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, other.m_bVisibleARBB);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3ColorColliding, other.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, other.m_v3ColorNotColliding);

	std::swap(m_v3CenterL, other.m_v3CenterL);
	std::swap(m_v3CenterG, other.m_v3CenterG);
	std::swap(m_v3MinL, other.m_v3MinL);
	std::swap(m_v3MaxL, other.m_v3MaxL);

	std::swap(m_v3MinG, other.m_v3MinG);
	std::swap(m_v3MaxG, other.m_v3MaxG);

	std::swap(m_v3HalfWidth, other.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, other.m_v3ARBBSize);

	std::swap(m_m4ToWorld, other.m_m4ToWorld);

	std::swap(m_uCollidingCount, other.m_uCollidingCount);
	std::swap(m_CollidingArray, other.m_CollidingArray);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3CenterL; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){ return m_v3CenterG; }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
MyRigidBody::PRigidBody* MyRigidBody::GetColliderArray(void) { return m_CollidingArray; }
uint MyRigidBody::GetCollidingCount(void) { return m_uCollidingCount; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	m_v3CenterG = vector3(m_m4ToWorld * vector4(m_v3CenterL, 1.0f));
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3CenterL = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3CenterL, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = other.m_pMeshMngr;

	m_bVisibleBS = other.m_bVisibleBS;
	m_bVisibleOBB = other.m_bVisibleOBB;
	m_bVisibleARBB = other.m_bVisibleARBB;

	m_fRadius = other.m_fRadius;

	m_v3ColorColliding = other.m_v3ColorColliding;
	m_v3ColorNotColliding = other.m_v3ColorNotColliding;

	m_v3CenterL = other.m_v3CenterL;
	m_v3CenterG = other.m_v3CenterG;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;
	m_v3ARBBSize = other.m_v3ARBBSize;

	m_m4ToWorld = other.m_m4ToWorld;

	m_uCollidingCount = other.m_uCollidingCount;
	m_CollidingArray = other.m_CollidingArray;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- other Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* other)
{
	//if its already in the list return
	if (IsInCollidingArray(other))
		return;
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	
	//insert the entry
	PRigidBody* pTemp;
	pTemp = new PRigidBody[m_uCollidingCount + 1];
	if (m_CollidingArray)
	{
		memcpy(pTemp, m_CollidingArray, sizeof(MyRigidBody*) * m_uCollidingCount);
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
	pTemp[m_uCollidingCount] = other;
	m_CollidingArray = pTemp;

	++m_uCollidingCount;
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* other)
{
	//if there are no dimensions return
	if (m_uCollidingCount == 0)
		return;

	//we look one by one if its the one wanted
	for (uint i = 0; i < m_uCollidingCount; i++)
	{
		if (m_CollidingArray[i] == other)
		{
			//if it is, then we swap it with the last one and then we pop
			std::swap(m_CollidingArray[i], m_CollidingArray[m_uCollidingCount - 1]);
			PRigidBody* pTemp;
			pTemp = new PRigidBody[m_uCollidingCount - 1];
			if (m_CollidingArray)
			{
				memcpy(pTemp, m_CollidingArray, sizeof(uint) * (m_uCollidingCount - 1));
				delete[] m_CollidingArray;
				m_CollidingArray = nullptr;
			}
			m_CollidingArray = pTemp;

			--m_uCollidingCount;
			return;
		}
	}
}
void MyRigidBody::ClearCollidingList(void)
{
	m_uCollidingCount = 0;
	if (m_CollidingArray)
	{
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
}
uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	float ra, rb;
	glm::mat3 R, AbsR;

	// Compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = glm::dot(GetModelMatrix()[i], a_pOther->GetModelMatrix()[j]);

	// Compute translation vector t
	vector3 t = a_pOther->GetCenterGlobal() - GetCenterGlobal();

	// Bring translation into a's coordinate frame
	t = vector3(glm::dot(t, glm::vec3(GetModelMatrix()[0])), glm::dot(t, glm::vec3(GetModelMatrix()[1])), glm::dot(t, glm::vec3(GetModelMatrix()[2])));

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR[i][j] = glm::abs(R[i][j]) + std::numeric_limits<float>::epsilon();

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) {
		ra = GetHalfWidth()[i];
		rb = a_pOther->GetHalfWidth()[0] * AbsR[i][0] + a_pOther->GetHalfWidth()[1] * AbsR[i][1] + a_pOther->GetHalfWidth()[2] * AbsR[i][2];
		if (glm::abs(t[i]) > ra + rb) return 1;
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		ra = GetHalfWidth()[0] * AbsR[0][i] + GetHalfWidth()[1] * AbsR[1][i] + GetHalfWidth()[2] * AbsR[2][i];
		rb = a_pOther->GetHalfWidth()[i];
		if (glm::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return 1;
	}

	// Test axis L = A0 x B0
	ra = GetHalfWidth()[1] * AbsR[2][0] + GetHalfWidth()[2] * AbsR[1][0];
	rb = a_pOther->GetHalfWidth()[1] * AbsR[0][2] + a_pOther->GetHalfWidth()[2] * AbsR[0][1];
	if (glm::abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return 1;

	// Test axis L = A0 x B1
	ra = GetHalfWidth()[1] * AbsR[2][1] + GetHalfWidth()[2] * AbsR[1][1];
	rb = a_pOther->GetHalfWidth()[0] * AbsR[0][2] + a_pOther->GetHalfWidth()[2] * AbsR[0][0];
	if (glm::abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return 1;

	// Test axis L = A0 x B2
	ra = GetHalfWidth()[1] * AbsR[2][2] + GetHalfWidth()[2] * AbsR[1][2];
	rb = a_pOther->GetHalfWidth()[0] * AbsR[0][1] + a_pOther->GetHalfWidth()[1] * AbsR[0][0];
	if (glm::abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return 1;

	// Test axis L = A1 x B0
	ra = GetHalfWidth()[0] * AbsR[2][0] + GetHalfWidth()[2] * AbsR[0][0];
	rb = a_pOther->GetHalfWidth()[1] * AbsR[1][2] + a_pOther->GetHalfWidth()[2] * AbsR[1][1];
	if (glm::abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return 1;

	// Test axis L = A1 x B1
	ra = GetHalfWidth()[0] * AbsR[2][1] + GetHalfWidth()[2] * AbsR[0][1];
	rb = a_pOther->GetHalfWidth()[0] * AbsR[1][2] + a_pOther->GetHalfWidth()[2] * AbsR[1][0];
	if (glm::abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return 1;

	// Test axis L = A1 x B2
	ra = GetHalfWidth()[0] * AbsR[2][2] + GetHalfWidth()[2] * AbsR[0][2];
	rb = a_pOther->GetHalfWidth()[0] * AbsR[1][1] + a_pOther->GetHalfWidth()[1] * AbsR[1][0];
	if (glm::abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return 1;

	// Test axis L = A2 x B0
	ra = GetHalfWidth()[0] * AbsR[1][0] + GetHalfWidth()[1] * AbsR[0][0];
	rb = a_pOther->GetHalfWidth()[1] * AbsR[2][2] + a_pOther->GetHalfWidth()[2] * AbsR[2][1];
	if (glm::abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return 1;

	// Test axis L = A2 x B1
	ra = GetHalfWidth()[0] * AbsR[1][1] + GetHalfWidth()[1] * AbsR[0][1];
	rb = a_pOther->GetHalfWidth()[0] * AbsR[2][2] + a_pOther->GetHalfWidth()[2] * AbsR[2][0];
	if (glm::abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return 1;

	// Test axis L = A2 x B2
	ra = GetHalfWidth()[0] * AbsR[1][2] + GetHalfWidth()[1] * AbsR[0][2];
	rb = a_pOther->GetHalfWidth()[0] * AbsR[2][1] + a_pOther->GetHalfWidth()[1] * AbsR[2][0];
	if (glm::abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return 1;

	//there is no axis test that separates this two objects
	return 0;
}
bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), other->GetCenterGlobal()) < m_fRadius + other->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if (SAT(other) != 0)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(other);
		other->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(other);
		other->RemoveCollisionWith(this);
	}

	return bColliding;
}

void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_uCollidingCount > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_uCollidingCount > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_uCollidingCount > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}
bool MyRigidBody::IsInCollidingArray(MyRigidBody* a_pEntry)
{
	//see if the entry is in the set
	for (uint i = 0; i < m_uCollidingCount; i++)
	{
		if (m_CollidingArray[i] == a_pEntry)
			return true;
	}
	return false;
}
