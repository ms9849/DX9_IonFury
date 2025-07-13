#include "BossUpperFly.h"
#include "GameInstance.h"

CBossUpperFly::CBossUpperFly(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{

}

CBossUpperFly::CBossUpperFly(const CBossUpperFly& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CBossUpperFly::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossUpperFly::Initialize(void* pArg)
{
	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);
	/*if (FAILED(__super::Initialize()))
		return E_FAIL;*/

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		//m_Desc = *static_cast<Fly_DESC*>(pArg);
		m_pTransformUpperBody = static_cast<CTransform*>(pArg);
	}

	//m_pTransformUpperBody = m_Desc.pTransform;
	Safe_AddRef(m_pTransformUpperBody);
	m_pTransformCom->Set_State(STATE::POSITION, m_pTransformUpperBody->Get_State(STATE::POSITION));
	m_pTransformCom->Set_Scale({ 3.5f, 6.f, 1.f });


	//_float3 r = m_pTransformUpperBody->Get_State(STATE::RIGHT);
	//_float3 u = m_pTransformUpperBody->Get_State(STATE::UP);
	//_float3 l = m_pTransformUpperBody->Get_State(STATE::LOOK);

	//std::wstring debugText =
	//	L"[디버그] RIGHT: " + std::to_wstring(r.x) + L", " + std::to_wstring(r.y) + L", " + std::to_wstring(r.z) + L"\n" +
	//	L"[디버그] UP   : " + std::to_wstring(u.x) + L", " + std::to_wstring(u.y) + L", " + std::to_wstring(u.z) + L"\n" +
	//	L"[디버그] LOOK : " + std::to_wstring(l.x) + L", " + std::to_wstring(l.y) + L", " + std::to_wstring(l.z) + L"\n";

	//// 출력
	//OutputDebugStringW(debugText.c_str());

	return S_OK;
}

void CBossUpperFly::Priority_Update(_float fTimeDelta)
{

}

void CBossUpperFly::Update(_float fTimeDelta)
{
	if (m_pTransformUpperBody == nullptr)
		m_isDead = true;

	_float3 vBasePos = m_pTransformUpperBody->Get_State(STATE::POSITION);
	_float3 vRight = m_pTransformUpperBody->Get_State(STATE::RIGHT);
	vRight.y = 0.f;
	D3DXVec3Normalize(&vRight, &vRight);

	_float3 vOffset = vBasePos + vRight * -0.15f + _float3(0.f, -4.5f, 0.f);
	/*if (!m_isFly)
		vOffset = vBasePos + vRight * -0.15f + _float3(0.f, -4.5f, 0.f);
	else
		vOffset = vBasePos + vRight * -0.15f + _float3(0.f, -4.5f, 0.f);*/


	m_pTransformCom->Set_State(STATE::POSITION, vOffset);
}

void CBossUpperFly::Late_Update(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CBossUpperFly::Render()
{
	if (FAILED(Begin_RenderTestState()))
		return E_FAIL;

	RotateToPlayer(m_pTransformCom);
	auto iter = m_pTextureComs.find(m_strFrameKey);
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));
	m_pVIBufferCom->Render();

	if (FAILED(End_RenderTestState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossUpperFly::Begin_RenderTestState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CBossUpperFly::End_RenderTestState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CBossUpperFly::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.0f) };
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

	return S_OK;
}

HRESULT CBossUpperFly::Ready_Animations()
{
	CAnimation::FRAME_DESC Desc_0{};

	//Boss_Attack_Front
	auto iter = m_pTextureComs.find(TEXT("Boss_FlyLeft"));
	Desc_0.iFrameSpeed = 12;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Boss_FlyLeft"), Desc_0);

	return S_OK;
}

void CBossUpperFly::RotateToPlayer(CTransform* pTranform)
{
	_float4x4 matWorldTemp = m_pGameInstance->Get_CameraWorld();

	_float3 vLook;
	memcpy(&vLook, *(_float3*)&matWorldTemp.m[2][0], sizeof(_float3));
	vLook *= -1;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	_float3 vUp = { 0.f, 1.f, 0.f };

	_float3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	_float3 scale = pTranform->Get_Scaled();

	vRight *= scale.x;
	vUp *= scale.y;
	vLook *= scale.z;

	_float4x4 matWorld = *pTranform->Get_WorldMatrixPtr();
	_float3 vPos = pTranform->Get_State(STATE::POSITION);

	memcpy(&matWorld.m[0][0], &vRight, sizeof(_float3));
	memcpy(&matWorld.m[1][0], &vUp, sizeof(_float3));
	memcpy(&matWorld.m[2][0], &vLook, sizeof(_float3));
	memcpy(&matWorld.m[3][0], &vPos, sizeof(_float3));

	pTranform->Set_Transform(matWorld);

	return;
}

CBossUpperFly* CBossUpperFly::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossUpperFly* pInstance = new CBossUpperFly(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossUpperFly::Clone(void* pArg)
{
	CBossUpperFly* pInstance = new CBossUpperFly(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossUpperFly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossUpperFly::Free()
{
	__super::Free();
	//Safe_Release(m_Desc);
	//Safe_Release(m_pTransformUpperBody);
	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}
	m_pTextureComs.clear();

	Safe_Release(m_pTransformUpperBody);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayerTransform);
}
