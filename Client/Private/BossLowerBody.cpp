#include "BossLowerBody.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "BossGrenade.h"
#include "Particle_Manager.h"
#include "Bullet_Manager.h"
#include "Effect_Manager.h"

CBossLowerBody::CBossLowerBody(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{

}

CBossLowerBody::CBossLowerBody(const CBossLowerBody& Prototype)
	: CMonster{ Prototype }
{

}

HRESULT CBossLowerBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossLowerBody::Initialize(void* pArg)
{
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);
	m_pCoreTranform = static_cast<CTransform*>(pArg);
	Safe_AddRef(m_pCoreTranform);

	m_pObjectDesc.iLayerLevel = ENUM_CLASS(LEVEL::BOSSFIGHT);

	if (m_pCoreTranform == nullptr)
		return E_FAIL;

	if (m_pPlayerTransform == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 7.f, 7.f, 1.f });

	//_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
	// 최초 위치 코어의 위치값으로 설정
	m_pTransformCom->Set_State(STATE::POSITION, m_pCoreTranform->Get_State(STATE::POSITION));

	m_fChaseRange = 20.f;
	m_fSafeDistance = 5.f;
	m_fStopMoveTime = 2.f;
	m_fDefaultStopMoveTime = m_fStopMoveTime;
	// 기존 0.05
	m_fMoveCoolTime = 0.2f;
	m_strFrameKey = TEXT("Boss_Front_LeftLeg");
	m_strLegFrameKey = TEXT("Boss_Front_LeftLeg");
	m_fDirDuraionTime = 3.f;
	m_fSumDirDurarionTime = 0.f;
	m_fCurHp = m_fMaxHp;

	return S_OK;
}

void CBossLowerBody::Priority_Update(_float fTimeDelta)
{

}

void CBossLowerBody::Update(_float fTimeDelta)
{
	//_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

	//wchar_t szBuffer[128];
	//swprintf_s(szBuffer, 128, L"[디버그] 하체 위치: X: %.3f, Y: %.3f, Z: %.3f\n", vPos.x, vPos.y, vPos.z);
	//OutputDebugStringW(szBuffer);
	if (m_isDead)
	{
		return;
	}

	//m_isBodyLive == false ? m_fStopMoveTime *= 0.01f : m_fStopMoveTime = m_fDefaultStopMoveTime;
	if (!m_isBodyLive)
	{
		m_fDirDuraionTime = 0.3f;
		m_fStopMoveTime = 0.002f;
	}
	else
	{
		m_fStopMoveTime = m_fDefaultStopMoveTime;
		m_fDirDuraionTime = 3.f;
		m_fStopMoveTime = 2.f;
	}

	if (!m_isMove)
		m_fSumStopMoveTime += fTimeDelta;
	/*else
		m_fSumMoveCoolTime += fTimeDelta;*/

	__super::Jump(fTimeDelta);

	m_fSumMoveCoolTime += fTimeDelta;
	m_fSumDirDurarionTime += fTimeDelta;

	if (m_fCurHp <= 0)
	{
		m_pGameInstance->PlaySoundOnce(TEXT("Boss_lost.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		m_bAnimationLock = true;
		m_isDead = true;
		CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Grenade_Explosion"), ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Effect"),
			m_pTransformCom->Get_State(STATE::POSITION));
	}
	else if (m_pSightCom->Check_Sight(fTimeDelta))
	{
		_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);

		if ((D3DXVec3Length(&vDiff) <= m_fChaseRange) && (D3DXVec3Length(&vDiff) >= m_fSafeDistance) && (m_fSumMoveCoolTime >= m_fMoveCoolTime))
		{
			if (m_fSumStopMoveTime >= m_fStopMoveTime)
			{
				if (m_isBodyLive)
				{
					Move(fTimeDelta);
				}
				else
				{
					Escape(fTimeDelta);
				}
				m_fSumMoveCoolTime = 0.f;
			}
		}
	}
	
	// 이동 후 core위치 잡아주기
	//__super::Jump(fTimeDelta);
	m_pCoreTranform->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION));
	SetUp_OnTerrain(m_pTransformCom, 1.2f, &m_bJump);
}

