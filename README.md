# 注意：这是add-readme-docs分支进行的小改动～
# （指添加了两行字在readme里面）

# rm-first-work — 理想针孔相机重投影 (C++17 + CMake)

输入**三维点**、**相机内参**和**已知外参**，输出**二维像素坐标**，并计算与对应观测点之间的**像素欧氏距离**。
采用理想针孔模型，**忽略畸变**。

## 数学模型

外参约定为**世界到相机**的刚体变换：

```
Pc = R * Pw + t
```

| 符号 | 含义 |
| --- | --- |
| `Pw` | 世界坐标系下的三维点 `(x, y, z)` |
| `Pc` | 相机坐标系下的三维点 `(Xc, Yc, Zc)` |
| `R` | 3×3 旋转矩阵，世界 → 相机 |
| `t` | 平移向量，表示在相机坐标系下 |

投影公式（要求 `Zc > 0`）：

```
      [ fx  0  cx ]                u = fx * Xc / Zc + cx
  K = [  0 fy  cy ]                v = fy * Yc / Zc + cy
      [  0  0   1 ]
```

像素欧氏距离：

```
d = sqrt((u - u_obs)^2 + (v - v_obs)^2)
```

**单位约定**：`Pw` 与 `t` 使用**相同长度单位**（示例用米）；`fx`、`fy`、`cx`、`cy` 单位为**像素**，
因此投影结果 `(u, v)` 恒为像素，`d` 的单位也是像素。

## 文件结构

```
rm-first-work/
├── CMakeLists.txt
├── include/reprojection.h     # 类型与函数声明
├── src/reprojection.cpp       # 核心计算：外参变换 + 针孔投影 + 像素距离 + 异常判断
├── src/main.cpp               # cin 读入，打印结果
├── input.txt                  # 示例输入
└── README.md / GIT_GUIDE.md
```

## 接口

```cpp
namespace reprojection {

struct Point2D { double u, v; };                  // 像素点
struct Point3D { double x, y, z; };               // 三维点
struct CameraIntrinsics { double fx, fy, cx, cy; };

struct CameraExtrinsics {                         // 世界 -> 相机：Pc = R * Pw + t
  double R[3][3];
  double t[3];
};

struct ReprojectionResult {
  Point2D pixel;              // 投影像素 (u, v)
  double pixel_distance;      // 与观测像素的欧氏距离（像素）
};

// 成功返回结果；非正深度等异常返回 std::nullopt
std::optional<ReprojectionResult> reproject(const Point3D& pw,
                                            const CameraIntrinsics& K,
                                            const CameraExtrinsics& ext,
                                            const Point2D& observed);
}
```

## 构建与运行

### 方式一：CMake（推荐）

```bash
cd rm-first-work
mkdir -p build
cmake -S . -B build              # 配置：-S 源码目录，-B 构建目录
cmake --build build -j"$(nproc)" # 构建
./build/reprojection < input.txt # 运行
```

配置只需要跑一次，之后改了代码直接 `cmake --build build` 即可。

### 方式二：纯手动 g++，一条命令（不用 CMake）

```bash
cd rm-first-work
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude \
    src/main.cpp src/reprojection.cpp -o build/reprojection_manual
./build/reprojection_manual < input.txt
```

| 参数 | 含义 |
| --- | --- |
| `-std=c++17` | 使用 C++17（`std::optional` 需要） |
| `-Iinclude` | 头文件搜索路径，让 `#include "reprojection.h"` 能被找到 |
| `-o` | 指定输出文件名 |
| `-Wall -Wextra -Wpedantic` | 打开警告（本项目在这三个选项下**零警告**） |

### 方式三：手动分步编译 + 链接

更接近编译器和链接器实际做的事：

```bash
cd rm-first-work
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -c src/reprojection.cpp -o build/reprojection.o
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -c src/main.cpp         -o build/main.o
g++ build/reprojection.o build/main.o -o build/reprojection_manual
./build/reprojection_manual < input.txt
```

`-c` 表示只编译成目标文件 `.o`，不做链接；最后一步才把两个 `.o` 链接成可执行文件。

> 手写 g++ 的麻烦在于要自己维护文件依赖：改一个头文件就得把所有相关文件重编。
> 这正是 CMake 的价值，所以日常还是用方式一。

