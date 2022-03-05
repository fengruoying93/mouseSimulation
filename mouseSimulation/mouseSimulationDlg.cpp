
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
#include "cJSON.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ASYNC_TASK

typedef struct
{
	ACTION_TYPE_E type;
	int x;
	int y;
	int delay;
	std::string input;
}CLICK_TASK_NODE;

typedef struct
{
	int loop;
	int nodeNum;
	CLICK_TASK_NODE node[256];
}CLICK_TASK;

static CLICK_TASK g_task;

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
	ON_WM_CTLCOLOR()

	ON_COMMAND(IDM_ABOUT, &CmouseSimulationDlg::OnAbout)
	ON_COMMAND(IDM_RUN, &CmouseSimulationDlg::OnRun)
	ON_COMMAND(IDM_SETTING, &CmouseSimulationDlg::OnSetting)

	ON_BN_CLICKED(IDC_BUTTON_ADD_ACTION, &CmouseSimulationDlg::OnBnClickedButtonAddAction)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CmouseSimulationDlg::OnBnClickedButtonRun)

	ON_NOTIFY(NM_RCLICK, IDC_LIST, &CmouseSimulationDlg::OnNMRClickPlaylist)

	ON_MESSAGE(WM_MyMessage, &CmouseSimulationDlg::OnMyMessage)
	ON_COMMAND(ID_DELETE_ACTION, &CmouseSimulationDlg::OnDeleteAction)
	ON_COMMAND(ID_EXIT, &CmouseSimulationDlg::OnExit)
	ON_COMMAND(ID_IMPORT, &CmouseSimulationDlg::OnImport)
	ON_COMMAND(ID_EXPORT, &CmouseSimulationDlg::OnExport)
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

	usr_brush = (HBRUSH)CreateSolidBrush(RGB(66, 213, 73));

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
		//CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		//dc.TextOut(100, 100, _T("x: y:"));

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

	return 0;
}

/************************************************************************/
/* ������:AddFileToPlayList
/* ��������
/* ���������Ӹ���������ͬʱ���б�ؼ�������Ŀ��                                 */
/************************************************************************/
void CmouseSimulationDlg::AddItemToListCtrl(CString &action, CString &param1, CString &param2)
{
	LVITEM  item;			 
	int		nIndex;				 //��ǰ����λ�ã��У�
	CString strTmp;

	item.mask = LVIF_TEXT;	
	item.iSubItem = 0;

	item.iItem = (m_myListCtrl.GetItemCount() <= 0) ? 0 : m_myListCtrl.GetItemCount(); 
	strTmp.Format(TEXT("%d"), ++item.iItem);				//��ʽ����ǰ����
	item.pszText = strTmp.GetBuffer(strTmp.GetLength());	//�ڵ�һ�в������

	nIndex = m_myListCtrl.InsertItem(&item);				//�ڵ�һ�в������

	m_myListCtrl.SetItemText(nIndex, ID_ACTION, action);	//second column of the ListCtrl
	m_myListCtrl.SetItemText(nIndex, ID_PARAM1, param1);
	m_myListCtrl.SetItemText(nIndex, ID_PARAM2, param2);
	m_myListCtrl.SetFocus();
	m_myListCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
}

/************************************************************************/
/* ������:DeleteItem
/* ��������
/* ������ɾ��������Ŀ��ͬʱ���б�ؼ�ɾ���ı���                                 */
/************************************************************************/
void CmouseSimulationDlg::DeleteItem(void)
{
	CString strTmp;
	int nIndexSelection = 0;

	POSITION pos =  m_myListCtrl.GetFirstSelectedItemPosition();     
	nIndexSelection = m_myListCtrl.GetNextSelectedItem(pos); //��õ�ǰ�����б��ѡ����λ��

	m_myListCtrl.DeleteItem(nIndexSelection);
	nIndexSelection--;
	m_myListCtrl.SetFocus();
	m_myListCtrl.SetItemState(nIndexSelection, LVIS_SELECTED, LVIS_SELECTED);
}

