#pragma once
#include "Component.h"

#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;



public://Get
	_uint Get_NumMeshes() const	{ return m_iNumMeshes; }
	
	_uint Get_NumAnims() const	{ return m_iNumAnimations; }

	class CMesh* Get_Mesh(_uint iIndex) { return m_Meshes[iIndex]; }

	class CBone* Get_Bone(const char* pBoneName);
	class CBone* Get_Bone(_uint iIndex)	{ return m_Bones[iIndex]; }

	_int Get_BoneIndex(const char* pBoneName);

	_int	Get_Animation_Size() { return (_int)m_Animations.size(); }
	class CAnimation* Get_Animation() { return m_Animations[m_iCurrentAnimIndex]; }
	class CAnimation* Get_Animation(_int index) { return m_Animations[index]; }
	vector<class CAnimation*> Get_vecAnimation() { return m_Animations; }

	_uint Get_iCurrentAnimIndex() { 
		return m_iCurrentAnimIndex; }


	_bool	Get_AnimFinish(_int AnimIndex) {
		
		_bool Finish = m_Animations[AnimIndex]->Get_AnimationDesc().m_isFinish;
		

		return Finish;
		
	}
	_bool	Check_PickAnimRatio(_uint iAnimIndex, _double Ratio, _double dTimeDelta) {
		return m_Animations[iAnimIndex]->Check_AnimRatio(Ratio, dTimeDelta);
	}
	_bool Get_AnimRatio(_uint iAnimIndex, _double Ratio) {
		return m_Animations[iAnimIndex]->Get_AnimRatio(Ratio);
	}
	_bool Get_CurAnimRatio(_double Ratio) {
		return m_Animations[m_iCurrentAnimIndex]->Get_AnimRatio(Ratio);
	}
	_double Get_dAnimRatio(_uint iAnimIndex) {
		return m_Animations[iAnimIndex]->Get_dAnimRatio();
	}

	_bool Get_IsNormalTexture(_uint iMaterialIdx) {
		return m_isNormalTexture[iMaterialIdx];
	}

public://Set
	void Set_Animation(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }

	
	void Set_Animation_Control(_int index, CAnimation::CONTROLDESC ControlDesc) {
		m_Animations[index]->Set_ControlDesc(ControlDesc);
	}

	void	Set_EarlyEnd(_int index, _bool bEnd, _float fEarlyRatio = 0.65) { 
		m_Animations[index]->Set_EarlyEnd(bEnd); 
		m_Animations[index]->Set_Ratio_EarlyEnd(fEarlyRatio);
	}
	void	Set_AnimisFinish(_uint iAnimationIdx) { m_Animations[iAnimationIdx]->Reset_Finish(); }
	void	Set_AnimResetTimeAcc(_uint iAnimationIdx) { m_Animations[iAnimationIdx]->Reset_TimeAcc(); }

	void	Set_EventReset(_uint iAnimIdx) {
		m_Animations[iAnimIdx]->Reset_Event();
	}

	void	Set_LinearDuration(_int iAnimIdx, _double Duration) {
		m_Animations[iAnimIdx]->Set_LinearDuration(Duration);
	}

public:
	HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Play_Animation(_double dTimeDelta);
	HRESULT Play_Animation_For_Boss(_double dTimeDelta);
	HRESULT Render(_uint iMeshIndex);
	HRESULT Bind_ShaderResource(_uint iMeshIndex, class CShader* pShader, const char* pConstantName, MESHMATERIALS::TEXTURETYPE eType);
	HRESULT Bind_ShaderBoneMatrices(_uint iMeshIndex, class CShader* pShader, const char* pConstantName);

public:// AnimTool용
	_bool	Get_isPlay() { return m_isPlay; }
	void	Set_isPlay(_bool Play) { m_isPlay = Play; }

	_bool	Get_Combo_Trigger() { return m_isCombo_Trigger; }
	void	Set_Combo_Trigger(_bool combo) { m_isCombo_Trigger = combo; }
	_bool	Get_Combo_Doing() { return m_isCombo_Doing; }
	void	Set_Combo_Doing(_bool bComboDo) { m_isCombo_Doing = bComboDo; }

	
	void Set_Combo_Another(_int AnotherRoute) { 
		m_isCombo_Another = true; 
		m_iCombo_AnotherRoute = AnotherRoute;
	}

private: // AnimTool용
	_bool	m_isPlay = { true };


	// 콤보공격용
	_bool	m_isCombo_Trigger = { false };
	_bool	m_isCombo_Doing = { false };

	_int	m_iCombo_AnotherRoute = { 0 };
	_bool	m_isCombo_Another = { false };

private: // 선형보간용
	_bool	m_isLinearOn = { false };
	vector<KEYFRAME> m_LastKeys;

private:
	MODELDATA					m_ModelData;

private:
	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = { TYPE_END };

private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

private:
	_uint						m_iNumMaterials = { 0 };
	vector<MESHMATERIALS>		m_Materials;

private:
	vector<class CBone*>		m_Bones;

private:
	_uint						m_iCurrentAnimIndex = { 0 };
	_uint						m_iSaveAnimIndex = { 0 };
	_uint						m_iPreAnimIndex = { 0 };

	_uint						m_iNumAnimations = { 0 };
	vector<class CAnimation*>	m_Animations;


private:
	_bool						m_isNormalTexture[50] = { false };
	

private:
	HRESULT	Ready_ModelData(const char* pModelFilePath, TYPE eModelType);
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyBones();
	HRESULT Ready_Animations();
	HRESULT Clear_LoadData(TYPE eModelType);

public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const char* pModelFilePath, _matrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END