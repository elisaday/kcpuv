# KCPUV

使用[KCP](https://github.com/skywind3000/kcp)和[libuv](https://www.github.com/libuv/libuv)的简单网络模块。

# 用法

* 创建kcpuv对象

```cpp
kcpuv_t kcpuv = kcpuv_create();
```

* 如果用作服务器，那么监听UDP端口

```cpp
kcpuv_listen(kcpuv, "0.0.0.0", 9527);
```

* 如果用作客户端，那么创建连接

```cpp
kcpuv_conv_t conv = 0x12345678; // 定义回话ID
kcpuv_connect(kcpuv, conv, "127.0.0.1", 9527);
```

* 处理网络消息

```cpp
void process_network() {
	// 让kcpuv处理一下内部事务
	kcpuv_run(kcpuv);

	while (true) {
		kcpuv_msg_t msg;
		while (true) {
			// 接收消息
			int r = kcpuv_recv(kcpuv, &msg);
			if (r < 0) break;

			// 处理消息
			// ...

			// 销毁消息
			kcpuv_msg_free(&msg);
		}
	}
}
```

* 发送消息

```cpp
const char* data = "hello";
uint32_t size = 5;
kcpuv_send(kcpuv, conv, data, size);
```


# 版本状态

目前版本处于DEMO阶段，一些显而易见的问题还没有处理：
* 回话ID（conv）是客户端分配，服务器不做任何处理，全盘接受。
* 内存分配和释放很粗放。
* ...