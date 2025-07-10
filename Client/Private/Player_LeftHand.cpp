#include "Player_LeftHand.h"
#include "Player.h"

CPlayer_LeftHand::CPlayer_LeftHand(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{

}

CPlayer_LeftHand::CPlayer_LeftHand(const CPlayer_LeftHand& Prototype)
	: CGameObject( Prototype )
{

}

HRESULT CPlayer_LeftHand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_LeftHand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_LeftHand::Priority_Update(_float fTimeDelta)
{
	_float3 vHandPos = {};
	_float4x4 PlayerMatrix{};

	_wstring strItem = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")))->Get_Player_Info().strItem;
	_wstring strItemAction = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")))->Get_Player_Info().strItemAction;

	if (strItem.compare(TEXT("CardKey")) == 0)
	{
		if (strItemAction.compare(TEXT("Down")) == 0)
			vHandPos = { -0.75f, -0.5f, 1.5f };
		else if(strItemAction.compare(TEXT("Up")) == 0)
			vHandPos = { -0.75f, -1.4f, 1.5f };
		else
			vHandPos = { -0.75f, -2.5f, 1.5f };
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

	if (strItem.compare(TEXT("CardKey")) == 0)
	{
		m_pTransformCom->Set_Scale(_float3{ 0.5f, 1.f, 1.f });
	}
	// 애니메이션 프레임 증가
	m_pAnimationCom->Play_Animation(m_strFrameKey, m_fPlayerTimeDelta);
}

void CPlayer_LeftHand::Update(_float fTimeDelta)
{
}

void CPlayer_LeftHand::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND_LATE, this);
}

HRESULT CPlayer_LeftHand::Render()
{
	m_pTransformCom->Set_Transform();

	// 텍스쳐 컴포넌트로 텍스쳐 렌더링
	// 텍스쳐는 MainApp에서 STATIC으로 세팅
	auto iter = m_pTextureComs.find(m_strFrameKey);
	if (iter->second->Get_Texture_Length() == 1)
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

void CPlayer_LeftHand::Set_Player_Transform(CTransform* pTransform)
{
	m_pPlayerTransformCom = pTransform;
}

void CPlayer_LeftHand::Set_Current_Animation(const _wstring& strFrameKey)
{
	m_strFrameKey = strFrameKey;
}

void CPlayer_LeftHand::Set_Player_TimeDelta(_float fTimeDelta)
{
	m_fPlayerTimeDelta = fTimeDelta;
}

HRESULT CPlayer_LeftHand::Ready_Components()
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

		CTexture* pTextureCom{ nullptr };

		wsprintf(strPrototypeTag, TEXT("Prototype_Component_Texture_Player_LeftHand_%s"), m_strFrameKeys[i].c_str());
		wsprintf(strComponentTag, TEXT("Com_LeftHand_%s_Texture"), m_strFrameKeys[i].c_str());

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

HRESULT CPlayer_LeftHand::Ready_Animations()
{
	// 여기서 벡터로 <구조체> 들고 있고 한 애니메이션 마다 미리 싹 세팅
	auto iter = m_pTextureComs.find(TEXT("CardKey_Idle"));
#pragma region 카드키
	//CardKey_Idle
	CAnimation::FRAME_DESC CardKeyIdleDesc{};
	CardKeyIdleDesc.iEnd = 0;

	m_pAnimationCom->Set_Animation(TEXT("CardKey_Idle"), CardKeyIdleDesc);

	//CardKey_Up
	CAnimation::FRAME_DESC CardKeyUpDesc{};
	iter = m_pTextureComs.find(TEXT("CardKey_Up"));
	CardKeyUpDesc.iEnd = 10;
	CardKeyUpDesc.iFrameSpeed = 2;
	CardKeyUpDesc.Poses.reserve(CardKeyUpDesc.iEnd);

	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.1f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.2f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.3f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.4f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.5f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.6f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.7f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.8f, 0.f });
	CardKeyUpDesc.Poses.push_back(_float3{ 0.f, 0.9f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("CardKey_Up"), CardKeyUpDesc);

	//CardKey_Down
	CAnimation::FRAME_DESC CardKeyDownDesc{};
	iter = m_pTextureComs.find(TEXT("CardKey_Down"));
	CardKeyDownDesc.iEnd = 10;
	CardKeyDownDesc.iFrameSpeed = 2;
	CardKeyDownDesc.Poses.reserve(CardKeyDownDesc.iEnd);

	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, 0.f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.1f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.2f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.3f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.4f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.5f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.6f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.7f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.8f, 0.f });
	CardKeyDownDesc.Poses.push_back(_float3{ 0.f, -0.9f, 0.f });

	m_pAnimationCom->Set_Animation(TEXT("CardKey_Down"), CardKeyDownDesc);
#pragma endregion

	return S_OK;
}

HRESULT CPlayer_LeftHand::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);

	return S_OK;
}

HRESULT CPlayer_LeftHand::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

CPlayer_LeftHand* CPlayer_LeftHand::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer_LeftHand* pInstance = new CPlayer_LeftHand(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_LeftHand::Clone(void* pArg)
{
	CPlayer_LeftHand* pInstance = new CPlayer_LeftHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_LeftHand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_LeftHand::Free()
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
