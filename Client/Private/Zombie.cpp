#include "Zombie.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "Particle_Manager.h"
#include "Effect_Manager.h"

CZombie::CZombie(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{

}

CZombie::CZombie(const CZombie& Prototype)
	: CMonster(Prototype)
{

}

HRESULT CZombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CZombie::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	m_fDamage = 30.f;
	m_fAttackRange = 3.5f;
	m_fAttackCoolTime = 4.f;
	m_fChaseRange = 10.f;
	m_fMaxRange = 20.f;
	m_fRandomMoveTime = 3.f;
	m_fSumRandomMoveTime = 0.f;
	m_fCurHp = 40.f;

	// 스포너 관련
	//if (pArg != nullptr)				// 스포너의 위치를 받아온다
	//{
	//	m_Desc = static_cast<CZombie::ZOMBIE_DESC*>(pArg);
	//	m_vPos = m_Desc->vPos;
	//	m_isAwake = m_Desc->isAwake;
	//}
	
	/* 스포터 관련
	if (m_Desc->isTarget)
	{
		m_isTarget = m_Desc->isTarget;
		m_isLeft = m_Desc->isLeft;
		m_uPosLen = m_Desc->TargetPos.size();
		for (int i = 0; i < m_uPosLen; i++)
		{
			m_vTargetPos.push_back(m_Desc->TargetPos[i]);
		}
	}*/

	if (m_pPlayerTransform == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	// m_pTransformCom 세팅
	m_pTransformCom->Set_State(STATE::RIGHT, m_pObjectDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(STATE::UP, m_pObjectDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(STATE::LOOK, m_pObjectDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(STATE::POSITION, m_pObjectDesc.matWorld.m[3]);
	m_pTransformCom->Set_Scale(_float3{ 1.75f, 1.75f, 1.f});
	
	// 게임 플레이에서만 동작하게
	if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::GAMEPLAY))
	{
		if (m_pObjectDesc.iObjectID >= 12 || m_pObjectDesc.iObjectID <= 51)
		{
			//m_isTarget = true;
			if (m_pTransformCom->Get_State(STATE::POSITION).z >= 45.f)
			{
				m_isLeft = true;
				m_vPos = { 30.5f, 1.f, 58.5f };
			}
			else
			{
				m_isLeft = false;
				m_vPos = { 30.5f, 1.f, 33.5f };
			}
		}
		else
		{
			m_isTarget = false;
			// 일어나는 연출 필요할 경우
			//m_isAwake = true;
		}
	}

	/* 스포너 관련
	if (m_vPos != nullptr)
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(
			m_vPos.x + m_pGameInstance->Random(0.f, 2.f),
			0.f,
			m_vPos.z + m_pGameInstance->Random(0.f, 2.f)));
	}
	else
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(
			m_pGameInstance->Random(0.f, 2.f),
			0.f,
			m_pGameInstance->Random(0.f, 2.f)));
	}*/

	//m_AttackfCoolTime = 1.f;
	//SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);

	/* 스포너 관련
	if (m_isAwake)
	{
		m_strFrameKey = TEXT("Zombie_Awake");
		m_bAnimationLock = true;
		m_pGameInstance->PlaySoundOnce(TEXT("zombie_recog_1.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	}*/

	return S_OK;
}

void CZombie::Priority_Update(_float fTimeDelta)
{
}

void CZombie::Update(_float fTimeDelta)
{
	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	_float fDist = D3DXVec3Length(&vDiff);

	if (m_bDying && !m_bAnimationLock && fDist >= m_fMaxRange)
	{
		m_isDead = true;
		return;
	}

	if (m_bDying)
	{
		SetUp_OnTerrain(m_pTransformCom, 0.9f, &m_bJump);
		return;
	}

	m_bRideCube = true;
	m_fSumAttackCoolTime += fTimeDelta;
	m_fSumMoveCoolTime += fTimeDelta;

	MoveAnimationCheck();

	if (m_isAwake)
	{
		__super::Jump(fTimeDelta);
		SetUp_OnTerrain(m_pTransformCom, 1.f, &m_bJump);
		return;
	}
	else if (m_isTarget)
	{
		if (m_fSumMoveCoolTime >= m_fMoveCoolTime)
		{
			TargetMove(fTimeDelta, m_vPos);
			m_isMove = true;
			m_fSumMoveCoolTime = 0.f;
		}

		m_fSightFailTime = 0.f;
	}

	if (m_fCurHp <= 0 && m_bDying == false)
	{
		if (m_isHead)
			m_strFrameKey = TEXT("Zombie_Die_HeadShot");
		else
			m_strFrameKey = TEXT("Zombie_Die_Default");


		m_pBoxColliderCom->Set_Scale({ 1.2f, 0.20f, 1.2f });
		m_pBoxColliderCom->Set_Position({ 0.f, -0.6f, 0.f });
		m_pBoxColliderHead->Set_Scale({ 0.f, 0.f, 0.f });

		m_bAnimationLock = true;
		m_bDying = true;
		m_pGameInstance->PlaySoundOnce(TEXT("zombie_dead_1.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	}
	/*else if (m_isAwake)
	{
		__super::Jump(fTimeDelta);
		SetUp_OnTerrain(m_pTransformCom, 0.45f, &m_bJump);
		return;
	}*/
	else if (m_pSightCom->Check_Sight(fTimeDelta) && !m_bAnimationLock)
	{
		m_isTarget = false;

		if (!m_bFirstEncounter)
		{
			m_bFirstEncounter = true;
			m_pGameInstance->PlaySoundOnce(TEXT("Zombie_Contact.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		}

		if (m_fSumAttackCoolTime >= m_fAttackCoolTime)
		{
			_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
			if (D3DXVec3Length(&vDiff) <= m_fAttackRange)
			{
				m_pGameInstance->PlaySoundOnce(TEXT("zombie_swing_2.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
				Attack();
				m_fSumRandomMoveTime = 0.f;
				m_fSightFailTime = 0.f;
			}
		}

		if (m_fSumMoveCoolTime >= m_fMoveCoolTime)
		{
			_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
			if (D3DXVec3Length(&vDiff) <= m_fChaseRange && (D3DXVec3Length(&vDiff) >= m_fSafeDistance) && D3DXVec3Length(&vDiff) >= m_fAttackRange - 1.f)
			{
				Move(fTimeDelta);
				m_isMove = true;
				m_fSumMoveCoolTime = 0.f;
				m_fSumRandomMoveTime = 0.f;
				m_fSightFailTime = 0.f;
			}
		}
	}

	__super::Jump(fTimeDelta);
	SetUp_OnTerrain(m_pTransformCom, 1.f, &m_bJump);
}

void CZombie::Late_Update(_float fTimeDelta)
{
	if (m_isMove || m_bAnimationLock || m_bDying)
		m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);

	if (m_bAnimationLock && m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
	{
		if (m_bDying)
		{
			//m_isDead = true;
			if (m_isHead)
				m_strFrameKey = TEXT("Zombie_Die_HeadShot_Idle");
			else
				m_strFrameKey = TEXT("Zombie_Die_Idle");
			m_bAnimationLock = false;
			return;
		}
		else
		{
			if (m_isAwake)
				m_isAwake = false;
			m_bAnimationLock = false;
			m_strFrameKey = TEXT("Zombie_Front");
		}
	}

	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CZombie::Render()
{
	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));
	m_pBoxColliderHead->Render(m_pTransformCom->Get_State(STATE::POSITION));

	RotateToPlayer(m_pTransformCom);

	auto iter = m_pTextureComs.find(m_strFrameKey);
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));

	if (FAILED(Begin_RenderTestState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderTestState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Ready_Animations()
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

	auto iter = m_pTextureComs.find(TEXT("Zombie_Attack"));
	Desc_0.iFrameSpeed = 12;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Attack"), Desc_0);

	//Zombie_Die_Default
	iter = m_pTextureComs.find(TEXT("Zombie_Die_Default"));
	Desc_1.iFrameSpeed = 8;
	Desc_1.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Die_Default"), Desc_1);

	//Zombie_Direction_NE
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_NE"));
	Desc_2.iFrameSpeed = 6;
	Desc_2.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_NE"), Desc_2);

	//Zombie_Direction_NW
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_NW"));
	Desc_3.iFrameSpeed = 6;
	Desc_3.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_NW"), Desc_3);

	//Zombie_Direction_SE
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_SE"));
	Desc_4.iFrameSpeed = 6;
	Desc_4.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_SE"), Desc_4);

	//Zombie_Direction_SW
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_SW"));
	Desc_5.iFrameSpeed = 6;
	Desc_5.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_SW"), Desc_5);

	//Zombie_Front
	iter = m_pTextureComs.find(TEXT("Zombie_Front"));
	Desc_6.iFrameSpeed = 6;
	Desc_6.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Front"), Desc_6);

	//Zombie_Back
	iter = m_pTextureComs.find(TEXT("Zombie_Back"));
	Desc_7.iFrameSpeed = 6;
	Desc_7.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Back"), Desc_7);

	//Zombie_Left
	iter = m_pTextureComs.find(TEXT("Zombie_Left"));
	Desc_8.iFrameSpeed = 6;
	Desc_8.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Left"), Desc_8);

	//Zombie_Right
	iter = m_pTextureComs.find(TEXT("Zombie_Right"));
	Desc_9.iFrameSpeed = 6;
	Desc_9.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Right"), Desc_9);

	//Zombie_Die_Explosion
	iter = m_pTextureComs.find(TEXT("Zombie_Die_Explosion"));
	Desc_10.iFrameSpeed = 60;
	Desc_10.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Die_Explosion"), Desc_10);

	//Zombie_Die_Idle
	iter = m_pTextureComs.find(TEXT("Zombie_Die_Idle"));
	Desc_11.iFrameSpeed = 20;
	Desc_11.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Die_Idle"), Desc_11);

	//Zombie_Die_HeadShot
	iter = m_pTextureComs.find(TEXT("Zombie_Die_HeadShot"));
	Desc_12.iFrameSpeed = 8;
	Desc_12.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Die_HeadShot"), Desc_12);

	//Zombie_Die_HeadShot_Idle
	iter = m_pTextureComs.find(TEXT("Zombie_Die_HeadShot_Idle"));
	Desc_13.iFrameSpeed = 20;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Die_HeadShot_Idle"), Desc_13);

	//Zombie_Awake
	iter = m_pTextureComs.find(TEXT("Zombie_Awake"));
	Desc_14.iFrameSpeed = 40;
	Desc_14.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Awake"), Desc_14);

	return S_OK;
}

HRESULT CZombie::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
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
	CSight::SIGHT_DESC		SightDesc{ m_fChaseRange, D3DXToRadian(90.0f), m_pPlayerTransform, m_pTransformCom, true };
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Sight"),
		TEXT("Com_Sight"), reinterpret_cast<CComponent**>(&m_pSightCom), &SightDesc)))
		return E_FAIL;

	/* Com_BoxCollider */
	CBoxCollider::BOXCOLLIDER_DESC Desc;
	Desc.vPosition = { 0.f, 0.10f, 0.f };
	Desc.fScaleX = 0.6f;
	Desc.fScaleY = 1.0f;
	Desc.fScaleZ = 0.6f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	Desc.vPosition = { 0.f, 0.7f, 0.f };
	Desc.fScaleX = 0.25f;
	Desc.fScaleY = 0.28f;
	Desc.fScaleZ = 0.25f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider_Head"), reinterpret_cast<CComponent**>(&m_pBoxColliderHead), &Desc)))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 50);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);



	return S_OK;
}

HRESULT CZombie::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CZombie::Begin_RenderTestState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CZombie::End_RenderTestState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CZombie::Attack()
{
	//m_bAttackStarted = true;
	//m_bAttackStarted = true;
	//m_strFrameKey = TEXT("Zombie_Die_Explosion");
	m_fSumAttackCoolTime = 0.f;
	m_strFrameKey = TEXT("Zombie_Attack");
	m_pGameInstance->PlaySoundOnce(TEXT("zombie_swing_2.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	/*auto iter = m_Frames.find(m_strFrameKey);
	m_pAnimationCom->Set_Animation(&iter->second);*/
	m_bAnimationLock = true;
	//m_bFrameBlock = true;
	_float3 vDir = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	//_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_float3 vPos = m_pPlayerTransform->Get_State(STATE::POSITION);
	D3DXVec3Normalize(&vDir, &vDir);

	/*CBullet::BULLET_DESC Desc;
	Desc.vDir = vDir;
	Desc.vPos = vPos;*/
	GAMEOBJECT_DESC DescTemp;
	DescTemp = m_pObjectDesc;
	DescTemp.matWorld = *m_pPlayerTransform->Get_WorldMatrixPtr();

	m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Melee_Attack"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Melee_Attack"), &DescTemp);
	//m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Bullet"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Zombie_Bullet"), &Desc);
}

void CZombie::Move(_float fTimeDelta)
{
	_float3 fPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
	_float3 fMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	_float3 vDirection = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	vDirection.y = 0.f;

	D3DXVec3Normalize(&fPlayerLook, &fPlayerLook);
	D3DXVec3Normalize(&fMonsterLook, &fMonsterLook);
	D3DXVec3Normalize(&vDirection, &vDirection);

	m_vNextDir = vDirection;

	_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, fRadian);

	if(m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::GAMEPLAY))
		m_pTransformCom->Go_Direction(vDirection, fTimeDelta * 1.5f);
	else
		m_pTransformCom->Go_Direction(vDirection, fTimeDelta);

	m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));


	//m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fMoveTime);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}

//void CZombie::TargetMove(_float fTimeDelta, _float3 vPos)		// 정해져 있는 장소로 이동 시키는 함수
//{
//	//_float3 vDir = vPos - fMonsterPos;
//	//D3DXVec3Normalize(&vDir, &vDir);
//	m_pTransformCom->Chase(vPos, fTimeDelta);
//	_float3 fMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
//	_float3 vDir = vPos - fMonsterPos;
//
//	_float fDistance = D3DXVec3Length(&vDir);
//	if (fDistance < 0.5f)
//		m_uIdx++;
//}

void CZombie::TargetMove(_float fTimeDelta, _float3 vPos)		// 정해져 있는 장소로 이동 시키는 함수
{
	if (m_isArrive)
	{
		if (m_isLeft)
		{
			//0.5
			m_pTransformCom->Turn(_float3(0.f, -1.f, 0.f), fTimeDelta * 0.8f);
		}
		else
		{
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * 0.8f);
		}
		
		_float3 vLook = m_pTransformCom->Get_State(STATE::LOOK);

		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);
		m_pTransformCom->Set_State(STATE::LOOK, vLook);
		m_vNextDir = vLook;

		m_fSumTime += fTimeDelta;
		m_pTransformCom->Go_Straight(fTimeDelta * 1.5f);

		if (m_fSumTime >= 2.f)
		{
			m_isTarget = false;
		}

		return;
	}

	_float3 fMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
	_float3 vDir = vPos - fMonsterPos;
	
	_float fDistance = D3DXVec3Length(&vDir);

	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->Set_State(STATE::LOOK, vDir);
	m_pTransformCom->Go_Straight(fTimeDelta * 5.f);

	if (fDistance <= 1.5f)
	{
		m_isArrive = true;
	}
}

