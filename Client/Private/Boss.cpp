#include "Boss.h"
#include "GameInstance.h"
#include "BossUpperBody.h"
#include "BossLowerBody.h"
#include "Bullet.h"
#include "Particle_Manager.h"
#include "Bullet_Manager.h"
#include "Effect_Manager.h"
#include "Terrain_Manager.h"
#include "Effect_Black_Sight.h"
#include "Camera.h"

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
		m_pGameInstance->Random(63.5f, 63.5f),
		0.f,
		m_pGameInstance->Random(63.5f, 63.5f)));

	UPPER_DESC desc;
	desc.pCoreTransform = m_pTransformCom;
	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Monster_Boss_Upper"), ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss1_Upper"), &desc);
	m_pBossUpperBody = dynamic_cast<CBossUpperBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss1_Upper")));
	Safe_AddRef(m_pBossUpperBody);

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Monster_Boss_Lower"), ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss1_Lower"), m_pTransformCom);
	m_pBossLowerBody = dynamic_cast<CBossLowerBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss1_Lower")));
	Safe_AddRef(m_pBossLowerBody);

	m_pObjectDesc.iLayerLevel = ENUM_CLASS(LEVEL::BOSSFIGHT);
	m_fMaxHp = m_pBossUpperBody->Get_Hp() + m_pBossLowerBody->Get_Hp();

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
	/*_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

	wchar_t szBuffer[128];
	swprintf_s(szBuffer, 128, L"[디버그] 코어 위치: X: %.3f, Y: %.3f, Z: %.3f\n", vPos.x, vPos.y, vPos.z);
	OutputDebugStringW(szBuffer);*/
	//if (!isUpperLive)								// 상체 죽었으면 시간 누적
	m_fCurHp = m_pBossLowerBody->Get_Hp() + m_pBossUpperBody->Get_Hp();
	wchar_t szBuffer[128];
	swprintf_s(szBuffer, 128, L"[디버그] 보스 전체 체력: %.3f\n",m_fCurHp);
	OutputDebugStringW(szBuffer);
	if(m_pBossUpperBody->isDead())
	{
		m_fSumResurrectionTime += fTimeDelta;
		isUpperLive = false;
		//OutputDebugStringA("디버그 메시지: 상체 사망 상태\n");
		m_pBossLowerBody->Set_BodyLive(false);		// 하체한테 상체 죽음 알림
	}

	if (m_pBossLowerBody->isDead())		// 
	{
		//isLowerLive = false;
		isLowerLive = false;
		//OutputDebugStringA("디버그 메시지: 하체 사망 상태\n");
		m_pBossUpperBody->Set_LowerDead();			// 상체한테 하체 죽음 알림
	}

	if (m_fSumResurrectionTime >= m_fResurrectionTime)	// 부활 시간 지났으면
	{
		//OutputDebugStringA("디버그 메시지: 상체 부활\n");
		Resurrection();									// 부활 시전
		m_fSumResurrectionTime = 0.f;					// 부활 누적 시간 0으로 초기화
		isUpperLive = true;								// 상체 살아있음으로 상태 변경
		m_pBossLowerBody->Set_BodyLive(true);			// 하체에 상체 살아났음을 알려줌
	}

	//if (m_pBossLowerBody->isDead() && isLowerLive)		// 
	//{
	//	isLowerLive = false;
	//	m_pBossUpperBody->Set_LowerDead();
	//}

	//if (m_pBossUpperBody->isDead() && m_pBossLowerBody->isDead())			// 둘다 죽었으면 완전 사망
	//if (!isUpperLive && !isLowerLive)
	if (m_fCurHp <= 0)
	{
		//OutputDebugStringA("디버그 메시지: 둘 다 사망 상태\n");
		m_isDead = true;

		m_pGameInstance->PlaySoundOnce(TEXT("Boss1_Die.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		CEffect_Black_Sight* pBlackSight = static_cast<CEffect_Black_Sight*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_Black_Sight"), nullptr));
		m_pGameInstance->Add_Clone_ToLayer(pBlackSight, ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Effect"));
		/*CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Boss_Die"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"),
			m_pTransformCom->Get_State(STATE::POSITION));*/
	}

	//if (m_pBossUpperBody->isDead())						// 상체 죽었으면
	//{
	//	// bool 변수 하나 체크 시킨다음 부활시간 증가
	//	// 부활 시간이 다 차면 새로운 상체 클론해서 붙이기
	//	// 아니면 특정 위치로 가서 하게 해도 될듯
	//	//Safe_Release(m_pBossUpperBody);
	//	isUpperLive = false;							// 상체 죽은거 체크
	//}

	//if (m_pBossLowerBody->isDead())						// 하체 죽었으면
	//{
	//	// bool 변수 하나 체크 시킨다음 부활시간 증가
	//	// 부활 시간이 다 차면 새로운 상체 클론해서 붙이기
	//	// 아니면 특정 위치로 가서 하게 해도 될듯
	//	//Safe_Release(m_pBossUpperBody);
	//	isLowerLive = false;							// 하체 죽은거 체크
	//}
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
	UPPER_DESC desc;
	desc.pCoreTransform = m_pTransformCom;
	desc.isRegenerate = true;
	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Monster_Boss_Upper"), ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss1_Upper"), &desc);
	Safe_Release(m_pBossUpperBody);
	m_pBossUpperBody = dynamic_cast<CBossUpperBody*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss1_Upper")));

	static_cast<CCamera*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Camera"), nullptr))->Shaking(0.2f);

	Safe_AddRef(m_pBossUpperBody);
}

_float CBoss::Get_Hp()
{
	return m_fCurHp;
}

_float CBoss::Get_MaxHp()
{
	return m_fMaxHp;
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
