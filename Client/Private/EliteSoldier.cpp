#include "EliteSoldier.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "BehaviorNode.h"
#include "Particle_Manager.h"
#include "Bullet_Manager.h"

CEliteSoldier::CEliteSoldier(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{

}

CEliteSoldier::CEliteSoldier(const CEliteSoldier& Prototype)
	: CMonster(Prototype)
{

}

HRESULT CEliteSoldier::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEliteSoldier::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));

	//if (pArg != nullptr)				// 스포너의 위치를 받아온다
	//{
	//	m_vPos = static_cast<_float3*>(pArg);
	//}

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

	/*if (m_vPos != nullptr)
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(
			m_vPos->x + m_pGameInstance->Random(0.f, 2.f),
			0.f,
			m_vPos->z + m_pGameInstance->Random(0.f, 2.f)));
	}
	else
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(
			m_pGameInstance->Random(0.f, 2.f),
			0.f,
			m_pGameInstance->Random(0.f, 2.f)));
	}*/

	m_fAttackRange = 5.f;
	m_fChaseRange = 15.f;
	m_fMaxRange = 10.f;

	return S_OK;
}

void CEliteSoldier::Priority_Update(_float fTimeDelta)
{

}

void CEliteSoldier::Update(_float fTimeDelta)
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
		SetUp_OnTerrain(m_pTransformCom, 0.55f, &m_bJump);
		return;
	}

	__super::Jump(fTimeDelta);

	m_fSumAttackCoolTime += fTimeDelta;
	m_fSumMoveCoolTime += fTimeDelta;

	//_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
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
			m_strFrameKey = TEXT("EliteSoldier_Front");
		}
		else if (dot <= -fFov)
		{
			m_strFrameKey = TEXT("EliteSoldier_Back");
		}
		else
		{
			if (vCross.y > 0)
			{
				if (dot > 0)
					m_strFrameKey = TEXT("EliteSoldier_Direction_SW");
				else
					m_strFrameKey = TEXT("EliteSoldier_Direction_NW");
			}
			else
			{
				if (dot > 0)
					m_strFrameKey = TEXT("EliteSoldier_Direction_SE");
				else
					m_strFrameKey = TEXT("EliteSoldier_Direction_NE");
			}
		}
		m_isMove = false;
	}

	if (m_fHp <= 0)
	{
		if (m_isHead)
			m_strFrameKey = TEXT("EliteSoldier_Die_HeadShot");
		else
			m_strFrameKey = TEXT("EliteSoldier_Die_Default");
		m_bAnimationLock = true;
		m_bDying = true;
		m_pGameInstance->PlaySoundOnce(TEXT("EliteSoldier_Die.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	}
	else if (m_pSightCom->Check_Sight(fTimeDelta) && !m_bAnimationLock)
	{
		if (!m_bFirstEncounter)
		{
			m_bFirstEncounter = true;
			m_pGameInstance->PlaySoundOnce(TEXT("EliteSoldier_Contact.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
		}

		if (m_fSumAttackCoolTime >= m_fAttackCoolTime)
		{
			_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
			if (D3DXVec3Length(&vDiff) <= m_fAttackRange)
			{
				m_uTempNum = rand() % static_cast<int>(EliteSoldierState::END);
				m_eState = static_cast<EliteSoldierState>(m_uTempNum);

				//if (m_eState == EliteSoldierState::END)
				//{
				//	// 랜덤으로 하나 골라지게 함
				//	//m_eState = EliteSoldierState::ONESHOT;
				//	m_eState = EliteSoldierState::MASS;
				//}

				Attack(m_eState);
			}
		}

		if (m_fSumMoveCoolTime >= m_fMoveCoolTime)
		{
			_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
			if (D3DXVec3Length(&vDiff) <= m_fChaseRange && D3DXVec3Length(&vDiff) >= m_fAttackRange - 1.f)
			{
				Move(fTimeDelta);
				m_isMove = true;
				m_fSumMoveCoolTime = 0.f;
			}
		}
	}

	SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);
}

void CEliteSoldier::Late_Update(_float fTimeDelta)
{
	if (m_isMove || m_bAnimationLock || m_bDying)
		m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);

	if (m_bAnimationLock && m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
	{
		if (m_bDying)
		{
			if (m_isHead)
				m_strFrameKey = TEXT("EliteSoldier_Die_HeadShot_Idle");
			else
				m_strFrameKey = TEXT("EliteSoldier_Die_Idle");
			m_bAnimationLock = false;
			return;
		}
		else
		{
			m_bAnimationLock = false;
			m_strFrameKey = TEXT("EliteSoldier_Front");
		}
	}

	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CEliteSoldier::Render()
{
	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));
	m_pBoxColliderHead->Render(m_pTransformCom->Get_State(STATE::POSITION));
	RotateToPlayer(m_pTransformCom);

	auto iter = m_pTextureComs.find(m_strFrameKey);
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEliteSoldier::Ready_Animations()
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

	//EliteSoldier_Attack
	auto iter = m_pTextureComs.find(TEXT("EliteSoldier_Attack_Front"));
	Desc_0.iFrameSpeed = 5;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Attack_Front"), Desc_0);

	//EliteSoldier_Attack_Seat
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Attack_Seat"));
	Desc_1.iFrameSpeed = 7;
	Desc_1.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Attack_Seat"), Desc_1);

	//EliteSoldier_Die_Default
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Die_Default"));
	Desc_2.iFrameSpeed = 10;
	Desc_2.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Die_Default"), Desc_2);

	//EliteSoldier_Direction_NE
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Direction_NE"));
	Desc_3.iFrameSpeed = 4;
	Desc_3.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Direction_NE"), Desc_3);

	//EliteSoldier_Direction_NW
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Direction_NW"));
	Desc_4.iFrameSpeed = 4;
	Desc_4.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Direction_NW"), Desc_4);

	//EliteSoldier_Direction_SE
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Direction_SE"));
	Desc_5.iFrameSpeed = 4;
	Desc_5.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Direction_SE"), Desc_5);

	//EliteSoldier_Direction_SW
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Direction_SW"));
	Desc_6.iFrameSpeed = 4;
	Desc_6.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Direction_SW"), Desc_6);

	//EliteSoldier_Front
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Front"));
	Desc_7.iFrameSpeed = 4;
	Desc_7.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Front"), Desc_7);

	//EliteSoldier_Back
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Back"));
	Desc_8.iFrameSpeed = 4;
	Desc_8.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Back"), Desc_8);

	//EliteSoldier_Left
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Left"));
	Desc_9.iFrameSpeed = 4;
	Desc_9.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Left"), Desc_9);

	//EliteSoldier_Right
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Right"));
	Desc_10.iFrameSpeed = 4;
	Desc_10.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Right"), Desc_10);

	//EliteSoldier_Right
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Die_Idle"));
	Desc_11.iFrameSpeed = 4;
	Desc_11.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Die_Idle"), Desc_11);

	//EliteSoldier_Right
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Die_HeadShot"));
	Desc_12.iFrameSpeed = 10;
	Desc_12.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Die_HeadShot"), Desc_12);

	//EliteSoldier_Right
	iter = m_pTextureComs.find(TEXT("EliteSoldier_Die_HeadShot_Idle"));
	Desc_13.iFrameSpeed = 4;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("EliteSoldier_Die_HeadShot_Idle"), Desc_13);

	return S_OK;
}

void CEliteSoldier::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
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
				m_pGameInstance->PlaySoundOnce(TEXT("EliteSoldier_Hit.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
			}
			CParticle_Manager::GetInstance()->Create_Particle(TEXT("Particle_Blood"), m_pObjectDesc.iLayerLevel,
				TEXT("Layer_Particle"), m_pTransformCom->Get_State(STATE::POSITION));
		}
	}

	return;
}

