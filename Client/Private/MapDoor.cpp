#include "MapDoor.h"

#include "GameInstance.h"
#include "Button.h"

CMapDoor::CMapDoor(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CMapDoor::CMapDoor(const CMapDoor& Prototype)
	: CGameObject(Prototype)
{
}

/*
오브젝트 아이디 추가해야됨
*/

void CMapDoor::Set_Door_Open(_bool bOpen)
{
	m_bOpen = bOpen;
}

_bool CMapDoor::Get_Door_Open()
{
	return m_bOpen;
}

_bool CMapDoor::Get_Door_End()
{
	return m_bEnd;
}

void CMapDoor::Door_Animation(_float fTimeDelta)
{
	_float3 vDoorPos = m_pTransformCom->Get_State(STATE::POSITION);

	m_fTimeStack += fTimeDelta;

	if (vDoorPos.z > (m_pObjectDesc.matWorld.m[3][2] + m_pTransformCom->Get_Scaled().x))
	{
		m_fTimeStack = 0.f;
		m_bOpen = false;
		m_bEnd = true;
	}
	vDoorPos.z += (fTimeDelta * 5);

	m_pTransformCom->Set_State(STATE::POSITION, vDoorPos);
}

HRESULT CMapDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapDoor::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CMapDoor::Priority_Update(_float fTimeDelta)
{
}

void CMapDoor::Update(_float fTimeDelta)
{
	if (m_bOpen)
	{
		Door_Animation(fTimeDelta);
	}
}

void CMapDoor::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapDoor::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(m_pObjectDesc.iTextureIndex);

	m_pVIBufferCom->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CMapDoor::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Map_Door"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Collider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CMapDoor::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
}

const COLLISION_DESC& CMapDoor::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

CMapDoor* CMapDoor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapDoor* pInstance = new CMapDoor(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapDoor::Clone(void* pArg)
{
	CMapDoor* pInstance = new CMapDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapDoor::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBoxColliderCom);
}
