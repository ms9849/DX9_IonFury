#include "MapFan.h"

#include "GameInstance.h"

CMapFan::CMapFan(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CMapFan::CMapFan(const CMapFan& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CMapFan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapFan::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(0.f, 0.5f, 10.f));
	m_pTransformCom->Set_Scale(_float3{ 1.f, 0.6f, 1.f });

	return S_OK;
}

void CMapFan::Priority_Update(_float fTimeDelta)
{
}

void CMapFan::Update(_float fTimeDelta)
{
}

void CMapFan::Late_Update(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);

	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CMapFan::Render()
{
	m_pTransformCom->Set_Transform();

	// 텍스쳐 컴포넌트로 텍스쳐 렌더링
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

HRESULT CMapFan::Ready_Components()
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

		wsprintf(strPrototypeTag, TEXT("Prototype_Component_Texture_Map_%s"), m_strFrameKeys[i].c_str());
		wsprintf(strComponentTag, TEXT("Com_%s_Texture"), m_strFrameKeys[i].c_str());

		/* Com_Texture */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), strPrototypeTag,
			strComponentTag, reinterpret_cast<CComponent**>(&pTextureCom))))
			return E_FAIL;

		m_pTextureComs.emplace(m_strFrameKeys[i], pTextureCom);
	}

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Animation */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimationCom))))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	CAnimation::FRAME_DESC DefaultFanIdleDesc{};

	DefaultFanIdleDesc.iEnd = 0;

	m_pAnimationCom->Set_Animation(TEXT("Default_Fan_Idle"), DefaultFanIdleDesc);

	auto iter = m_pTextureComs.find(TEXT("Default_Fan_Active"));

	CAnimation::FRAME_DESC DefaultFanActiveDesc{};

	DefaultFanActiveDesc.iEnd = iter->second->Get_Texture_Length();
	DefaultFanActiveDesc.iFrameSpeed = 1;

	m_pAnimationCom->Set_Animation(TEXT("Default_Fan_Active"), DefaultFanActiveDesc);

	CAnimation::FRAME_DESC BloodFanIdleDesc{};

	BloodFanIdleDesc.iEnd = 0;

	m_pAnimationCom->Set_Animation(TEXT("Blood_Fan_Idle"), BloodFanIdleDesc);

	iter = m_pTextureComs.find(TEXT("Blood_Fan_Active"));

	CAnimation::FRAME_DESC BloodFanActiveDesc{};

	BloodFanActiveDesc.iEnd = iter->second->Get_Texture_Length();
	BloodFanActiveDesc.iFrameSpeed = 1;

	m_pAnimationCom->Set_Animation(TEXT("Blood_Fan_Active"), BloodFanActiveDesc);

	return S_OK;
}

HRESULT CMapFan::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMapFan::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CMapFan::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	/*if (eColType == COLLISION::SPHERE)
		_uint a = 10;*/
}

const COLLISION_DESC& CMapFan::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

CMapFan* CMapFan::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapFan* pInstance = new CMapFan(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapFan::Clone(void* pArg)
{
	CMapFan* pInstance = new CMapFan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapFan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapFan::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSphereColliderCom);

	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}

	m_pTextureComs.clear();
}