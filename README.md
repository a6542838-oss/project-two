# MyDrawBoard - 简易绘图板

这是一个基于 MFC (Microsoft Foundation Classes) 和 GDI+ 技术开发的简易矢量绘图程序。支持绘制基本图形、撤销重做、文件保存以及多语言界面。

## ✨ 主要功能 (Features)

* **基本绘图**: 支持直线、矩形、椭圆三种基本图元。
* **交互操作**: 支持鼠标拖拽预览绘制。
* **属性设置**: 支持自定义颜色、线宽 (1px, 3px, 5px, 8px)。
* **撤销/重做**: 基于命令模式 (Command Pattern) 的无限次撤销重做 (Ctrl+Z / Ctrl+Y)。
* **文件存取**: 自定义二进制文件格式保存与读取，支持异常处理。
* **多语言**: 支持 中文 / English / 日本語 实时切换。
* **UI体验**: 状态栏实时显示鼠标坐标，双缓冲技术防止闪烁。

## 🛠️ 技术栈 (Tech Stack)

* **语言**: C++ 17
* **框架**: MFC (Document/View Architecture)
* **测试**: Google Test (覆盖率 > 70%)
* **环境**: Visual Studio 2022 (v143)

## 🚀 如何构建 (Build)

1.  使用 Visual Studio 2022 打开 `MyDrawBoard.sln`。
2.  确保已安装 "C++ MFC for latest v143 build tools"。
3.  选择配置 `Debug x64` 或 `Release x64`。
4.  点击 `Build Solution` (F7)。
5.  点击 `Run` (F5)。

## ⚠️ AI 辅助声明
本项目部分核心逻辑（工厂模式、序列化）由 AI 辅助生成，所有生成代码均已通过人工审查、单元测试验证，并添加了 RAII 资源管理保护。