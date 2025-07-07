#include "MapGrinder.h"

#include "GameInstance.h"
#include "Player.h"

CMapGrinder::CMapGrinder(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CMapGrinder::CMapGrinder(const CMapGrinder& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CMapGrinder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapGrinder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(0.f, 0.5f, 3.f));
	m_pTransformCom->Set_Scale(_float3{ 1.f, 0.6f, 1.f });

	return S_OK;
}

void CMapGrinder::Priority_Update(_float fTimeDelta)
{
}

void CMapGrinder::Update(_float fTimeDelta)
{
}

void CMapGrinder::Late_Update(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(TEXT("Grinder"), fTimeDelta);

	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CMapGrinder::Render()
{
	m_pTransformCom->Set_Transform();

	// 텍스쳐 컴포넌트로 텍스쳐 렌더링
	m_pTextureCom->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(TEXT("Grinder")));

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapGrinder::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Map_Grinder"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

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

	CAnimation::FRAME_DESC GrinderDesc{};

	GrinderDesc.iEnd = m_pTextureCom->Get_Texture_Length();
	GrinderDesc.iFrameSpeed = 1;

	m_pAnimationCom->Set_Animation(TEXT("Grinder"), GrinderDesc);

	return S_OK;
}

HRESULT CMapGrinder::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMapGrinder::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CMapGrinder::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	/*if (eColType == COLLISION::SPHERE)
		_uint a = 10;*/
}

const COLLISION_DESC& CMapGrinder::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

CMapGrinder* CMapGrinder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapGrinder* pInstance = new CMapGrinder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapGrinder::Clone(void* pArg)
{
	CMapGrinder* pInstance = new CMapGrinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapGrinder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapGrinder::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSphereColliderCom);
}