void CBossLowerBody::Late_Update(_float fTimeDelta)
{
	if (m_isMove)
	{
		m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);
		if (m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
		{
			m_isLeftLeg = !m_isLeftLeg;

			/*if (m_strLegFrameKey == TEXT("Boss_LeftLeg"))
			{
				m_strLegFrameKey = TEXT("Boss_RightLeg");
			}
			else
			{
				m_strLegFrameKey = TEXT("Boss_LeftLeg");
			}*/
			m_isMove = false;
			m_bAnimationLock = false;
			m_fSumStopMoveTime = 0.f;
		}
	}

	//RotateWithParentTransform();				// 이러면 위 아래가 기본 pos에 고정됨
	//
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CBossLowerBody::Render()
{
	//
	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));
	
	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	//
	RotateToPlayer(m_pTransformCom);
	auto iter = m_pTextureComs.find(m_strFrameKey);
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));
	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

// 혹시 몰라 남겨두기
//void CBossLowerBody::RotateWithParentTransform()
//{
//	_float3 vRootPos = m_pTransformCom->Get_State(STATE::POSITION);
//
//	m_pTransformCom->Set_State(STATE::POSITION, vRootPos);
//
//	m_pTransformCom->Set_State(STATE::RIGHT, m_pTransformCom->Get_State(STATE::RIGHT));
//	m_pTransformCom->Set_State(STATE::UP, m_pTransformCom->Get_State(STATE::UP));
//	m_pTransformCom->Set_State(STATE::LOOK, m_pTransformCom->Get_State(STATE::LOOK));
//}

