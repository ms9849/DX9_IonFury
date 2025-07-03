#include "Player.h"

#include "GameInstance.h"
#include "Bullet.h"
#include "ItemArmor.h"
#include "ItemHealpack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"
#include "ItemCardKey.h"
#include "Player_RightHand.h"
#include "Player_LeftHand.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CLandObject(Prototype)
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

	// 오른손
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_RightHand"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_RightHand"))))
		return E_FAIL;

	m_pRightHand = dynamic_cast<CPlayer_RightHand*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_RightHand")));
	Safe_AddRef(m_pRightHand);

	m_pRightHandAnimationCom = dynamic_cast<CAnimation*>(m_pRightHand->Find_Component(TEXT("Com_Animation")));
	Safe_AddRef(m_pRightHandAnimationCom);

	// 왼손
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_LeftHand"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_LeftHand"))))
		return E_FAIL;

	m_pLeftHand = dynamic_cast<CPlayer_LeftHand*>(m_pGameInstance->Find_GameObject_ToLayer(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_LeftHand")));
	Safe_AddRef(m_pLeftHand);

	m_pLeftHandAnimationCom = dynamic_cast<CAnimation*>(m_pLeftHand->Find_Component(TEXT("Com_Animation")));
	Safe_AddRef(m_pLeftHandAnimationCom);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	/* 점프 로직*/
	if (!m_bJump && m_pGameInstance->Key_Down(VK_SPACE))
	{
		m_bJump = true;
		m_fTime = 0.f;
	}

	if (m_bJump)
	{
		//4.f는 점프 스피드
		m_fFallSpeed = (4.f * m_fTime - 30.f * m_fTime * m_fTime);

		if (m_fFallSpeed <= -0.2f)
			m_fFallSpeed = -0.2f;

		m_fTime += 0.3f * fTimeDelta;
		_float3 vPosition = m_pTransformCom->Get_State(STATE::POSITION);
		vPosition.y = vPosition.y + m_fFallSpeed;
		m_pTransformCom->Set_State(STATE::POSITION, vPosition);
	}

	/* 애니메이션 제어 */
	auto iter = m_Weapons.find(m_tInfo.strWeapon);

	if (m_tInfo.strAction.compare(TEXT("Reload")) != 0)
	{
		if (m_pGameInstance->Key_Down('1'))
		{
			m_strNextWeapon = TEXT("Pistol");
			if (m_tInfo.strWeapon != m_strNextWeapon)
			{
				m_tInfo.strAction = TEXT("Down");
				m_bWeaponChange = true;
			}
		}
		if (m_pGameInstance->Key_Down('2'))
		{
			m_strNextWeapon = TEXT("ShootGun");
			if (m_tInfo.strWeapon != m_strNextWeapon)
			{
				m_tInfo.strAction = TEXT("Down");
				m_bWeaponChange = true;
			}
		}
		if (m_bCanUseCardKey && m_pGameInstance->Key_Down('E'))
		{
			m_tInfo.strItem = TEXT("CardKey");
			m_tInfo.strItemAction = TEXT("Up");
			m_tInfo.strAction = TEXT("Down");
			m_bUseItem = true;
			m_bCanUseCardKey = false;
		}
	}

	// 아이템 : 카드키 사용
	if (m_bUseItem)
	{
		if (m_pLeftHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strItem, TEXT("Up"))))
		{
			m_tInfo.strItemAction = TEXT("Down");
			m_tInfo.strAction = TEXT("Up");
		}
		if (m_pLeftHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strItem, TEXT("Down"))))
		{
			m_bUseItem = false;
		}
	}
	if (m_pLeftHandAnimationCom->Check_Animation_Finish())
	{
		if(!m_bUseItem)
			m_tInfo.strItemAction = TEXT("Idle");
	}

	// 무기 교체
	if (m_bWeaponChange)
	{
		if (m_pRightHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strWeapon, TEXT("Down"))))
		{
			m_tInfo.strWeapon = m_strNextWeapon;
			m_tInfo.strAction = TEXT("Up");
			iter = m_Weapons.find(m_tInfo.strWeapon);
			m_tInfo.iBullets = iter->second.iCurrentBullets;
			iter->second.iShootBullets = iter->second.iShootBullets;
			m_tInfo.iShootBullets = iter->second.iShootBullets;
		}

		if (m_pRightHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strWeapon, TEXT("Up"))))
		{
			m_bWeaponChange = false;
		}
	}

	// 오른손 애니메이션 끝나면 idle로
	if (m_pRightHandAnimationCom->Check_Animation_Finish())
	{
		if(!m_bWeaponChange && !m_bUseItem)
			m_tInfo.strAction = TEXT("Idle");
	}
	else
	{
		_float3 vLook = m_pTransformCom->Get_State(STATE::LOOK);
		vLook.y = 0.f;

		_float3 vRight = m_pTransformCom->Get_State(STATE::RIGHT);
		vRight.y = 0.f;

		// 이동
		if (m_pGameInstance->Key_Pressing('W'))
		{
			m_pTransformCom->Go_Direction(vLook, fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		if (m_pGameInstance->Key_Pressing('S'))
		{
			m_pTransformCom->Go_Direction(-1.f * vLook, fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		if (m_pGameInstance->Key_Pressing('A'))
		{
			m_pTransformCom->Go_Direction(-1.f * vRight, fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		if (m_pGameInstance->Key_Pressing('D'))
		{
			m_pTransformCom->Go_Direction(vRight, fTimeDelta);
			if (m_tInfo.strAction.compare(TEXT("Idle")) == 0)
				m_tInfo.strAction = TEXT("Walk");
		}
		// 장전
		if (m_pGameInstance->Key_Down('R'))
		{
			m_tInfo.strAction = TEXT("Reload");
			iter->second.iShootBullets = iter->second.iCanShootBullets;
			m_tInfo.iShootBullets = iter->second.iShootBullets;
		}
		// 총알 발사
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

				if (m_tInfo.strWeapon.compare(TEXT("Pistol")) == 0)
				{
					m_pGameInstance->PlaySoundOnce(TEXT("Pistol_Shoot.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
				}
				else if (m_tInfo.strWeapon.compare(TEXT("ShootGun")) == 0)
				{

				}

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

	m_pRightHand->Set_Current_Animation(Set_FrameKey(m_tInfo.strWeapon, m_tInfo.strAction));
	m_pLeftHand->Set_Current_Animation(Set_FrameKey(m_tInfo.strItem, m_tInfo.strItemAction));

	SetUp_OnTerrain(m_pTransformCom, 0.5f, &m_bJump);

	m_pRightHand->Set_Player_Transform(m_pTransformCom);
	m_pLeftHand->Set_Player_Transform(m_pTransformCom);
	
}

void CPlayer::Update(_float fTimeDelta)
{	
	if(!m_ItemQueues.empty())
		Pop_ItemDesc(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
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

_bool CPlayer::Get_CanUse_CardKey()
{
	return m_bCanUseCardKey;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 4.f, D3DXToRadian(90.0f) };
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
	m_pGameInstance->Check_RayCollision(vCamPos, vCamLook, TEXT("Layer_Cube"), ENUM_CLASS(LEVEL::GAMEPLAY), &vCollisionPos);

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

void CPlayer::Insert_ItemDesc(const _wstring strItemText)
{
	if (m_ItemQueues.size() > 2)
	{
		m_ItemQueues.pop_back();
	}

	m_ItemQueues.push_front(strItemText);
}

void CPlayer::Pop_ItemDesc(_float fTimeDelta)
{
	m_fTimeStack += fTimeDelta;

	for (size_t i = 0; i < m_ItemQueues.size(); ++i)
	{
		if (m_fTimeStack > 2.f)
		{
			m_fTimeStack = 0.f;
			m_ItemQueues.pop_back();
		}
	}
}

_wstring CPlayer::Set_FrameKey(_wstring strDst, _wstring strSrc)
{
	_tchar strResult[256];
	wsprintf(strResult, TEXT("%s_%s"), strDst.c_str(), strSrc.c_str());
	return strResult;
}

void CPlayer::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (eColType == COLLISION::SPHERE)
	{
		if (dynamic_cast<CItemArmor*>(pDst))
		{
			m_tInfo.iArmor += 10;
			Insert_ItemDesc(TEXT("Get Armor [Armor+10]"));
		}
		if (dynamic_cast<CItemHealpack*>(pDst))
		{
			m_tInfo.iHp += 10;
			Insert_ItemDesc(TEXT("Get Healpack [HP+10]"));
		}
		if (dynamic_cast<CItemPistolBullet*>(pDst))
		{
			auto iter = m_Weapons.find(TEXT("Pistol"));
			iter->second.iCurrentBullets += 10;

			if (iter->second.iCurrentBullets >= iter->second.iBulletsMax)
				iter->second.iCurrentBullets = iter->second.iBulletsMax;

			if(m_tInfo.strWeapon.compare(TEXT("Pistol")) == 0)
				m_tInfo.iBullets = iter->second.iCurrentBullets;

			Insert_ItemDesc(TEXT("Get Pistol Bullets [Bullet+10]"));
		}
		if (dynamic_cast<CItemShootGunBullet*>(pDst))
		{
			auto iter = m_Weapons.find(TEXT("ShootGun"));

			iter->second.iCurrentBullets += 10;

			if (iter->second.iCurrentBullets >= iter->second.iBulletsMax)
				iter->second.iCurrentBullets = iter->second.iBulletsMax;

			if (m_tInfo.strWeapon.compare(TEXT("ShootGun")) == 0)
				m_tInfo.iBullets = iter->second.iCurrentBullets;

			Insert_ItemDesc(TEXT("Get ShootGun Bullets [Bullet+10]"));
		}
		if (dynamic_cast<CItemCardKey*>(pDst))
		{
			Insert_ItemDesc(TEXT("Get CardKey"));
			m_bCanUseCardKey = true;
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
	return m_ItemQueues[iIndex];
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
	Safe_Release(m_pLeftHand);
	Safe_Release(m_pLeftHandAnimationCom);
	Safe_Release(m_pRightHand);
	Safe_Release(m_pRightHandAnimationCom);
	Safe_Release(m_pSphereColliderCom);
}
