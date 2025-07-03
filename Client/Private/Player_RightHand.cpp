#include "Player_RightHand.h"
#include "Player.h"

CPlayer_RightHand::CPlayer_RightHand(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{

}

CPlayer_RightHand::CPlayer_RightHand(const CPlayer_RightHand& Prototype)
	: CGameObject(Prototype)
{

}

HRESULT CPlayer_RightHand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_RightHand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_RightHand::Priority_Update(_float fTimeDelta)
{
	// 여기서 플레이어한테서 받아온 애니메이션 타입에 맞춰 애니메이션 세팅
	// Set_Animation

	_float3 vHandPos = {};
	_float4x4 PlayerMatrix{};

	_wstring strWeapon = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")))->Get_Player_Info().strWeapon;

	_wstring strAction = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")))->Get_Player_Info().strAction;

	if (strWeapon.compare(TEXT("Pistol")) == 0)
	{
		if(strAction.compare(TEXT("Up")) == 0)
			vHandPos = { 0.75f, -1.4f, 1.5f };
		else
			vHandPos = { 0.75f, -0.5f, 1.5f };
	}
	else if (strWeapon.compare(TEXT("ShootGun")) == 0)
	{
		if (strAction.compare(TEXT("Up")) == 0)
			vHandPos = { 0.5f, -1.5f, 1.5f };
		else
			vHandPos = { 0.5f, -0.6f, 1.5f };
	}

	if (m_pAnimationCom->Get_Frame_Desc(m_strFrameKey)->Poses.size() > 1)
	{
		vHandPos += m_pAnimationCom->Get_Frame_Desc(m_strFrameKey)->Poses[m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey)];
	}

	PlayerMatrix = *dynamic_cast<CTransform*>(
		m_pGameInstance->Get_Component(
			ENUM_CLASS(LEVEL::GAMEPLAY),
			TEXT("Layer_Player"),
			TEXT("Com_Transform")))->Get_WorldMatrixPtr();

	D3DXVec3TransformCoord(&vHandPos, &vHandPos, &PlayerMatrix);

	m_pTransformCom->Set_State(STATE::RIGHT, *(_float3*)&PlayerMatrix.m[0][0]);
	m_pTransformCom->Set_State(STATE::UP, *(_float3*)&PlayerMatrix.m[1][0]);
	m_pTransformCom->Set_State(STATE::LOOK, *(_float3*)&PlayerMatrix.m[2][0]);
	m_pTransformCom->Set_State(STATE::POSITION, vHandPos);

	if (strWeapon.compare(TEXT("Pistol")) == 0)
	{
		m_pTransformCom->Set_Scale(_float3{ 1.f, 1.f, 1.f });
	}
	else if (strWeapon.compare(TEXT("ShootGun")) == 0)
	{
		m_pTransformCom->Set_Scale(_float3{ 1.5f, 1.f, 1.f });
	}
	m_pAnimationCom->Play_Animation(m_strFrameKey, m_fPlayerTimeDelta);
}

void CPlayer_RightHand::Update(_float fTimeDelta)
{
}

void CPlayer_RightHand::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND_LATE, this);
}

HRESULT CPlayer_RightHand::Render()
{
 	m_pTransformCom->Set_Transform();

	// 텍스쳐 컴포넌트로 텍스쳐 렌더링
	auto iter = m_pTextureComs.find(m_strFrameKey);
	if(iter->second->Get_Texture_Length() == 1)
		iter->second->Set_Texture(0);
	else
		iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_RightHand::Set_Player_Transform(CTransform* pTransform)
{
	m_pPlayerTransformCom = pTransform;
}

void CPlayer_RightHand::Set_Current_Animation(const _wstring& strFrameKey)
{
	m_strFrameKey = strFrameKey;
}

void CPlayer_RightHand::Set_Player_TimeDelta(_float fTimeDelta)
{
	m_fPlayerTimeDelta = fTimeDelta;
}

HRESULT CPlayer_RightHand::Ready_Components()
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

HRESULT CPlayer_RightHand::Ready_Animations()
{
	// 여기서 벡터로 <구조체> 들고 있고 한 애니메이션 마다 미리 싹 세팅
	auto iter = m_pTextureComs.find(TEXT("Pistol_Idle"));
#pragma region 권총

	//Pistol_Idle
	CAnimation::FRAME_DESC PistolIdleDesc{};
	PistolIdleDesc.iEnd = 0;
	m_pAnimationCom->Set_Animation(TEXT("Pistol_Idle"), PistolIdleDesc);
	
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

	m_pAnimationCom->Set_Animation(TEXT("Pistol_Walk"), PistolWalkDesc);

	//Pistol_Down
	CAnimation::FRAME_DESC PistolDownDesc{};
	iter = m_pTextureComs.find(TEXT("Pistol_Down"));
	PistolDownDesc.iEnd = 10;
	PistolDownDesc.iFrameSpeed = 2;
	PistolDownDesc.Poses.reserve(PistolDownDesc.iEnd);

	PistolDownDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.1f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.2f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.3f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.4f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.5f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.6f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.7f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.8f, 0.f });
	PistolDownDesc.Poses.push_back(_float3{ 0.f, -0.9f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("Pistol_Down"), PistolDownDesc);

	//Pistol_Up
	CAnimation::FRAME_DESC PistolUpDesc{};
	iter = m_pTextureComs.find(TEXT("Pistol_Up"));
	PistolUpDesc.iEnd = 10;
	PistolUpDesc.iFrameSpeed = 2;
	PistolUpDesc.Poses.reserve(PistolUpDesc.iEnd);

	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.1f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.2f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.3f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.4f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.5f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.6f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.7f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.8f, 0.f });
	PistolUpDesc.Poses.push_back(_float3{ 0.f, 0.9f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("Pistol_Up"), PistolUpDesc);

	// Pistol_Reload
	CAnimation::FRAME_DESC PistolReloadDesc{};
	iter = m_pTextureComs.find(TEXT("Pistol_Reload"));
	PistolReloadDesc.iEnd = iter->second->Get_Texture_Length();
	PistolReloadDesc.Poses.reserve(PistolReloadDesc.iEnd);
	
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.3f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.6f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.9f, 0.f });

	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -1.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -1.f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -1.f, 0.f });

	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.9f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.6f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.3f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.0f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.0f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.0f, 0.f });
	PistolReloadDesc.Poses.push_back(_float3{ 0.f, -0.0f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("Pistol_Reload"), PistolReloadDesc);

	// Pistol_Shoot
	CAnimation::FRAME_DESC PistolShootDesc{};
	iter = m_pTextureComs.find(TEXT("Pistol_Shoot"));
	PistolShootDesc.iEnd = iter->second->Get_Texture_Length();
	PistolShootDesc.iFrameSpeed = 4;
	PistolShootDesc.Poses.reserve(PistolShootDesc.iEnd);

	PistolShootDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	PistolShootDesc.Poses.push_back(_float3{ 0.f, 0.05f, -0.35f });
	PistolShootDesc.Poses.push_back(_float3{ 0.f, 0.1f, -0.7f });
	PistolShootDesc.Poses.push_back(_float3{ 0.f, 0.05f, -0.35f });
	PistolShootDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("Pistol_Shoot"), PistolShootDesc);