HRESULT CBossLowerBody::Ready_Animations()
{
	CAnimation::FRAME_DESC Desc_0{};
	CAnimation::FRAME_DESC Desc_1{};
	CAnimation::FRAME_DESC Desc_2{};
	CAnimation::FRAME_DESC Desc_3{};
	CAnimation::FRAME_DESC Desc_4{};
	CAnimation::FRAME_DESC Desc_5{};
	CAnimation::FRAME_DESC Desc_6{};
	CAnimation::FRAME_DESC Desc_7{};
	CAnimation::FRAME_DESC Desc_8{};
	CAnimation::FRAME_DESC Desc_9{};
	CAnimation::FRAME_DESC Desc_10{};
	CAnimation::FRAME_DESC Desc_11{};
	CAnimation::FRAME_DESC Desc_12{};
	CAnimation::FRAME_DESC Desc_13{};
	CAnimation::FRAME_DESC Desc_14{};
	CAnimation::FRAME_DESC Desc_15{};
	CAnimation::FRAME_DESC Desc_16{};
	CAnimation::FRAME_DESC Desc_17{};
	CAnimation::FRAME_DESC Desc_18{};
	CAnimation::FRAME_DESC Desc_19{};
	CAnimation::FRAME_DESC Desc_20{};
	CAnimation::FRAME_DESC Desc_21{};
	CAnimation::FRAME_DESC Desc_22{};
	CAnimation::FRAME_DESC Desc_23{};

	//Boss_Leg_Direction_NE
	auto iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_NE"));
	Desc_0.iFrameSpeed = 5;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Leg_Direction_NE"), Desc_0);

	//Boss_Leg_Direction_NW
	iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_NW"));
	Desc_1.iFrameSpeed = 5;
	Desc_1.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Leg_Direction_NW"), Desc_1);

	//Boss_Direction_SE
	iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_SE"));
	Desc_2.iFrameSpeed = 5;
	Desc_2.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Leg_Direction_SE"), Desc_2);

	//Boss_Direction_SW
	iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_SW"));
	Desc_3.iFrameSpeed = 5;
	Desc_3.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Leg_Direction_SW"), Desc_3);

	//Boss_Leg_Front
	iter = m_pTextureComs.find(TEXT("Boss_Front_Leg"));
	Desc_4.iFrameSpeed = 5;
	Desc_4.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Front_Leg"), Desc_4);

	//Boss_Leg_Back
	iter = m_pTextureComs.find(TEXT("Boss_Back_Leg"));
	Desc_5.iFrameSpeed = 5;
	Desc_5.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Back_Leg"), Desc_5);

	//Boss_Leg_Left
	iter = m_pTextureComs.find(TEXT("Boss_Left_Leg"));
	Desc_6.iFrameSpeed = 5;
	Desc_6.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Left_Leg"), Desc_6);

	//Boss_Leg_Right
	iter = m_pTextureComs.find(TEXT("Boss_Right_Leg"));
	Desc_7.iFrameSpeed = 5;
	Desc_7.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Right_Leg"), Desc_7);

	//Boss_Front_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Front_LeftLeg"));
	Desc_8.iFrameSpeed = 20;
	Desc_8.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Front_LeftLeg"), Desc_8);

	//Boss_Front_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Front_RightLeg"));
	Desc_9.iFrameSpeed = 20;
	Desc_9.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Front_RightLeg"), Desc_9);

	//Boss_Back_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Back_LeftLeg"));
	Desc_10.iFrameSpeed = 20;
	Desc_10.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Back_LeftLeg"), Desc_10);

	//Boss_Back_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Back_RightLeg"));
	Desc_11.iFrameSpeed = 20;
	Desc_11.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Back_RightLeg"), Desc_11);

	//Boss_Left_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Left_LeftLeg"));
	Desc_12.iFrameSpeed = 20;
	Desc_12.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Left_LeftLeg"), Desc_12);

	//Boss_Left_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Left_RightLeg"));
	Desc_13.iFrameSpeed = 20;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Left_RightLeg"), Desc_13);

	//Boss_Right_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Right_LeftLeg"));
	Desc_14.iFrameSpeed = 20;
	Desc_14.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Right_LeftLeg"), Desc_14);

	//Boss_Right_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Right_RightLeg"));
	Desc_15.iFrameSpeed = 20;
	Desc_15.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Right_RightLeg"), Desc_15);

	//Boss_Direction_NW_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NW_LeftLeg"));
	Desc_16.iFrameSpeed = 20;
	Desc_16.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_NW_LeftLeg"), Desc_16);

	//Boss_Direction_NW_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NW_RightLeg"));
	Desc_17.iFrameSpeed = 20;
	Desc_17.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_NW_RightLeg"), Desc_17);

	//Boss_Direction_NE_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NE_LeftLeg"));
	Desc_18.iFrameSpeed = 20;
	Desc_18.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_NE_LeftLeg"), Desc_18);

	//Boss_Direction_NE_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NE_RightLeg"));
	Desc_19.iFrameSpeed = 20;
	Desc_19.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_NE_RightLeg"), Desc_19);

	//Boss_Direction_SW_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SW_LeftLeg"));
	Desc_20.iFrameSpeed = 20;
	Desc_20.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_SW_LeftLeg"), Desc_20);

	//Boss_Direction_SW_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SW_RightLeg"));
	Desc_21.iFrameSpeed = 20;
	Desc_21.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_SW_RightLeg"), Desc_21);

	//Boss_Direction_SE_LeftLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SE_LeftLeg"));
	Desc_22.iFrameSpeed = 20;
	Desc_22.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_SE_LeftLeg"), Desc_22);

	//Boss_Direction_SE_RightLeg
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SE_RightLeg"));
	Desc_23.iFrameSpeed = 20;
	Desc_23.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_SE_RightLeg"), Desc_23);

	return S_OK;
}

