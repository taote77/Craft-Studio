
## 架构设计


## 使用

有两种插件服务，一般服务(GUI 无关的)，只要集成 MicroService 并实现 serviceName 接口即可

带有 GUI 的插件

```c++
Q_PROPERTY(QString withGui READ withGui CONSTANT)
```
必须声明 withGui 属性，以便能使其运行在 QApplication gui 主线程

