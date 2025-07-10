#include "BossUpperBody.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "BossGrenade.h"
#include "Spawner.h"
#include "Particle_Manager.h"
#include "Bullet_Manager.h"
#include "Effect_Manager.h"
#include "Terrain_Manager.h"

CBossUpperBody::CBossUpperBody(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{

}

CBossUpperBody::CBossUpperBody(const CBossUpperBody& Prototype)
	: CMonster{ Prototype }
{

}

HRESULT CBossUpperBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossUpperBody::Initialize(void* pArg)
{
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));
	m_pCoreTranform = static_cast<CTransform*>(pArg);
	Safe_AddRef(m_pCoreTranform);

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

	// 최초 코어가 생성된다음 그 위치값을 토대로 생성을 해야하는데 지금은 보장이 안된다
	// 순서를 보장하기 위해 코어가 클론이 될때  상,하체를 클론 되게 만들었음
	m_pTransformCom->Set_Scale({ 7.f, 7.f, 1.f });

	/*m_pTransformCom->Set_State(STATE::POSITION, _float3(
		m_pGameInstance->Random(30.f, 50.f),
		0.f,
		m_pGameInstance->Random(30.f, 50.f)));*/
	m_pTransformCom->Set_State(STATE::POSITION, m_pCoreTranform->Get_State(STATE::POSITION));

	// 오프셋 준다음 꾸준히 보여줄떄마다 올려야할거 같은데
	/*_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, {vPos.x, vPos.y + 2.0f, vPos.z});*/

	//m_pTransformCom_Down->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION));

	m_fAttackRange = 15.f;
	m_fAttackCoolTime = 5.f;
	m_fSumAttackCoolTime = 5.f;
	m_uCurBullets = 0;
	m_uMaxBullets = 10;
	m_uCurExplosionBullets = 0;
	m_uMaxExplosionBullets = 3;
	m_vUpOffset = { 0.f, 2.9f, 0.f };

	return S_OK;
}

void CBossUpperBody::Priority_Update(_float fTimeDelta)
{

}

void CBossUpperBody::Update(_float fTimeDelta)
{
	if (m_isDead)
	{
		return;
	}

	if (!m_bAttacking)
		m_fSumAttackCoolTime += fTimeDelta;
	else
		m_fAttackFailTime += fTimeDelta;

	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
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

	if (!m_bAnimationLock)
	{
		if (dot >= fFov)
		{
			m_strFrameKey = TEXT("Boss_Front");
		}
		else if (dot <= -fFov)
		{
			m_strFrameKey = TEXT("Boss_Back");
		}
		else
		{
			if (vCross.y > 0)
			{
				if (dot > 0.2f)
				{
					m_strFrameKey = TEXT("Boss_Direction_SW");
				}
				else if (dot > 0)
				{
					m_strFrameKey = TEXT("Boss_Left");
				}
				else
				{
					m_strFrameKey = TEXT("Boss_Direction_NW");
				}
			}
			else
			{
				if (dot > 0.2f)
				{
					m_strFrameKey = TEXT("Boss_Direction_SE");
				}
				else if (dot > 0)
				{
					m_strFrameKey = TEXT("Boss_Right");
				}
				else
				{
					m_strFrameKey = TEXT("Boss_Direction_NE");
				}
			}
		}
	}
	else
	{
		if (dot >= fFov)
		{
			
		}
		else if (dot <= -fFov)
		{
			
		}
		else
		{
			if (vCross.y > 0)
			{
				if (dot > 0.2f)
				{
					m_strFrameKey = TEXT("Boss_Attack_SW");
				}
			}
			else
			{
				if (dot > 0.2f)
				{
					m_strFrameKey = TEXT("Boss_Attack_SE");
				}
			}
		}
	}

	if (m_fAttackFailTime >= 7.f)						// 공격 상태가 아닌 상태가 어떤 경우던 7초가 넘었다면
	{													// 비정상 상태로 판단하고 초기화 작업
		m_uCurBullets = 0;
		m_bAnimationLock = false;
		m_pAnimationCom->Clear_Animation();
		m_strFrameKey = TEXT("Boss_Front");
		m_fSumAttackCoolTime = 0.f;
		m_eState = BossAttackState::END;
		m_bAttacking = false;
		m_fAttackFailTime = 0.f;
	}

	if (m_fHp <= 0)
	{
		m_pGameInstance->PlaySoundOnce(TEXT("Boss_lost.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		m_bAnimationLock = true;
		m_isDead = true;
		CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Grenade_Explosion"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"),
			m_pTransformCom->Get_State(STATE::POSITION));
	}
	else if (m_bAttacking)							// 어택중이면 계속 어택
	{
		Attack(fTimeDelta, m_eState);
	}
	else if (m_pSightCom->Check_Sight(fTimeDelta))
	{
		_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);

		if ((D3DXVec3Length(&vDiff) <= m_fAttackRange) && (m_fSumAttackCoolTime >= m_fAttackCoolTime))
		{
			// 랜덤값으로 공격 종류를 정하게 할까?
			//m_uTempNum = rand() % 3;
			// 패턴 다 보여주기 위해서 하나씩 올라가도록 임시적 설정

			if (m_uTempNum == 0)
			{
				m_uTempNum = rand() % 2;
				m_eState = static_cast<BossAttackState>(m_uTempNum);
				m_uTempNum = 0;
			}
			else if (m_uTempNum == 1)
			{
				m_eState = BossAttackState::BOOM;
			}
			else
			{
				m_eState = BossAttackState::SUMMON;
			}

			Attack(fTimeDelta, m_eState);
			m_uTempNum++;
			if (m_uTempNum > 2)
				m_uTempNum = 0;
		}
	}
	m_pTransformCom->Set_State(STATE::POSITION, m_pCoreTranform->Get_State(STATE::POSITION));
	//SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);
}

