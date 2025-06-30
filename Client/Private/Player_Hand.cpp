#include "Player_Hand.h"
#include "Player.h"

CPlayer_Hand::CPlayer_Hand(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{

}

CPlayer_Hand::CPlayer_Hand(const CPlayer_Hand& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPlayer_Hand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Hand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;


	return S_OK;
}

void CPlayer_Hand::Priority_Update(_float fTimeDelta)
{
}

void CPlayer_Hand::Update(_float fTimeDelta)
{
	// 여기서 플레이어한테서 받아온 애니메이션 타입에 맞춰 애니메이션 세팅
	// Set_Animation
	auto iter = m_Frames.find(m_strFrameKey);
	m_pAnimationCom->Set_Animation(&iter->second);
}

void CPlayer_Hand::Late_Update(_float fTimeDelta)
{

	/*
		// 공부 해야함
	// 크자이공부 중 이공부 만 한거
	_float4x4 handMatrix{}, CameraMatrix{};
	D3DXMatrixIdentity(&handMatrix);

	//handMatrix = *m_pTransformCom->Get_WorldMatrixPtr();

	_float3 vHandMotionPos = m_pAnimationCom->Get_Animation()->Poses[m_pAnimationCom->Get_Frame_Index()];

	handMatrix.m[3][0] = 0.75f + vHandMotionPos.x;
	handMatrix.m[3][1] = -0.6f + vHandMotionPos.y;
	handMatrix.m[3][2] = 1.5f + vHandMotionPos.z;

	//CameraMatrix = *m_pPlayerTransformCom->Get_WorldMatrixPtr();
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &CameraMatrix);
	D3DXMatrixInverse(&CameraMatrix, nullptr, &CameraMatrix);
	_float3 vCameraPos = m_pPlayerTransformCom->Get_State(STATE::POSITION);
	CameraMatrix.m[3][0] = 0.f;
	CameraMatrix.m[3][1] = 0.f;
	CameraMatrix.m[3][2] = 0.f;

	D3DXMatrixMultiply(&handMatrix, &handMatrix, &CameraMatrix);

	_float3 vHandPos = *(_float3*)(&handMatrix.m[3][0]);
	vHandPos.x += vCameraPos.x;
	vHandPos.y += vCameraPos.y;
	vHandPos.z += vCameraPos.z;                               

	m_pTransformCom->Set_State(STATE::RIGHT, (_float3)&handMatrix.m[0][0]);
	m_pTransformCom->Set_State(STATE::UP, (_float3)&handMatrix.m[1][0]);
	m_pTransformCom->Set_State(STATE::LOOK, (_float3)&handMatrix.m[2][0]);
	m_pTransformCom->Set_State(STATE::POSITION, vHandPos);
	*/

	_float3 vHandPos = {};
	_float4x4 CameraMatrix{};

	//handMatrix = *m_pTransformCom->Get_WorldMatrixPtr();

	vHandPos = { 0.75f, -0.5f, 1.5f };
	if (m_pAnimationCom->Get_Animation()->Poses.size() > 1)
	{
		vHandPos += m_pAnimationCom->Get_Animation()->Poses[m_pAnimationCom->Get_Frame_Index()];
	}

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &CameraMatrix);
	D3DXMatrixInverse(&CameraMatrix, nullptr, &CameraMatrix);
	
	D3DXVec3TransformCoord(&vHandPos, &vHandPos, &CameraMatrix);

	m_pTransformCom->Set_State(STATE::RIGHT, *(_float3 *)&CameraMatrix.m[0][0]);
	m_pTransformCom->Set_State(STATE::UP, *(_float3 *)&CameraMatrix.m[1][0]);
	m_pTransformCom->Set_State(STATE::LOOK, *(_float3 *)&CameraMatrix.m[2][0]);
	m_pTransformCom->Set_State(STATE::POSITION, vHandPos);

	// 애니메이션 프레임 증가
	m_pAnimationCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::BLEND_PRIORITY, this);
}

