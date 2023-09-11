#include "pch.h"
#include "..\Public\Dialog.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "DialogManager.h"
#include "MissionManager.h"


CDialog::CDialog(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CDialog::CDialog(const CDialog & rhs)
	: CUI(rhs)
{
}

HRESULT CDialog::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDialog::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// Frame
	if (m_UI_Desc.m_Type == 0) {

		m_fX = 640;
		m_fY = 660;
		m_Origin_PosX = 640;
		m_Origin_PosY = 620;
		m_Start_PosX = 640;
		m_Start_PosY = 660;

		m_Origin_X = 1308.f;
		m_Origin_Y = 228.f;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

	// Name_Frame
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 398;
		m_fY = 565;
		m_Origin_PosX = 398;
		m_Origin_PosY = 525;
		m_Start_PosX = 398;
		m_Start_PosY = 565;

		m_Origin_X = 470;
		m_Origin_Y = 69;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

	// Arrow
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 640;
		m_fY = 740;
		m_Origin_PosX = 640;
		m_Origin_PosY = 700;
		m_Start_PosX = 640;
		m_Start_PosY = 740;

		m_Origin_X = 32;
		m_Origin_Y = 32;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 2;
	}


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();


	m_szName[TANJIRO] = L"ī���� ź����";
	m_szName[GIRL] = L"�ҳ�";
	m_szName[MOTHER] = L"����";
	m_szName[BOY] = L"���ھ���";
	m_szName[MAN] = L"����";
	m_szName[ZENITSU] = L"�ư����� ������";



// Main
	// 0
	m_szMain_1.push_back(L"���صǴϱ� ����");
	m_szMain_1.push_back(L"������ ���Ͻó׿�");
	m_szMain_1.push_back(L"�� �����ֽǷ���?");
	
	// 1
	m_szMain_2.push_back(L"���� �����ϴ� ������ ������");
	m_szMain_2.push_back(L"�׷� �˰ھ�");
	
	// 2
	m_szMain_3.push_back(L"���� �����ϴ� ������ ������");
	m_szMain_3.push_back(L"�׷� �˰ھ�");
	
// Sub
	// 3
	m_szSub_1.push_back(L"Ȥ�� ������ ���͸� ���� �� �ֳ���?");
	m_szSub_1.push_back(L"����..?? ��..�� �𸣰ڴ°�");
	m_szSub_1.push_back(L"�׷� ������ �̻��� �����̶� ��������?");
	m_szSub_1.push_back(L"��..��! ���� ���� �ҳ���� ���ڱ� ������ٴ� �ҹ��� �־�");
	m_szSub_1.push_back(L"(�ҳ���� ������ٰ�,,?) ��..�׷�����. �����մϴ�!");
	// 4
	m_szSub_2.push_back(L"����..���� �� ������� �ɱ��?");
	m_szSub_2.push_back(L"�� ��...�� ���� �������..�ƹ��� �Ͼ����� �ʾ�..�ʵ� �Ȱ�����");

	// 5
	m_szSub_3.push_back(L"���ؿ��� ��� �̻��� ����� ������..");
	m_szSub_3.push_back(L"(����..? �ܼ��� �������� ���� ������!)");

	// 6
	m_szSub_4.push_back(L"���� �ƺ��� ���ƾ��..���� �����ּ���!!");
	m_szSub_4.push_back(L"�̷�!! �������..!");

	// 7
	m_szSub_5.push_back(L"�� ���� �� �����̶�� �׸��ֿ�. �󸶳� ��������..�Ҹ��� ��ģ�ٱ���");
	


// Talk
	// 8
	m_szTalk_1.push_back(L"���صǴϱ� ����");
	m_szTalk_1.push_back(L"������ ���Ͻó׿�");

	// 9
	m_szTalk_2.push_back(L"ź����!! �Ա���! �� ���� ���� ������..��������");
	m_szTalk_2.push_back(L"�� ������ ���밡 �վ�������..�����߰ھ�.");

	// 10
	m_szTalk_3.push_back(L"�ֺ��� ���͵��� �־�. ��Ű�� �ʰ� ������ �ؾ���");
	m_szTalk_3.push_back(L"�� �и� �����ž�!! ������ ź����!!!");


// Village Smell
	// 11
	m_szVillageSmell_1.push_back(L"(�� ������....!!!! ���ʹ�)");
	m_szVillageSmell_1.push_back(L"(....���ڵ��� ����� ������..Ȥ��?)");

	// 12
	m_szVillageSmell_2.push_back(L"(������ ������ ���� �����־�)");
	m_szVillageSmell_2.push_back(L"(....���θ���...!)");

	// 13
	m_szVillageSmell_3.push_back(L"(������ �� ��������..�ƴ� �̰� ���� �޶�)");
	m_szVillageSmell_3.push_back(L"(��� �����°���..�� �ʸ��� �� ���ΰ�?)");


// Village Talk
	// 14
	m_szHouseSmell_1.push_back(L"(������� ��� �������־�! �� �տ� ���Ͱ�..?!)");
	
	// 15
	m_szHouseSmell_2.push_back(L"(������ ������ �����־�2)");
	m_szHouseSmell_2.push_back(L"(....���θ���)");

	// 16
	m_szHouseSmell_3.push_back(L"(������ ������ �����־�3)");
	m_szHouseSmell_3.push_back(L"(....���θ���)");



	return S_OK;
}

