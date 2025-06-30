#include "Item.h"

#include "GameInstance.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CItem::CItem(const CItem& Prototype)
	: CLandObject{ Prototype }
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	/* Com_SphereCollider*/
	if (FAILED(Ready_Collider()))
		return E_FAIL;

	return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
}
void CItem::Update(_float fTimeDelta)
{
}
void CItem::Late_Update(_float fTimeDelta)
{
}
HRESULT CItem::Render()
{
	return S_OK;
}

HRESULT CItem::Ready_Components()
{
	return S_OK;
}
HRESULT CItem::Begin_RenderState()
{
	return S_OK;
}
HRESULT CItem::End_RenderState()
{
	return S_OK;
}

void CItem::Item_Animation(_float fTimeDelta)
{
	_float3 vItemPos = m_pTransformCom->Get_State(STATE::POSITION);

	if (_isGoUp)
	{
		vItemPos.y += (fTimeDelta / 5);

		if (vItemPos.y > m_fItemOriginPosY + 0.25f)
			_isGoUp = false;
	}
	else
	{
		vItemPos.y -= (fTimeDelta / 5);

		if (vItemPos.y < m_fItemOriginPosY)
			_isGoUp = true;
	}

	m_pTransformCom->Set_State(STATE::POSITION, vItemPos);
}

HRESULT CItem::Ready_Collider()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

const COLLISION_DESC& CItem::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pCollider = m_pSphereColliderCom;
	Desc.pTransform = m_pTransformCom;

	return Desc;
}

void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSphereColliderCom);
}
