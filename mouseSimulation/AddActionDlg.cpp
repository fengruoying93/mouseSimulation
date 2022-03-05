// AddActionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mouseSimulation.h"
#include "AddActionDlg.h"
#include "afxdialogex.h"


// CAddActionDlg 对话框

IMPLEMENT_DYNAMIC(CAddActionDlg, CDialogEx)

CAddActionDlg::CAddActionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddActionDlg::IDD, pParent)
{
}

CAddActionDlg::~CAddActionDlg()
{
}

void CAddActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LEFT_X, editLeftX);
	DDX_Control(pDX, IDC_EDIT_LEFT_Y, editLeftY);
	DDX_Control(pDX, IDC_EDIT_RIGHT_X, editRightX);
	DDX_Control(pDX, IDC_EDIT_RIGHT_Y, editRightY);
	DDX_Control(pDX, IDC_EDIT_KEY_INPUT, editKeyInput);
	DDX_Control(pDX, IDC_EDIT_SLEEP, editDelay);
}

BOOL CAddActionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	editLeftX.EnableWindow(false);
	editLeftY.EnableWindow(false);
	editRightX.EnableWindow(false);
	editRightY.EnableWindow(false);
	editKeyInput.EnableWindow(false);
	editDelay.EnableWindow(false);

	return true;
}

BEGIN_MESSAGE_MAP(CAddActionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_LEFT_CLICK, &CAddActionDlg::OnBnClickedRadioLeftClick)
	ON_BN_CLICKED(IDC_RADIO_RIGHT_CLICK, &CAddActionDlg::OnBnClickedRadioRightClick)
	ON_BN_CLICKED(IDC_RADIO_KEY_INPUT, &CAddActionDlg::OnBnClickedRadioKeyInput)
	ON_BN_CLICKED(IDC_RADIO_SLEEP, &CAddActionDlg::OnBnClickedRadioSleep)
	ON_BN_CLICKED(IDOK, &CAddActionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddActionDlg 消息处理程序


void CAddActionDlg::OnBnClickedRadioLeftClick()
{
	editLeftX.EnableWindow(true);
	editLeftY.EnableWindow(true);
	editRightX.EnableWindow(false);
	editRightY.EnableWindow(false);
	editKeyInput.EnableWindow(false);
	editDelay.EnableWindow(false);

	detail.action_type = LEFT_CLICK;
}


void CAddActionDlg::OnBnClickedRadioRightClick()
{
	editLeftX.EnableWindow(false);
	editLeftY.EnableWindow(false);
	editRightX.EnableWindow(true);
	editRightY.EnableWindow(true);
	editKeyInput.EnableWindow(false);
	editDelay.EnableWindow(false);

	detail.action_type = RIGHT_CLICK;
}


void CAddActionDlg::OnBnClickedRadioKeyInput()
{
	editLeftX.EnableWindow(false);
	editLeftY.EnableWindow(false);
	editRightX.EnableWindow(false);
	editRightY.EnableWindow(false);
	editKeyInput.EnableWindow(true);
	editDelay.EnableWindow(false);

	detail.action_type = KEY_INPUT;
}


void CAddActionDlg::OnBnClickedRadioSleep()
{
	editLeftX.EnableWindow(false);
	editLeftY.EnableWindow(false);
	editRightX.EnableWindow(false);
	editRightY.EnableWindow(false);
	editKeyInput.EnableWindow(false);
	editDelay.EnableWindow(true);

	detail.action_type = DELAY_TIME;
}


void CAddActionDlg::OnBnClickedOk()
{
	if(LEFT_CLICK == detail.action_type){
		editLeftX.GetWindowText(detail.param1);
		editLeftY.GetWindowText(detail.param2);
	}else if(RIGHT_CLICK == detail.action_type){
		editRightX.GetWindowText(detail.param1);
		editRightY.GetWindowText(detail.param2);
	}else if(KEY_INPUT == detail.action_type){
		editKeyInput.GetWindowText(detail.param1);
		detail.param2.Empty();
	}else if(DELAY_TIME == detail.action_type){
		editDelay.GetWindowText(detail.param1);
		detail.param2.Empty();
	}
	HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	::SendMessage(hwnd, WM_MyMessage, (WPARAM)&detail, 0);

	CDialogEx::OnOK();
}
