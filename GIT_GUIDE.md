# 把项目上传到自己的仓库 + 分支开发 + 一次 PR 合并

本文按顺序执行即可。**命令都不要直接在 root 下跑，也不需要 sudo。**

下面用 `YOUR_NAME` / `YOUR_EMAIL` / `YOUR_USER` / `YOUR_REPO` 作占位符，请替换成你自己的：

- `YOUR_USER`：GitHub 用户名
- `YOUR_REPO`：仓库名，本文假设为 `rm-first-work`

---

## 第 0 步：确认身份配置（只需一次）

```bash
git config --global user.name  "YOUR_NAME"
git config --global user.email "YOUR_EMAIL"
git config --global init.defaultBranch main
git config --global --list
```

`user.name` / `user.email` 没设置的话，`git commit` 会直接报错。这里的邮箱建议用你 GitHub
账号绑定的邮箱（或 GitHub 提供的 `<ID>+<用户名>@users.noreply.github.com`）。

## 第 1 步：选择连接方式（HTTPS 或 SSH，二选一）

### 方案 A：HTTPS + Personal Access Token（简单，适合第一次）

1. GitHub → 右上角头像 → **Settings** → **Developer settings** →
   **Personal access tokens** → **Tokens (classic)** → **Generate new token (classic)**
2. 勾选 `repo` 权限，生成后**立即复制** token（只显示一次）
3. 推送时用户名填 GitHub 用户名，**密码处粘贴 token**（不是账号密码）

想让 git 记住，避免每次输入：

```bash
git config --global credential.helper store   # 明文保存在 ~/.git-credentials
```

### 方案 B：SSH 密钥（推荐，长期使用）

你的机器上目前没有 SSH 密钥（`~/.ssh` 下只有空的 `authorized_keys`），先生成一个：

```bash
ssh-keygen -t ed25519 -C "YOUR_EMAIL"     # 一路回车即可，可留空口令
cat ~/.ssh/id_ed25519.pub                 # 复制这一整行输出
```

把复制的内容粘贴到 GitHub → **Settings** → **SSH and GPG keys** → **New SSH key**。
然后验证：

```bash
ssh -T git@github.com
# 首次会问 fingerprint，输入 yes；看到 "Hi YOUR_USER! You've successfully authenticated"
# 就成功了（提示 shell access 不可用是正常的）
```

## 第 2 步：在 GitHub 上创建空仓库

GitHub → 右上角 **+** → **New repository**：

- **Repository name**：`rm-first-work`
- **Public / Private** 按需选择
- **不要**勾选 "Add a README file"、**不要**选 .gitignore 或 license

> 关键：必须创建**空**仓库。否则远端会有一个初始提交，首次 `push` 会因为历史不同被拒绝。

## 第 3 步：初始化本地仓库并提交到 `main`

```bash
cd ~/000/Robomaster/rm-first-work

git init -b main
git status                      # 确认 build/ 没有被列入（.gitignore 已忽略它）
git add .
git status                      # 再确认一次：只应看到源码与文档
git commit -m "feat: 理想针孔重投影 (C++17 + CMake)"
```

确认提交内容正确（应该只有 8 个文件）：

```bash
git ls-files
# .gitignore
# CMakeLists.txt
# GIT_GUIDE.md
# README.md
# include/reprojection.h
# input.txt
# src/main.cpp
# src/reprojection.cpp
```

关联远端并推送。**二选一**：

```bash
# 方案 A（HTTPS）
git remote add origin https://github.com/YOUR_USER/YOUR_REPO.git

# 方案 B（SSH）
git remote add origin git@github.com:YOUR_USER/YOUR_REPO.git
```

```bash
git remote -v                   # 检查地址是否写对
git push -u origin main
```

刷新 GitHub 页面，应该能看到代码和 README 渲染结果。

## 第 4 步：分支开发（本次任务要求的「分支开发」）

新建一个特性分支，在它上面做一次真实改动：

```bash
git switch -c feature/add-image-bounds-check
```

做一个有意义的改动 —— 例如在 `include/reprojection.h` 里加一个判断像素是否落在图像范围内的
函数声明：

```cpp
/// 判断像素是否落在图像范围内（宽高单位像素）。
bool isInsideImage(const Point2D& pixel, double width, double height);
```

在 `src/reprojection.cpp` 里实现，再在 `src/main.cpp` 里调用（例如把超出图像范围的点标记出来），最后：

```bash
cmake --build build -j"$(nproc)"     # 先确认能编译
./build/reprojection < input.txt     # 再确认输出正常
```