void CEliteSoldier::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider)
{
	if (m_isDead || m_bDying)
		return;

	if (eColType == COLLISION::RAY)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
		{
			m_pTransformCom->Turn({ 0.f, 1.0f, 0.f }, fTimeDelta);
			m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

			if ((m_fHp -= (pBullet->Get_Damage())) > 0)
			{
				m_pGameInstance->PlaySoundOnce(TEXT("Soldier_Pain01.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);

				CParticle_Manager::GetInstance()->Create_Particle(TEXT("Particle_Blood"), m_pObjectDesc.iLayerLevel,
					TEXT("Layer_Particle"), m_pTransformCom->Get_State(STATE::POSITION));
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

const COLLISION_DESC& CEliteSoldier::Get_CollisionDesc(COLLISION eColType)
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

HRESULT CEliteSoldier::Ready_Components()
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
		//tempNum++;
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
	CSight::SIGHT_DESC		SightDesc{ 15.f, D3DXToRadian(120.0f), m_pPlayerTransform, m_pTransformCom};
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Sight"),
		TEXT("Com_Sight"), reinterpret_cast<CComponent**>(&m_pSightCom), &SightDesc)))
		return E_FAIL;

	/* Com_BoxCollider */
	CBoxCollider::BOXCOLLIDER_DESC Desc;
	Desc.vPosition = { -0.1f, -0.07f, -0.1f };
	Desc.fScaleX = 0.4f;
	Desc.fScaleZ = 0.4f;
	Desc.fScaleY = 0.8f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	Desc.vPosition = { -0.3f, 2.35f, -0.3f };
	Desc.fScaleX = 0.2f;
	Desc.fScaleZ = 0.2f;
	Desc.fScaleY = 0.2f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider_Head"), reinterpret_cast<CComponent**>(&m_pBoxColliderHead), &Desc)))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEliteSoldier::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	/*
	float4		vSourColor, vDestColor;
	vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);
	*/

	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);*/



	return S_OK;
}

