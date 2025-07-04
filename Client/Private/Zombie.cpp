#include "Zombie.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "MeleeAttack.h"
#include "BehaviorNode.h"

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
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (m_pPlayerTransform == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(
		m_pGameInstance->Random(0.f, 20.f),
		0.f,
		m_pGameInstance->Random(0.f, 20.f)));

	m_fDamage = 30.f;
	m_fAttackRange = 2.f;
	m_fAttackCoolTime = 5.f;
	//m_AttackfCoolTime = 1.f;

	//m_pTransformCom->Rotation({0.f, 1.f, 0.f}, m_pGameInstance->Random(0.f, 180.f));
	//m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));


	// 구조 변경

	//CSelectorNode* root = new CSelectorNode();

	//CSequenceNode* CCheckHpSequence = new CSequenceNode();
	//CCheckHpSequence->AddChild(new CConditionNode([this]() {
	//	return this->m_fHp <= 0;
	//	}));

	//CCheckHpSequence->AddChild(new CActionNode([this]() {
	//	m_strFrameKey = TEXT("Zombie_Die_Default");		// 나중엔 함수만들어서 조절하는게 좋을거 같음
	//	m_bAnimationLock = true;
	//	m_bDying = true;
	//	}));

	//CSequenceNode* CAttackSequence = new CSequenceNode();
	//CAttackSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	//OutputDebugStringA(("Zombie Update m_fSumAttackCoolTime: " + std::to_string(m_fSumAttackCoolTime) + "\n").c_str());
	//	return this->m_pSightCom->Check_Sight(fTimeDelta);
	//	}));

	//CAttackSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	return m_fSumAttackCoolTime >= m_fAttackfCoolTime;
	//	}));

	//CAttackSequence->AddChild(new CConditionNode([this]() {
	//	//OutputDebugStringA(("Zombie Update m_fSumAttackCoolTime: " + std::to_string(m_fSumAttackCoolTime) + "\n").c_str());
	//	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);

	//	return D3DXVec3Length(&vDiff) <= m_fAttackRange;
	//	}));

	//CAttackSequence->AddChild(new CActionNode([this]() {
	//	OutputDebugStringW((L"Zombie FrameKey: " + m_strFrameKey + L"\n").c_str());
	//	if (m_strFrameKey == TEXT("Zombie_Attack"))
	//		return false;

	//	this->Attack();

	//	}));
	//
	//CSelectorNode* CMoveCheckSequence = new CSelectorNode();
	//CSequenceNode* CSightSucessSequence = new CSequenceNode();
	//CSequenceNode* CSightFailSequence = new CSequenceNode();
	//CSightFailSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	//OutputDebugStringA(("Zombie Update TimeDelta: " + std::to_string(fTimeDelta) + "\n").c_str());
	//	return !this->m_pSightCom->Check_Sight(fTimeDelta);
	//	}));

	//CSightFailSequence->AddChild(new CConditionNode([this]() {
	//	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	//	return this->m_fChaseRange >= D3DXVec3Length(&vDiff);
	//	}));

	//CSightFailSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	return m_fSumMoveCoolTime >= m_fMoveCoolTime;
	//	}));

	//CSightFailSequence->AddChild(new CActionNode([this](_float fTimeDelta) {
	//	this->Move(fTimeDelta);
	//	}));

	//CSightSucessSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	return this->m_pSightCom->Check_Sight(fTimeDelta);
	//	}));

	//CSightSucessSequence->AddChild(new CConditionNode([this]() {
	//	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	//	return this->m_fChaseRange >= D3DXVec3Length(&vDiff) && m_fAttackRange <= D3DXVec3Length(&vDiff);
	//	}));

	//CSightSucessSequence->AddChild(new CConditionNode([this](_float fTimeDelta) {
	//	return m_fSumMoveCoolTime >= m_fMoveCoolTime;
	//	}));

	//CSightSucessSequence->AddChild(new CActionNode([this](_float fTimeDelta) {
	//	this->Move(fTimeDelta);
	//	}));

	//CMoveCheckSequence->AddChild(CSightSucessSequence);
	//CMoveCheckSequence->AddChild(CSightFailSequence);

	//root->AddChild(CCheckHpSequence);
	//root->AddChild(CAttackSequence);
	//root->AddChild(CMoveCheckSequence);

	//m_pRoot = root;

	return S_OK;
}

