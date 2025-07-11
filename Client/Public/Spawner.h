#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTransform;
class CBoxCollider;
NS_END

NS_BEGIN(Client)
class CSpawner final : public CGameObject
{
public:
	typedef struct tagSpawnerDesc {
		_float3 vPos;
		_uint idx;
		_bool isLeft = false;
	}SPAWNER_DESC;

private:
	CSpawner(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSpawner(const CSpawner& Prototype);
	virtual ~CSpawner() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components();
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta);
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider);
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

public:
	static CSpawner* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	tagSpawnerDesc m_Desc = { };

	class CTransform* m_pTransformCom = { nullptr };
	class CBoxCollider* m_pBoxColliderCom = { nullptr };
	_bool m_isActive = true;
	_bool m_isLeft = false;

	const _wstring m_strFrameKeys[4] = {
		TEXT("Prototype_GameObject_Monster_Zombie"), TEXT("Prototype_GameObject_Monster_Spider"),
		TEXT("Prototype_GameObject_Monster_Soldier"), TEXT("Prototype_GameObject_Monster_EliteSoldier")
	};
};

NS_END