void CDialog::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if(m_Is_In == true)
		Fade_In(TimeDelta);

	if (m_Is_Out == true)
		Fade_Out(TimeDelta);

	Set_UI();

}

void CDialog::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Player_Info(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	
	m_Dialog_Type = CDialogManager::GetInstance()->Get_Dialog_Type();

	if (pGameInstance->Get_DIKeyDown(DIK_F) && m_Is_Font_Render == true && m_Dialog_Type != 99) {

		m_Cur_Num++;
		m_Name_Type *= -1;

		// Main
		if (m_Dialog_Type == 0) {
			if (m_Cur_Num > m_szMain_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 1) {
			if (m_Cur_Num > m_szMain_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 2) {
			if (m_Cur_Num > m_szMain_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// Sub
		else if (m_Dialog_Type == 3) {
			if (m_Cur_Num > m_szSub_1.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 4) {
			if (m_Cur_Num > m_szSub_2.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 5) {
			if (m_Cur_Num > m_szSub_3.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 6) {
			if (m_Cur_Num > m_szSub_4.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 7) {
			if (m_Cur_Num > m_szSub_5.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// Talk
		else if (m_Dialog_Type == 8) {
			if (m_Cur_Num > m_szTalk_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 9) {
			if (m_Cur_Num > m_szTalk_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 10) {
			if (m_Cur_Num > m_szTalk_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// Village Smell
		else if (m_Dialog_Type == 11) {
			if (m_Cur_Num > m_szVillageSmell_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 12) {
			if (m_Cur_Num > m_szVillageSmell_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 13) {
			if (m_Cur_Num > m_szVillageSmell_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// House Smell
		else if (m_Dialog_Type == 14) {
			if (m_Cur_Num > m_szHouseSmell_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 15) {
			if (m_Cur_Num > m_szHouseSmell_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 16) {
			if (m_Cur_Num > m_szHouseSmell_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}

	}

	if (pGameInstance->Get_DIKeyDown(DIK_F) && m_Is_In == false && m_Is_Out == false && m_Dialog_Type != 99) {
		m_Is_In = true;
		m_Is_Font_Render = true;
	}

	if (m_Is_Font_Render == true)
		CMissionManager::GetInstance()->Set_Is_Dialog_On(true);
	else {
		CMissionManager::GetInstance()->Set_Is_Dialog_On(false);
	}



	Safe_Release(pGameInstance);

	
	if (m_Alpha == 0.f) {
		m_Is_Render = false;
	}
	else {
		m_Is_Render = true;
	}

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CDialog::Render()
{
	if (m_Is_Render == true) {

		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		if (m_Is_Reverse == false)
			m_pShaderCom->Begin(1);
		else {
			m_pShaderCom->Begin(2);
		}


		if (m_Is_CutScene == false) {

			m_pVIBufferCom->Render();

		}
	}

	// ��Ʈ ����
	if (m_Is_Font_Render) {

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);


		if (m_UI_Desc.m_Type == 0) {
			if (m_Dialog_Type == 0) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szMain_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szMain_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 1) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szMain_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szMain_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 2) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szMain_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szMain_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 3) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szSub_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 4) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 60.f - (_float)m_szSub_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 5) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 50.f - (_float)m_szSub_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 6) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_4[m_Cur_Num].c_str(), _float2((_float)m_fX - 50.f - (_float)m_szSub_4[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 7) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_5[m_Cur_Num].c_str(), _float2((_float)m_fX - 85.f - (_float)m_szSub_5[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 8) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTalk_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szTalk_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 9) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTalk_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 60.f - (_float)m_szTalk_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 10) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTalk_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szTalk_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 11) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szVillageSmell_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 12) {
				if (m_Cur_Num == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_2[m_Cur_Num].c_str(), _float2((_float)m_fX + 0.f - (_float)m_szVillageSmell_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szVillageSmell_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				
			}
			else if (m_Dialog_Type == 13) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szVillageSmell_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 14) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szHouseSmell_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szHouseSmell_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 15) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szHouseSmell_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szHouseSmell_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 16) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szHouseSmell_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szHouseSmell_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}

		}
		else if (m_UI_Desc.m_Type == 1) {
			if (m_Dialog_Type == 0) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 1) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[MOTHER].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 2) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 3) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[MAN].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 4) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[MAN].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 5) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 6) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[BOY].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 7) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 8) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 9) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[ZENITSU].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 10) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[ZENITSU].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type >= 11 && m_Dialog_Type <= 16) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
		}


		Safe_Release(pGameInstance);

	}


	return S_OK;
}