void CmouseSimulationDlg::OnAbout()
{
	CAboutDlg aboutdlg;
	aboutdlg.DoModal();
}

void CmouseSimulationDlg::OnRun()
{
	OnBnClickedButtonRun();
}

void CmouseSimulationDlg::OnSetting()
{
#if 0
	HWND hwnd = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	CString tmp;
	tmp.Format(TEXT("%d"), hwnd);
	MessageBox(tmp, _T("����"), MB_OK);
#endif
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
	ADD_ACTION_DETAIL *detail = (ADD_ACTION_DETAIL*)wParam;
	CString strAction;
	if(LEFT_CLICK == detail->action_type){
		strAction = CString(TEXT("������"));
	}else if(RIGHT_CLICK == detail->action_type){
		strAction = CString(TEXT("����Ҽ�"));
	}else if(KEY_INPUT == detail->action_type){
		strAction = CString(TEXT("��������"));
	}else if(DELAY_TIME == detail->action_type){
		strAction = CString(TEXT("��ʱ"));
	}
	AddItemToListCtrl(strAction, detail->param1, detail->param2);
	return 0;
}
 
//��ĳ����ַ:website ��ʹ��Ĭ��������� 
void CmouseSimulationDlg::open_web(char *website)
{
	ShellExecuteA(0,"open", website,0,0,1);
}
 
 
//ģ�����������  ��x,y����Ҫ�����λ�� 
void CmouseSimulationDlg::left_click(int x, int y, int screenX, int screenY)
{
	//��������ƶ��� ָ����λ��     ��������Ļ�ֱ���1600x900  ���������ϵͳ�У���Ļ��ˮƽ�ʹ�ֱ�����Ͼ��ȷָ��65535��65535����Ԫ
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, x*65535/screenX, y*65535/screenY, 0, 0);
	
	Sleep(50);//��΢��ʱ50ms 
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);//���������� 
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//������̧��
 }

//ģ������Ҽ����  ��x,y����Ҫ�����λ�� 
void CmouseSimulationDlg::right_click(int x, int y, int screenX, int screenY)
{
	//��������ƶ��� ָ����λ��     ��������Ļ�ֱ���1600x900  ���������ϵͳ�У���Ļ��ˮƽ�ʹ�ֱ�����Ͼ��ȷָ��65535��65535����Ԫ
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, x*65535/screenX, y*65535/screenY, 0, 0);
	
	Sleep(50);//��΢��ʱ50ms 
    mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);//���������� 
    mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);//������̧��
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

#ifdef ASYNC_TASK
DWORD WINAPI runTask(LPVOID lpParam)
{
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	Sleep(3000);
	for(int i = 0; i < g_task.loop; i++)
	{
		for(int j = 0; j < g_task.nodeNum; j++)
		{
			if(LEFT_CLICK == g_task.node[j].type){
				CmouseSimulationDlg::left_click(g_task.node[j].x, g_task.node[j].y, cxScreen, cyScreen);
			}else if(RIGHT_CLICK == g_task.node[j].type){
				CmouseSimulationDlg::right_click(g_task.node[j].x, g_task.node[j].y, cxScreen, cyScreen);
			}else if(KEY_INPUT == g_task.node[j].type){
				CmouseSimulationDlg::input(g_task.node[j].input);
			}else if(DELAY_TIME == g_task.node[j].type){
				Sleep(g_task.node[j].delay);
			}
		}
	}
	return 0;
}
#endif