void CBossLowerBody::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (m_isDead || m_bDying)
		return;

	if (eColType == COLLISION::SPHERE)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
		{
			if ((m_fCurHp -= (pBullet->Get_Damage())) > 0)
			{
				// 보스는 피격 사운드 넣는게 애매해서 일단 보류
				//m_pGameInstance->PlaySoundOnce(TEXT("Spider_Hit.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
			}
			else
			{
				// 보스는 추후 적용
				/*CParticle_Manager::GetInstance()->Create_Particle(TEXT("Particle_Blood"), ENUM_CLASS(LEVEL::GAMEPLAY),
					TEXT("Layer_Particle"), m_pTransformCom->Get_State(STATE::POSITION));*/
				/*CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Boss_Die"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"),
					m_pTransformCom->Get_State(STATE::POSITION));*/
			}
		}
	}

	return;
}

void CBossLowerBody::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos)
{
	if (m_isDead || m_bDying)
		return;

	if (eColType == COLLISION::RAY)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
		{
			if ((m_fCurHp -= (pBullet->Get_Damage())) > 0)
			{
				m_pGameInstance->PlaySoundOnce(TEXT("Boss1_hit2.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
			}
			else
			{
				m_fCurHp = 0.f;
			}

			CParticle_Manager::GetInstance()->Create_Particle(TEXT("Particle_YellowDust"), m_pObjectDesc.iLayerLevel,
				TEXT("Layer_Particle"), vPos);
		}
	}
}

const COLLISION_DESC& CBossLowerBody::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	else if (eColType == COLLISION::BOX)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

HRESULT CBossLowerBody::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	for (size_t i = 0; i < (sizeof(m_strFrameKeys) / sizeof(m_strFrameKeys[0])); ++i)
	{
		_tchar strPrototypeTag[256];
		_tchar strComponentTag[256];

		CTexture* pTextureCom{ nullptr };

		wsprintf(strPrototypeTag, TEXT("Prototype_Component_Texture_Monster_%s"), m_strFrameKeys[i].c_str());
		wsprintf(strComponentTag, TEXT("Com_%s_Texture"), m_strFrameKeys[i].c_str());

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), strPrototypeTag,
			strComponentTag, reinterpret_cast<CComponent**>(&pTextureCom))))
			return E_FAIL;

		m_pTextureComs.emplace(m_strFrameKeys[i], pTextureCom);
	}

	/* Com_Animation */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimationCom))))
		return E_FAIL;


	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Sight */
	CSight::SIGHT_DESC		SightDesc{ 30.f, D3DXToRadian(90.0f), m_pPlayerTransform, m_pTransformCom, true };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Sight"),
		TEXT("Com_Sight"), reinterpret_cast<CComponent**>(&m_pSightCom), &SightDesc)))
		return E_FAIL;

	/* Com_BoxCollider */
	CBoxCollider::BOXCOLLIDER_DESC Desc;
	Desc.vPosition = { 0.f, 0.f, 0.f };
	Desc.fScaleX = 1.5f;
	Desc.fScaleZ = 1.5f;
	Desc.fScaleY = 2.5f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossLowerBody::Begin_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}


