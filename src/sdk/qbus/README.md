# 一个基于Qt的微服务框架


## feature

* 支持热加载
* 插件即服务


## Class UML


```c++
Application

+ init()

+ exec()

- ServiceHub _hub
```

```c++
ServiceHub

+ init()
+ start()

- std::vector<Thread>

- std::vector<Service>


```


```c++
IService

+ init()
+ start()

- std::vector<Thread>

- std::vector<Service>


```