void CmouseSimulationDlg::OnBnClickedButtonRun()
{
	DWORD threadID;
	HANDLE hThread;
	if(IDOK == MessageBox(_T("���ȷ��3s��ʼ����!"), _T("����"), MB_OKCANCEL))
	{
		int i, j, loopCount;
		int x, y;
		CString action, param1, param2;

		editLoopCount.GetWindowText(str_loop);
		loopCount = (_ttoi(str_loop) > 0) ? _ttoi(str_loop) : 1;

		#ifdef ASYNC_TASK
		memset(&g_task, 0, sizeof(g_task));
		g_task.loop = loopCount;
		for(i = 0; i < m_myListCtrl.GetItemCount(); i++)
		{
			action = m_myListCtrl.GetItemText(i, ID_ACTION);
			param1 = m_myListCtrl.GetItemText(i, ID_PARAM1);
			param2 = m_myListCtrl.GetItemText(i, ID_PARAM2);
			if(TEXT("������") == action){
				g_task.node[i].type = LEFT_CLICK;
				g_task.node[i].x = _ttoi(param1);
				g_task.node[i].y = _ttoi(param2);
			}else if(TEXT("����Ҽ�") == action){
				g_task.node[i].type = RIGHT_CLICK;
				g_task.node[i].x = _ttoi(param1);
				g_task.node[i].y = _ttoi(param2);
			}else if(TEXT("��������") == action){
				g_task.node[i].type = KEY_INPUT;
				std::string s_input(CW2A(param1.GetString())); //CString->string
				g_task.node[i].input = s_input;
			}else if(TEXT("��ʱ") == action){
				g_task.node[i].type = DELAY_TIME;
				g_task.node[i].delay = _ttoi(param1);
			}
			g_task.nodeNum++;
		}
		hThread = CreateThread(NULL,0, runTask, NULL,0,&threadID);

		#else

		Sleep(3000);
		for(i = 0; i < loopCount; i++)
		{
			for(j = 0; j <m_myListCtrl.GetItemCount(); j++)
			{
				m_myListCtrl.SetFocus();
				m_myListCtrl.SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
				action = m_myListCtrl.GetItemText(j, ID_ACTION);
				param1 = m_myListCtrl.GetItemText(j, ID_PARAM1);
				param2 = m_myListCtrl.GetItemText(j, ID_PARAM2);

				if(TEXT("������") == action){
					x = _ttoi(param1);
					y = _ttoi(param2);
					left_click(x, y, cxScreen, cyScreen);
				}else if(TEXT("����Ҽ�") == action){
					x = _ttoi(param1);
					y = _ttoi(param2);
					right_click(x, y, cxScreen, cyScreen);
				}else if(TEXT("��������") == action){
					std::string s_input(CW2A(param1.GetString())); //CString->string
					input(s_input);
				}else if(TEXT("��ʱ") == action){
					Sleep(_ttoi(param1));
				}
			}
		}
		#endif
	}
}

//���б�ؼ��ϵ�������Ҽ�����ʾ�����Ĳ˵���
void CmouseSimulationDlg::OnNMRClickPlaylist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nIndexSelection = 0;
	POSITION pos =  m_myListCtrl.GetFirstSelectedItemPosition();     
	nIndexSelection = m_myListCtrl.GetNextSelectedItem(pos); //��õ�ǰ�����б��ѡ����λ��
	if (nIndexSelection != -1)
	{
		CMenu menu;
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
		VERIFY( menu.LoadMenu(IDR_CONTEXTMENU));
		CMenu* popupMenu = menu.GetSubMenu(0);
		
		//ASSERT( popup != NULL );
		popupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}

	*pResult = 0;
}

void CmouseSimulationDlg::OnDeleteAction()
{
	DeleteItem();
}

HBRUSH CmouseSimulationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT id = pWnd->GetDlgCtrlID();

	if(nCtlColor == CTLCOLOR_STATIC) //��̬�ؼ�
	{
	}
	else if(nCtlColor == CTLCOLOR_BTN) //��ť
	{
		if(id == IDC_BUTTON_RUN)
		{
			pDC->SetBkMode(TRANSPARENT);
			return usr_brush;
		}
	}
	else if(nCtlColor == CTLCOLOR_EDIT||CTLCOLOR_LISTBOX == nCtlColor) //�༭��
	{
	}
	return hbr;
}


void CmouseSimulationDlg::OnExit()
{
	SendMessage(WM_CLOSE,0,0);
}


