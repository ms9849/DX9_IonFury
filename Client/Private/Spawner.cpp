#include "Spawner.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Terrain_Manager.h"

CSpawner::CSpawner(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CSpawner::CSpawner(const CSpawner& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CSpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpawner::Initialize(void* pArg)
{
	/*if (FAILED(__super::Initialize()))
		return E_FAIL;*/

	// 스포너 생성 후 초기화 시 생성하고 싶은 장소값이 넘어오면 그 장소에 생성되도록 구현
	m_Desc.idx = -1;
	if (pArg != nullptr)
	{
		m_Desc = *static_cast<SPAWNER_DESC*>(pArg);
	}
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg == nullptr)
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(
			m_pGameInstance->Random(0.f, 20.f),
			0.f,
			m_pGameInstance->Random(0.f, 20.f)));
	}
	else
	{
		m_pTransformCom->Set_State(STATE::POSITION, m_Desc.vPos);
	}

	return S_OK;
}

HRESULT CSpawner::Ready_Components()
{
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)));

	CBoxCollider::BOXCOLLIDER_DESC Desc;
	Desc.vPosition = { 0.f, -0.3f, 0.f };
	Desc.fScaleX = 1.5f;
	Desc.fScaleZ = 1.5f;
	Desc.fScaleY = 2.0f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)));

	return S_OK;
}

void CSpawner::Priority_Update(_float fTimeDelta)
{
}

void CSpawner::Update(_float fTimeDelta)
{
}

void CSpawner::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CSpawner::Render()
{
	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));

	return S_OK;
}

void CSpawner::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	for (size_t i = 0; i < 5; i++)
	{
		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Zombie"),
		//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster"))))
		//	return;	// 이러면 몬스터 레이어에 있는거 아무거나 들어가는거 아닌가

		CGameObject* pClone = nullptr;
		if (m_Desc.idx != -1)
		{
			pClone = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), 
				m_strFrameKeys[m_Desc.idx], m_pTransformCom->Get_State(STATE::POSITION)));
		}
		else
		{
			pClone = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), 
				m_strFrameKeys[0], m_pTransformCom->Get_State(STATE::POSITION)));
		}

		m_pGameInstance->Add_Clone_ToLayer(pClone, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster"));

		m_pTerrain_Manager->Add_LandObject_One(pClone);
	}

	m_isDead = true;
	return;
}

void CSpawner::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider)
{
	// 소환시키고 삭제
	for (size_t i = 0; i < 5; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Zombie"),
			ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster"))))
			return;
	}

	m_isDead = true;
	return;
}

const COLLISION_DESC& CSpawner::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::BOX)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

CSpawner* CSpawner::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSpawner* pInstance = new CSpawner(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpawner::Clone(void* pArg)
{
	CSpawner* pInstance = new CSpawner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Spawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpawner::Free()
{
	__super::Free();
	Safe_Release(m_pTerrain_Manager);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBoxColliderCom);
}
