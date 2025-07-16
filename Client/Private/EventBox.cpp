#include "EventBox.h"

#include "GameInstance.h"
#include "Player.h"
#include "Level_Loading.h"

CEventBox::CEventBox(LPDIRECT3DDEVICE9 pGraphicDev) :
	CGameObject { pGraphicDev }
{
}

CEventBox::CEventBox(const CGameObject& Prototype) :
	CGameObject { Prototype }
{
}

HRESULT CEventBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventBox::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(STATE::POSITION, _float3{ 5.f, 3.f, 5.f });

	return S_OK;
}

void CEventBox::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CEventBox::Update(_float fTimeDelta)
{
	int a = 10;
}

void CEventBox::Late_Update(_float fTimeDelta)
{
	int a = 10;
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CEventBox::Render()
{
	m_pTransformCom->Set_Transform();

	m_pBoxColliderCom->Render(m_pTransformCom->Get_State(STATE::POSITION));

	return S_OK;
}

HRESULT CEventBox::Ready_Components()
{
	/* Com_Transform */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
		return E_FAIL;

	CBoxCollider::BOXCOLLIDER_DESC Desc;
	Desc.vPosition = { -0.1f, -0.07f, -0.1f };
	Desc.fScaleX = 7.0f;
	Desc.fScaleZ = 5.0f;
	Desc.fScaleY = 1.0f;

	/* Com_BoxCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEventBox::Begin_RenderState()
{
	return S_OK;
}

HRESULT CEventBox::End_RenderState()
{
	return S_OK;
}

void CEventBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	// 여기서 충돌 로직이 나와야 한다.
	CPlayer::PLAYER_INFO Desc{};

	if (m_pGameInstance->Get_CurrentLevelID() == ENUM_CLASS(LEVEL::GAMEPLAY))
	{
		Desc = dynamic_cast<CPlayer*>(
			m_pGameInstance->Find_GameObject_ToLayer(
				ENUM_CLASS(LEVEL::GAMEPLAY),
				TEXT("Layer_Player")))->Get_Player_Info();

		m_pGameInstance->Change_Level(CLevel_Loading::Create(
			m_pGraphic_Device, LEVEL::LOADING, LEVEL::JUSIN, &Desc));
	}
	else if (m_pGameInstance->Get_CurrentLevelID() == ENUM_CLASS(LEVEL::JUSIN))
	{
		Desc = dynamic_cast<CPlayer*>(
			m_pGameInstance->Find_GameObject_ToLayer(
				ENUM_CLASS(LEVEL::JUSIN),
				TEXT("Layer_Player")))->Get_Player_Info();

		m_pGameInstance->Change_Level(CLevel_Loading::Create(
			m_pGraphic_Device, LEVEL::LOADING, LEVEL::BOSSFIGHT, &Desc));
	}
}

const COLLISION_DESC& CEventBox::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pCollider = m_pBoxColliderCom;
	Desc.pTransform = m_pTransformCom;

	return Desc;
}

CEventBox* CEventBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEventBox* pInstance = new CEventBox(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: EventBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEventBox::Clone(void* pArg)
{
	CEventBox* pInstance = new CEventBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed: EventBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventBox::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBoxColliderCom);
}
