#include "Monster.h"

#include "Bullet.h"
#include "BehaviorNode.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{

}

CMonster::CMonster(const CMonster& Prototype)
	: CLandObject{ Prototype }
{

}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMonster::Initialize()
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));

	//if (m_pPlayerTransform == nullptr)
	//	return E_FAIL;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	//if (FAILED(Ready_Animations()))
	//	return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

}

void CMonster::Update(_float fTimeDelta)
{

}

void CMonster::Late_Update(_float fTimeDelta)
{

}

HRESULT CMonster::Render()
{

	return S_OK;
}

HRESULT CMonster::Ready_Animations()
{
	return S_OK;
}

HRESULT CMonster::Ready_Components()
{
	return S_OK;
}

HRESULT CMonster::Begin_RenderState()
{
	return S_OK;
}

HRESULT CMonster::End_RenderState()
{
	return S_OK;
}

void CMonster::Attack()
{

}

void CMonster::Move()
{
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSightCom);

	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}

}
