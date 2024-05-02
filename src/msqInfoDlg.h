#if !defined(AFX_MSQINFODLG_H__3D45D996_F17B_4B01_A333_12A6F4C5D8C9__INCLUDED_)
#define AFX_MSQINFODLG_H__3D45D996_F17B_4B01_A333_12A6F4C5D8C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// msqInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// msqInfoDlg dialog

class msqInfoDlg : public CDialog
{
// Construction
public:
   msqInfoDlg(CWnd* pParent = NULL);   // standard constructor

   //{{AFX_DATA(msqInfoDlg)
      enum { IDD = IDD_MSQINFO };
      CEdit   m_info;
   //}}AFX_DATA


   //{{AFX_VIRTUAL(msqInfoDlg)
   protected:
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(msqInfoDlg)
      virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSQINFODLG_H__3D45D996_F17B_4B01_A333_12A6F4C5D8C9__INCLUDED_)
