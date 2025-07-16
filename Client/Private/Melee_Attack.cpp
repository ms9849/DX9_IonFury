#include "MeleeAttack.h"

#include "GameInstance.h"

CMeleeAttack::CMeleeAttack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject{ pGraphicDev }
{
}

CMeleeAttack::CMeleeAttack(const CMeleeAttack& Prototype )
	: CGameObject{ Prototype }
{
}

HRESULT CMeleeAttack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeleeAttack::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, *(_float3*)&m_pObjectDesc.matWorld.m[3][0]);
	
	return S_OK;
}

void CMeleeAttack::Priority_Update(_float fTimeDelta)
{
}

void CMeleeAttack::Update(_float fTimeDelta)
{
	m_fAccumulationTime += fTimeDelta;
	if (m_fAccumulationTime >= m_fDurationTime)
	{
		m_isDead = true;
	}
}

void CMeleeAttack::Late_Update(_float fTimeDelta)
{

}

HRESULT CMeleeAttack::Render()
{
	m_pTransformCom->Set_Transform();

	return S_OK;
}

void CMeleeAttack::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	m_isDead = true;
}

const COLLISION_DESC& CMeleeAttack::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

HRESULT CMeleeAttack::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 3.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

 	return S_OK;
}

CMeleeAttack* CMeleeAttack::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMeleeAttack* pInstance = new CMeleeAttack(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: CMeleeAttack");
	}

	return pInstance;
}

CGameObject* CMeleeAttack::Clone(void* pArg)
{
	CMeleeAttack* pInstance = new CMeleeAttack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: CMeleeAttack");
	}

	return pInstance;
}

void CMeleeAttack::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pSphereColliderCom);
}
