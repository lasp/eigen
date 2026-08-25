// Translation unit for the freestanding compile check
// (.github/scripts/check_freestanding.sh).
//
// It pulls in the modules the downstream flight software uses, because the
// freestanding configuration is guarded module by module: a macro or shim that
// is wrong for Geometry or Eigenvalues will not show up from Eigen/Core alone.
// Nothing here needs to run -- the check is compile-only, and the bugs it exists
// to catch (a leaked macro, a stray #pragma message) surface while the
// preprocessor and front end walk these headers.

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <Eigen/SVD>
#include <Eigen/LU>

// Instantiate a little of each module so the definitions are actually walked
// rather than just parsed. Kept to float and double, the two scalar types the
// flight software uses.
namespace {

template <typename Scalar>
void touchEveryModule() {
    using Vector3 = Eigen::Matrix<Scalar, 3, 1>;
    using Matrix3 = Eigen::Matrix<Scalar, 3, 3>;

    Vector3 a = Vector3::Zero();
    Vector3 b = Vector3::UnitZ();
    Matrix3 m = Matrix3::Identity();

    // Core
    (void)a.dot(b);
    (void)a.norm();
    (void)(m * a);

    // Geometry: declared in Eigen/Core, defined in Eigen/src/Geometry.
    (void)a.cross(b);
    (void)b.unitOrthogonal();

    // LU
    (void)m.determinant();

    // Eigenvalues
    Eigen::SelfAdjointEigenSolver<Matrix3> eig(m);
    (void)eig.eigenvalues();

    // SVD
    Eigen::JacobiSVD<Matrix3> svd(m, Eigen::ComputeFullU | Eigen::ComputeFullV);
    (void)svd.singularValues();
}

}  // namespace

int main() {
    touchEveryModule<float>();
    touchEveryModule<double>();
    return 0;
}