void CBossUpperBody::Late_Update(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);

	if (m_bAnimationLock && m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
	{
		m_strFrameKey = TEXT("Boss_Attack_Front");
	}

	RotateWithParentTransform();				// 이러면 위 아래가 기본 pos에 고정됨
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);

	// 컴퓨터 디스턴스를 구한 값이 게임오브젝트에 1개가 저장되고 그것을 비교하여 정렬하는 구조이기 때문에
	// 현재 위아래로 나눈 구조에서는 맞지않는듯하다
}

HRESULT CBossUpperBody::Render()
{
	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));

	if (FAILED(Begin_RenderTestState()))
		return E_FAIL;

	RotateToPlayer(m_pTransformCom);
	auto iter = m_pTextureComs.find(m_strFrameKey);
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));
	m_pVIBufferCom->Render();


	if (FAILED(End_RenderTestState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossUpperBody::Begin_RenderTestState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CBossUpperBody::End_RenderTestState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CBossUpperBody::RotateWithParentTransform()
{
	_float3 vRootPos = m_pTransformCom->Get_State(STATE::POSITION);

	m_pTransformCom->Set_State(STATE::POSITION, vRootPos + m_vUpOffset);

	m_pTransformCom->Set_State(STATE::RIGHT, m_pTransformCom->Get_State(STATE::RIGHT));
	m_pTransformCom->Set_State(STATE::UP, m_pTransformCom->Get_State(STATE::UP));
	m_pTransformCom->Set_State(STATE::LOOK, m_pTransformCom->Get_State(STATE::LOOK));
}

HRESULT CBossUpperBody::Ready_Animations()
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

	//Boss_Attack_Front
	auto iter = m_pTextureComs.find(TEXT("Boss_Attack_Front"));
	Desc_0.iFrameSpeed = 9;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Attack_Front"), Desc_0);

	//Boss_Attack_SE
	iter = m_pTextureComs.find(TEXT("Boss_Attack_SE"));
	Desc_1.iFrameSpeed = 12;
	Desc_1.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Attack_SE"), Desc_1);

	//Boss_Attack_SW
	iter = m_pTextureComs.find(TEXT("Boss_Attack_SW"));
	Desc_2.iFrameSpeed = 12;
	Desc_2.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Attack_SW"), Desc_2);

	//Boss_Direction_NE
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NE"));
	Desc_3.iFrameSpeed = 5;
	Desc_3.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_NE"), Desc_3);

	//Boss_Direction_NW
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NW"));
	Desc_4.iFrameSpeed = 5;
	Desc_4.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_NW"), Desc_4);

	//Boss_Direction_SE
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SE"));
	Desc_5.iFrameSpeed = 5;
	Desc_5.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_SE"), Desc_5);

	//Boss_Direction_SW
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SW"));
	Desc_6.iFrameSpeed = 5;
	Desc_6.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Direction_SW"), Desc_6);

	//Boss_Front
	iter = m_pTextureComs.find(TEXT("Boss_Front"));
	Desc_7.iFrameSpeed = 5;
	Desc_7.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Front"), Desc_7);

	//Boss_Back
	iter = m_pTextureComs.find(TEXT("Boss_Back"));
	Desc_8.iFrameSpeed = 5;
	Desc_8.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Back"), Desc_8);

	//Boss_Left
	iter = m_pTextureComs.find(TEXT("Boss_Left"));
	Desc_9.iFrameSpeed = 5;
	Desc_9.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Left"), Desc_9);

	//Boss_Right
	iter = m_pTextureComs.find(TEXT("Boss_Right"));
	Desc_10.iFrameSpeed = 5;
	Desc_10.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_Right"), Desc_10);

	return S_OK;
}

void CBossUpperBody::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (m_isDead || m_bDying)
		return;

	if (eColType == COLLISION::SPHERE)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
		{
			if ((m_fHp -= (pBullet->Get_Damage())) > 0)
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

void CBossUpperBody::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider)
{
	if (m_isDead || m_bDying)
		return;

	if (eColType == COLLISION::RAY)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
		{
			if ((m_fHp -= (pBullet->Get_Damage())) > 0)
			{
				m_pGameInstance->PlaySoundOnce(TEXT("Boss1_hit1.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
			}
			else
			{
				if (pCollider == m_pBoxColliderHead)
				{
					m_isHead = true;
				}
			}
		}
	}
}

const COLLISION_DESC& CBossUpperBody::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	else if (eColType == COLLISION::BOX)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

HRESULT CBossUpperBody::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Sight"),
		TEXT("Com_Sight"), reinterpret_cast<CComponent**>(&m_pSightCom), &SightDesc)))
		return E_FAIL;

	/* Com_BoxCollider */
	CBoxCollider::BOXCOLLIDER_DESC Desc;
	Desc.vPosition = { 0.f, -0.3f, 0.f };
	Desc.fScaleX = 1.5f;
	Desc.fScaleZ = 1.5f;
	Desc.fScaleY = 2.0f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