编译和运行都正常后再提交：

```bash
git status
git diff                             # 提交前看一眼改了什么
git add include/reprojection.h src/reprojection.cpp src/main.cpp
git commit -m "feat: 增加像素是否落在图像范围内的判断"
git push -u origin feature/add-image-bounds-check
```

> 好的习惯：**一次提交只做一件事**，提交信息用「类型: 说明」的格式
> （`feat` 新功能 / `fix` 修 bug / `docs` 文档 / `refactor` 重构 / `chore` 杂项）。
> 提交前先保证能编译、能跑通，这样 CI 或评审时才不会返工。

## 第 5 步：发起并合并 PR（一次 PR 合并）

1. 推送后 GitHub 页面会出现 **Compare & pull request** 按钮，点击它
   （也可以走 **Pull requests** → **New pull request**，base 选 `main`，compare 选你的特性分支）
2. 填写标题和说明，例如：
   - 标题：`feat: 增加像素是否落在图像范围内的判断`
   - 说明：做了什么、为什么、如何验证（贴上 `./build/reprojection_demo` 的输出结论）
3. 点击 **Create pull request**
4. 自己评审自己的代码：切到 **Files changed** 逐行看一遍 diff，确认没有误提交 `build/`
5. 点击 **Merge pull request** → **Confirm merge**
6. 合并后 GitHub 会提示删除分支，点 **Delete branch**（远端分支清理掉）

回到本地同步并清理：

```bash
git switch main
git pull                                  # 把合并结果拉回本地
git branch -d feature/add-image-bounds-check   # 删除本地已合并分支
git log --oneline --graph -5              # 查看提交历史
```

## 第 6 步：验证「重新 clone 后能够构建运行」

这一步是任务的验收标准，务必真的做一遍。**换一个干净目录**，模拟别人拿到你的仓库：

```bash
cd /tmp
rm -rf verify-clone
git clone https://github.com/YOUR_USER/YOUR_REPO.git verify-clone
# SSH 用户：git clone git@github.com:YOUR_USER/YOUR_REPO.git verify-clone

cd verify-clone
ls -a                                     # 应该没有 build/ 目录
cmake -S . -B build
cmake --build build -j"$(nproc)"
./build/reprojection < input.txt          # 应打印 4 个点的结果
echo "exit code: $?"                      # 应输出 0
```

预期输出：

```
计算结果：
  投影像素 = (840.0000, 160.0000)
  像素欧氏距离 = 0.5000
```

只要 `clone` → `cmake` → `build` → 运行 全部成功，就说明仓库是自包含的：
没有依赖 `build/`、没有依赖绝对路径、没有漏提交文件。

---

## 常见问题

**`git push` 报 `failed to push some refs` / `Updates were rejected`**
远端不是空仓库（创建时勾了 README）。要么把远端 `main` 与本地合并：

```bash
git pull --rebase origin main
git push -u origin main
```

要么删掉远端仓库重建一个空的。

**`git push` 要求输入密码，输账号密码却报 `Authentication failed`**
GitHub 早已禁用账号密码推送。改用第 1 步方案 A 的 **Personal Access Token**，或改用 SSH。

**`Permission denied (publickey)`**
SSH 公钥还没加到 GitHub，或加错了。重跑 `ssh -T git@github.com` 定位：
- 提示 `Permission denied` → 公钥没加成功，重新 `cat ~/.ssh/id_ed25519.pub` 复制
- 提示 `Hi ... You've successfully authenticated` → 是远端地址写错（检查 `git remote -v`）

**`git commit` 报 `Please tell me who you are`**
第 0 步的身份没配置，或配到了别的用户下。用 `git config --global --list` 确认。

**不小心把 `build/` 提交了**

```bash
git rm -r --cached build
git commit -m "chore: 停止跟踪 build 目录"
```

`.gitignore` 只对**未跟踪**文件生效，已经提交过的文件必须先 `git rm --cached`。

**`cmake` 报找不到 C++ 编译器**
`build-essential` 没装全。`sudo apt install -y build-essential cmake`，
然后用 `g++ --version` 确认。

**`CMake Error: could not find CMAKE_ROOT` 之类，或 `cmake` 行为异常**
很可能是 Snap 版的 cmake 混进了 PATH（`which -a cmake` 若出现 `/snap/bin/cmake` 即是）。
按 README 第 4 节把 Snap 版本移除，确保用 `/usr/bin/cmake`。