HRESULT CEliteSoldier::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CEliteSoldier::Attack(EliteSoldierState eState)
{
	if (eState == EliteSoldierState::ONESHOT)
	{
		m_fSumAttackCoolTime = 0.f;
		m_strFrameKey = TEXT("EliteSoldier_Attack_Seat");
		m_bAnimationLock = true;

		m_pGameInstance->PlaySoundOnce(TEXT("EliteSoldier_Attack_OneShot.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);

		_float3 vDir = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
		_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		D3DXVec3Normalize(&vDir, &vDir);

		CBullet::BULLET_DESC Desc;
		Desc.vDir = vDir;
		Desc.vPos = vPos;
		Desc.vBulletScale = { 0.005f, 0.005f, 0.2f };
		//Desc.vBulletScale = { 0.2f, 0.2f, 0.01f };
		Desc.fBulletSpeed = 8.f;
		Desc.fDuration = 5.f;
		Desc.isPlayerBullet = false;

		CBullet_Manager::GetInstance()->Create_Bullet(TEXT("Bullet"), Desc, m_pObjectDesc.iLayerLevel, TEXT("Layer_Monster_Bullet"));
		//m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Bullet"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Monster_Bullet"), &Desc);
	}
	else if (eState == EliteSoldierState::MASS)
	{
		m_fSumAttackCoolTime = 0.f;
		m_strFrameKey = TEXT("EliteSoldier_Attack_Front");
		m_bAnimationLock = true;

		m_pGameInstance->PlaySoundOnce(TEXT("EliteSoldier_Attack_Mass.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);

		_float3 vDir = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
		_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
		D3DXVec3Normalize(&vDir, &vDir);

		_uint len = sizeof(m_vShootPosOffset) / sizeof(m_vShootPosOffset[0]);

		for (int i = 0; i < len; i++)		// 방향벡터는 유지하되 생성되는 위치를 근처로 해서 쏘게하자
		{
			CBullet::BULLET_DESC Desc;
			Desc.vDir = vDir + m_vShootPosOffset[i];
			Desc.vPos = vPos;
			Desc.vBulletScale = { 0.005f, 0.005f, 0.2f };
			//Desc.vBulletScale = { 0.2f, 0.2f, 0.01f };
			Desc.fBulletSpeed = 5.f;
			Desc.fDuration = 5.f;
			Desc.isPlayerBullet = false;
			CBullet_Manager::GetInstance()->Create_Bullet(TEXT("Bullet"), Desc, m_pObjectDesc.iLayerLevel, TEXT("Layer_Monster_Bullet"));
			//m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Bullet"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Monster_Bullet"), &Desc);
		}
	}
}

void CEliteSoldier::Move(_float fTimeDelta)
{
	_float3 fPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
	_float3 fMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	_float3 vDirection = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	vDirection.y = 0.f;

	D3DXVec3Normalize(&fPlayerLook, &fPlayerLook);
	D3DXVec3Normalize(&fMonsterLook, &fMonsterLook);
	D3DXVec3Normalize(&vDirection, &vDirection);

	/*_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, fRadian);
	m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fTimeDelta);
	m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));*/

	_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, fRadian);

	m_pTransformCom->Go_Direction(vDirection, fTimeDelta);

	m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

	//m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fMoveTime);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}

void CEliteSoldier::Move()
{
	m_pTransformCom->Get_State(STATE::POSITION);
}

CEliteSoldier* CEliteSoldier::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEliteSoldier* pInstance = new CEliteSoldier(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEliteSoldier::Clone(void* pArg)
{
	CEliteSoldier* pInstance = new CEliteSoldier(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEliteSoldier");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEliteSoldier::Free()
{
	__super::Free();
}
