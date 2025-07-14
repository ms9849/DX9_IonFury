#include "DoorLock.h"

#include "GameInstance.h"
#include "MapGate.h"
#include "MapDoor.h"
#include "Lever.h"

CDoorLock::CDoorLock(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CDoorLock::CDoorLock(const CDoorLock& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CDoorLock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoorLock::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(STATE::POSITION, _float3(0.f, 0.5f, 10.f));
	//m_pTransformCom->Set_Scale(_float3{ 1.f, 0.6f, 1.f });

	return S_OK;
}

void CDoorLock::Priority_Update(_float fTimeDelta)
{	
}

void CDoorLock::Update(_float fTimeDelta)
{
	if (m_bCanOpen && !m_bOpen)
		m_strFrameKey = TEXT("DoorLock_Unlock");
}

void CDoorLock::Late_Update(_float fTimeDelta)
{
	m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);

	if (m_pAnimationCom->Check_Animation_Finish(TEXT("DoorLock_Unlock")))
	{
		m_bOpen = true;
		if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::GAMEPLAY))
		{
			dynamic_cast<CMapGate*>(
				m_pGameInstance->Get_GameObject_By_ID(
					m_pObjectDesc.iLayerLevel,
					TEXT("Layer_Map_Objects_Gate"),
					m_iTargetID))->Set_Open(true);
		}
		else if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::JUSIN))
		{
			dynamic_cast<CMapDoor*>(
				m_pGameInstance->Get_GameObject_By_ID(
					m_pObjectDesc.iLayerLevel,
					TEXT("Layer_Map_Objects_Gate"),
					m_iTargetID))->Set_Door_Open(true);
		}
		m_strFrameKey = TEXT("DoorLock_Open");
	}

	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CDoorLock::Render()
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

void CDoorLock::Set_TargetID(_uint iTargetID)
{
	m_iTargetID = iTargetID;
}

void CDoorLock::Set_Can_Open(_bool bCanOpen)
{
	m_bCanOpen = bCanOpen;
}

HRESULT CDoorLock::Ready_Components()
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

		wsprintf(strPrototypeTag, TEXT("Prototype_Component_Texture_Interaction_%s"), m_strFrameKeys[i].c_str());
		wsprintf(strComponentTag, TEXT("Com_%s_Texture"), m_strFrameKeys[i].c_str());

		/* Com_Texture */
		if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, strPrototypeTag,
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

	CAnimation::FRAME_DESC DoorLockIdleDesc{};

	DoorLockIdleDesc.iEnd = 0;

	m_pAnimationCom->Set_Animation(TEXT("DoorLock_Idle"), DoorLockIdleDesc);

	CAnimation::FRAME_DESC DoorLockOpenDesc{};

	DoorLockOpenDesc.iEnd = 0;

	m_pAnimationCom->Set_Animation(TEXT("DoorLock_Open"), DoorLockOpenDesc);

	auto iter = m_pTextureComs.find(TEXT("DoorLock_Unlock"));

	CAnimation::FRAME_DESC DoorLockUnlockDesc{};

	DoorLockUnlockDesc.iEnd = iter->second->Get_Texture_Length();
	DoorLockUnlockDesc.iFrameSpeed = 10;

	m_pAnimationCom->Set_Animation(TEXT("DoorLock_Unlock"), DoorLockUnlockDesc);

	return S_OK;
}

HRESULT CDoorLock::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CDoorLock::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CDoorLock::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	/*if (eColType == COLLISION::SPHERE)
		_uint a = 10;*/
}

const COLLISION_DESC& CDoorLock::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

CDoorLock* CDoorLock::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDoorLock* pInstance = new CDoorLock(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoorLock::Clone(void* pArg)
{
	CDoorLock* pInstance = new CDoorLock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDoorLock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoorLock::Free()
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