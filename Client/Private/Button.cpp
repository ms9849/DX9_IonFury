#include "Button.h"

#include "GameInstance.h"
#include "MapDoor.h"

CButton::CButton(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CButton::CButton(const CButton& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CButton::Priority_Update(_float fTimeDelta)
{
}

void CButton::Update(_float fTimeDelta)
{
	if (m_bActive && !dynamic_cast<CMapDoor*>(
		m_pGameInstance->Get_GameObject_By_ID(
			m_pObjectDesc.iLayerLevel,
			TEXT("Layer_Map_Objects_Gate"),
			m_iTargetID))->Get_Door_End())
	{
		Button_Control(fTimeDelta);
	}
	else
	{
		m_bActive = false;
	}
}

void CButton::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CButton::Render()
{
	m_pTransformCom->Set_Transform();

	if (m_bActive && m_bOpen)
		m_pTextureCom->Set_Texture(2);
	else if (m_bActive && !m_bOpen)
		m_pTextureCom->Set_Texture(1);
	else if (!m_bActive && !m_bOpen)
		m_pTextureCom->Set_Texture(0);
	else 
		m_pTextureCom->Set_Texture(0);

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CButton::Button_Control(_float fTimeDelta)
{
	//m_bActive = false;
	m_bOpen = true;
	dynamic_cast<CMapDoor*>(
		m_pGameInstance->Get_GameObject_By_ID(
			m_pObjectDesc.iLayerLevel,
			TEXT("Layer_Map_Objects_Gate"),
			m_iTargetID))->Set_Door_Open(true);
}

void CButton::Set_Active(_bool bActive)
{
	m_bActive = bActive;
}

void CButton::Set_TargetID(_uint iTargetID)
{
	m_iTargetID = iTargetID;
}

HRESULT CButton::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Interaction_Button"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CButton::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CButton::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CButton::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	/*if (eColType == COLLISION::SPHERE)
		_uint a = 10;*/
}

const COLLISION_DESC& CButton::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

CButton* CButton::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CButton* pInstance = new CButton(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CButton::Clone(void* pArg)
{
	CButton* pInstance = new CButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CButton::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSphereColliderCom);
}