HRESULT CDialog::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dialog"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CDialog::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_R", &m_Is_Side_Cut_R, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_L", &m_Is_Side_Cut_L, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
		return E_FAIL;



	return S_OK;
}

void CDialog::Set_UI()
{
	m_fSizeX = m_Origin_X * (_double)m_Size_Param;
	m_fSizeY = m_Origin_Y * (_double)m_Size_Param;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		Viewport;
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);

	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix.r[0] = TransformMatrix.r[0] * (_float)m_fSizeX;
	TransformMatrix.r[1] = TransformMatrix.r[1] * (_float)m_fSizeY;

	TransformMatrix.r[3] = XMVectorSetX(TransformMatrix.r[3], (_float)m_fX - Viewport.Width * 0.5f);
	TransformMatrix.r[3] = XMVectorSetY(TransformMatrix.r[3], -(_float)m_fY + Viewport.Height * 0.5f);

	XMStoreFloat4x4(&m_WorldMatrix, TransformMatrix);

}

void CDialog::Get_Player_Info(_double TimeDelta)
{

	
}

void CDialog::Fade_In(_double TimeDelta)
{
	m_Alpha += (_float)TimeDelta * 2.f;

	if (m_Alpha >= 1.f)
	{
		m_Alpha = 1.f;
	}

	m_fY -= TimeDelta * 120.0;

	if (m_fY <= m_Origin_PosY)
	{
		m_fY = m_Origin_PosY;
		
	}

	if (m_Alpha >= 1.f && m_fY <= m_Origin_PosY) {
		m_Is_In = false;
	}


}

void CDialog::Fade_Out(_double TimeDelta)
{

	m_Alpha -= (_float)TimeDelta * 2.5f;

	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
	}

	m_fY += TimeDelta * 100.0;

	if (m_fY >= m_Start_PosY)
	{
		m_fY = m_Start_PosY;
		
	}

	if (m_Alpha <= 0.f && m_fY >= m_Start_PosY) {
		m_Is_Out = false;
	}

}



CDialog * CDialog::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDialog*		pInstance = new CDialog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CDialog::Clone(void * pArg)
{
	CDialog*		pInstance = new CDialog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDialog::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
