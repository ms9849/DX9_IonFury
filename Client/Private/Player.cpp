#include "Player.h"

#include "GameInstance.h"
#include "Bullet.h"
#include "ItemArmor.h"
#include "ItemHealpack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"

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

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Weapons()))
		return E_FAIL;

	m_tInfo.iHp = 70;
	m_tInfo.iArmor = 70;
	
	auto iter = m_Weapons.find(m_tInfo.strWeapon);

	m_tInfo.iBullets = iter->second.iCurrentBullets;
	m_tInfo.iShootBullets = iter->second.iShootBullets;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(0.f, 0.f, 0.f));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Hand"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Right_Hand"))))
		return E_FAIL;

	m_pRightHand = dynamic_cast<CPlayer_Hand*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Right_Hand")));
	Safe_AddRef(m_pRightHand);

	m_pRightHandAnimationCom = dynamic_cast<CAnimation*>(m_pRightHand->Find_Component(TEXT("Com_Animation")));
	Safe_AddRef(m_pRightHandAnimationCom);

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Hand"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Left_Hand"))))
		return E_FAIL;

	m_pLeftHand = dynamic_cast<CPlayer_Hand*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_Left_Hand")));*/

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	auto iter = m_Weapons.find(m_tInfo.strWeapon);

	if (m_tInfo.strAction.compare(TEXT("Reload")) != 0)
	{
		if (m_pGameInstance->Key_Down('1'))
		{
			m_tInfo.strWeapon = TEXT("Pistol");
			iter = m_Weapons.find(m_tInfo.strWeapon);
			m_tInfo.iBullets = iter->second.iCurrentBullets;
			iter->second.iShootBullets = iter->second.iShootBullets;
			m_tInfo.iShootBullets = iter->second.iShootBullets;
		}
		if (m_pGameInstance->Key_Down('2'))
		{
			m_tInfo.strWeapon = TEXT("ShootGun");
			iter = m_Weapons.find(m_tInfo.strWeapon);
			m_tInfo.iBullets = iter->second.iCurrentBullets;
			iter->second.iShootBullets = iter->second.iShootBullets;
			m_tInfo.iShootBullets = iter->second.iShootBullets;
		}
	}
	

	// 애니메이션 종료 처리 해야됨
	if (m_pRightHandAnimationCom->Check_Animation_Finish())
	{
		m_tInfo.strAction = TEXT("Idle");
	}
	else
	{
		if (m_pGameInstance->Key_Pressing('W'))
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		if (m_pGameInstance->Key_Pressing('S'))
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		if (m_pGameInstance->Key_Pressing('A'))
		{
			m_pTransformCom->Go_Left(fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		if (m_pGameInstance->Key_Pressing('D'))
		{
			m_pTransformCom->Go_Right(fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		if (m_pGameInstance->Key_Down('R'))
		{
			m_tInfo.strAction = TEXT("Reload");
			iter->second.iShootBullets = iter->second.iCanShootBullets;
			m_tInfo.iShootBullets = iter->second.iShootBullets;
		}

		if (m_tInfo.strAction.compare(TEXT("Reload")) != 0)
		{
			if (m_pGameInstance->Key_Down(VK_LBUTTON)
				&& iter->second.iShootBullets > 0
				&& iter->second.iCurrentBullets > 0)
			{
				m_pRightHandAnimationCom->Clear_Animation();
				iter->second.iCurrentBullets -= 1;
				m_tInfo.iBullets = iter->second.iCurrentBullets;
				iter->second.iShootBullets -= 1;
				m_tInfo.iShootBullets = iter->second.iShootBullets;
				m_tInfo.strAction = TEXT("Shoot");

				_float3 vOffset = _float3{ 0.f, 0.f, 0.f };
				_float3 vDir = Calc_BulletDir(&vOffset);
				_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

				D3DXVec3Normalize(&vDir, &vDir);

				CBullet::BULLET_DESC Desc;
				Desc.vDir = vDir;
				Desc.vPos = vPos + vOffset;

				m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"),
					ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_PlayerBullet"), &Desc);
			}
		}
	}

	_tchar strCurrentAnimation[256];

	wsprintf(strCurrentAnimation, TEXT("%s_%s"), m_tInfo.strWeapon.c_str(), m_tInfo.strAction.c_str());

	/* 점프 로직*/
	if (!m_bJump && m_pGameInstance->Key_Down(VK_SPACE))
	{
		m_bJump = true;
		m_fTime = 0.f;
	}

	m_pRightHand->Set_Current_Animation(strCurrentAnimation);
}

void CPlayer::Update(_float fTimeDelta)
{
	if (m_bJump)
	{
		m_fTime += 0.5f * fTimeDelta;
		_float3 vPosition = m_pTransformCom->Get_State(STATE::POSITION);
		vPosition.y = vPosition.y + (m_fJumpSpeed * m_fTime - 4.9f * m_fTime * m_fTime);
		m_pTransformCom->Set_State(STATE::POSITION, vPosition);
	}
	else
	{
		SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);
	}

	m_pRightHand->Set_Player_Transform(m_pTransformCom);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
}

HRESULT CPlayer::Render()
{
	m_pTransformCom->Set_Transform();

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

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;	

	/* Com_BoxCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom))))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Weapons()
{
	WEAPON_INFO PistolDesc{};

	PistolDesc.iBulletsMax = 250;
	PistolDesc.iCurrentBullets = 230;
	PistolDesc.iCanShootBullets = 7;
	PistolDesc.iShootBullets = PistolDesc.iCanShootBullets;

	m_Weapons.emplace(TEXT("Pistol"), PistolDesc);

	WEAPON_INFO ShootGunDesc{};

	ShootGunDesc.iBulletsMax = 150;
	ShootGunDesc.iCurrentBullets = 130;
	ShootGunDesc.iCanShootBullets = 2;
	ShootGunDesc.iShootBullets = ShootGunDesc.iCanShootBullets;

	m_Weapons.emplace(TEXT("ShootGun"), ShootGunDesc);

	return S_OK;
}

HRESULT CPlayer::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPlayer::End_RenderState()
{
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

void CPlayer::Insert_ItemDesc(_float fCreateTime, const _wstring strItemText)
{
	if (m_ItemQueues.size() > 2)
	{
		m_ItemQueues.pop_front();
	}

	ITEM_DESC Desc{};
	Desc.fCreateTime = fCreateTime;
	Desc.strItemText = strItemText;
	m_ItemQueues.push_front(Desc);
}

void CPlayer::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (eColType == COLLISION::SPHERE)
	{
		if (dynamic_cast<CItemArmor*>(pDst))
		{
			m_tInfo.iArmor += 10;
			Insert_ItemDesc(fTimeDelta, TEXT("Get Armor [Armor+10]"));
		}
		if (dynamic_cast<CItemHealpack*>(pDst))
		{
			m_tInfo.iHp += 10;
			Insert_ItemDesc(fTimeDelta, TEXT("Get Healpack [HP+10]"));
		}
		if (dynamic_cast<CItemPistolBullet*>(pDst))
		{
			auto iter = m_Weapons.find(TEXT("Pistol"));
			iter->second.iCurrentBullets += 10;

			if (iter->second.iCurrentBullets >= iter->second.iBulletsMax)
				iter->second.iCurrentBullets = iter->second.iBulletsMax;

			if(m_tInfo.strWeapon.compare(TEXT("Pistol")) == 0)
				m_tInfo.iBullets = iter->second.iCurrentBullets;

			Insert_ItemDesc(fTimeDelta, TEXT("Get Pistol Bullets [Bullet+10]"));
		}
		if (dynamic_cast<CItemShootGunBullet*>(pDst))
		{
			auto iter = m_Weapons.find(TEXT("ShootGun"));

			iter->second.iCurrentBullets += 10;

			if (iter->second.iCurrentBullets >= iter->second.iBulletsMax)
				iter->second.iCurrentBullets = iter->second.iBulletsMax;

			if (m_tInfo.strWeapon.compare(TEXT("ShootGun")) == 0)
				m_tInfo.iBullets = iter->second.iCurrentBullets;

			Insert_ItemDesc(fTimeDelta, TEXT("Get ShootGun Bullets [Bullet+10]"));
		}
	}
}

const COLLISION_DESC& CPlayer::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	else if (eColType == COLLISION::OBB)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

_wstring CPlayer::Get_ItemText(size_t iIndex)
{
	return m_ItemQueues[iIndex].strItemText;
}

size_t CPlayer::Get_ItemQueue_Length()
{
	return m_ItemQueues.size();
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : player");
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

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBoxColliderCom);
	Safe_Release(m_pRightHand);
	Safe_Release(m_pRightHandAnimationCom);
	Safe_Release(m_pSphereColliderCom);
}
