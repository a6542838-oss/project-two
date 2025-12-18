// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// AI 辅助声明: 核心逻辑由 AI 生成，已人工审核。
// =================================================================================
#pragma once
#include <afxwin.h>

/// @brief GDI 对象资源管理包装器 (RAII)
/// @details 确保 GDI 对象（如 CPen, CBrush）在作用域结束时自动释放，防止资源泄漏。
///          对应需求: F-07, NF-04
/// @tparam T GDI 对象类型 (如 CPen, CBrush, CFont)
template <typename T>
class CGdiObjectWrapper {
public:
    /// @brief 构造函数，接管对象所有权
    /// @param pObject 指向 GDI 对象的指针
    CGdiObjectWrapper(T* pObject) : m_pObject(pObject) {}

    /// @brief 析构函数，自动释放资源
    ~CGdiObjectWrapper() {
        if (m_pObject) {
            m_pObject->DeleteObject();
            delete m_pObject;
        }
    }

    /// @brief 获取原始指针
    /// @return GDI 对象指针
    T* Get() const { return m_pObject; }

    /// @brief 重载箭头操作符，使其像原生指针一样使用
    T* operator->() const { return m_pObject; }

private:
    T* m_pObject; ///< 托管的 GDI 对象指针
};