#include "Boss.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "BehaviorNode.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster{ pGraphic_Device }
{

}

CBoss::CBoss(const CBoss& Prototype)
	: CMonster{ Prototype }
{

}

HRESULT CBoss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (m_pPlayerTransform == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 10.f, 10.f, 1.f });

	m_pTransformCom->Set_State(STATE::POSITION, _float3(
		m_pGameInstance->Random(10.f, 30.f),
		0.f,
		m_pGameInstance->Random(10.f, 30.f)));

	/*m_pTransformCom_Up->Set_State(STATE::POSITION, _float3(
		vPos.x,
		vPos.y,
		vPos.z));

	m_pTransformCom_Down->Set_State(STATE::POSITION, _float3(
		vPos.x,
		vPos.y,
		vPos.z));*/

	_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom_Up->Set_State(STATE::POSITION, {vPos.x, vPos.y + 2.0f, vPos.z});

	m_pTransformCom_Down->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION));
	//m_pTransformCom->Set_Scale({ 100.f, 100.f, 10.f });
	
	/*m_pTransformCom->Set_Transform();*/

	m_fAttackRange = 5.f;

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
	//	m_strFrameKey = TEXT("Boss_Die_Explosion");
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
	//	m_strFrameKey = TEXT("Boss_Attack_Front");
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

void CBoss::Priority_Update(_float fTimeDelta)
{

}

