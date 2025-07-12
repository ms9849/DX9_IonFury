#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pGraphic_Device{ Prototype.m_pGraphic_Device }
	, m_pGameInstance{ Prototype.m_pGameInstance }
	, m_isDead { Prototype.m_isDead }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

//void CGameObject::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
//{
//	// 이 주석을 보고 있다면
//	// 하위 클래스에서 OnCollision 구현 안한거니까
//	// 다들 조심
//	int a = 10;
//}

void CGameObject::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	// 이 주석을 보고 있다면
	// 하위 클래스에서 OnCollision 구현 안한거니까
	// 다들 조심
}

void CGameObject::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos)
{
	/* 콜라이더의 정보가 필요하다면 이 메소드를 구현하면 됩니다. */

	// 이 주석을 보고 있다면
	// 하위 클래스에서 OnCollision 구현 안한거니까
	// 다들 조심
}

void CGameObject::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos, const _float3& vPlaneNormal)
{
}

const COLLISION_DESC& CGameObject::Get_CollisionDesc(COLLISION eColType)
{
	// 이 주석을 보고 있다면
	// 하위 클래스에서 Get_CollisionDesc 구현 안한거니까
	// 다들 조심
	COLLISION_DESC desc;
	return desc;
}

const RAY_DESC& CGameObject::Get_RayDesc()
{
	RAY_DESC desc;
	return desc;
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}


CComponent* CGameObject::Find_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if(iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Compute_CamDistance(const _float3& vWorldPos)
{
	/*
	직교 투영 되는 것들은 Compute_CamDistance할 의미가 없나?..
	*/
	_float3			vCamPos = {};
	memcpy(&vCamPos, &m_pGameInstance->Get_CameraWorld().m[3][0], sizeof(_float3));

	_float3			vDir = vCamPos - vWorldPos;
	m_fCamDistance = D3DXVec3Length(&vDir);

	//_float4x4		ViewMatrix = {};
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	//D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	//_float3			vCamPos = {};
	//memcpy(&vCamPos, &ViewMatrix.m[3], sizeof(_float3));

	//_float3			vDir = vCamPos - vWorldPos;
	//m_fCamDistance = D3DXVec3Length(&vDir);
	////////////////////////////////////////////////////

}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CComponent*			pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