HRESULT CBossLowerBody::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CBossLowerBody::RotationCheck()
{
	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	//_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_vTargetPos;
	//_float3 vDiff = m_vTargetPos - m_pPlayerTransform->Get_State(STATE::POSITION);
	vDiff.y = 0.f;
	D3DXVec3Normalize(&vDiff, &vDiff);

	//_float3 vMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	_float3 vMonsterLook = m_vTargetPos - m_pTransformCom->Get_State(STATE::POSITION);
	vMonsterLook.y = 0.f;
	D3DXVec3Normalize(&vMonsterLook, &vMonsterLook);

	_float dot = D3DXVec3Dot(&vMonsterLook, &vDiff);
	dot = max(-1.f, min(1.f, dot));

	_float3 vCross;
	D3DXVec3Cross(&vCross, &vMonsterLook, &vDiff);

	_float fFov = cosf(D3DXToRadian(45.f));

	_float angle30 = cosf(D3DXToRadian(30.f));
	_float angle60 = cosf(D3DXToRadian(60.f));

	if (dot >= fFov)
	{
		if (m_isLeftLeg)
			m_strFrameKey = TEXT("Boss_Front_LeftLeg");
		else
			m_strFrameKey = TEXT("Boss_Front_RightLeg");
	}
	else if (dot <= -fFov)
	{
		if (m_isLeftLeg)
			m_strFrameKey = TEXT("Boss_Back_LeftLeg");
		else
			m_strFrameKey = TEXT("Boss_Back_RightLeg");
	}
	else
	{
		if (vCross.y > 0)
		{
			if (dot > 0.2f)
			{
				if (m_isLeftLeg)
					m_strFrameKey = TEXT("Boss_Direction_SW_LeftLeg");
				else
					m_strFrameKey = TEXT("Boss_Direction_SW_RightLeg");
			}
			else if (dot > 0)
			{
				if (m_isLeftLeg)
					m_strFrameKey = TEXT("Boss_Left_LeftLeg");
				else
					m_strFrameKey = TEXT("Boss_Left_RightLeg");
			}
			else
			{
				if (m_isLeftLeg)
					m_strFrameKey = TEXT("Boss_Direction_NW_LeftLeg");
				else
					m_strFrameKey = TEXT("Boss_Direction_NW_RightLeg");
			}
		}
		else
		{
			if (dot > 0.2f)
			{
				if (m_isLeftLeg)
					m_strFrameKey = TEXT("Boss_Direction_SE_LeftLeg");
				else
					m_strFrameKey = TEXT("Boss_Direction_SE_RightLeg");
			}
			else if (dot > 0)
			{
				if (m_isLeftLeg)
					m_strFrameKey = TEXT("Boss_Right_LeftLeg");
				else
					m_strFrameKey = TEXT("Boss_Right_RightLeg");
			}
			else
			{
				if (m_isLeftLeg)
					m_strFrameKey = TEXT("Boss_Direction_NE_LeftLeg");
				else
					m_strFrameKey = TEXT("Boss_Direction_NE_RightLeg");
			}
		}
	}
}

void CBossLowerBody::StopRotationCheck()
{
	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	//_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_vTargetPos;
	vDiff.y = 0.f;
	D3DXVec3Normalize(&vDiff, &vDiff);

	_float3 vMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	vMonsterLook.y = 0.f;
	D3DXVec3Normalize(&vMonsterLook, &vMonsterLook);

	_float dot = D3DXVec3Dot(&vMonsterLook, &vDiff);
	dot = max(-1.f, min(1.f, dot));

	_float3 vCross;
	D3DXVec3Cross(&vCross, &vMonsterLook, &vDiff);

	_float fFov = cosf(D3DXToRadian(45.f));

	_float angle30 = cosf(D3DXToRadian(30.f));
	_float angle60 = cosf(D3DXToRadian(60.f));

	if (dot >= fFov)
	{
		m_strFrameKey = TEXT("Boss_Front_Leg");
	}
	else if (dot <= -fFov)
	{
		m_strFrameKey = TEXT("Boss_Back_Leg");
	}
	else
	{
		if (vCross.y > 0)
		{
			if (dot > 0.2f)
			{
				m_strFrameKey = TEXT("Boss_Leg_Direction_SW");
			}
			else if (dot > 0)
			{
				m_strFrameKey = TEXT("Boss_Left_Leg");
			}
			else
			{
				m_strFrameKey = TEXT("Boss_Leg_Direction_NW");
			}
		}
		else
		{
			if (dot > 0.2f)
			{
				m_strFrameKey = TEXT("Boss_Leg_Direction_SE");
			}
			else if (dot > 0)
			{
				m_strFrameKey = TEXT("Boss_Right_Leg");
			}
			else
			{
				m_strFrameKey = TEXT("Boss_Leg_Direction_NE");
			}
		}
	}
}

void CBossLowerBody::Attack()
{
}

