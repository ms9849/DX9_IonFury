#include "UICardKey.h"

#include "GameInstance.h"
#include "Player.h"

CUICardKey::CUICardKey(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUICardKey::CUICardKey(const CUICardKey& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUICardKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUICardKey::Initialize(void* pArg)
{
	UIOBJECT_DESC* pTemp = static_cast<UIOBJECT_DESC*>(pArg);

	m_tagDesc.fSizeX = pTemp->fSizeX;
	m_tagDesc.fSizeY = pTemp->fSizeY;
	m_tagDesc.iLayerLevelIndex = pTemp->iLayerLevelIndex;
	m_tagDesc.fX = pTemp->fX;
	m_tagDesc.fY = pTemp->fY;

	if (FAILED(__super::Initialize(&m_tagDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(
		m_pGameInstance->Find_GameObject_ToLayer(
			m_tagDesc.iLayerLevelIndex, TEXT("Layer_Player")));

	return S_OK;
}

void CUICardKey::Priority_Update(_float fTimeDelta)
{
}

void CUICardKey::Update(_float fTimeDelta)
{
	__super::Update_Transform(m_pTransformCom);
}

void CUICardKey::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUICardKey::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	__super::Begin();

	if (m_bCanUseCardKey)
		m_pVIBufferCom->Render();

	__super::End();

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CUICardKey::Set_CardKey()
{
	m_bCanUseCardKey = m_pPlayer->Get_CanUse_CardKey();
}

HRESULT CUICardKey::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_tagDesc.iLayerLevelIndex, TEXT("Prototype_Component_Texture_UI_CardKey"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUICardKey* CUICardKey::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUICardKey* pInstance = new CUICardKey(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUICardKey::Clone(void* pArg)
{
	CUICardKey* pInstance = new CUICardKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUICardKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUICardKey::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