void CZombie::Priority_Update(_float fTimeDelta)
{
}

void CZombie::Update(_float fTimeDelta)
{
	m_bRideCube = true;
	m_fSumAttackCoolTime += fTimeDelta;
	m_fSumMoveCoolTime += fTimeDelta;

	//OutputDebugStringA(("Zombie Update m_fSumAttackCoolTime: " + std::to_string(m_fSumAttackCoolTime) + "\n").c_str());
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

	//_float angle30 = cosf(D3DXToRadian(30.f));
	//_float angle60 = cosf(D3DXToRadian(60.f));

	////m_pRoot->Run(fTimeDelta);

	//if (!m_bAnimationLock)
	//{
	//	_float angle30 = cosf(D3DXToRadian(30.f));
	//	_float angle60 = cosf(D3DXToRadian(60.f));

	//	if (dot >= fFov)
	//	{
	//		m_strFrameKey = TEXT("Zombie_Front");
	//	}
	//	else if (dot <= -fFov)
	//	{
	//		m_strFrameKey = TEXT("Zombie_Back");
	//	}
	//	else
	//	{
	//		if (vCross.y > 0)
	//		{
	//			if (dot > 0)
	//				m_strFrameKey = TEXT("Zombie_Direction_SW");
	//			else
	//				m_strFrameKey = TEXT("Zombie_Direction_NW");
	//		}
	//		else
	//		{
	//			if (dot > 0)
	//				m_strFrameKey = TEXT("Zombie_Direction_SE");
	//			else
	//				m_strFrameKey = TEXT("Zombie_Direction_NE");
	//		}
	//	}
	//}

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
	}


	if (m_fHp <= 0)
	{
		m_strFrameKey = TEXT("Zombie_Die_Default");
		m_bAnimationLock = true;
		m_bDying = true;
	}
	else if (m_pSightCom->Check_Sight(fTimeDelta) && !m_bAnimationLock)
	{
		if (m_fSumAttackCoolTime >= m_fAttackCoolTime)
		{
			_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
			if (D3DXVec3Length(&vDiff) <= m_fAttackRange)
			{
				Attack();
			}
		}
		else if (m_fSumMoveCoolTime >= m_fMoveCoolTime)
		{
			_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
			if (D3DXVec3Length(&vDiff) <= m_fChaseRange && D3DXVec3Length(&vDiff) >= m_fAttackRange - 1.f)
			{
				Move(fTimeDelta);
				m_fSumMoveCoolTime = 0.f;
			}
		}
	}

	/*m_pRoot->Run(fTimeDelta);*/

	/*if (!m_bAnimationLock)
		m_pRoot->Run(fTimeDelta);*/
	//m_pRoot->Run(fTimeDelta);

	/*auto iter = m_Frames.find(m_strFrameKey);
	m_pAnimationCom->Set_Animation(&iter->second);*/


		//auto iter = m_Frames.find(m_strFrameKey);
	/*auto iter = m_Frames.find(m_strFrameKey);
	m_pAnimationCom->Set_Animation(&iter->second);*/

	/*if (m_bAnimationLock)
	{
		if (m_pAnimationCom->Check_Animation_Finish())
			m_bAnimationLock = false;
	}*/
	//auto iter = m_Frames.find(m_strFrameKey);
	//m_pAnimationCom->Set_Animation(&iter->second);

	__super::Jump(fTimeDelta);
	SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);
}