void CBoss::Update(_float fTimeDelta)
{
	m_fSumAttackCoolTime += fTimeDelta;
	m_fSumMoveCoolTime += fTimeDelta;

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
			m_strUpFrameKey = TEXT("Boss_Front");
			m_strDownFrameKey = TEXT("Boss_Front_Leg");
		}
		else if (dot <= -fFov)
		{
			m_strUpFrameKey = TEXT("Boss_Back");
			m_strDownFrameKey = TEXT("Boss_Back_Leg");
		}
		else
		{
			if (vCross.y > 0)
			{
				if (dot > 0.2f)
				{
					m_strUpFrameKey = TEXT("Boss_Direction_SW");
					m_strDownFrameKey = TEXT("Boss_Leg_Direction_SW");
				}
				else if (dot > 0)
				{
					m_strUpFrameKey = TEXT("Boss_Left");
					m_strDownFrameKey = TEXT("Boss_Left_Leg");
				}
				else
				{
					m_strUpFrameKey = TEXT("Boss_Direction_NW");
					m_strDownFrameKey = TEXT("Boss_Leg_Direction_NW");
				}
			}
			else
			{
				if (dot > 0.2f)
				{
					m_strUpFrameKey = TEXT("Boss_Direction_SE");
					m_strDownFrameKey = TEXT("Boss_Leg_Direction_SE");
				}
				else if (dot > 0)
				{
					m_strUpFrameKey = TEXT("Boss_Right");
					m_strDownFrameKey = TEXT("Boss_Right_Leg");
				}
				else
				{
					m_strUpFrameKey = TEXT("Boss_Direction_NE");
					m_strDownFrameKey = TEXT("Boss_Leg_Direction_NE");
				}
			}
		}
	}


	if (m_fHp <= 0)
	{
		m_strUpFrameKey = TEXT("Boss_Die");
		//m_strDownFrameKey = TEXT("Boss_Front_Leg");
		m_bAnimationLock = true;
		m_bDying = true;
	}
	else if (m_pSightCom->Check_Sight(fTimeDelta) && !m_bAnimationLock)
	{
		if (m_fSumAttackCoolTime >= m_fAttackfCoolTime)
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

	/*if (!m_bAnimationLock)
	{
		_float angle30 = cosf(D3DXToRadian(30.f));
		_float angle60 = cosf(D3DXToRadian(60.f));

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
				if (dot > 0)
					m_strFrameKey = TEXT("Boss_Direction_SW");
				else
					m_strFrameKey = TEXT("Boss_Direction_NW");
			}
			else
			{
				if (dot > 0)
					m_strFrameKey = TEXT("Boss_Direction_SE");
				else
					m_strFrameKey = TEXT("Boss_Direction_NE");
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

	SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CBoss::Late_Update(_float fTimeDelta)
{
	//auto iter = m_tFrames.find(m_strUpFrameKey);
	//m_pAnimationCom_Up->Set_Animation(&iter->second);
	//m_pAnimationCom_Up->Play_Animation(fTimeDelta);
	m_pAnimationCom_Up->Play_Animation(m_strUpFrameKey, fTimeDelta);
	m_pAnimationCom_Down->Play_Animation(m_strDownFrameKey, fTimeDelta);

	//auto iter_1 = m_tFrames.find(m_strDownFrameKey);

	/*m_pAnimationCom_Down->Set_Animation(&iter_1->second);
	m_pAnimationCom_Down->Play_Animation(fTimeDelta);*/

	if (m_bAnimationLock && m_pAnimationCom_Up->Check_Animation_Finish(m_strUpFrameKey))
	{
		if (m_bDying)
		{
			m_isDead = true;
			m_bAnimationLock = false;
			return;
		}
		else
		{
			m_bAnimationLock = false;
			m_strUpFrameKey = TEXT("Boss_Front");
		}
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CBoss::Render()
{
	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	RotateWithParentTransform();

	RotateToPlayer(m_pTransformCom_Up);
	auto iter = m_pTextureComs.find(m_strUpFrameKey);
	iter->second->Set_Texture(m_pAnimationCom_Up->Get_Frame_Current_Index(m_strUpFrameKey));
	m_pVIBufferCom_Up->Render();

	RotateToPlayer(m_pTransformCom_Down);
	iter = m_pTextureComs.find(m_strDownFrameKey);
	iter->second->Set_Texture(m_pAnimationCom_Down->Get_Frame_Current_Index(m_strDownFrameKey));
	m_pVIBufferCom_Down->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CBoss::RotateToPlayer(CTransform* pTranform)
{
	_float4x4 matWorldTemp = *pTranform->Get_WorldMatrixPtr();

	/*
	matWorldTemp? ->현재 트랜스폼의 위치와 회전값을 그대로 가져옴.

	지금 이 상태에서 플레이어를 바라보게끔 회전만 시키면 되는 상황

	실제 회전값과는 무관하게 플레이어를 바라보게만 만든 행렬
	*/

	_float3 fMonsterPos = pTranform->Get_State(STATE::POSITION);
	_float3 fPlayerPos = m_pPlayerTransform->Get_State(STATE::POSITION);

	_float3 fLook = fPlayerPos - fMonsterPos;
	fLook.y = 0.0f;								// y축 회전용
	D3DXVec3Normalize(&fLook, &fLook);

	_float3 fUp = { 0.0f, 1.0f, 0.0f };

	_float3 fRight;
	D3DXVec3Cross(&fRight, &fUp, &fLook);
	D3DXVec3Normalize(&fRight, &fRight);

	_float3 scale = pTranform->Get_Scaled();

	fRight *= scale.x;
	fUp *= scale.y;
	fLook *= scale.z;

	memcpy(&matWorldTemp.m[0][0], &fRight, sizeof(_float3));
	memcpy(&matWorldTemp.m[1][0], &fUp, sizeof(_float3));
	memcpy(&matWorldTemp.m[2][0], &fLook, sizeof(_float3));

	pTranform->Set_Transform(matWorldTemp);

	//return matWorldTemp;
}

void CBoss::RotateWithParentTransform()
{
	_float3 vRootPos = m_pTransformCom->Get_State(STATE::POSITION);

	_float3 vUpOffset = { 0.f, 3.8f, 0.f }; 
	_float3 vDownOffset = { 0.f, 0.f, 0.f };

	D3DXMATRIX matRot;
	//D3DXMatrixRotationY(&matRot, m_pTransformCom->Get_State(STATE::POSITION).y);

	m_pTransformCom_Up->Set_State(STATE::POSITION, vRootPos + vUpOffset);
	m_pTransformCom_Down->Set_State(STATE::POSITION, vRootPos + vDownOffset);

	m_pTransformCom_Up->Set_State(STATE::RIGHT, m_pTransformCom->Get_State(STATE::RIGHT));
	m_pTransformCom_Up->Set_State(STATE::LOOK, m_pTransformCom->Get_State(STATE::LOOK));
	m_pTransformCom_Up->Set_State(STATE::UP, m_pTransformCom->Get_State(STATE::UP));

	m_pTransformCom_Down->Set_State(STATE::RIGHT, m_pTransformCom->Get_State(STATE::RIGHT));
	m_pTransformCom_Down->Set_State(STATE::LOOK, m_pTransformCom->Get_State(STATE::LOOK));
	m_pTransformCom_Down->Set_State(STATE::UP, m_pTransformCom->Get_State(STATE::UP));
}

HRESULT CBoss::Ready_Animations()
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

	//Boss_Attack_Front
	auto iter = m_pTextureComs.find(TEXT("Boss_Attack_Front"));
	Desc_0.iFrameSpeed = 15;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Attack_Front"), Desc_0);

	//Boss_Attack_SE
	iter = m_pTextureComs.find(TEXT("Boss_Attack_SE"));
	Desc_1.iFrameSpeed = 15;
	Desc_1.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Attack_SE"), Desc_1);

	//Boss_Attack_SW
	iter = m_pTextureComs.find(TEXT("Boss_Attack_SW"));
	Desc_2.iFrameSpeed = 15;
	Desc_2.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Attack_SW"), Desc_2);

	//Boss_Die
	iter = m_pTextureComs.find(TEXT("Boss_Die"));
	Desc_3.iFrameSpeed = 12;
	Desc_3.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Die"), Desc_3);

	//Boss_Direction_NE
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NE"));
	Desc_4.iFrameSpeed = 12;
	Desc_4.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Direction_NE"), Desc_4);

	//Boss_Direction_NW
	iter = m_pTextureComs.find(TEXT("Boss_Direction_NW"));
	Desc_5.iFrameSpeed = 12;
	Desc_5.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Direction_NW"), Desc_5);

	//Boss_Direction_SE
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SE"));
	Desc_6.iFrameSpeed = 7;
	Desc_6.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Direction_SE"), Desc_6);

	//Boss_Direction_SW
	iter = m_pTextureComs.find(TEXT("Boss_Direction_SW"));
	Desc_7.iFrameSpeed = 7;
	Desc_7.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Direction_SW"), Desc_7);

	//Boss_Front
	iter = m_pTextureComs.find(TEXT("Boss_Front"));
	Desc_8.iFrameSpeed = 7;
	Desc_8.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Front"), Desc_8);

	//Boss_Back
	iter = m_pTextureComs.find(TEXT("Boss_Back"));
	Desc_9.iFrameSpeed = 7;
	Desc_9.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Back"), Desc_9);

	//Boss_Left
	iter = m_pTextureComs.find(TEXT("Boss_Left"));
	Desc_10.iFrameSpeed = 7;
	Desc_10.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Left"), Desc_10);

	//Boss_Right
	iter = m_pTextureComs.find(TEXT("Boss_Right"));
	Desc_11.iFrameSpeed = 7;
	Desc_11.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Up->Set_Animation(TEXT("Boss_Right"), Desc_11);

	//Boss_Leg_Direction_NE
	iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_NE"));
	Desc_12.iFrameSpeed = 7;
	Desc_12.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Leg_Direction_NE"), Desc_12);

	//Boss_Leg_Direction_NW
	iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_NW"));
	Desc_13.iFrameSpeed = 7;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Leg_Direction_NW"), Desc_13);

	//Boss_Direction_SE
	iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_SE"));
	Desc_13.iFrameSpeed = 7;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Leg_Direction_SE"), Desc_14);

	//Boss_Direction_SW
	iter = m_pTextureComs.find(TEXT("Boss_Leg_Direction_SW"));
	Desc_13.iFrameSpeed = 7;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Leg_Direction_SW"), Desc_15);

	//Boss_Leg_Front
	iter = m_pTextureComs.find(TEXT("Boss_Front_Leg"));
	Desc_13.iFrameSpeed = 7;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Front_Leg"), Desc_16);

	//Boss_Leg_Back
	iter = m_pTextureComs.find(TEXT("Boss_Back_Leg"));
	Desc_13.iFrameSpeed = 7;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Back_Leg"), Desc_17);

	//Boss_Leg_Left
	iter = m_pTextureComs.find(TEXT("Boss_Left_Leg"));
	Desc_13.iFrameSpeed = 7;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Left_Leg"), Desc_18);

	//Boss_Leg_Right
	iter = m_pTextureComs.find(TEXT("Boss_Right_Leg"));
	Desc_13.iFrameSpeed = 7;
	Desc_13.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom_Down->Set_Animation(TEXT("Boss_Right_Leg"), Desc_19);

	return S_OK;
}

void CBoss::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (eColType == COLLISION::SPHERE)
	{
		CBullet* pBullet = dynamic_cast<CBullet*>(pDst);
		if (pBullet != nullptr)
			m_fHp -= pBullet->Get_Damage();
	}

	return;
}

const COLLISION_DESC& CBoss::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	else if (eColType == COLLISION::OBB)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

HRESULT CBoss::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_1"), reinterpret_cast<CComponent**>(&m_pTransformCom_Up), &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_2"), reinterpret_cast<CComponent**>(&m_pTransformCom_Down), &TransformDesc)))
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

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation_Up"), reinterpret_cast<CComponent**>(&m_pAnimationCom_Up))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation_Down"), reinterpret_cast<CComponent**>(&m_pAnimationCom_Down))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Up"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Up))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Down"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Down))))
		return E_FAIL;

	/* Com_Sight */
	CSight::SIGHT_DESC		SightDesc{ 5.f, D3DXToRadian(90.0f), m_pPlayerTransform, m_pTransformCom };
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

HRESULT CBoss::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	/*
	float4		vSourColor, vDestColor;
	vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);
	*/

	
	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);*/
	
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);



	return S_OK;
}

HRESULT CBoss::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CBoss::Attack()
{
	m_fSumAttackCoolTime = 0.f;
	m_strUpFrameKey = TEXT("Boss_Attack_Front");
	m_bAnimationLock = true;

	_float3 vDir = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
	D3DXVec3Normalize(&vDir, &vDir);

	CBullet::BULLET_DESC Desc;
	Desc.vDir = vDir;
	Desc.vPos = vPos;

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster_Bullet"), &Desc);
}

void CBoss::Move(_float fTimeDelta)
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

void CBoss::Move()
{
	//m_pTransformCom->R
	m_pTransformCom->Get_State(STATE::POSITION);
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

	return pInstance;
}

void CBoss::Free()
{
	//m_pRoot->ReleaseSubtree();
	__super::Free();
	Safe_Release(m_pVIBufferCom_Up);
	Safe_Release(m_pVIBufferCom_Down);
	Safe_Release(m_pAnimationCom_Up);
	Safe_Release(m_pAnimationCom_Down);
	Safe_Release(m_pTransformCom_Up);
	Safe_Release(m_pTransformCom_Down);
}
