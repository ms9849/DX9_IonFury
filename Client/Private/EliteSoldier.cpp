#include "EliteSoldier.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "BehaviorNode.h"
#include "Particle_Manager.h"

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
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (m_pPlayerTransform == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 1.5f, 1.5f, 1.f });

	m_pTransformCom->Set_State(STATE::POSITION, _float3(
		m_pGameInstance->Random(0.f, 20.f),
		0.f,
		m_pGameInstance->Random(0.f, 20.f)));

	m_fAttackRange = 5.f;
	m_fChaseRange = 15.f;
	m_fMaxRange = 10.f;
	//m_pTransformCom->Rotation({0.f, 1.f, 0.f}, m_pGameInstance->Random(0.f, 180.f));
	//m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

	//CSelectorNode* root = new CSelectorNode();

	//CSequenceNode* CCheckHpSequence = new CSequenceNode();
	//CCheckHpSequence->AddChild(new CConditionNode([this]() {
	//	/*if (m_fHp >= 0)
	//		m_fHp -= 0.1f;*/

	//	return this->m_fHp <= 0;
	//	}));

	//CCheckHpSequence->AddChild(new CActionNode([this]() {
	//	m_strFrameKey = TEXT("Soldier_Die_Explosion");
	//	m_bAnimationLock = true;
	//	m_bDying = true;
	//	}));

	//CSequenceNode* CAttackSequence = new CSequenceNode();
	//CAttackSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	return this->m_pSightCom->Check_Sight(fTimeDelta);
	//	}));

	//CAttackSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	m_fSumAttackCoolTime += fTimeDelta;
	//	return m_fSumAttackCoolTime >= m_fAttackfCoolTime;
	//	}));

	//CAttackSequence->AddChild(new CConditionNode([this]() {
	//	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);

	//	return D3DXVec3Length(&vDiff) <= m_fAttackRange;
	//	}));

	//CAttackSequence->AddChild(new CActionNode([this]() {
	//	m_strFrameKey = TEXT("Soldier_Attack_Front");
	//	m_bAnimationLock = true;
	//	this->Attack();
	//	m_fSumAttackCoolTime = 0.f;
	//	}));

	//CSelectorNode* CMoveCheckSequence = new CSelectorNode();
	//CSequenceNode* CSightSucessSequence = new CSequenceNode();
	//CSequenceNode* CSightFailSequence = new CSequenceNode();
	//CSightFailSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	return !this->m_pSightCom->Check_Sight(fTimeDelta);
	//	}));

	//CSightFailSequence->AddChild(new CConditionNode([this]() {
	//	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	//	return this->m_fChaseRange >= D3DXVec3Length(&vDiff);
	//	}));

	//CSightFailSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	m_fSumMoveCoolTime += fTimeDelta;
	//	return m_fSumMoveCoolTime >= m_fMoveCoolTime;
	//	}));

	//CSightFailSequence->AddChild(new CActionNode([this](_float fTimeDelta) {
	//	this->Move(fTimeDelta);
	//	m_fSumMoveCoolTime = 0.f;
	//	}));

	//CSightSucessSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	return this->m_pSightCom->Check_Sight(fTimeDelta);
	//	}));

	//CSightSucessSequence->AddChild(new CConditionNode([this]() {
	//	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	//	return this->m_fChaseRange >= D3DXVec3Length(&vDiff) && m_fAttackRange <= D3DXVec3Length(&vDiff);
	//	}));

	//CSightSucessSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	m_fSumMoveCoolTime += fTimeDelta;
	//	return m_fSumMoveCoolTime >= m_fMoveCoolTime;
	//	}));

	//CSightSucessSequence->AddChild(new CActionNode([this](_float fTimeDelta) {
	//	this->Move(fTimeDelta);
	//	m_fSumMoveCoolTime = 0.f;
	//	}));

	//CMoveCheckSequence->AddChild(CSightSucessSequence);
	//CMoveCheckSequence->AddChild(CSightFailSequence);

	//root->AddChild(CCheckHpSequence);
	//root->AddChild(CAttackSequence);
	//root->AddChild(CMoveCheckSequence);

	//m_pRoot = root;

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
		SetUp_OnTerrain(m_pTransformCom, 0.35f, &m_bJump);
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
				if (m_eState == EliteSoldierState::END)
				{
					// 랜덤으로 하나 골라지게 함
					//m_eState = EliteSoldierState::ONESHOT;
					m_eState = EliteSoldierState::MASS;
				}

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

	/*if (!m_bAnimationLock)
	{
		_float angle30 = cosf(D3DXToRadian(30.f));
		_float angle60 = cosf(D3DXToRadian(60.f));

		if (dot >= fFov)
		{
			m_strFrameKey = TEXT("Soldier_Front");
		}
		else if (dot <= -fFov)
		{
			m_strFrameKey = TEXT("Soldier_Back");
		}
		else
		{
			if (vCross.y > 0)
			{
				if (dot > 0)
					m_strFrameKey = TEXT("Soldier_Direction_SW");
				else
					m_strFrameKey = TEXT("Soldier_Direction_NW");
			}
			else
			{
				if (dot > 0)
					m_strFrameKey = TEXT("Soldier_Direction_SE");
				else
					m_strFrameKey = TEXT("Soldier_Direction_NE");
			}
		}
	}*/

	//m_pRoot->Run(fTimeDelta);

	/*if (m_bAnimationLock && m_pAnimationCom->Check_Animation_Finish())
	{
		if (m_bDying)
			m_isDead = true;
		m_bAnimationLock = false;
	}*/

	/*szBuffer[128];
	swprintf_s(szBuffer, L"몬스터 방향 벡터 x : %.1f, y : %.1f, z : %.1f\n", m_pTransformCom->Get_State(STATE::LOOK).x, m_pTransformCom->Get_State(STATE::LOOK).y, m_pTransformCom->Get_State(STATE::LOOK).z);
	OutputDebugString(szBuffer);*/

	SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);
}