void CBossLowerBody::Move(_float fTimeDelta)
{	
	if (!m_isMoveDir)
	{
		m_bAnimationLock = true;
		m_isMoveDir = true;
		m_vTargetPos = m_pPlayerTransform->Get_State(STATE::POSITION);
		//RotationCheck();
	}

	RotationCheck();
	if (m_isMove)
	{
		//m_fSumDirDurarionTime += fTimeDelta;
		//RotationCheck();
		_wstring tag = m_pAnimationCom->Get_FrameKey();
		m_pAnimationCom->Get_Frame_Desc(tag)->iEnd;
		if (m_pAnimationCom->Get_Frame_Current_Index(tag) == m_pAnimationCom->Get_Frame_Desc(tag)->iEnd)
			return;
	}

	/*if (!m_isMove && m_strLegFrameKey == TEXT("Boss_LeftLeg"))*/
	if (!m_isMove && m_isLeftLeg)
	{
		m_isMove = true;
		//RotationCheck();
		m_pGameInstance->PlaySoundOnce(TEXT("Boss1_Move_0.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		/*m_strFrameKey = TEXT("Boss_LeftLeg");*/
	}
	else if (!m_isMove)
	{
		m_isMove = true;
		//RotationCheck();
		m_pGameInstance->PlaySoundOnce(TEXT("Boss1_Move_1.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		//m_strFrameKey = TEXT("Boss_RightLeg");
	}

	/*_float3 fPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
	_float3 fMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	D3DXVec3Normalize(&fPlayerLook, &fPlayerLook);
	D3DXVec3Normalize(&fMonsterLook, &fMonsterLook);

	_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);*/

	/*m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fTimeDelta);*/

	if (m_fSumDirDurarionTime >= m_fDirDuraionTime)
	{
		m_isMoveDir = false;
		m_fSumDirDurarionTime = 0.f;
		//RotationCheck();
	}

	m_pTransformCom->Chase(m_vTargetPos, fTimeDelta);
}

void CBossLowerBody::Escape(_float fTimeDelta)
{
	if (!m_isMoveDir)
	{
		m_bAnimationLock = true;
		m_isMoveDir = true;
		_float3 dirToPlayer = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
		D3DXVec3Normalize(&dirToPlayer, &dirToPlayer);
		m_vTargetPos = m_pTransformCom->Get_State(STATE::POSITION) - dirToPlayer * 10.f;
	}

	RotationCheck();
	if (m_isMove)
	{
		_wstring tag = m_pAnimationCom->Get_FrameKey();
		m_pAnimationCom->Get_Frame_Desc(tag)->iEnd;
		if (m_pAnimationCom->Get_Frame_Current_Index(tag) == m_pAnimationCom->Get_Frame_Desc(tag)->iEnd)
			return;
	}

	/*if (!m_isMove && m_strLegFrameKey == TEXT("Boss_LeftLeg"))*/
	if (!m_isMove && m_isLeftLeg)
	{
		m_isMove = true;
		m_pGameInstance->PlaySoundOnce(TEXT("Boss1_Move_0.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	}
	else if (!m_isMove)
	{
		m_isMove = true;
		m_pGameInstance->PlaySoundOnce(TEXT("Boss1_Move_1.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	}

	if (m_fSumDirDurarionTime >= m_fDirDuraionTime)
	{
		m_isMoveDir = false;
		m_fSumDirDurarionTime = 0.f;
	}

	m_pTransformCom->Chase(m_vTargetPos, fTimeDelta * 5.f);
}

void CBossLowerBody::Set_BodyLive(_bool isLive)
{
	m_isBodyLive = isLive;
}

void CBossLowerBody::Move()
{
	m_pTransformCom->Get_State(STATE::POSITION);
}

CBossLowerBody* CBossLowerBody::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossLowerBody* pInstance = new CBossLowerBody(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossLowerBody::Clone(void* pArg)
{
	CBossLowerBody* pInstance = new CBossLowerBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BossLowerBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossLowerBody::Free()
{
	__super::Free();

	Safe_Release(m_pCoreTranform);
	Safe_Release(m_pPlayerTransform);
}
