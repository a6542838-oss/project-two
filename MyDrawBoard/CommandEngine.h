// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// AI 辅助声明: 核心逻辑由 AI 生成，已人工审核。
// =================================================================================
#pragma once
#include <vector>
#include <memory>
#include <stack>
#include "ShapeEngine.h"

/// @brief 绘图命令接口 (Command Pattern)
/// @details 对应需求 F-04 撤销/重做
class IDrawCommand {
public:
    virtual ~IDrawCommand() {}
    /// @brief 执行命令
    virtual void Execute() = 0;
    /// @brief 撤销命令
    virtual void Undo() = 0;
};

/// @brief 添加图形的具体命令
/// @details 封装了"在画布上添加一个图形"的操作，支持逆操作。
class CAddShapeCommand : public IDrawCommand {
private:
    std::shared_ptr<IShape> m_shape; ///< 要添加的图形
    std::vector<std::shared_ptr<IShape>>& m_canvasShapes; ///< 对画布数据的引用

public:
    /// @brief 构造函数
    /// @param shape 新创建的图形
    /// @param canvas 文档中的图形列表引用
    CAddShapeCommand(std::shared_ptr<IShape> shape, std::vector<std::shared_ptr<IShape>>& canvas)
        : m_shape(shape), m_canvasShapes(canvas) {
    }

    /// @brief 执行：将图形加入列表
    void Execute() override {
        m_canvasShapes.push_back(m_shape);
    }

    /// @brief 撤销：将图形从列表末尾移除
    void Undo() override {
        if (!m_canvasShapes.empty()) {
            m_canvasShapes.pop_back();
        }
    }
};

/// @brief 命令管理器 (Invoker)
/// @details 维护撤销栈和重做栈，负责命令的调度。
class CommandManager {
private:
    std::stack<std::shared_ptr<IDrawCommand>> m_undoStack; ///< 撤销栈
    std::stack<std::shared_ptr<IDrawCommand>> m_redoStack; ///< 重做栈

public:
    /// @brief 执行新命令
    /// @param cmd 命令对象智能指针
    void ExecuteCommand(std::shared_ptr<IDrawCommand> cmd) {
        cmd->Execute();
        m_undoStack.push(cmd);
        // 执行新操作时，重做栈失效，必须清空
        while (!m_redoStack.empty()) {
            m_redoStack.pop();
        }
    }

    /// @brief 撤销当前步骤
    void Undo() {
        if (!m_undoStack.empty()) {
            auto cmd = m_undoStack.top();
            m_undoStack.pop();
            cmd->Undo();
            m_redoStack.push(cmd);
        }
    }

    /// @brief 重做上一步骤
    void Redo() {
        if (!m_redoStack.empty()) {
            auto cmd = m_redoStack.top();
            m_redoStack.pop();
            cmd->Execute();
            m_undoStack.push(cmd);
        }
    }
};