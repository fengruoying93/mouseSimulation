
// mouseSimulationDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CmouseSimulationDlg 对话框



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
	ON_WM_LBUTTONDOWN() //左键
	//ON_WM_MOUSEMOVE() //鼠标移动
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


// CmouseSimulationDlg 消息处理程序

BOOL CmouseSimulationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	InitMyMenu();					//初始化菜单
	InitControls();					//初始化控件
	InitMyListCtrl();				//初始化列表控件

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmouseSimulationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmouseSimulationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CmouseSimulationDlg::InitControls(void)
{
	this->SetWindowText(TEXT("鼠标模拟器"));

	timerId = 1;
	SetTimer(timerId,200,NULL); //200ms定时器

	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);

	return TRUE;
}

BOOL CmouseSimulationDlg::InitMyMenu(void)
{
	pMainMenu  =  new CMenu();
	pMainMenu->LoadMenu(IDR_MAINFRAME);			//  IDR_MAINFRAME为菜单ID
	SetMenu(pMainMenu);
	pMainMenu->Detach();
	return TRUE;
}

/************************************************************************/
/* 函数名:InitMyListCtrl
/* 参数：无		返回值：初始化成功返回TRUE
/* 描述：初始化列表控件的风格，并初始化列表控件的列内容									   */
/************************************************************************/
BOOL CmouseSimulationDlg::InitMyListCtrl(void)
{
	m_myListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
	m_myListCtrl.ModifyStyle(0,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL);

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH ;
	lvc.fmt  = LVCFMT_CENTER ;
	lvc.cx   = SERIALNUMBERWIDTH ;
	lvc.pszText = _T("序号");
	m_myListCtrl.InsertColumn(ID_SERIALNUMBER, &lvc);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_CENTER; //LVCFMT_LEFT
	lvc.cx = ACTIONWIDTH;
	lvc.pszText = _T("动作");
	m_myListCtrl.InsertColumn(ID_ACTION, &lvc);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_CENTER;
	lvc.cx = PARAM1WIDTH;
	lvc.pszText = _T("参数1");
	m_myListCtrl.InsertColumn(ID_PARAM1, &lvc);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_CENTER;
	lvc.cx = PARAM2WIDTH;
	lvc.pszText = _T("参数2");
	m_myListCtrl.InsertColumn(ID_PARAM2, &lvc);

	AddItemToListCtrl(0, CString("100"), CString("100"));

	return 0;
}

/************************************************************************/
/* 函数名:AddFileToPlayList
/* 参数：无
/* 描述：增加歌曲函数，同时在列表控件增加项目。                                 */
/************************************************************************/
void CmouseSimulationDlg::AddItemToListCtrl(int action, CString &param1, CString &param2)
{
	LVITEM  item;			 
	int		nIndex;				 //当前索引位置（行）
	CString strTmp;
	CString strAction;

	item.mask = LVIF_TEXT;	
	item.iSubItem = 0;

	item.iItem = (m_myListCtrl.GetItemCount() <= 0) ? 0 : m_myListCtrl.GetItemCount(); 
	strTmp.Format(TEXT("%d"), ++item.iItem);				//格式化当前索引
	item.pszText = strTmp.GetBuffer(strTmp.GetLength());	//在第一列插入序号

	nIndex = m_myListCtrl.InsertItem(&item);				//在第一列插入序号

	if(LEFT_CLICK == action){
		strAction = CString(TEXT("鼠标左键"));
	}else if(RIGHT_CLICK == action){
		strAction = CString(TEXT("鼠标右键"));
	}else if(KEY_INPUT == action){
		strAction = CString(TEXT("键盘输入"));
	}else if(DELAY_TIME == action){
		strAction = CString(TEXT("延时"));
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
	MessageBox(text, _T("点击确定运行"), MB_OK);
	Sleep(3000);
	click(70,1060);
	Sleep(2000);
	input((string)"12345");
}

void CmouseSimulationDlg::OnSetting()
{
	MessageBox(_T("OnSetting!"), _T("按键触发"), MB_OK);
}

void CmouseSimulationDlg::OnTimer(UINT_PTR nIDEvent)
{
	POINT point;
	CString text;

	if(timerId == nIDEvent)
	{
		GetCursorPos(&point);
		text.Format(_T("鼠标模拟器 (x=%d y=%d)"), point.x, point.y);
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

//自定义消息处理
LRESULT CmouseSimulationDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	MessageBox(_T("OnMyMessage!"), _T("自定义消息"), MB_OK);
	return 0;
}
 
//打开某个网址:website （使用默认浏览器） 
void CmouseSimulationDlg::open_web(char *website)
{
	ShellExecuteA(0,"open", website,0,0,1);
}
 
 
//模拟鼠标点击  （x,y）是要点击的位置 
void CmouseSimulationDlg::click(int x, int y)
{
	//将鼠标光标移动到 指定的位置     例子中屏幕分辨率1600x900  在鼠标坐标系统中，屏幕在水平和垂直方向上均匀分割成65535×65535个单元
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, x*65535/cxScreen, y*65535/cyScreen, 0, 0);
	
	Sleep(50);//稍微延时50ms 
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);//鼠标左键按下 
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//鼠标左键抬起
 }
 
//模拟键盘输入 keybd_event(要按下的字符,0,动作,0);动作为0是按下，动作为2是抬起 
void CmouseSimulationDlg::input(string &input)
{
	int i;
	
	for(i=0;i<sizeof(input.size());i++)
	{
		keybd_event(input[i],0,0,0);
		keybd_event(input[i],0,2,0);
		Sleep(30);	
	}
	
	//tab键 对应的编号是0x09  让密码输入框 获取焦点 
	//keybd_event(0x09,0,0,0);//按下 
	//keybd_event(0x09,0,2,0); //松开 
	//Sleep(30);	
}
 
 
//将chrome.exe进程杀掉，在例子中尚未使用 
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
	if(IDOK == MessageBox(_T("点击确定3s后开始运行!"), _T("运行"), MB_OKCANCEL))
	{
		Sleep(3000);
		click(70,1060);
		Sleep(2000);
		input((string)"12345");
	}
}
