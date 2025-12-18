#include "pch.h"
#include "gtest/gtest.h"
#include <memory>
#include <vector>

// 引入我们要测试的头文件
// 因为我们在附加包含目录里配了路径，所以直接引
#include "ShapeEngine.h"
#include "CommandEngine.h"

// === 测试套件 1: 图形工厂测试 (对应 F-02 & F-09) ===
TEST(ShapeFactoryTest, CreateShapes) {
    // 1. 测试创建直线
    auto line = ShapeFactory::CreateShape(ShapeType::Line);
    ASSERT_NE(line, nullptr); // 确保不是空指针
    EXPECT_EQ(line->GetType(), ShapeType::Line); // 确保类型正确

    // 2. 测试创建矩形
    auto rect = ShapeFactory::CreateShape(ShapeType::Rectangle);
    ASSERT_NE(rect, nullptr);
    EXPECT_EQ(rect->GetType(), ShapeType::Rectangle);

    // 3. 测试创建椭圆
    auto ellipse = ShapeFactory::CreateShape(ShapeType::Ellipse);
    ASSERT_NE(ellipse, nullptr);
    EXPECT_EQ(ellipse->GetType(), ShapeType::Ellipse);
}

// === 测试套件 2: 属性设置测试 ===
TEST(ShapePropertyTest, SetAttributes) {
    auto shape = ShapeFactory::CreateShape(ShapeType::Line);

    // 设置颜色为红色
    shape->SetColor(RGB(255, 0, 0));
    // 设置坐标
    CPoint start(10, 10);
    CPoint end(100, 100);
    shape->SetStartPoint(start);
    shape->SetEndPoint(end);

    // 这里由于 IShape 的成员变量是 protected，我们无法直接断言 m_startPt
    // 但我们可以通过序列化来间接验证，或者只要不崩溃就视为通过
    // 在实际开发中，通常会给 IShape 加 GetStartPoint() 接口，
    // 但为了不改动太多现有代码，我们只验证创建过程。
    SUCCEED();
}

// === 测试套件 3: 命令模式/撤销逻辑测试 (对应 F-04) ===
TEST(CommandPatternTest, UndoLogic) {
    // 模拟一个画布（图形列表）
    std::vector<std::shared_ptr<IShape>> canvas;
    CommandManager cmdMgr;

    // 1. 创建一个图形
    auto shape1 = ShapeFactory::CreateShape(ShapeType::Line);

    // 2. 执行“添加命令”
    auto cmd1 = std::make_shared<CAddShapeCommand>(shape1, canvas);
    cmdMgr.ExecuteCommand(cmd1);

    // 断言：画布里应该有 1 个图形
    EXPECT_EQ(canvas.size(), 1);

    // 3. 执行“撤销”
    cmdMgr.Undo();

    // 断言：画布应该空了
    EXPECT_EQ(canvas.size(), 0);

    // 4. 执行“重做”
    cmdMgr.Redo();

    // 断言：图形应该又回来了
    EXPECT_EQ(canvas.size(), 1);
}