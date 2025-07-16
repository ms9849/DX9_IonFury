#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc {
		_uint iObjectID{};
		_float4x4 matWorld{};
		_uint iProtoLevel{};
		_uint iLayerLevel{};
		_uint iTextureIndex{0};
		_wstring strLayer{};
		_wstring strProto{};
		_wstring strTerrainSize{};
	}GAMEOBJECT_DESC;

protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	/* 충돌한 상대 객체와, 충돌 타입을 리턴 */
	//virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta);

	/* 충돌한 상대 객체, 충돌이 발생한 내 자신의 콜라이더, 충돌 타입을 리턴 */
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType,_float fTimeDelta);
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos);
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos, const _float3& vPlaneNormal);

	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);
	virtual const RAY_DESC& Get_RayDesc();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_bool isDead() const {
		return m_isDead;
	}

	_uint Get_ID() const {
		return m_pObjectDesc.iObjectID;
	}

	GAMEOBJECT_DESC Get_Desc() {
		return m_pObjectDesc;
	}

	void Set_ID(_uint iId) {
		m_pObjectDesc.iObjectID = iId;
	}

	void Set_Proto(_wstring strProto, _uint iProtoLevel)
	{
		m_pObjectDesc.strProto = strProto;
		m_pObjectDesc.iProtoLevel = iProtoLevel;
	}

	void Set_Leyer(_wstring strLayer, _uint iLayerLevel)
	{
		m_pObjectDesc.strLayer = strLayer;
		m_pObjectDesc.iLayerLevel = iLayerLevel;
	}

	void Set_Texture_Index(_uint iIndex)
	{
		m_pObjectDesc.iTextureIndex = iIndex;
	}

	void Set_Terrain_Size(_wstring strTerrainSize)
	{
		m_pObjectDesc.strTerrainSize = strTerrainSize;
	}

	void Set_Desc(GAMEOBJECT_DESC pObjectDesc) {
		m_pObjectDesc = pObjectDesc;
	}

	void  Set_Dead(_bool bDead) {
		m_isDead = bDead;
	}

	_float Get_CamDistance() const {
		return m_fCamDistance;
	}
	class CComponent* Find_Component(const _wstring& strComponentTag);

protected:
	GAMEOBJECT_DESC				m_pObjectDesc;
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_bool						m_isDead = { false };
	_float						m_fCamDistance = {};
	map<const _wstring, class CComponent*>		m_Components;

protected:
	void	Compute_CamDistance(const _float3& vWorldPos);
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END