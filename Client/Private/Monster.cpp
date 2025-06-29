#include "Monster.h"

#include "GameInstance.h"
#include "Bullet.h"
#include "BehaviorNode.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{

}

CMonster::CMonster(const CMonster& Prototype)
	: CLandObject{ Prototype }
{

}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (m_pPlayerTransform == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(
		m_pGameInstance->Random(0.f, 20.f), 
		0.f, 
		m_pGameInstance->Random(0.f, 20.f)));

	//m_pTransformCom->Rotation({0.f, 1.f, 0.f}, m_pGameInstance->Random(0.f, 180.f));
	//m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

	CSelectorNode* root = new CSelectorNode();

	//SequenceNode* SightSequence = new SequenceNode();
	CSequenceNode* CAttackSequence = new CSequenceNode();
	CAttackSequence->AddChild(new CConditionNode([this](float fTimeDelta) {
		return this->m_pSightCom->Check_Sight(fTimeDelta);
		}));

	CAttackSequence->AddChild(new CConditionNode([this](float fTimeDelta) {
		m_fAccumulation += fTimeDelta;
		return m_fAccumulation >= m_fCoolTime;
		}));

	CAttackSequence->AddChild(new CActionNode([this]() {
		this->Attack();
		m_fAccumulation = 0.f;
		}));

	root->AddChild(CAttackSequence);
	m_pRoot = root;

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	
}

void CMonster::Update(_float fTimeDelta)
{
	m_pRoot->Run(fTimeDelta);

	/*if (m_fAccumulation > m_fCoolTime)
	{
		Attack();
		m_fAccumulation = 0.f;
	}
	else
	{
		m_fAccumulation += fTimeDelta;
	}
	m_pSightCom->Check_Sight(fTimeDelta);*/
	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	vDiff.y = 0.f;
	D3DXVec3Normalize(&vDiff, &vDiff); // 정규화 필수

	_float3 vMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	vMonsterLook.y = 0.f;
	D3DXVec3Normalize(&vMonsterLook, &vMonsterLook);

	// 내적: 각도용
	_float dot = D3DXVec3Dot(&vMonsterLook, &vDiff);
	dot = max(-1.f, min(1.f, dot)); // 안전 보정

	// 외적: 왼쪽/오른쪽 판별
	_float3 vCross;
	D3DXVec3Cross(&vCross, &vMonsterLook, &vDiff);

	// 시야각 90도 (45도 양방향)
	_float fFov = cosf(D3DXToRadian(45.f));

	if (vCross.y > 0)
	{
		// 오른쪽
		if (dot >= fFov)
			m_strFrameKey = TEXT("Soldier_Front");
		else if (dot > 0.f)
			m_strFrameKey = TEXT("Soldier_Attack_Front");
		else
			m_strFrameKey = TEXT("Soldier_Back");
	}
	else
	{
		// 왼쪽
		if (dot >= fFov)
			m_strFrameKey = TEXT("Soldier_Front");
		else if (dot > 0.f)
			m_strFrameKey = TEXT("Soldier_Attack_Front");
		else
			m_strFrameKey = TEXT("Soldier_Back");
	}

	auto iter = m_Frames.find(m_strFrameKey);
	m_pAnimationCom->Set_Animation(&iter->second);

	/*szBuffer[128];
	swprintf_s(szBuffer, L"몬스터 방향 벡터 x : %.1f, y : %.1f, z : %.1f\n", m_pTransformCom->Get_State(STATE::LOOK).x, m_pTransformCom->Get_State(STATE::LOOK).y, m_pTransformCom->Get_State(STATE::LOOK).z);
	OutputDebugString(szBuffer);*/

	SetUp_OnTerrain(m_pTransformCom, 0.5f);
}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(fTimeDelta);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMonster::Render()
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
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Index());

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Ready_Animations()
{
	CAnimation::FRAME_DESC Desc{};

	//Soldier_Attack_Front
	auto iter = m_pTextureComs.find(TEXT("Soldier_Attack_Front"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Attack_Front"), Desc);

	//Soldier_Attack_SE
	iter = m_pTextureComs.find(TEXT("Soldier_Attack_SE"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Attack_SE"), Desc);

	//Soldier_Attack_SW
	iter = m_pTextureComs.find(TEXT("Soldier_Attack_SW"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Attack_SW"), Desc);

	//Soldier_Die_Default
	iter = m_pTextureComs.find(TEXT("Soldier_Die_Default"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Die_Default"), Desc);

	//Soldier_Die_Explosion
	iter = m_pTextureComs.find(TEXT("Soldier_Die_Explosion"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Die_Explosion"), Desc);

	//Soldier_Die_HeadShot
	iter = m_pTextureComs.find(TEXT("Soldier_Die_HeadShot"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Die_HeadShot"), Desc);

	//Soldier_Direction_NE
	iter = m_pTextureComs.find(TEXT("Soldier_Direction_NE"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Direction_NE"), Desc);

	//Soldier_Direction_NW
	iter = m_pTextureComs.find(TEXT("Soldier_Direction_NW"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Direction_NW"), Desc);

	//Soldier_Direction_SE
	iter = m_pTextureComs.find(TEXT("Soldier_Direction_SE"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Direction_SE"), Desc);

	//Soldier_Direction_SW
	iter = m_pTextureComs.find(TEXT("Soldier_Direction_SW"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Direction_SW"), Desc);

	//Soldier_Front
	iter = m_pTextureComs.find(TEXT("Soldier_Front"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Front"), Desc);

	//Soldier_Back
	iter = m_pTextureComs.find(TEXT("Soldier_Back"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Back"), Desc);

	//Soldier_Left
	iter = m_pTextureComs.find(TEXT("Soldier_Left"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Left"), Desc);

	//Soldier_Right
	iter = m_pTextureComs.find(TEXT("Soldier_Right"));
	Desc.iEnd = iter->second->Get_Texture_Length();
	m_Frames.emplace(TEXT("Soldier_Right"), Desc);


	return S_OK;
}

HRESULT CMonster::Ready_Components()
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
		Safe_AddRef(pTextureCom);

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
	CSight::SIGHT_DESC		SightDesc{ 5.f, D3DXToRadian(90.0f), m_pPlayerTransform, m_pTransformCom };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Sight"),
		TEXT("Com_Sight"), reinterpret_cast<CComponent**>(&m_pSightCom), &SightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	/*
	float4		vSourColor, vDestColor;
	vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);
	*/

	//
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);



	return S_OK;
}

HRESULT CMonster::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	// m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CMonster::Attack()
{
	_float3 vDir = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
	_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
	D3DXVec3Normalize(&vDir, &vDir);

	CBullet::BULLET_DESC Desc;
	Desc.vDir = vDir;
	Desc.vPos = vPos;

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Bullet"), &Desc);
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonster* pInstance = new CMonster(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}
}
