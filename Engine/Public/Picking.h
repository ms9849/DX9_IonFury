#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(LPDIRECT3DDEVICE9 pGraphicDevice);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd);
	_bool Picking_InWorldSpace(const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	_bool Picking_InLocalSpace(const _float4x4* pWorldMatrixInverse, const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);

private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = { nullptr };
	HWND				m_hWnd = {};
	_uint				m_iWinSizeX{}, m_iWinSizeY{};

public:
	static CPicking* Create(LPDIRECT3DDEVICE9 pGraphicDevice, HWND hWnd);
	virtual void Free() override;
};

NS_END