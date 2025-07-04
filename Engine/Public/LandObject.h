#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLandObject abstract : public CGameObject
{
public:
	typedef struct tagLandObjectDesc
	{
		class CVIBuffer* pLandVIBuffer = { nullptr };
		class CTransform* pLandTransform = { nullptr };
	} LANDOBJECT_DESC;
protected:
	CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLandObject(const CLandObject& Prototype);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	void	Jump(_float fTimeDelta);
	virtual void Change_Land(LANDOBJECT_DESC* pLandDesc);

	_bool Get_RideCube() { return m_bRideCube; }
	_bool Get_Jump() { return m_bJump; }
	void Set_Jump(_bool bJump) { m_bJump = bJump; }
	void Set_Time(_float fTime) { m_fTime = fTime; }
	
	LANDOBJECT_DESC Get_LastLandDesc() {
		LANDOBJECT_DESC Desc;
		Desc.pLandTransform = m_pLandTransformLast;
		Desc.pLandVIBuffer = m_pLandBufferLast;

		return Desc;
	}

protected:
	/* 점프, 점프 시간 */
	_bool			m_bJump = { false };
	_bool			m_bRideCube = { false };
	_float			m_fTime = 0.f;
	_float			m_fFallSpeed = 0.f;

	class CTransform*		m_pLandTransform = { nullptr };
	class CVIBuffer*		m_pLandVIBuffer = { nullptr };
	class CTransform*		m_pTransformCom = { nullptr };

	class CTransform*		m_pLandTransformLast = { nullptr };
	class CVIBuffer*		m_pLandBufferLast = { nullptr };
protected:
	void SetUp_OnTerrain(class CTransform* pTransform, _float fOffset = 0.f, _bool* bJump = nullptr);
	
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END