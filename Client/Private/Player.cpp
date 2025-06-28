#include "Player.h"

#include "GameInstance.h"
#include "Bullet.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CLandObject{ Prototype }
{

}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	m_tInfo.iHp = 100;
	m_tInfo.iArmor = 100;
	m_tInfo.iBullets = 100;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(0.f, 0.f, 0.f));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Hand"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Right_Hand"))))
		return E_FAIL;

	m_pRightHand = dynamic_cast<CPlayer_Hand*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Right_Hand")));
	
	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Hand"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Left_Hand"))))
		return E_FAIL;

	m_pLeftHand = dynamic_cast<CPlayer_Hand*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Left_Hand")));*/

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Pressing('W'))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pRightHand->Set_Current_Animation(TEXT("Pistol_Idle"));
	}
	if (m_pGameInstance->Key_Pressing('S'))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
		m_pRightHand->Set_Current_Animation(TEXT("Pistol_Idle"));
	}
	if (m_pGameInstance->Key_Pressing('A'))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
		m_pRightHand->Set_Current_Animation(TEXT("Pistol_Idle"));
	}
	if (m_pGameInstance->Key_Pressing('D'))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
		m_pRightHand->Set_Current_Animation(TEXT("Pistol_Idle"));
	}
	if (m_pGameInstance->Key_Down('R'))
	{
		m_pRightHand->Set_Current_Animation(TEXT("Pistol_Reload"));
	}

	if (m_pGameInstance->Key_Down(VK_LBUTTON))
	{
		m_tInfo.iBullets -= 1;
		m_pRightHand->Set_Current_Animation(TEXT("Pistol_Shoot"));

		_float3 vOffset = _float3{ 0.f, 0.f, 0.f };
		_float3 vDir = Calc_BulletDir(&vOffset);
		_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

		D3DXVec3Normalize(&vDir, &vDir);

		CBullet::BULLET_DESC Desc;
		Desc.vDir = vDir;
		Desc.vPos = vPos + vOffset;

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"),
			ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Bullet"), &Desc);
	}
}

void CPlayer::Update(_float fTimeDelta)
{
	SetUp_OnTerrain(m_pTransformCom, 0.5f);
	m_pRightHand->Set_Player_Transform(m_pTransformCom);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CPlayer::Render()
{
	m_pTransformCom->Set_Transform();

	//m_pTextureCom->Set_Texture(0);

	/*if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;*/

	return S_OK;
}

CPlayer::PLAYER_INFO CPlayer::Get_Player_Info()
{
	return m_tInfo;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	///* Com_Texture */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Player"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

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

HRESULT CPlayer::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	/*
	float4		vSourColor, vDestColor;
	vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);
	*/

	//
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);



	return S_OK;
}

HRESULT CPlayer::End_RenderState()
{
	// m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

_float3 CPlayer::Calc_BulletDir(_float3* vOffset)
{
	_float3 vCollisionPos{0.f, 0.f, 0.f};

	//mat view Inv는 카메라의 월드 행렬.
	_float4x4 m_matView, m_matViewInv;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_matView);
	D3DXMatrixInverse(&m_matViewInv, nullptr, &m_matView);

	/*
	vLook, vRight 가져와서 보정하기
	*/
	_float3 vCamPos = m_pTransformCom->Get_State(STATE::POSITION);
	_float3 vCamRight = *(_float3 *)(&m_matViewInv.m[0][0]);
	_float3 vCamLook = *(_float3*)(&m_matViewInv.m[2][0]);


	D3DXVec3Normalize(&vCamLook, &vCamLook);
	m_pGameInstance->Check_LookCollision(vCamPos, vCamLook, TEXT("Layer_Cube"), ENUM_CLASS(LEVEL::GAMEPLAY), &vCollisionPos);

	*vOffset = (*D3DXVec3Normalize(&vCamLook, &vCamLook) / 10.f + (*D3DXVec3Normalize(&vCamRight, &vCamRight) / 10.f));

	if (vCollisionPos == _float3{ 0.f, 0.f, 0.f })
		return m_pTransformCom->Get_State(STATE::LOOK);

	else
	{
		_float3 vDir = vCollisionPos - (vCamPos + *vOffset);
		D3DXVec3Normalize(&vDir, &vDir);
		return vDir;
	}
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

}
