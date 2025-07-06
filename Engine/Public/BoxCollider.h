#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBoxCollider final : public CComponent
{
public:
	typedef struct tagBoxColliderDesc {

		_float fScaleX;
		_float fScaleY;
		_float fScaleZ;

	} BOXCOLLIDER_DESC;

private:
	CBoxCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoxCollider(const CBoxCollider& rhs);
	virtual ~CBoxCollider() = default;

public:
	const _float3& Get_LocalPos(_uint iIdx) const
	{
		return m_vLocalPos[iIdx];
	}

	const _float& Get_Scale(_uint iIdx) const {
		return *(_float *)&m_vScale[iIdx];
	}

	void Set_Scale(const _float3& vScale) {
		memcpy(m_vScale, vScale, sizeof(_float3));
	}

	const _float3& Get_Min() const {
		return m_vLocalPos[3];
	}

	const _float3& Get_Max() const {
		return m_vLocalPos[5];
	}
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	/*
	콜라이더의 크기에 따라 동적으로 보여줘야 할 것이다
	*/
	void Render(const _float3& vPos);
private:
	/* VIBuffer에서 그대로 가져온, 콜라이더 범위 출력을 위한 정점 */
	LPDIRECT3DVERTEXBUFFER9				m_pVB = { nullptr };
	LPDIRECT3DINDEXBUFFER9				m_pIB = { nullptr };

	_uint								m_iVertexStride = { };

	_uint								m_iNumVerticesX{}, m_iNumVerticesZ{};
	_uint								m_iNumVertices = { };

	_uint								m_iIndexStride = {};
	_uint								m_iNumIndices = {};

	_uint								m_iFVF = {};
	D3DPRIMITIVETYPE					m_ePrimitiveType = {};
	_uint								m_iNumPrimitive = {};

	D3DFORMAT							m_eIndexFormat = {};


	_float3 m_vLocalPos[8] = {};
	_float3 m_vScale = {};
	_float3 m_vAxis[3] = {};
public:
	static CBoxCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
