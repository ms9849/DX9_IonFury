#include "MapElevator.h"

#include "GameInstance.h"
#include "Lever.h"

CMapElevator::CMapElevator(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CMapElevator::CMapElevator(const CMapElevator& Prototype)
	: CGameObject(Prototype)
{
}

/*
오브젝트 아이디 추가해야됨
*/

void CMapElevator::Set_Elevator_Active(_bool bActive)
{
	m_bActive = bActive;
}

_bool CMapElevator::Get_Elevator_End()
{
	return m_bEnd;
}

void CMapElevator::Elevator_Animation(_float fTimeDelta)
{
	_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

	if (m_bUp)
	{
		m_fTimeStack += fTimeDelta;
		m_bEnd = false;
		
		if (vPos.y > 25.f)
		{
			m_fTimeStack = 0.f;
			m_bActive = false;
			m_bUp = false;
			m_bEnd = true;
		}

		if(m_fTimeStack >= 1.f)
			vPos.y += (fTimeDelta * 5);
	}
	else
	{
		m_fTimeStack += fTimeDelta;
		m_bEnd = false;
		if (vPos.y <= 0.5f)
		{
			m_fTimeStack = 0.f;
			m_bActive = false;
			m_bUp = true;
			m_bEnd = true;
		}
		if (m_fTimeStack >= 1.f)
			vPos.y -= (fTimeDelta * 5);
	}

	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

HRESULT CMapElevator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElevator::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(STATE::POSITION, { 15.f, 0.125f, 15.f });
	//m_pTransformCom->Set_Scale(_float3{ 3.f, 0.5f, 3.f });

	return S_OK;
}

void CMapElevator::Priority_Update(_float fTimeDelta)
{
}

void CMapElevator::Update(_float fTimeDelta)
{
	if (m_bActive)
	{
		Elevator_Animation(fTimeDelta);
	}
}

void CMapElevator::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapElevator::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(m_pObjectDesc.iTextureIndex);

	m_pVIBufferCom->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CMapElevator::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Map_Elevator"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Collider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_BoxColliderCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CMapElevator::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
}

const COLLISION_DESC& CMapElevator::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_BoxColliderCom;

	return Desc;
}

CMapElevator* CMapElevator::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapElevator* pInstance = new CMapElevator(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElevator::Clone(void* pArg)
{
	CMapElevator* pInstance = new CMapElevator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElevator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapElevator::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_BoxColliderCom);
}
