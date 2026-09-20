#include "reprojection.h"

#include <cmath>

namespace reprojection {

std::optional<ReprojectionResult> reproject(const Point3D& pw,
                                            const CameraIntrinsics& K,
                                            const CameraExtrinsics& ext,
                                            const Point2D& observed) {
  // 内参非法（程序性错误，直接判定失败）
  if (!(K.fx > 0.0) || !(K.fy > 0.0)) {
    return std::nullopt;
  }

  // 输入含 NaN / Inf
  if (!std::isfinite(pw.x) || !std::isfinite(pw.y) || !std::isfinite(pw.z) ||
      !std::isfinite(ext.t[0]) || !std::isfinite(ext.t[1]) || !std::isfinite(ext.t[2])) {
    return std::nullopt;
  }

  // 世界坐标 -> 相机坐标：Pc = R * Pw + t
  const double Xc = ext.R[0][0] * pw.x + ext.R[0][1] * pw.y + ext.R[0][2] * pw.z + ext.t[0];
  const double Yc = ext.R[1][0] * pw.x + ext.R[1][1] * pw.y + ext.R[1][2] * pw.z + ext.t[1];
  const double Zc = ext.R[2][0] * pw.x + ext.R[2][1] * pw.y + ext.R[2][2] * pw.z + ext.t[2];

  // 非正深度：Zc = 0 在成像平面上，Zc < 0 在相机后方，针孔模型无意义
  if (!(Zc > 0.0)) {
    return std::nullopt;
  }

  // 针孔投影
  const double u = K.fx * Xc / Zc + K.cx;
  const double v = K.fy * Yc / Zc + K.cy;

  // 深度极小时可能溢出成 NaN / Inf
  if (!std::isfinite(u) || !std::isfinite(v)) {
    return std::nullopt;
  }

  ReprojectionResult result;
  result.pixel.u = u;
  result.pixel.v = v;
  result.pixel_distance = std::sqrt((u - observed.u) * (u - observed.u) +
                                    (v - observed.v) * (v - observed.v));
  return result;
}

}  // namespace reprojection