void CmouseSimulationDlg::OnImport()
{
	int ret = 0;
	cJSON *pJsonRoot = NULL;
	cJSON *loopJson = NULL;
	cJSON *actionList = NULL;
	cJSON *pSubJson = NULL;
	cJSON *actionJson = NULL;
	cJSON *param1Json = NULL;
	cJSON *param2Json = NULL;
	CString action;
	CString param1;
	CString param2;
	char *json_data = NULL;
	FILE *json_file = NULL;
	char Ascii[256] = {0};

	//���ļ��Ի���֧�ֵ��ļ���չ��
	LPCTSTR szFilters = TEXT("���뷽��(*.json)|*.json|�����ļ� (*.*)|*.*||");
	//����һ�����ļ��Ի������
	CFileDialog FileDlg(TRUE,TEXT("json"),NULL,OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST , szFilters );
	if(IDCANCEL == FileDlg.DoModal())
	{
		ret = -1;
		goto EXIT;
	}
	else 
	{
		CString strFilePath = FileDlg.GetPathName();	//�ļ�·����
		CString strFileName = FileDlg.GetFileTitle();	//�ļ���
		//�����ֽ�ת��Ϊ���ֽ�
		int len = WideCharToMultiByte( CP_ACP , 0 , strFilePath , strFilePath.GetLength() , NULL , 0 , NULL , NULL );
		len = WideCharToMultiByte( CP_ACP , 0 , strFilePath , strFilePath.GetLength(), Ascii , len +1 , NULL ,NULL );
		Ascii[len] = 0;
		//path = (char*)strFilePath.GetBuffer(strFilePath.GetLength());
		json_file = fopen(Ascii, "r+"); //ע��������a+ֻ�����ļ�ĩβ׷�����ݶ������ƶ����
		if(!json_file)
		{
			ret = -1;
			goto EXIT;
		}
		if(-1 == fseek(json_file, 0, SEEK_SET))
		{
			ret = -1;
			goto EXIT;
		}
		json_data = (char*)malloc(100*1024);
		if(!json_data)
		{
			ret = -1;
			goto EXIT;
		}
		memset(json_data, 0, 100*1024);
		int ret = fread(json_data, 100*1024, 1, json_file);
		if(ret < 0)
		{
			memset(json_data, 0, 100*1024);
			ret = -1;
			goto EXIT;
		}
		pJsonRoot = cJSON_Parse(json_data);
		if(NULL == pJsonRoot)                                                                                         
		{
			ret = -1;
			goto EXIT;
		}
		loopJson = cJSON_GetObjectItem(pJsonRoot, "loop");
		if(loopJson)                                                                                         
		{
			str_loop.Format(_T("%d"), loopJson->valueint);
			editLoopCount.SetWindowText(str_loop);
		}
		actionList = cJSON_GetObjectItem(pJsonRoot, "actionList");
		if(NULL == actionList)                                                                                         
		{
			ret = -1;
			goto EXIT;
		}
		int array_size = cJSON_GetArraySize(actionList);
		m_myListCtrl.DeleteAllItems();
		for(int i = 0; i < array_size; i++)
		{
			pSubJson = cJSON_GetArrayItem(actionList, i);
			if(NULL == pSubJson)                                                                                         
			{
				ret = -1;
				goto EXIT;
			}
			actionJson = cJSON_GetObjectItem(pSubJson, "action");
			if(actionJson)
			{
				action = CString(actionJson->valuestring);
			}
			param1Json = cJSON_GetObjectItem(pSubJson, "param1");
			if(param1Json)
			{
				param1 = CString(param1Json->valuestring);
			}
			param2Json = cJSON_GetObjectItem(pSubJson, "param2");
			if(param2Json)
			{
				param2 = CString(param2Json->valuestring);
			}
			AddItemToListCtrl(action, param1, param2);
		}
	}
	ret = 0;

EXIT:
	SAFE_FREE(json_data);
	if(json_file)
	{
		fclose(json_file);
	}
	if(pJsonRoot)
	{
		cJSON_Delete(pJsonRoot);
	}
	if(0 == ret)
		MessageBox(_T("����ɹ�"), _T("����"), MB_OK);
	else
		MessageBox(_T("����ʧ��"), _T("����"), MB_OK);
}