HRESULT CPlayer_Hand::Render()
{
	m_pTransformCom->Set_Transform();

	// 텍스쳐 컴포넌트로 텍스쳐 렌더링
	// 텍스쳐는 MainApp에서 STATIC으로 세팅
	auto iter = m_pTextureComs.find(m_strFrameKey);
	if(iter->second->Get_Texture_Length() == 1)
		iter->second->Set_Texture(0);
	else
		iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Index());

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Hand::Set_Player_Transform(CTransform* pTransform)
{
	m_pPlayerTransformCom = pTransform;
}

void CPlayer_Hand::Set_Current_Animation(const _wstring& strFrameKey)
{
	m_strFrameKey = strFrameKey;
}

HRESULT CPlayer_Hand::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	for (size_t i = 0; i < (sizeof(m_strFrameKeys) / sizeof(m_strFrameKeys[0])); ++i)
	{
		_tchar strPrototypeTag[256];
		_tchar strComponentTag[256];

		CTexture* pTextureCom{nullptr};

		wsprintf(strPrototypeTag, TEXT("Prototype_Component_Texture_Player_%s"), m_strFrameKeys[i].c_str());
		wsprintf(strComponentTag, TEXT("Com_%s_Texture"), m_strFrameKeys[i].c_str());

		/* Com_Texture */
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

	return S_OK;
}

HRESULT CPlayer_Hand::Ready_Animations()
{
	// 여기서 벡터로 <구조체> 들고 있고 한 애니메이션 마다 미리 싹 세팅
	auto iter = m_pTextureComs.find(TEXT("Pistol_Idle"));

	//Pistol_Idle
	CAnimation::FRAME_DESC PistolIdleDesc{};
	PistolIdleDesc.iEnd = 0;
	m_Frames.emplace(TEXT("Pistol_Idle"), PistolIdleDesc);
	
	//Pistol_Walk
	CAnimation::FRAME_DESC PistolWalkDesc{};
	iter = m_pTextureComs.find(TEXT("Pistol_Walk"));
	PistolWalkDesc.iEnd = 9;
	PistolWalkDesc.Poses.reserve(PistolWalkDesc.iEnd);

	PistolWalkDesc.Poses.push_back(_float3{ -0.05f, 0.05f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.1f, 0.075f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.15f, 0.1f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.2f, 0.075f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.25f, 0.05f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.2f, 0.075f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.15f, 0.1f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.1f, 0.075f, 0.f });
	PistolWalkDesc.Poses.push_back(_float3{ -0.05f, 0.05f, 0.f });

	m_Frames.emplace(TEXT("Pistol_Walk"), PistolWalkDesc);

	// Pistol_Reload
	CAnimation::FRAME_DESC PistolReloadDesc{};
	iter = m_pTextureComs.find(TEXT("Pistol_Reload"));
	PistolReloadDesc.iEnd = iter->second->Get_Texture_Length();
	PistolReloadDesc.Poses.reserve(PistolReloadDesc.iEnd);
	
	PistolReloadDesc.Poses.push_back(_float3{0.f, 0.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, 0.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, 0.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, 0.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, 0.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, 0.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -0.3f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -0.6f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -0.9f, 0.f});

	PistolReloadDesc.Poses.push_back(_float3{0.f, -1.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -1.f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -1.f, 0.f});

	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.9f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.6f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.3f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{0.f, -0.0f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -0.0f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -0.0f, 0.f});
	PistolReloadDesc.Poses.push_back(_float3{0.f, -0.0f, 0.f});

	m_Frames.emplace(TEXT("Pistol_Reload"), PistolReloadDesc);

	// Pistol_Shoot
	CAnimation::FRAME_DESC PistolShootDesc{};
	iter = m_pTextureComs.find(TEXT("Pistol_Shoot"));
	PistolShootDesc.iEnd = iter->second->Get_Texture_Length();
	PistolShootDesc.iFrameSpeed = 4;
	m_Frames.emplace(TEXT("Pistol_Shoot"), PistolShootDesc);

	return S_OK;
}

HRESULT CPlayer_Hand::Begin_RenderState()
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

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);


	return S_OK;
}

HRESULT CPlayer_Hand::End_RenderState()
{
	// m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

CPlayer_Hand* CPlayer_Hand::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer_Hand* pInstance = new CPlayer_Hand(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Hand::Clone(void* pArg)
{
	CPlayer_Hand* pInstance = new CPlayer_Hand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Hand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Hand::Free()
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

	m_pTextureComs.clear();
}