#pragma endregion

#pragma region 샷건
	//ShootGun_Idle
	iter = m_pTextureComs.find(TEXT("ShootGun_Idle"));
	CAnimation::FRAME_DESC ShootGunIdleDesc{};
	ShootGunIdleDesc.iEnd = 0;
	m_pAnimationCom->Set_Animation(TEXT("ShootGun_Idle"), ShootGunIdleDesc);

	//ShootGun_Walk
	CAnimation::FRAME_DESC ShootGunWalkDesc{};
	iter = m_pTextureComs.find(TEXT("ShootGun_Walk"));
	ShootGunWalkDesc.iEnd = 9;
	ShootGunWalkDesc.Poses.reserve(ShootGunWalkDesc.iEnd);

	ShootGunWalkDesc.Poses.push_back(_float3{ -0.05f, 0.05f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.1f, 0.075f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.15f, 0.1f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.2f, 0.075f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.25f, 0.05f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.2f, 0.075f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.15f, 0.1f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.1f, 0.075f, 0.f });
	ShootGunWalkDesc.Poses.push_back(_float3{ -0.05f, 0.05f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("ShootGun_Walk"), ShootGunWalkDesc);

	//ShootGun_Down
	CAnimation::FRAME_DESC ShootGunDownDesc{};
	iter = m_pTextureComs.find(TEXT("ShootGun_Down"));
	ShootGunDownDesc.iEnd = 10;
	ShootGunDownDesc.iFrameSpeed = 2;
	ShootGunDownDesc.Poses.reserve(ShootGunDownDesc.iEnd);

	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.1f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.2f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.3f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.4f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.5f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.6f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.7f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.8f, 0.f });
	ShootGunDownDesc.Poses.push_back(_float3{ 0.f, -0.9f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("ShootGun_Down"), ShootGunDownDesc);

	//ShootGun_Up
	CAnimation::FRAME_DESC ShootGunUpDesc{};
	iter = m_pTextureComs.find(TEXT("ShootGun_Up"));
	ShootGunUpDesc.iEnd = 10;
	ShootGunUpDesc.iFrameSpeed = 2;
	ShootGunUpDesc.Poses.reserve(ShootGunUpDesc.iEnd);

	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.1f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.2f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.3f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.4f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.5f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.6f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.7f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.8f, 0.f });
	ShootGunUpDesc.Poses.push_back(_float3{ 0.f, 0.9f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("ShootGun_Up"), ShootGunUpDesc);

	// ShootGun_Reload
	CAnimation::FRAME_DESC ShootGunReloadDesc{};
	iter = m_pTextureComs.find(TEXT("ShootGun_Reload"));
	ShootGunReloadDesc.iEnd = iter->second->Get_Texture_Length();
	ShootGunReloadDesc.iFrameSpeed = 4;

	m_pAnimationCom->Set_Animation(TEXT("ShootGun_Reload"), ShootGunReloadDesc);

	// ShootGun_Shoot
	CAnimation::FRAME_DESC ShootGunShootDesc{};
	iter = m_pTextureComs.find(TEXT("ShootGun_Shoot"));
	ShootGunShootDesc.iEnd = iter->second->Get_Texture_Length();
	ShootGunShootDesc.iFrameSpeed = 2;

	ShootGunShootDesc.Poses.reserve(PistolShootDesc.iEnd);

	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.01f, -0.15f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.02f, -0.3f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.03f, -0.45f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.04f, -0.6f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.05f, -0.7f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.04f, -0.6f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.03f, -0.45f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.02f, -0.3f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.01f, -0.15f });
	ShootGunShootDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("ShootGun_Shoot"), ShootGunShootDesc);
#pragma endregion


	return S_OK;
}

HRESULT CPlayer_RightHand::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);

	return S_OK;
}

HRESULT CPlayer_RightHand::End_RenderState()
{
	// m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

CPlayer_RightHand* CPlayer_RightHand::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer_RightHand* pInstance = new CPlayer_RightHand(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_RightHand::Clone(void* pArg)
{
	CPlayer_RightHand* pInstance = new CPlayer_RightHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_RightHand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_RightHand::Free()
{
	__super::Free();

	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}

	m_pTextureComs.clear();
}
