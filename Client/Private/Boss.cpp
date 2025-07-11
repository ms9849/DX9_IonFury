#include "Boss.h"
#include "GameInstance.h"
#include "BossUpperBody.h"
#include "BossLowerBody.h"
#include "Bullet.h"
#include "Particle_Manager.h"
#include "Bullet_Manager.h"
#include "Effect_Manager.h"
#include "Terrain_Manager.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{

}

CBoss::CBoss(const CBoss& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CBoss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
	/*if (FAILED(__super::Initialize()))
		return E_FAIL;*/

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(
		m_pGameInstance->Random(30.f, 50.f),
		0.f,
		m_pGameInstance->Random(30.f, 50.f)));

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Boss_Upper"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Upper"), m_pTransformCom);
	m_pBossUpperBody = dynamic_cast<CBossUpperBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Upper")));
	Safe_AddRef(m_pBossUpperBody);

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Boss_Lower"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Lower"), m_pTransformCom);
	m_pBossLowerBody = dynamic_cast<CBossLowerBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Lower")));
	Safe_AddRef(m_pBossLowerBody);

	//m_pTerrain_Manager = CTerrain_Manager::Create();
	//m_pTerrain_Manager->Add_LandObject_One(m_pBossLowerBody);
	CTerrain_Manager::GetInstance()->Add_LandObject_One(m_pBossLowerBody);

	return S_OK;
}

HRESULT CBoss::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

void CBoss::Priority_Update(_float fTimeDelta)
{

}

void CBoss::Update(_float fTimeDelta)
{
	if (!isUpperLive)
		m_fSumResurrectionTime += fTimeDelta;

	if (m_fSumResurrectionTime >= m_fResurrectionTime)
	{
		Resurrection();
		m_fSumResurrectionTime = 0.f;
		isUpperLive = true;
	}

	if (m_pBossLowerBody->isDead() && isLowerLive)
	{
		isLowerLive = false;
		m_pBossUpperBody->Set_LowerDead();
	}

	if (m_pBossUpperBody->isDead() && m_pBossLowerBody->isDead())
	{
		m_isDead = true;

		m_pGameInstance->PlaySoundOnce(TEXT("Boss1_Die.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		/*CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Boss_Die"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"),
			m_pTransformCom->Get_State(STATE::POSITION));*/
	}

	if (m_pBossUpperBody->isDead())
	{
		// bool 변수 하나 체크 시킨다음 부활시간 증가
		// 부활 시간이 다 차면 새로운 상체 클론해서 붙이기
		// 아니면 특정 위치로 가서 하게 해도 될듯
		//Safe_Release(m_pBossUpperBody);
		isUpperLive = false;
	}
}

void CBoss::Late_Update(_float fTimeDelta)
{

}

HRESULT CBoss::Render()
{
	return S_OK; 
}

void CBoss::Resurrection()
{
	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Boss_Upper"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Upper"), m_pTransformCom);
	Safe_Release(m_pBossUpperBody);
	m_pBossUpperBody = dynamic_cast<CBossUpperBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Upper")));

	Safe_AddRef(m_pBossUpperBody);
}

CBoss* CBoss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBoss* pInstance = new CBoss(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss::Clone(void* pArg)
{
	CBoss* pInstance = new CBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss");
		Safe_Release(pInstance);
	}
	
	/*m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Boss_Upper"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Upper"), pInstance->m_pTransformCom);
	pInstance->m_pBossUpperBody = dynamic_cast<CBossUpperBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Upper")));
	Safe_AddRef(m_pBossUpperBody);

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Boss_Lower"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Lower"), pInstance->m_pTransformCom);
	pInstance->m_pBossLowerBody = dynamic_cast<CBossLowerBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Boss1_Lower")));
	Safe_AddRef(m_pBossLowerBody);

	m_pTerrain_Manager = CTerrain_Manager::Create();
	m_pTerrain_Manager->Add_LandObject_One(pInstance->m_pBossLowerBody);*/

	return pInstance;
}

void CBoss::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBossUpperBody);
	Safe_Release(m_pBossLowerBody);
}
