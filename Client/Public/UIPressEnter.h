#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIPressEnter final : public CUIObject
{
private:
	CUIPressEnter(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIPressEnter(const CUIPressEnter& Prototype);
	virtual ~CUIPressEnter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Guide(_bool bFinish);

private:
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	class CUIText* m_pText{ nullptr };
	UIOBJECT_DESC	m_tagDesc{};

private:
	HRESULT Ready_Components();

public:
	static CUIPressEnter* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END