void CBossUpperBody::Attack(_float fTimeDelta, BossAttackState state)
{
	//m_bJump = true;
	if (!m_bAttacking)
	{
		m_bAttacking = true;						// 공격중 상태로 전환
		m_vAttackPos = m_pPlayerTransform->Get_State(STATE::POSITION);
	}
	m_fAttackFailTime += fTimeDelta;

	if (state == BossAttackState::MASS)			// 공격모드가 난사면
	{
		if (!m_bAnimationLock)					// 애니메이션 락이 걸렸나 확인(처음 공격하는건지 체크)
		{										// 첫 공격이니까 프레임 키 공격으로 바꾸고 애니메이션 락을 건다
			m_strFrameKey = TEXT("Boss_Attack_Front");
			m_bAnimationLock = true;
		}
		m_fSumLaunchCoolTime += fTimeDelta;		// 난사 쿨타임 증가

		if (m_fLaunchCoolTime <= m_fSumLaunchCoolTime)			// 누적 시간이 정해둔 쿨타임보다 길면 공격
		{
			m_pGameInstance->PlaySoundOnce(TEXT("Boss1_SMG.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);

			_float3 vMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
			//vMonsterPos.y += m_vUpOffset.y;
			_float3 vDir = m_vAttackPos - vMonsterPos;									// 나중에 m_vAttackPos를 약간 랜덤하게 위치 지정
			_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
			D3DXVec3Normalize(&vDir, &vDir);

			CBullet::BULLET_DESC Desc;
			Desc.vDir = vDir;
			Desc.vPos = vPos;
			//Desc.vPos = {vPos.x ,vPos.y += m_vUpOffset.y, vPos.z };
			Desc.fBulletSpeed = 10.f;
			Desc.vBulletScale = { 0.005f, 0.005f, 0.2f };
			//Desc.vBulletScale = { 0.2f, 0.2f, 0.1f };
			Desc.isPlayerBullet = false;
			Desc.fDuration = 7.f;
			CBullet_Manager::GetInstance()->Create_Bullet(TEXT("Bullet"), Desc, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"));
			//m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"), &Desc);
			m_fSumLaunchCoolTime = 0.f;								// 난사 한발 사용했으므로 누적 시간 초기화
			//m_fSumAttackCoolTime = 0.f;
			m_uCurBullets++;										// 현재 사용한 총알 수 증가
			//OutputDebugStringA("디버그 메시지: 총알 발사 완료\n");
		}

		if (m_uCurBullets >= m_uMaxBullets)					// 정해둔 총알을 다 사용했으면
		{
			m_uCurBullets = 0;								// 총알 수 0으로 초기화
			m_bAnimationLock = false;						// 애니메이션 락 해제
			m_pAnimationCom->Clear_Animation();			// 진행중 애니메이션 정지
			m_strFrameKey = TEXT("Boss_Front");			// idle 키로 전환
			m_eState = BossAttackState::END;				// 공격 상태 종료로 변환
			m_bAttacking = false;							// 공격 진행중 상태 바꿈
			m_fSumAttackCoolTime = 0.f;						// 다음 공격 시간을 위한 누적시간 초기화
			m_fAttackFailTime = 0.f;						// 비정상 상태 종료를 위한 누적시간
		}
	}
	else if (state == BossAttackState::CHASE_MASS)
	{
		if (!m_bAnimationLock)					// 애니메이션 락이 걸렸나 확인(처음 공격하는건지 체크)
		{										// 첫 공격이니까 프레임 키 공격으로 바꾸고 애니메이션 락을 건다
			m_strFrameKey = TEXT("Boss_Attack_Front");
			m_bAnimationLock = true;
		}
		m_fSumLaunchCoolTime += fTimeDelta;		// 난사 쿨타임 증가

		if (m_fLaunchCoolTime <= m_fSumLaunchCoolTime)			// 누적 시간이 정해둔 쿨타임보다 길면 공격
		{
			m_pGameInstance->PlaySoundOnce(TEXT("Boss1_SMG.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);

			_float3 vMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
			//vMonsterPos.y += m_vUpOffset.y;

			_float3 vDir = m_pPlayerTransform->Get_State(STATE::POSITION) - vMonsterPos;									// 나중에 m_vAttackPos를 약간 랜덤하게 위치 지정
			_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
			D3DXVec3Normalize(&vDir, &vDir);

			CBullet::BULLET_DESC Desc;
			Desc.vDir = vDir;
			Desc.vPos = vPos;
			//Desc.vPos = { vPos.x ,vPos.y += m_vUpOffset.y, vPos.z };
			Desc.fBulletSpeed = 10.f;
			Desc.vBulletScale = { 0.005f, 0.005f, 0.2f };
			//Desc.vBulletScale = { 0.2f, 0.2f, 0.1f };
			Desc.isPlayerBullet = false;
			Desc.fDuration = 7.f;
			CBullet_Manager::GetInstance()->Create_Bullet(TEXT("Bullet"), Desc, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"));
			//m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"), &Desc);
			m_fSumLaunchCoolTime = 0.f;								// 난사 한발 사용했으므로 누적 시간 초기화
			//m_fSumAttackCoolTime = 0.f;
			m_uCurBullets++;										// 현재 사용한 총알 수 증가
			//OutputDebugStringA("디버그 메시지: 총알 발사 완료\n");
		}

		if (m_uCurBullets >= m_uMaxBullets)					// 정해둔 총알을 다 사용했으면
		{
			m_uCurBullets = 0;								// 총알 수 0으로 초기화
			m_bAnimationLock = false;						// 애니메이션 락 해제
			m_pAnimationCom->Clear_Animation();			// 진행중 애니메이션 정지
			m_strFrameKey = TEXT("Boss_Front");			// idle 키로 전환
			m_eState = BossAttackState::END;				// 공격 상태 종료로 변환
			m_bAttacking = false;							// 공격 진행중 상태 바꿈
			m_fSumAttackCoolTime = 0.f;						// 다음 공격 시간을 위한 누적시간 초기화
			m_fAttackFailTime = 0.f;						// 비정상 상태 종료를 위한 누적시간
		}
	}
	else if (state == BossAttackState::BOOM)
	{
		// 다른 공격 방식
		if (!m_bAnimationLock)					// 애니메이션 락이 걸렸나 확인(처음 공격하는건지 체크)
		{										// 첫 공격이니까 프레임 키 공격으로 바꾸고 애니메이션 락을 건다
			m_strFrameKey = TEXT("Boss_Attack_Front");
			m_bAnimationLock = true;
		}
		m_fSumLaunchCoolTime += fTimeDelta;		// 난사 쿨타임 증가

		if (m_fLaunchCoolTime <= m_fSumLaunchCoolTime)			// 누적 시간이 정해둔 쿨타임보다 길면 공격
		{
			m_pGameInstance->PlaySoundOnce(TEXT("Boss1_Grenade.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);

			_float3 vMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
			vMonsterPos.y += m_vUpOffset.y;
			_float3 vDir = m_vAttackPos - vMonsterPos;
			_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
			D3DXVec3Normalize(&vDir, &vDir);

			CBossGrenade::BULLET_DESC Desc;
			Desc.vDir = vDir;
			//Desc.vOffSet = m_vUpOffset;
			Desc.fAngle = 45.f;
			Desc.vStartPos = vMonsterPos;
			Desc.vPlayerPos = m_pPlayerTransform->Get_State(STATE::POSITION);
			Desc.isPlayerBullet = false;
			Desc.fBulletSpeed = 10.f;
			Desc.vBulletScale = { 0.1f, 0.1f, 0.1f };
			Desc.pPlayerTransform = m_pPlayerTransform;
			Desc.fDuration = 7.f;
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_BossGrenade"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"), &Desc);	
			m_fSumLaunchCoolTime = 0.f;								// 유탄 한발 사용했으므로 누적 시간 초기화
			m_uCurExplosionBullets++;										// 현재 사용한 총알 수 증가

			//OutputDebugStringA("디버그 메시지: 유탄 발사 완료\n");
		}

		if (m_uCurBullets >= m_uMaxBullets || m_uCurExplosionBullets >= m_uMaxExplosionBullets)					// 정해둔 총알을 다 사용했으면
		{
			m_uCurBullets = 0;								// 총알 수 0으로 초기화
			m_uCurExplosionBullets = 0;						// 유탄 수 0으로 초기화
			m_bAnimationLock = false;						// 애니메이션 락 해제
			m_pAnimationCom->Clear_Animation();			// 진행중 애니메이션 정지
			m_strFrameKey = TEXT("Boss_Front");			// idle 키로 전환
			m_eState = BossAttackState::END;				// 공격 상태 종료로 변환
			m_bAttacking = false;							// 공격 진행중 상태 바꿈
			m_fSumAttackCoolTime = 0.f;						// 다음 공격 시간을 위한 누적시간 초기화
			m_fAttackFailTime = 0.f;						// 비정상 상태 종료를 위한 누적시간
		}
	}
	else if (m_eState == BossAttackState::SUMMON)
	{
		SummonMonster();

		m_uCurBullets = 0;								// 총알 수 0으로 초기화
		m_uCurExplosionBullets = 0;						// 유탄 수 0으로 초기화
		m_bAnimationLock = false;						// 애니메이션 락 해제
		m_pAnimationCom->Clear_Animation();			// 진행중 애니메이션 정지
		m_strFrameKey = TEXT("Boss_Front");			// idle 키로 전환
		m_eState = BossAttackState::END;				// 공격 상태 종료로 변환
		m_bAttacking = false;							// 공격 진행중 상태 바꿈
		m_fSumAttackCoolTime = 0.f;						// 다음 공격 시간을 위한 누적시간 초기화
		m_fAttackFailTime = 0.f;						// 비정상 상태 종료를 위한 누적시간
	}
}

void CBossUpperBody::SummonMonster()				// 추후 필요하면 인덱스 받을 수 있도록 변경하기
{
	for (size_t i = 0; i < 5; i++)
	{
		CGameObject* pClone = nullptr;
		pClone = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY),
			m_MonsterKeys[0], m_pTransformCom->Get_State(STATE::POSITION)));

		m_pGameInstance->Add_Clone_ToLayer(pClone, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster"));

		CTerrain_Manager::GetInstance()->Add_LandObject_One(pClone);
	}
}

CBossUpperBody* CBossUpperBody::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossUpperBody* pInstance = new CBossUpperBody(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossUpperBody::Clone(void* pArg)
{
	CBossUpperBody* pInstance = new CBossUpperBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BossUpperBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossUpperBody::Free()
{
	__super::Free();
	Safe_Release(m_pCoreTranform);
}