void CZombie::Late_Update(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);

	if (m_bAnimationLock && m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
	{
		//m_pAnimationCom->Set_Animation(&iter->second);
		//m_Cnt++;

		if (m_bDying)
		{
			m_isDead = true;
			m_bAnimationLock = false;
			return;
		}
		else
		{
			m_bAnimationLock = false;
			m_strFrameKey = TEXT("Zombie_Front");
		}
	}
	/*m_pAnimationCom->Set_Animation(&iter->second);
	m_pAnimationCom->Play_Animation(fTimeDelta);*/
	//m_pAnimationCom->Play_Animation(fTimeDelta);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CZombie::Render()
{
	_float4x4 matWorldTemp = *m_pTransformCom->Get_WorldMatrixPtr();

	/*
	matWorldTemp? ->현재 트랜스폼의 위치와 회전값을 그대로 가져옴.

	지금 이 상태에서 플레이어를 바라보게끔 회전만 시키면 되는 상황

	실제 회전값과는 무관하게 플레이어를 바라보게만 만든 행렬
	*/

	_float3 fMonsterPos = m_pTransformCom->Get_State(STATE::POSITION);
	_float3 fPlayerPos = m_pPlayerTransform->Get_State(STATE::POSITION);
	//m_pTransformCom->Set_Scale({ 10.f, 10.f, 10.f });

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

	//m_pTransformCom->Set_Scale({ 10.f, 10.f, 10.f });

	m_pTransformCom->Set_Transform(matWorldTemp);

	auto iter = m_pTextureComs.find(m_strFrameKey);
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
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
	/*CAnimation::FRAME_DESC Desc_11{};
	CAnimation::FRAME_DESC Desc_12
	CAnimation::FRAME_DESC Desc_13{};*/

	auto iter = m_pTextureComs.find(TEXT("Zombie_Attack"));
	Desc_0.iFrameSpeed = 15;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Attack"), Desc_0);

	//Zombie_Die_Default
	iter = m_pTextureComs.find(TEXT("Zombie_Die_Default"));
	Desc_1.iFrameSpeed = 12;
	Desc_1.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Die_Default"), Desc_1);

	//Zombie_Direction_NE
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_NE"));
	Desc_2.iFrameSpeed = 7;
	Desc_2.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_NE"), Desc_2);

	//Zombie_Direction_NW
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_NW"));
	Desc_3.iFrameSpeed = 7;
	Desc_3.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_NW"), Desc_3);

	//Zombie_Direction_SE
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_SE"));
	Desc_4.iFrameSpeed = 7;
	Desc_4.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_SE"), Desc_4);

	//Zombie_Direction_SW
	iter = m_pTextureComs.find(TEXT("Zombie_Direction_SW"));
	Desc_5.iFrameSpeed = 7;
	Desc_5.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Direction_SW"), Desc_5);

	//Zombie_Front
	iter = m_pTextureComs.find(TEXT("Zombie_Front"));
	Desc_6.iFrameSpeed = 7;
	Desc_6.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Front"), Desc_6);

	//Zombie_Back
	iter = m_pTextureComs.find(TEXT("Zombie_Back"));
	Desc_7.iFrameSpeed = 7;
	Desc_7.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Back"), Desc_7);

	//Zombie_Left
	iter = m_pTextureComs.find(TEXT("Zombie_Left"));
	Desc_8.iFrameSpeed = 7;
	Desc_8.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Left"), Desc_8);

	//Zombie_Right
	iter = m_pTextureComs.find(TEXT("Zombie_Right"));
	Desc_9.iFrameSpeed = 7;
	Desc_9.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Right"), Desc_9);

	//Zombie_Die_Explosion
	iter = m_pTextureComs.find(TEXT("Zombie_Die_Explosion"));
	Desc_10.iFrameSpeed = 60;
	Desc_10.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Zombie_Die_Explosion"), Desc_10);


	return S_OK;
}

void CZombie::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (eColType == COLLISION::SPHERE)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
			m_fHp -= pBullet->Get_Damage();
	}

	return;
}

const COLLISION_DESC& CZombie::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	else if (eColType == COLLISION::OBB)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
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
	CSight::SIGHT_DESC		SightDesc{ 5.f, D3DXToRadian(90.0f), m_pPlayerTransform, m_pTransformCom, true };
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

HRESULT CZombie::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/


	
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);



	return S_OK;
}

HRESULT CZombie::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
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

	CMeleeAttack::MELEEATTACK_DESC Desc;
	Desc.vDir = vDir;
	Desc.vPos = vPos;
	Desc.fDamage = m_fDamage;
	Desc.fDurationTime = 10.f;

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Melee_Attack"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Melee_Attack"), &Desc);
	//m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Zombie_Bullet"), &Desc);
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

	_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);
	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, fRadian);

	m_pTransformCom->Go_Direction(vDirection, fTimeDelta);

	m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

	

	//m_pTransformCom->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fMoveTime);
	//m_pTransformCom->Go_Straight(fTimeDelta);
}

/*
void CZombie::Move(_float fTimeDelta)
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

void CZombie::Move()
{
	//m_pTransformCom->R
	m_pTransformCom->Get_State(STATE::POSITION);
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
}