void CmouseSimulationDlg::OnExport()
{
	int j = 0;
	CString action, param1, param2;
	cJSON *pJsonRoot = NULL;
	cJSON *actionList = NULL;
	cJSON *pSubJson = NULL;
	cJSON *loopJson = NULL;
	int ret = 0;
	char Ascii[256] = {0};

	pJsonRoot = cJSON_CreateObject();
	actionList = cJSON_CreateArray();
	editLoopCount.GetWindowText(str_loop);
	int loopCount = (_ttoi(str_loop) > 0) ? _ttoi(str_loop) : 1;
	cJSON_AddNumberToObject(pJsonRoot, "loop", loopCount);
	cJSON_AddItemToObject(pJsonRoot, "actionList", actionList);

	for(j = 0; j <m_myListCtrl.GetItemCount(); j++)
	{
		m_myListCtrl.SetFocus();
		m_myListCtrl.SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
		action = m_myListCtrl.GetItemText(j, ID_ACTION);
		param1 = m_myListCtrl.GetItemText(j, ID_PARAM1);
		param2 = m_myListCtrl.GetItemText(j, ID_PARAM2);
		
		pSubJson = cJSON_CreateObject();
		cJSON_AddNumberToObject(pSubJson, "id", j+1);
		//�����ֽ�ת��Ϊ���ֽ�
		int len = WideCharToMultiByte( CP_ACP , 0 , action , action.GetLength() , NULL , 0 , NULL , NULL );
		len = WideCharToMultiByte( CP_ACP , 0 , action , action.GetLength(), Ascii , len+1 , NULL ,NULL );
		Ascii[len] = 0;
		cJSON_AddStringToObject(pSubJson, "action", Ascii); //(char*)action.GetBuffer()

		len = WideCharToMultiByte( CP_ACP , 0 , param1 , param1.GetLength() , NULL , 0 , NULL , NULL );
		len = WideCharToMultiByte( CP_ACP , 0 , param1 , param1.GetLength(), Ascii , len+1 , NULL ,NULL );
		Ascii[len] = 0;
		cJSON_AddStringToObject(pSubJson, "param1", Ascii);  //(char*)param1.GetBuffer()

		len = WideCharToMultiByte( CP_ACP , 0 , param2 , param2.GetLength() , NULL , 0 , NULL , NULL );
		len = WideCharToMultiByte( CP_ACP , 0 , param2 , param2.GetLength(), Ascii , len+1 , NULL ,NULL );
		Ascii[len] = 0;
		cJSON_AddStringToObject(pSubJson, "param2", Ascii);  //(char*)param2.GetBuffer()
		cJSON_AddItemToArray(actionList, pSubJson);
	}

	char *p = cJSON_Print(pJsonRoot);
    if(NULL == p)
    {
        cJSON_Delete(pJsonRoot);
		ret = -1;
        goto EXIT;
    }
	cJSON_Delete(pJsonRoot);

	FILE *json_file = fopen("plan_list.json", "w+"); //�ļ������ڵĻ���w+����,�����ļ����Ƚض�Ϊ0����д
	if(json_file)
	{
		int ret = fwrite(p, strlen(p), 1, json_file);
		if(ret < 1)
		{
			fclose(json_file);
			SAFE_FREE(p);
			ret = -1;
			goto EXIT;
		}
		fclose(json_file);
	}
	SAFE_FREE(p);
	ret = 0;

EXIT:
	if(0 == ret)
		MessageBox(_T("�����ɹ�"), _T("����"), MB_OK);
	else
		MessageBox(_T("����ʧ��"), _T("����"), MB_OK);
}
