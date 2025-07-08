#include "Lever.h"

#include "GameInstance.h"
#include "MapElevator.h"

CLever::CLever(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CLever::CLever(const CLever& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CLever::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLever::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(STATE::POSITION, _float3(13.f, 0.5f, 15.f));
	//m_pTransformCom->Set_Scale(_float3{ 1.f, 1.f, 1.f });

	return S_OK;
}

void CLever::Priority_Update(_float fTimeDelta)
{
}

void CLever::Update(_float fTimeDelta)
{
	if (m_bActive &&
		dynamic_cast<CMapElevator*>(
			m_pGameInstance->Get_GameObject_By_ID(
				ENUM_CLASS(LEVEL::GAMEPLAY),
				TEXT("Layer_Map_Objects"),
				m_iTargetID))->Get_Elevator_End())
	{
		Lever_Control(fTimeDelta);
	}
	else
	{
		m_bActive = false;
	}
}

void CLever::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CLever::Render()
{
	m_pTransformCom->Set_Transform();

	if (m_bActive && m_bUp)
		m_pTextureCom->Set_Texture((m_pTextureCom->Get_Texture_Length() - 1) - m_pAnimationCom->Get_Frame_Current_Index(TEXT("Default_Lever")));
	else if (m_bActive && !m_bUp)
		m_pTextureCom->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(TEXT("Default_Lever")));
	else if (!m_bActive && m_bUp)
		m_pTextureCom->Set_Texture(m_pTextureCom->Get_Texture_Length() - 1);
	else if (!m_bActive && !m_bUp)
		m_pTextureCom->Set_Texture(0);

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CLever::Lever_Control(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(TEXT("Default_Lever"), fTimeDelta);

	if (m_pAnimationCom->Check_Animation_Finish(TEXT("Default_Lever")))
	{
		m_bActive = false;
		m_bUp = !m_bUp;
		dynamic_cast<CMapElevator*>(
			m_pGameInstance->Get_GameObject_By_ID(
				ENUM_CLASS(LEVEL::GAMEPLAY),
				TEXT("Layer_Map_Objects"),
				m_iTargetID))->Set_Elevator_Active(true);
	}
}

void CLever::Set_Active(_bool bActive)
{
	m_bActive = bActive;
}

void CLever::Set_TargetID(_uint iTargetID)
{
	m_iTargetID = iTargetID;
}

_bool CLever::Get_Lever_State()
{
	return m_bUp;
}

HRESULT CLever::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Interaction_Default_Lever"),
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

	CAnimation::FRAME_DESC DefaultLeverDesc{};

	DefaultLeverDesc.iEnd = m_pTextureCom->Get_Texture_Length();
	DefaultLeverDesc.iFrameSpeed = 10;
	
	m_pAnimationCom->Set_Animation(TEXT("Default_Lever"), DefaultLeverDesc);

	return S_OK;
}

HRESULT CLever::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CLever::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CLever::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	/*if (eColType == COLLISION::SPHERE)
		_uint a = 10;*/
}

const COLLISION_DESC& CLever::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

CLever* CLever::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLever* pInstance = new CLever(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLever::Clone(void* pArg)
{
	CLever* pInstance = new CLever(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLever::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSphereColliderCom);
}