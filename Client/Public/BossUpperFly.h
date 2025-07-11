#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTransform;
class CAnimation;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBossUpperFly final : public CGameObject
{
public:
	typedef struct tagFlyDesc {
		CTransform* pTransform;
	}Fly_DESC;
private:
	CBossUpperFly(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossUpperFly(const CBossUpperFly& Prototype);
	virtual ~CBossUpperFly() = default;

private:
	const _wstring m_strFrameKeys[1] = {
		TEXT("Boss_FlyLeft")
	};

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components();
	virtual HRESULT Ready_Animations();
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Begin_RenderTestState();
	virtual HRESULT End_RenderTestState();
	virtual void RotateToPlayer(CTransform* pTranform);

private:
	_bool m_isLeft = false;
	_wstring m_strFrameKey = TEXT("Boss_FlyLeft");
	Fly_DESC m_Desc = {};
	CTransform* m_pTransformUpperBody = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CAnimation* m_pAnimationCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	map<const _wstring, CTexture*> m_pTextureComs;

public:
	static CBossUpperFly* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END