// =================================================================================
// 项目名称: MyDrawBoard (Project 2 - Simple Drawing Board)
// 文件名称: [当前文件名，例如 ShapeEngine.h]
// 作者: [刘佳昊] (学号: [20250313013Z])
// 创建日期: 2025-11-18
//
// AI 辅助声明:
// 本文件核心逻辑（工厂模式/命令模式/序列化）由 AI (Gemini) 辅助生成。
// 涉及功能: F-01 到 F-09 所有核心绘图与架构实现。
// 人工审核: 已通过代码走查，确认无资源泄漏(RAII)与逻辑错误。
// =================================================================================
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MyDrawBoard.h"
#endif

#include "MyDrawBoardDoc.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMyDrawBoardDoc

IMPLEMENT_DYNCREATE(CMyDrawBoardDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyDrawBoardDoc, CDocument)
END_MESSAGE_MAP()

// CMyDrawBoardDoc 构造/析构

CMyDrawBoardDoc::CMyDrawBoardDoc() noexcept
{
}

CMyDrawBoardDoc::~CMyDrawBoardDoc()
{
}

BOOL CMyDrawBoardDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // 新建文件时，清空图形
    m_shapes.clear();

    return TRUE;
}

// === 核心：序列化 (保存/读取) + 异常处理 (F-07) ===
void CMyDrawBoardDoc::Serialize(CArchive& ar)
{
    // 使用 try-catch 块包裹所有 I/O 操作
    try
    {
        if (ar.IsStoring())
        {
            // === 保存 ===
            int count = (int)m_shapes.size();
            ar << count;

            for (const auto& shape : m_shapes) {
                ar << (int)shape->GetType();
                shape->Save(ar);
            }
        }
        else
        {
            // === 读取 ===
            // 1. 先读取到一个临时列表里，确保全部读成功了再替换 m_shapes
            // 这样符合“异常安全”原则：要么全成功，要么保持原样，不要只读一半。
            std::vector<std::shared_ptr<IShape>> tempShapes;

            int count = 0;
            ar >> count;

            // 简单的防崩溃检查：如果数量大得离谱，可能是读到了坏文件
            if (count < 0 || count > 100000) {
                AfxThrowArchiveException(CArchiveException::badSchema);
            }

            for (int i = 0; i < count; i++) {
                int typeId = 0;
                ar >> typeId;

                auto shape = ShapeFactory::CreateShape((ShapeType)typeId);
                if (shape) {
                    shape->Load(ar);
                    tempShapes.push_back(shape);
                }
                else {
                    // 如果读到了未知的图形类型
                    AfxThrowArchiveException(CArchiveException::badClass);
                }
            }

            // 全部读取成功，才安全地赋值给成员变量
            m_shapes = tempShapes;
            UpdateAllViews(NULL);
        }
    }
    catch (CException* e) // 捕获 MFC 的所有异常
    {
        e->ReportError(); // 弹出标准错误对话框
        e->Delete();      // 必须手动删除异常对象

        // 如果是读取失败，为了安全，清空当前画布
        if (!ar.IsStoring()) {
            m_shapes.clear();
            UpdateAllViews(NULL);
        }
    }
}
#ifdef _DEBUG
void CMyDrawBoardDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CMyDrawBoardDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG