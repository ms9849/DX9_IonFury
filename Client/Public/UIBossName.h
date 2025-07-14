#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIBossName final : public CUIObject
{
private:
	CUIBossName(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIBossName(const CUIBossName& Prototype);
	virtual ~CUIBossName() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	UIOBJECT_DESC	m_tagDesc{};

private:
	HRESULT Ready_Components();

public:
	static CUIBossName* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END