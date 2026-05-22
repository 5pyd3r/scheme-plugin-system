# Scheme Plugin System

基于 Cisco Chez Scheme 的嵌入式插件系统。将 Chez Scheme 解释器/编译器/FFI 能力嵌入 C 二进制，使宿主进程具备动态加载执行 Lisp/Scheme 代码的能力，支持 C ↔ Scheme 双向调用。

## 架构

```
┌─────────────────────────────────┐
│  main.c (Demo 宿主)              │
├─────────────────────────────────┤
│  plugin.h / plugin.c (公开 API)  │
├─────────────────────────────────┤
│  chez_adaptor (Chez 适配层)      │
├─────────────────────────────────┤
│  Chez Scheme Kernel (submodule)  │
│  ├── libkernel.a (运行时)        │
│  ├── petite.boot (解释器)        │
│  └── scheme.boot (编译器)        │
└─────────────────────────────────┘
```

## 依赖

- Git
- GCC / Clang
- Meson (>= 1.0) + Ninja
- Make

## 构建

### 1. 克隆仓库（含 submodule）

```bash
git clone --recurse-submodules --depth=1 --filter=blob:none \
  <repo-url> scheme-plugin-system
cd scheme-plugin-system
```

如果已克隆但未拉取 submodule：

```bash
git submodule update --init --depth=1
```

### 2. 构建 Chez Scheme

```bash
cd chezscheme
./configure --disable-x11 --disable-curses
make -j$(nproc)
cd ..
```

### 3. 构建插件系统

```bash
meson setup builddir --backend=ninja
meson compile -C builddir
```

构建产物：`builddir/scheme-plugin-host`

### 4. 运行 Demo

```bash
./builddir/scheme-plugin-host
```

预期输出：

```
=== Scheme Plugin System Demo ===

[C] Initializing Chez Scheme kernel...
[C] Registering host-multiply...
[C] Loading hello_plugin.ss...
[Scheme] run-plugin: calling host-multiply...
[C] host-multiply called, returning 42
[Scheme] run-plugin: result = 42
[C] Calling Scheme get-result...
[C] Scheme get-result returned: 42

[C] Demo complete.
```

## API 参考

### plugin.h — 公开接口

```c
#include "plugin.h"

// 1. 初始化插件系统
int plugin_system_init(const char *boot_path);
// boot_path: Chez Scheme boot 文件目录（含 petite.boot 和 scheme.boot）

// 2. 注册 C 函数供 Scheme 调用
typedef void* (*plugin_c_func)(void *arg);
int plugin_register_c_func(const char *name, plugin_c_func fn);
// name: Scheme 端可见的函数名
// fn: C 函数指针

// 3. 加载 Scheme 插件文件
int plugin_load(const char *path);
// path: .ss 或 .scm 文件路径

// 4. 调用 Scheme 函数
void* plugin_call(const char *name);
// name: Scheme 函数名
// 返回: Scheme 对象指针

// 5. 清理
void plugin_system_destroy(void);
```

### 典型使用流程

```c
plugin_system_init("/path/to/boot/dir");
plugin_register_c_func("my_c_func", my_c_function);
plugin_load("my_plugin.ss");
void *result = plugin_call("get-result");
// 处理 result...
plugin_system_destroy();
```

## 编写插件

### Scheme 端调用 C 函数

```scheme
;; 声明 C 函数 (无参数, 返回 int)
(define my-c-func
  (foreign-procedure "my_c_func" () integer-32))

;; 调用
(display (my-c-func))
```

### 暴露函数供 C 调用

```scheme
(define (get-result)
  42)  ; C 端通过 plugin_call("get-result") 调用
```

## 项目结构

```
scheme-plugin-system/
├── chezscheme/           # Chez Scheme submodule
├── src/
│   ├── plugin.h          # 公开 API 头文件
│   ├── plugin.c          # API 实现
│   ├── chez_adaptor.h    # Chez 适配层头文件
│   ├── chez_adaptor.c    # Chez 嵌入实现
│   └── main.c            # Demo 宿主
├── plugins/
│   └── hello_plugin.ss   # 示例插件
├── meson.build           # Meson 构建定义
└── README.md
```

## 技术要点

### Boot 文件加载

使用 `Sregister_boot_file()` 注册 boot 文件后调用 `Sbuild_heap(NULL, NULL)`：

```c
Sscheme_init(NULL);
Sregister_boot_file("path/to/petite.boot");
Sregister_boot_file("path/to/scheme.boot");
Sbuild_heap(NULL, NULL);
```

- `petite.boot`: 解释器运行时（必须）
- `scheme.boot`: 编译器（foreign-procedure 等需要）

### C → Scheme 调用

```c
ptr sym = Sstring_to_symbol("function-name");
ptr proc = Stop_level_value(sym);
ptr result = Scall0(proc);
```

### Scheme → C 调用

C 端: `Sforeign_symbol("c_func_name", function_ptr);`

Scheme 端: `(define c-func (foreign-procedure "c_func_name" () integer-32))`

## License

基于 Cisco Chez Scheme (Apache 2.0) 构建。
