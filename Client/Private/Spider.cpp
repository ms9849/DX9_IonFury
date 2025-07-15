#include "Spider.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "BehaviorNode.h"
#include "Particle_Manager.h"

CSpider::CSpider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{

}

CSpider::CSpider(const CSpider& Prototype)
	: CMonster(Prototype)
{

}

HRESULT CSpider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpider::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

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
	m_pTransformCom->Set_Scale(_float3{ 1.f, 1.f, 1.f });
	//m_pTransformCom->Set_Scale(_float3{ 1.f, 1.f, 1.f });

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

	m_fDamage = 30.f;
	m_fAttackRange = 1.5f;
	m_fAttackCoolTime = 5.f;
	m_fJumpPower = 3.f;
	m_fChaseRange = 7.f;
	m_fMaxRange = 10.f;
	//m_AttackfCoolTime = 1.f;

	return S_OK;
}

void CSpider::Priority_Update(_float fTimeDelta)
{
}

void CSpider::Update(_float fTimeDelta)
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
		SetUp_OnTerrain(m_pTransformCom, 0.6f, &m_bJump);
		return;
	}

	m_bRideCube = true;
	m_fSumAttackCoolTime += fTimeDelta;
	m_fSumMoveCoolTime += fTimeDelta;

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
			m_strFrameKey = TEXT("Spider_Front");
		}
		else if (dot <= -fFov)
		{
			m_strFrameKey = TEXT("Spider_Back");
		}
		else
		{
			if (vCross.y > 0)
			{
				if (dot > 0)
					m_strFrameKey = TEXT("Spider_Direction_SW");
				else
					m_strFrameKey = TEXT("Spider_Direction_NW");
			}
			else
			{
				if (dot > 0)
					m_strFrameKey = TEXT("Spider_Direction_SE");
				else
					m_strFrameKey = TEXT("Spider_Direction_NE");
			}
		}
		m_isMove = false;
	}


	if (m_fCurHp <= 0)
	{
		m_strFrameKey = TEXT("Spider_Die_Default");
		m_bAnimationLock = true;
		m_bDying = true;
		m_pGameInstance->PlaySoundOnce(TEXT("Spider_Die.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	}
	else if (m_pSightCom->Check_Sight(fTimeDelta) && !m_bAnimationLock)
	{
		if (m_fSumAttackCoolTime >= m_fAttackCoolTime)
		{
			_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
			if (D3DXVec3Length(&vDiff) <= m_fAttackRange)
			{
				m_bJump = true;
				m_fTime = 0.f;
				Attack();
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

	Jump(fTimeDelta, m_fJumpPower);
	SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);
}

void CSpider::Late_Update(_float fTimeDelta)
{
	if (m_isMove || m_bAnimationLock || m_bDying)
		m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);

	if (m_bAnimationLock && m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
	{
		//m_pAnimationCom->Set_Animation(&iter->second);
		//m_Cnt++;

		if (m_bDying)
		{
			//m_isDead = true;
			m_strFrameKey = TEXT("Spider_Die_Idle");
			m_bAnimationLock = false;
			return;
		}
		else
		{
			m_bAnimationLock = false;
			m_strFrameKey = TEXT("Spider_Front");
		}
	}

	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CSpider::Render()
{
	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));
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

HRESULT CSpider::Ready_Animations()
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

	auto iter = m_pTextureComs.find(TEXT("Spider_Attack"));
	Desc_0.iFrameSpeed = 15;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Attack"), Desc_0);

	//Spider_Die_Default
	iter = m_pTextureComs.find(TEXT("Spider_Die_Default"));
	Desc_1.iFrameSpeed = 7;
	Desc_1.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Die_Default"), Desc_1);

	//Spider_Direction_NE
	iter = m_pTextureComs.find(TEXT("Spider_Direction_NE"));
	Desc_2.iFrameSpeed = 10;
	Desc_2.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Direction_NE"), Desc_2);

	//Spider_Direction_NW
	iter = m_pTextureComs.find(TEXT("Spider_Direction_NW"));
	Desc_3.iFrameSpeed = 10;
	Desc_3.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Direction_NW"), Desc_3);

	//Spider_Direction_SE
	iter = m_pTextureComs.find(TEXT("Spider_Direction_SE"));
	Desc_4.iFrameSpeed = 10;
	Desc_4.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Direction_SE"), Desc_4);

	//Spider_Direction_SW
	iter = m_pTextureComs.find(TEXT("Spider_Direction_SW"));
	Desc_5.iFrameSpeed = 10;
	Desc_5.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Direction_SW"), Desc_5);

	//Spider_Front
	iter = m_pTextureComs.find(TEXT("Spider_Front"));
	Desc_6.iFrameSpeed = 5;
	Desc_6.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Front"), Desc_6);

	//Spider_Back
	iter = m_pTextureComs.find(TEXT("Spider_Back"));
	Desc_7.iFrameSpeed = 5;
	Desc_7.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Back"), Desc_7);

	//Spider_Left
	iter = m_pTextureComs.find(TEXT("Spider_Left"));
	Desc_8.iFrameSpeed = 5;
	Desc_8.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Left"), Desc_8);

	//Spider_Right
	iter = m_pTextureComs.find(TEXT("Spider_Right"));
	Desc_9.iFrameSpeed = 5;
	Desc_9.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Right"), Desc_9);

	//Spider_Die_Idle
	iter = m_pTextureComs.find(TEXT("Spider_Die_Idle"));
	Desc_10.iFrameSpeed = 10;
	Desc_10.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Spider_Die_Idle"), Desc_10);

	return S_OK;
}

void CSpider::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos)
{
	if (eColType == COLLISION::RAY)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
		{
			m_pTransformCom->Turn({ 0.f, 1.0f, 0.f }, fTimeDelta);
			m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

			if ((m_fCurHp -= (pBullet->Get_Damage())) > 0)
			{
				m_pGameInstance->PlaySoundOnce(TEXT("Spider_hit_1.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
			}

			CParticle_Manager::GetInstance()->Create_Particle(TEXT("Particle_Blood"), m_pObjectDesc.iLayerLevel,
				TEXT("Layer_Particle"), vPos);
		}
	}

	return;
}

const COLLISION_DESC& CSpider::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	else if (eColType == COLLISION::BOX)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

HRESULT CSpider::Ready_Components()
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
	Desc.vPosition = { 0.f, -0.5f, 0.f };
	Desc.fScaleX = 0.9f;
	Desc.fScaleY = 0.4f;
	Desc.fScaleZ = 0.8f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpider::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);*/



	return S_OK;
}

HRESULT CSpider::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CSpider::Attack()
{
	m_pGameInstance->PlaySoundOnce(TEXT("Spider_Attack.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
	//m_bAttackStarted = true;
	//m_bAttackStarted = true;
	//m_strFrameKey = TEXT("Spider_Die_Explosion");
	m_fSumAttackCoolTime = 0.f;
	m_strFrameKey = TEXT("Spider_Attack");
	//m_pGameInstance->PlaySoundOnce(TEXT("Spider_swing_2.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
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

	m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Melee_Attack"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Melee_Attack"), &m_pObjectDesc);
	//m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Bullet"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Spider_Bullet"), &Desc);
}

void CSpider::Move(_float fTimeDelta)
{
	_float3 fPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
	_float3 fMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	_float3 vDirection = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	vDirection.y = 0.f;

	D3DXVec3Normalize(&fPlayerLook, &fPlayerLook);
	D3DXVec3Normalize(&fMonsterLook, &fMonsterLook);
	D3DXVec3Normalize(&vDirection, &vDirection);

	_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, fRadian);

	m_pTransformCom->Go_Direction(vDirection, fTimeDelta);

	m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

	

	//m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fMoveTime);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}

/*
void CSpider::Move(_float fTimeDelta)
{
	_float3 fPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
	_float3 fMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	D3DXVec3Normalize(&fPlayerLook, &fPlayerLook);
	D3DXVec3Normalize(&fMonsterLook, &fMonsterLook);

	_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, fRadian);
	m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fTimeDelta);
	m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

	//m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fMoveTime);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}
*/

void CSpider::Move()
{
	m_pTransformCom->Get_State(STATE::POSITION);
}

void CSpider::Jump(_float fTimeDelta, _float fJumpPower)								// 임시 사용 함수 LandObject꺼 오버라이딩 구조 변경 or 오버로딩할것
{
	if (m_bJump)
	{
		m_fFallSpeed = (fJumpPower * m_fTime - 30.f * m_fTime * m_fTime);

		if (m_fFallSpeed <= -0.2f)
			m_fFallSpeed = -0.2f;

		m_fTime += 0.3f * fTimeDelta;
		_float3 vPosition = m_pTransformCom->Get_State(STATE::POSITION);

		_float3 vDir = m_pPlayerTransform->Get_State(STATE::POSITION) - vPosition;
		vDir.y = 0.f;
		D3DXVec3Normalize(&vDir, &vDir);

		vPosition += vDir * fJumpPower * fTimeDelta;
		vPosition.y += m_fFallSpeed;
		m_pTransformCom->Set_State(STATE::POSITION, vPosition);
	}
}

CSpider* CSpider::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSpider* pInstance = new CSpider(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpider::Clone(void* pArg)
{
	CSpider* pInstance = new CSpider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Spider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpider::Free()
{
	//m_pRoot->ReleaseSubtree();
	__super::Free();
	Safe_Release(m_pPlayerTransform);
}
