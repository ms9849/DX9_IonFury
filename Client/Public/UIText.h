#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "UIFont.h"


NS_BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIText final : public CUIObject
{
private:
	CUIText(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIText(const CUIText& Prototype);
	virtual ~CUIText() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Text(const _tchar* strText);
	void Set_Text(_uint iNumber);

private:
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CUIText* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	vector<CUIFont*> m_vecFont{};
	UIOBJECT_DESC m_tagDesc{};
};

NS_END

