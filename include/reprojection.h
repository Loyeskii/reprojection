// 理想针孔相机重投影（忽略畸变）
//
// 外参约定为世界到相机的变换：  Pc = R * Pw + t
// 三维坐标 Pw 与平移量 t 使用相同长度单位。
//
// 投影公式（Pc = (X, Y, Z)，要求 Z > 0）：
//     u = fx * X / Z + cx
//     v = fy * Y / Z + cy
#ifndef REPROJECTION_H_
#define REPROJECTION_H_

#include <optional>

namespace reprojection {

/// 二维像素点 (u, v)。
struct Point2D {
  double u{0.0};
  double v{0.0};
};

/// 三维点 (x, y, z)。
struct Point3D {
  double x{0.0};
  double y{0.0};
  double z{0.0};
};

/// 相机内参（单位：像素）。
struct CameraIntrinsics {
  double fx{0.0};  ///< x 方向焦距，必须 > 0
  double fy{0.0};  ///< y 方向焦距，必须 > 0
  double cx{0.0};  ///< 主点 u 坐标
  double cy{0.0};  ///< 主点 v 坐标
};

/// 相机外参：世界 -> 相机，即 Pc = R * Pw + t。
struct CameraExtrinsics {
  double R[3][3]{{1.0, 0.0, 0.0},
                 {0.0, 1.0, 0.0},
                 {0.0, 0.0, 1.0}};  ///< 旋转矩阵
  double t[3]{0.0, 0.0, 0.0};       ///< 平移向量，相机坐标系下
};

/// 重投影结果。
struct ReprojectionResult {
  Point2D pixel;            ///< 投影像素坐标 (u, v)
  double pixel_distance{0.0};  ///< 与观测像素之间的欧氏距离，单位像素
};

/// 重投影：世界点 -> 像素坐标，并计算与观测点之间的像素欧氏距离。
/// 成功返回结果；深度非正（Zc <= 0）等异常情况返回 std::nullopt。
std::optional<ReprojectionResult> reproject(const Point3D& pw,
                                            const CameraIntrinsics& K,
                                            const CameraExtrinsics& ext,
                                            const Point2D& observed);

}  // namespace reprojection

#endif  // REPROJECTION_H_