### 怎么给程序喂输入

程序从 `stdin` 读数据，所以运行时要提供输入。三种办法：

```bash
# A. 从文件重定向（推荐）
./build/reprojection < input.txt

# B. heredoc 直接写数据
./build/reprojection <<'EOF'
0.5 -0.5 2.0
800 800 640 360
1 0 0
0 1 0
0 0 1
0 0 0
840.3 159.6
EOF

# C. 管道
echo "0.5 -0.5 2.0
800 800 640 360
1 0 0
0 1 0
0 0 1
0 0 0
840.3 159.6" | ./build/reprojection
```

也可以不要重定向，直接运行后**手动逐行敲**，输完按 `Ctrl+D` 结束输入：

```bash
./build/reprojection
```

> 直接敲 `./build/reprojection` 而不管它，程序会一直停在 `cin` 等待输入 —— 这不是卡死。

### 清理

```bash
rm -rf build          # 构建产物全在 build/ 里，删掉即可
```

## 输入格式

按提示依次输入，全部用空白分隔（空格或换行）：

```
1) 世界点 Pw : x y z
2) 相机内参  : fx fy cx cy
3) 外参 R 第1行: r00 r01 r02
4) 外参 R 第2行: r10 r11 r12
5) 外参 R 第3行: r20 r21 r22
6) 外参 t    : tx ty tz
7) 观测像素  : u_obs v_obs
```

`input.txt` 示例（`Pw = (0.5, -0.5, 2)`，`fx=fy=800, cx=640, cy=360`，`R = I`，`t = 0`，观测 `(840.3, 159.6)`）：

```
0.5 -0.5 2.0
800 800 640 360
1 0 0
0 1 0
0 0 1
0 0 0
840.3 159.6
```

## 运行结果

```
计算结果：
  投影像素 = (840.0000, 160.0000)
  像素欧氏距离 = 0.5000
```

手算校核：`Pc = Pw = (0.5, -0.5, 2)`，
`u = 800 × 0.5 / 2 + 640 = 840`，`v = 800 × (-0.5) / 2 + 360 = 160`，
`d = sqrt(0.3² + 0.4²) = 0.5` px ✅

## 异常处理

`reproject()` 返回 `std::optional`，失败时返回 `std::nullopt`，不抛异常：

| 情况 | 结果 |
| --- | --- |
| `Zc <= 0`（点在相机后方或光心平面上） | `nullopt` → 程序打印 `[异常]` 并返回 2 |
| 输入含 `NaN` / `Inf` | `nullopt` |
| `fx` 或 `fy` ≤ 0 | `nullopt` |
| 深度极小时除法溢出 | `nullopt` |

退出码：

| 退出码 | 含义 |
| --- | --- |
| `0` | 计算成功 |
| `1` | 输入读取失败（数据不完整） |
| `2` | 投影失败（如深度非正） |

验证示例（点在相机后方）：

```
$ ./build/reprojection
1.0 1.0 -4.0
800 800 640 360
1 0 0
0 1 0
0 0 1
0 0 0
840 560
...
[异常] 投影失败：该点在相机坐标系下深度非正（Zc <= 0），可能位于相机后方或光心平面上。
$ echo $?
2
```

## 环境准备（Ubuntu，不使用 Snap）

本机实测：Ubuntu 24.04.5 LTS、git 2.43.0、g++ 13.3.0、cmake 3.28.3，
全部来自 apt（`/usr/bin/`），**不是 Snap**。

缺失时用 apt 安装：

```bash
sudo apt update
sudo apt install -y git build-essential cmake
```

验证：

```bash
which -a git g++ cmake                          # 应输出 /usr/bin/...，不应出现 /snap/bin/...
git --version && g++ --version && cmake --version
snap list 2>/dev/null | grep -E 'git|cmake|gcc' # 应当没有输出
```

配置 Git 身份（不配的话 `git commit` 会报错）：

```bash
git config --global user.name  "你的名字"
git config --global user.email "你的邮箱@example.com"
```

## 上传到自己的仓库

见 [GIT_GUIDE.md](GIT_GUIDE.md)：创建仓库、分支开发、提交、推送、一次 PR 合并，
以及重新 `clone` 后验证构建运行的完整步骤。
