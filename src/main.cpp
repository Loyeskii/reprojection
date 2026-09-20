#include "reprojection.h"

#include <iomanip>
#include <iostream>

using namespace reprojection;

int main() {
  std::cout << std::fixed << std::setprecision(4);
  std::cout << "=== 重投影计算 ===\n";
  std::cout << "请按顺序输入以下数据（每行用空格分隔）：\n";
  std::cout << "1) 世界点 Pw: x y z\n";
  std::cout << "2) 相机内参: fx fy cx cy\n";
  std::cout << "3) 外参 R 第1行: r00 r01 r02\n";
  std::cout << "4) 外参 R 第2行: r10 r11 r12\n";
  std::cout << "5) 外参 R 第3行: r20 r21 r22\n";
  std::cout << "6) 外参 t: tx ty tz\n";
  std::cout << "7) 观测像素: u v\n";
  std::cout << "----------------------------------------\n";

  Point3D pw;
  CameraIntrinsics K;
  CameraExtrinsics ext;
  Point2D observed;

  // 依次读取
  if (!(std::cin >> pw.x >> pw.y >> pw.z)) {
    std::cerr << "[错误] 读取世界点失败\n";
    return 1;
  }
  if (!(std::cin >> K.fx >> K.fy >> K.cx >> K.cy)) {
    std::cerr << "[错误] 读取相机内参失败\n";
    return 1;
  }
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (!(std::cin >> ext.R[i][j])) {
        std::cerr << "[错误] 读取旋转矩阵失败\n";
        return 1;
      }
    }
  }
  if (!(std::cin >> ext.t[0] >> ext.t[1] >> ext.t[2])) {
    std::cerr << "[错误] 读取平移向量失败\n";
    return 1;
  }
  if (!(std::cin >> observed.u >> observed.v)) {
    std::cerr << "[错误] 读取观测像素失败\n";
    return 1;
  }

  std::cout << "----------------------------------------\n";
  std::cout << "输入汇总：\n";
  std::cout << "  世界点 Pw = (" << pw.x << ", " << pw.y << ", " << pw.z << ")\n";
  std::cout << "  内参 fx=" << K.fx << " fy=" << K.fy << " cx=" << K.cx << " cy=" << K.cy << "\n";
  std::cout << "  平移 t = (" << ext.t[0] << ", " << ext.t[1] << ", " << ext.t[2] << ")\n";
  std::cout << "  观测像素 = (" << observed.u << ", " << observed.v << ")\n";
  std::cout << "----------------------------------------\n";

  // 调用重投影函数
  auto result = reproject(pw, K, ext, observed);
  if (!result) {
    std::cout << "[异常] 投影失败：该点在相机坐标系下深度非正（Zc <= 0），"
                 "可能位于相机后方或光心平面上。\n";
    return 2;
  }

  std::cout << "计算结果：\n";
  std::cout << "  投影像素 = (" << result->pixel.u << ", " << result->pixel.v << ")\n";
  std::cout << "  像素欧氏距离 = " << result->pixel_distance << "\n";
  return 0;
}
