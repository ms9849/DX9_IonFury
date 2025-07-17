#include "Spawner.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Terrain_Manager.h"
#include "Zombie.h"

CSpawner::CSpawner(LPDIRECT3DDEVICE9 pGraphicDev) :
	CGameObject{ pGraphicDev }
{
}

CSpawner::CSpawner(const CSpawner& Prototype) :
	CGameObject{ Prototype }
{
}

HRESULT CSpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpawner::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

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
	//m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CSpawner::Render()
{
	m_pTransformCom->Set_Transform();

//	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));

	return S_OK;
}

HRESULT CSpawner::Ready_Components()
{
	/* Com_Transform */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::RIGHT, m_pObjectDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(STATE::UP, m_pObjectDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(STATE::LOOK, m_pObjectDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(STATE::POSITION, m_pObjectDesc.matWorld.m[3]);

	CBoxCollider::BOXCOLLIDER_DESC Desc{};
	Desc.vPosition = { 0.f, -0.3f, 0.f };
	Desc.fScaleX = m_pTransformCom->Get_Scaled().x;
	Desc.fScaleZ = m_pTransformCom->Get_Scaled().z;
	Desc.fScaleY = m_pTransformCom->Get_Scaled().y;

	/* Com_BoxCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

void CSpawner::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	// 소환시키고 삭제
	for (size_t i = 0; i < 5; i++)
	{
		CGameObject::GAMEOBJECT_DESC Desc{};
		Desc = m_pObjectDesc;

		Desc.matWorld.m[3][0] = m_pTransformCom->Get_State(STATE::POSITION).x + m_pGameInstance->Random(-3.f, 3.f);
		Desc.matWorld.m[3][2] = m_pTransformCom->Get_State(STATE::POSITION).z + m_pGameInstance->Random(3.f, 6.f);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iProtoLevel, TEXT("Prototype_GameObject_Monster_Zombie"),
			m_pObjectDesc.iLayerLevel, TEXT("Layer_Monster"), &Desc)))
			return;

		dynamic_cast<CZombie*>(
			m_pGameInstance->Find_GameObject_ToLayer(m_pObjectDesc.iLayerLevel,
				TEXT("Layer_Monster")))->Set_Awake(true);
	}

	m_isDead = true;
}

const COLLISION_DESC& CSpawner::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pCollider = m_pBoxColliderCom;
	Desc.pTransform = m_pTransformCom;

	return Desc;
}

CSpawner* CSpawner::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSpawner* pInstance = new CSpawner(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: Spawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpawner::Clone(void* pArg)
{
	CSpawner* pInstance = new CSpawner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed: CSpawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpawner::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBoxColliderCom);
}
