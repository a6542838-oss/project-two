# 软件架构设计文档 (Architecture Design)

## 1. 总体架构 (Overview)
本项目采用经典的 **MFC Document/View (文档/视图)** 架构，严格遵循 **MVC (Model-View-Controller)** 设计原则。

* **Model (CMyDrawBoardDoc)**: 负责数据存储。维护一个 `std::vector<std::shared_ptr<IShape>>` 列表，管理所有图形对象的生命周期和序列化。
* **View (CMyDrawBoardView)**: 负责渲染与交互。通过 `OnDraw` 进行双缓冲绘制，捕获鼠标消息并转换为绘图命令。
* **Controller (CommandManager)**: 负责业务逻辑调度。管理命令栈，处理撤销/重做请求。

## 2. 设计模式 (Design Patterns)

为了满足高内聚、低耦合的要求，项目中使用了以下设计模式：

### 2.1 工厂模式 (Factory Pattern)
* **位置**: `ShapeEngine.h` -> `ShapeFactory`
* **作用**: 将具体的图形创建逻辑（`CLineShape`, `CRectShape`）与客户端解耦。当新增图形类型时，只需修改工厂类，无需修改视图层代码。

### 2.2 命令模式 (Command Pattern)
* **位置**: `CommandEngine.h` -> `CAddShapeCommand`, `CommandManager`
* **作用**: 将“绘图操作”封装为对象。使得请求的发起者（UI）与请求的执行者（数据层）解耦，并轻松实现撤销(Undo)和重做(Redo)功能。

### 2.3 策略模式 (Strategy Pattern)
* **位置**: `IShape` -> `Draw()`
* **作用**: 不同的图形对象（直线、矩形）实现了相同的 `Draw` 接口，但具体的绘制算法不同。视图层只需调用基类的 `Draw`，无需关心具体实现。

## 3. 关键技术点

### 3.1 RAII 资源管理
使用 `CGdiObjectWrapper` 模板类包装 GDI 对象（如 CPen, CBrush），利用 C++ 析构机制确保 GDI 句柄在任何异常情况下都能被正确释放 (NF-04)。

### 3.2 异常安全 (Exception Safety)
在文件序列化 (`Serialize`) 过程中使用 `try-catch` 块捕获 `CArchiveException`，防止坏文件导致程序崩溃 (F-07)。

### 3.3 双缓冲绘图 (Double Buffering)
使用内存 DC (`CreateCompatibleDC`) 先在内存中完成所有绘制，最后一次性 `BitBlt` 到屏幕，彻底解决了闪烁问题 (F-01)。