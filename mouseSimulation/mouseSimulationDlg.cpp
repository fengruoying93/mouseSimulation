
// mouseSimulationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mouseSimulation.h"
#include "mouseSimulationDlg.h"
#include "AddActionDlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> //ShellExecuteA()
#include <iostream>
#include <string>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CmouseSimulationDlg �Ի���



CmouseSimulationDlg::CmouseSimulationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CmouseSimulationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmouseSimulationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST, m_myListCtrl);
	DDX_Control(pDX, IDC_BUTTON_ADD_ACTION, buttonAddAction);
	DDX_Control(pDX, IDC_BUTTON_RUN, buttonRun);
	DDX_Control(pDX, IDC_EDIT_LOOP, editLoopCount);
}

BEGIN_MESSAGE_MAP(CmouseSimulationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN() //���
	//ON_WM_MOUSEMOVE() //����ƶ�
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()

	ON_COMMAND(IDM_ABOUT, &CmouseSimulationDlg::OnAbout)
	ON_COMMAND(IDM_RUN, &CmouseSimulationDlg::OnRun)
	ON_COMMAND(IDM_SETTING, &CmouseSimulationDlg::OnSetting)

	ON_MESSAGE(WM_MyMessage, &CmouseSimulationDlg::OnMyMessage)

	ON_BN_CLICKED(IDC_BUTTON_ADD_ACTION, &CmouseSimulationDlg::OnBnClickedButtonAddAction)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CmouseSimulationDlg::OnBnClickedButtonRun)
END_MESSAGE_MAP()


// CmouseSimulationDlg ��Ϣ�������

BOOL CmouseSimulationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	InitMyMenu();					//��ʼ���˵�
	InitControls();					//��ʼ���ؼ�
	InitMyListCtrl();				//��ʼ���б�ؼ�

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CmouseSimulationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CmouseSimulationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CmouseSimulationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CmouseSimulationDlg::InitControls(void)
{
	this->SetWindowText(TEXT("���ģ����"));

	timerId = 1;
	SetTimer(timerId,200,NULL); //200ms��ʱ��

	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);

	return TRUE;
}

BOOL CmouseSimulationDlg::InitMyMenu(void)
{
	pMainMenu  =  new CMenu();
	pMainMenu->LoadMenu(IDR_MAINFRAME);			//  IDR_MAINFRAMEΪ�˵�ID
	SetMenu(pMainMenu);
	pMainMenu->Detach();
	return TRUE;
}

/************************************************************************/
/* ������:InitMyListCtrl
/* ��������		����ֵ����ʼ���ɹ�����TRUE
/* ��������ʼ���б�ؼ��ķ�񣬲���ʼ���б�ؼ���������									   */
/************************************************************************/
BOOL CmouseSimulationDlg::InitMyListCtrl(void)
{
	m_myListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
	m_myListCtrl.ModifyStyle(0,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL);

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH ;
	lvc.fmt  = LVCFMT_CENTER ;
	lvc.cx   = SERIALNUMBERWIDTH ;
	lvc.pszText = _T("���");
	m_myListCtrl.InsertColumn(ID_SERIALNUMBER, &lvc);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_CENTER; //LVCFMT_LEFT
	lvc.cx = ACTIONWIDTH;
	lvc.pszText = _T("����");
	m_myListCtrl.InsertColumn(ID_ACTION, &lvc);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_CENTER;
	lvc.cx = PARAM1WIDTH;
	lvc.pszText = _T("����1");
	m_myListCtrl.InsertColumn(ID_PARAM1, &lvc);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_CENTER;
	lvc.cx = PARAM2WIDTH;
	lvc.pszText = _T("����2");
	m_myListCtrl.InsertColumn(ID_PARAM2, &lvc);

	AddItemToListCtrl(0, CString("100"), CString("100"));

	return 0;
}

