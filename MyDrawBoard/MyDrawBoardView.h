// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: MyDrawBoardView.h
// AI 辅助声明: 核心逻辑由 AI 生成，已人工审核。
// =================================================================================
#pragma once
#include "ShapeEngine.h"
#include "CommandEngine.h"

// 语言枚举
enum class AppLang {
    Chinese = 0,
    English = 1,
    Japanese = 2
};

class CMyDrawBoardView : public CView
{
protected:
    CMyDrawBoardView() noexcept;
    DECLARE_DYNCREATE(CMyDrawBoardView)

    // 特性
public:
    CMyDrawBoardDoc* GetDocument() const;

    // 操作
public:
    ShapeType m_currentTool = ShapeType::Line;
    COLORREF m_currentColor = RGB(0, 0, 0);
    int m_currentLineWidth = 1;
    AppLang m_currentLang = AppLang::Chinese;

    std::shared_ptr<IShape> m_pTempShape;
    CommandManager m_commandManager;
    bool m_isDrawing = false;
    CPoint m_startPoint;

    // 重写
public:
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

    // 实现
public:
    virtual ~CMyDrawBoardView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    // 工具栏
    afx_msg void OnToolLine();
    afx_msg void OnToolRect();
    afx_msg void OnToolEllipse();
    afx_msg void OnToolFreehand(); // === 新增 ===
    afx_msg void OnToolColor();

    // 线宽
    afx_msg void OnWidth1();
    afx_msg void OnWidth3();
    afx_msg void OnWidth5();
    afx_msg void OnWidth8();
    afx_msg void OnUpdateWidth1(CCmdUI* pCmdUI);
    afx_msg void OnUpdateWidth3(CCmdUI* pCmdUI);
    afx_msg void OnUpdateWidth5(CCmdUI* pCmdUI);
    afx_msg void OnUpdateWidth8(CCmdUI* pCmdUI);

    // 多语言
    afx_msg void OnLangCN();
    afx_msg void OnLangEN();
    afx_msg void OnLangJP();
    afx_msg void OnUpdateLangCN(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLangEN(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLangJP(CCmdUI* pCmdUI);

    void UpdateMenuText();
};

#ifndef _DEBUG
inline CMyDrawBoardDoc* CMyDrawBoardView::GetDocument() const
{
    return reinterpret_cast<CMyDrawBoardDoc*>(m_pDocument);
}
#endif