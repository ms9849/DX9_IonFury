#include "MapGate.h"

#include "GameInstance.h"

CMapGate::CMapGate(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CMapGate::CMapGate(const CMapGate& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CMapGate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapGate::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* Com_SphereCollider*/
	/*if (FAILED(Ready_Collider()))
		return E_FAIL;*/

	return S_OK;
}

void CMapGate::Priority_Update(_float fTimeDelta)
{
}

void CMapGate::Update(_float fTimeDelta)
{
	if(!m_bStop)
		Gate_Animation(fTimeDelta);
}

void CMapGate::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CMapGate::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(m_pObjectDesc.iTextureIndex);

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CMapGate::Set_Open(_bool bOpen)
{
	m_bOpen = bOpen;
}

HRESULT CMapGate::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Map_Gate"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_BoxCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom))))
		return E_FAIL;

	return S_OK;
}
HRESULT CMapGate::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}
HRESULT CMapGate::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CMapGate::Gate_Animation(_float fTimeDelta)
{
	_float3 vGatePos = m_pTransformCom->Get_State(STATE::POSITION);

	if (m_bOpen)
	{
		m_fTimeStack += fTimeDelta;

		if (vGatePos.y > 6.f)
		{
			m_fTimeStack = 0.f;
			m_bStop = true;
		}
		vGatePos.y += (fTimeDelta * 5);
	}

	m_pTransformCom->Set_State(STATE::POSITION, vGatePos);
}

//const COLLISION_DESC& CMapGate::Get_CollisionDesc(COLLISION eColType)
//{
//	COLLISION_DESC Desc;
//	Desc.pCollider = m_pSphereColliderCom;
//	Desc.pTransform = m_pTransformCom;
//
//	return Desc;
//}

void CMapGate::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
}

const COLLISION_DESC& CMapGate::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pCollider = m_pBoxColliderCom;
	Desc.pTransform = m_pTransformCom;
	
	return Desc;
}

CMapGate* CMapGate::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapGate* pInstance = new CMapGate(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapGate::Clone(void* pArg)
{
	CMapGate* pInstance = new CMapGate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapGate::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBoxColliderCom);
}