void CZombie::Move()
{
	//m_pTransformCom->R
	m_pTransformCom->Get_State(STATE::POSITION);
}

void CZombie::MoveAnimationCheck()
{
	/*_float3 vMonsterLook = {};
	if (isRandom)
	{
		vMonsterLook = m_vNextDir;
	}
	else
	{
		vMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	}*/
	//_float3 vMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	_float3 vMonsterLook = m_vNextDir;
	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);

	_float fDist = D3DXVec3Length(&vDiff);

	vDiff.y = 0.f;
	D3DXVec3Normalize(&vDiff, &vDiff);

	//_float3 vMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
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
			m_strFrameKey = TEXT("Zombie_Front");
		}
		else if (dot <= -fFov)
		{
			m_strFrameKey = TEXT("Zombie_Back");
		}
		else
		{
			if (vCross.y > 0)
			{
				if (dot > 0)
					m_strFrameKey = TEXT("Zombie_Direction_SW");
				else
					m_strFrameKey = TEXT("Zombie_Direction_NW");
			}
			else
			{
				if (dot > 0)
					m_strFrameKey = TEXT("Zombie_Direction_SE");
				else
					m_strFrameKey = TEXT("Zombie_Direction_NE");
			}
		}
		m_isMove = false;
	}
}

void CZombie::Set_TargetMove(_float bTarget)
{
	m_isTarget = true;
}