/************************************************************************/
/* ������:AddFileToPlayList
/* ��������
/* ���������Ӹ���������ͬʱ���б�ؼ�������Ŀ��                                 */
/************************************************************************/
void CmouseSimulationDlg::AddItemToListCtrl(int action, CString &param1, CString &param2)
{
	LVITEM  item;			 
	int		nIndex;				 //��ǰ����λ�ã��У�
	CString strTmp;
	CString strAction;

	item.mask = LVIF_TEXT;	
	item.iSubItem = 0;

	item.iItem = (m_myListCtrl.GetItemCount() <= 0) ? 0 : m_myListCtrl.GetItemCount(); 
	strTmp.Format(TEXT("%d"), ++item.iItem);				//��ʽ����ǰ����
	item.pszText = strTmp.GetBuffer(strTmp.GetLength());	//�ڵ�һ�в������

	nIndex = m_myListCtrl.InsertItem(&item);				//�ڵ�һ�в������

	if(LEFT_CLICK == action){
		strAction = CString(TEXT("������"));
	}else if(RIGHT_CLICK == action){
		strAction = CString(TEXT("����Ҽ�"));
	}else if(KEY_INPUT == action){
		strAction = CString(TEXT("��������"));
	}else if(DELAY_TIME == action){
		strAction = CString(TEXT("��ʱ"));
	}
	m_myListCtrl.SetItemText(nIndex, ID_ACTION, strAction);	//second column of the ListCtrl
	m_myListCtrl.SetItemText(nIndex, ID_PARAM1, param1);
	m_myListCtrl.SetItemText(nIndex, ID_PARAM2, param2);
	m_myListCtrl.SetFocus();
	m_myListCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
}
void CmouseSimulationDlg::OnAbout()
{
	CAboutDlg aboutdlg;
	aboutdlg.DoModal();
}

void CmouseSimulationDlg::OnRun()
{
	CString text;
	text.Format(_T("cxScreen=%d cyScreen=%d"), cxScreen, cyScreen);
	MessageBox(text, _T("���ȷ������"), MB_OK);
	Sleep(3000);
	click(70,1060);
	Sleep(2000);
	input((string)"12345");
}

void CmouseSimulationDlg::OnSetting()
{
	MessageBox(_T("OnSetting!"), _T("��������"), MB_OK);
}

void CmouseSimulationDlg::OnTimer(UINT_PTR nIDEvent)
{
	POINT point;
	CString text;

	if(timerId == nIDEvent)
	{
		GetCursorPos(&point);
		text.Format(_T("���ģ���� (x=%d y=%d)"), point.x, point.y);
		SetWindowText(text);
	}
	CDialog::OnTimer(nIDEvent);
}

void CmouseSimulationDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	return;
	CString text;
	text.Format(_T("lx=%d ly=%d"), point.x, point.y);
	SetWindowText(text);
}

#if 0
void CmouseSimulationDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CString text;
	text.Format(_T("x=%d y=%d"), point.x, point.y);
	SetWindowText(text);
}
#endif

//�Զ�����Ϣ����
LRESULT CmouseSimulationDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	MessageBox(_T("OnMyMessage!"), _T("�Զ�����Ϣ"), MB_OK);
	return 0;
}
 
//��ĳ����ַ:website ��ʹ��Ĭ��������� 
void CmouseSimulationDlg::open_web(char *website)
{
	ShellExecuteA(0,"open", website,0,0,1);
}
 
 
//ģ�������  ��x,y����Ҫ�����λ�� 
void CmouseSimulationDlg::click(int x, int y)
{
	//��������ƶ��� ָ����λ��     ��������Ļ�ֱ���1600x900  ���������ϵͳ�У���Ļ��ˮƽ�ʹ�ֱ�����Ͼ��ȷָ��65535��65535����Ԫ
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, x*65535/cxScreen, y*65535/cyScreen, 0, 0);
	
	Sleep(50);//��΢��ʱ50ms 
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);//���������� 
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//������̧��
 }
 
//ģ��������� keybd_event(Ҫ���µ��ַ�,0,����,0);����Ϊ0�ǰ��£�����Ϊ2��̧�� 
void CmouseSimulationDlg::input(string &input)
{
	int i;
	
	for(i=0;i<sizeof(input.size());i++)
	{
		keybd_event(input[i],0,0,0);
		keybd_event(input[i],0,2,0);
		Sleep(30);	
	}
	
	//tab�� ��Ӧ�ı����0x09  ����������� ��ȡ���� 
	//keybd_event(0x09,0,0,0);//���� 
	//keybd_event(0x09,0,2,0); //�ɿ� 
	//Sleep(30);	
}
 
 
//��chrome.exe����ɱ��������������δʹ�� 
void CmouseSimulationDlg::close_app()
{
	system("taskkill  /f  /im chrome.exe");
}


void CmouseSimulationDlg::OnBnClickedButtonAddAction()
{
	CAddActionDlg actiondlg;
	actiondlg.DoModal();
}


void CmouseSimulationDlg::OnBnClickedButtonRun()
{
	if(IDOK == MessageBox(_T("���ȷ��3s��ʼ����!"), _T("����"), MB_OKCANCEL))
	{
		Sleep(3000);
		click(70,1060);
		Sleep(2000);
		input((string)"12345");
	}
}