void CEliteSoldier::Late_Update(_float fTimeDelta)
{
	if (m_isMove || m_bAnimationLock || m_bDying)
		m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);
	//int num = m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey);

	/*auto iter = m_pAnimationCom->Get_Frame_Desc(m_strFrameKey);
	wchar_t szDebug_2[256];
	swprintf(szDebug_2, 256, L"프레임 키: %s, 현재 인덱스: %d, 마지막 인덱스: %d\n", m_strFrameKey.c_str(), m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey), iter->iEnd);
	OutputDebugStringW(szDebug_2);*/
	//Get_Frame_Current_Index(m_strFrameKey) == iter->iEnd - 1
	if (m_bAnimationLock && m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
	{
		if (m_bDying)
		{
			//m_isDead = true;
			m_strFrameKey = TEXT("EliteSoldier_Die_Idle");
			m_bAnimationLock = false;
			return;
		}
		else
		{
			m_bAnimationLock = false;
			/*wchar_t szDebug[256];
			swprintf(szDebug, 256, L"종료 프레임 키: %s, 현재 인덱스: %d\n", m_strFrameKey.c_str(), num);
			OutputDebugStringW(szDebug);
			m_pAnimationCom->Clear_Animation(m_strFrameKey);*/
			m_strFrameKey = TEXT("EliteSoldier_Front");
		}
	}
	/*m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);*/
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CEliteSoldier::Render()
{
	_float4x4 matWorldTemp = *m_pTransformCom->Get_WorldMatrixPtr();

	/*
	matWorldTemp? ->현재 트랜스폼의 위치와 회전값을 그대로 가져옴.

	지금 이 상태에서 플레이어를 바라보게끔 회전만 시키면 되는 상황

	실제 회전값과는 무관하게 플레이어를 바라보게만 만든 행렬
	*/

	_float3 fMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
	_float3 fPlayerPos = m_pPlayerTransform->Get_State(STATE::POSITION);

	_float3 fLook = fPlayerPos - fMonsterPos;
	fLook.y = 0.0f;								// y축 회전용
	D3DXVec3Normalize(&fLook, &fLook);

	_float3 fUp = { 0.0f, 1.0f, 0.0f };

	_float3 fRight;
	D3DXVec3Cross(&fRight, &fUp, &fLook);
	D3DXVec3Normalize(&fRight, &fRight);

	memcpy(&matWorldTemp.m[0][0], &fRight, sizeof(_float3));
	memcpy(&matWorldTemp.m[1][0], &fUp, sizeof(_float3));
	memcpy(&matWorldTemp.m[2][0], &fLook, sizeof(_float3));

	m_pTransformCom->Set_Transform(matWorldTemp);

	//m_pTextureCom->Set_Texture(0);
	/*m_pTextureCom->Set_Texture(m_iNum++);
	if (m_iNum > 3)
		m_iNum = 0;*/

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
			CParticle_Manager::GetInstance()->Create_Particle(TEXT("Particle_Blood"), ENUM_CLASS(LEVEL::GAMEPLAY),
				TEXT("Layer_Particle"), m_pTransformCom->Get_State(STATE::POSITION));
		}
	}

	return;
}

const COLLISION_DESC& CEliteSoldier::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	else if (eColType == COLLISION::OBB)
		Desc.pCollider = m_pBoxColliderCom;

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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Sight"),
		TEXT("Com_Sight"), reinterpret_cast<CComponent**>(&m_pSightCom), &SightDesc)))
		return E_FAIL;

	/* Com_BoxCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom))))
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
		Desc.vBulletScale = { 0.2f, 0.2f, 0.01f };
		Desc.fBulletSpeed = 8.f;
		Desc.fDuration = 5.f;
		Desc.isPlayerBullet = false;


		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"), &Desc);
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
			Desc.vBulletScale = { 0.2f, 0.2f, 0.01f };
			Desc.fBulletSpeed = 5.f;
			Desc.fDuration = 5.f;
			Desc.isPlayerBullet = false;
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"), &Desc);
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