void CZombie::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (m_isDead || m_bDying)
		return;
}

void CZombie::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos)
{
	if (eColType == COLLISION::RAY)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
		{
			if (pCollider == m_pBoxColliderHead)
			{
				m_fCurHp -= (pBullet->Get_Damage() * 2.f);
				if (m_fCurHp <= 0)
					m_isHead = true;
			}
			else
			{
				m_fCurHp -= pBullet->Get_Damage();
			}

			if (m_fCurHp > 0)
				m_pGameInstance->PlaySoundOnce(TEXT("zombie_hit_1.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);

			CParticle_Manager::GetInstance()->Create_Particle(TEXT("Particle_Blood"), m_pObjectDesc.iLayerLevel,
				TEXT("Layer_Particle"), vPos);
		}
	}
}

const COLLISION_DESC& CZombie::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;

	else if (eColType == COLLISION::BOX)
	{
		Desc.pCollider = m_pBoxColliderCom;
		Desc.pColliderSecond = m_pBoxColliderHead;
	}

	return Desc;
}

CZombie* CZombie::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CZombie* pInstance = new CZombie(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CZombie::Clone(void* pArg)
{
	CZombie* pInstance = new CZombie(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Zombie");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZombie::Free()
{
	//m_pRoot->ReleaseSubtree();
	__super::Free();
	Safe_Release(m_pPlayerTransform);
}
