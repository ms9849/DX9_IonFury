#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CFonts;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIFont final : public CUIObject
{
private:
	CUIFont(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIFont(const CUIFont& Prototype);
	virtual ~CUIFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Font_Type(_tchar ch);

private:
	map<const _wstring, CFonts*> m_pFontsComs{};
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	const _wstring m_strFontKeys[3] = {
	  TEXT("Default"), TEXT("Gray"), TEXT("Primary")
	};

private:
	HRESULT Ready_Components();

public:
	static CUIFont* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	_tchar m_pStrText{};
	_wstring m_strFontType{};
	UIOBJECT_DESC	m_tagDesc{};

};

NS_END

