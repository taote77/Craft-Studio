#ifndef SLICINGCORE_COORD_HPP
#define SLICINGCORE_COORD_HPP

#include <cstdint>
#include <cmath>
#include <vector>
#include <limits>

namespace slicing {

// ============================================================================
// Coordinate system: int64_t microns (1 unit = 0.001 mm)
//
// All geometry operations in the engine use signed 64-bit integer coordinates
// in microns. This matches Clipper2's native format and eliminates floating-
// point drift during boolean operations.
// ============================================================================

using coord_t = int64_t;
using coordf_t = double;

/// Epsilon for coordinate comparisons (0.01 mm in microns)
constexpr coord_t SCALED_EPSILON = 10;

/// Scaling factor: mm per coordinate unit
constexpr double SCALING_FACTOR = 0.001;

/// Convert millimeters to internal coordinate units (microns)
inline coord_t scale(double mm)
{
    return static_cast<coord_t>(std::round(mm / SCALING_FACTOR));
}

/// Convert internal coordinate units to millimeters
inline double unscale(coord_t u)
{
    return static_cast<double>(u) * SCALING_FACTOR;
}

/// Convert internal coordinate units to millimeters (floating-point)
inline coordf_t unscalef(coord_t u)
{
    return static_cast<coordf_t>(u) * SCALING_FACTOR;
}

// ============================================================================
// 2D Point
// ============================================================================

struct Point
{
    coord_t x = 0;
    coord_t y = 0;

    Point() = default;
    Point(coord_t x_, coord_t y_) : x(x_), y(y_) {}

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const
    {
        return !(*this == other);
    }

    Point operator+(const Point& other) const
    {
        return { x + other.x, y + other.y };
    }

    Point operator-(const Point& other) const
    {
        return { x - other.x, y - other.y };
    }

    Point operator*(coord_t s) const
    {
        return { x * s, y * s };
    }

    Point operator/(coord_t s) const
    {
        return { x / s, y / s };
    }

    Point& operator+=(const Point& other)
    {
        x += other.x; y += other.y;
        return *this;
    }

    Point& operator-=(const Point& other)
    {
        x -= other.x; y -= other.y;
        return *this;
    }

    /// Squared distance to another point
    coord_t distSq(const Point& other) const
    {
        coord_t dx = x - other.x;
        coord_t dy = y - other.y;
        return dx * dx + dy * dy;
    }

    /// Euclidean distance to another point (in microns)
    double dist(const Point& other) const
    {
        return std::sqrt(static_cast<double>(distSq(other)));
    }

    /// Check if point is approximately equal to another (within SCALED_EPSILON)
    bool approxEqual(const Point& other, coord_t epsilon = SCALED_EPSILON) const
    {
        return std::abs(x - other.x) <= epsilon
            && std::abs(y - other.y) <= epsilon;
    }
};

using Points = std::vector<Point>;

// ============================================================================
// 3D Point
// ============================================================================

struct Point3D
{
    coord_t x = 0;
    coord_t y = 0;
    coord_t z = 0;

    Point3D() = default;
    Point3D(coord_t x_, coord_t y_, coord_t z_) : x(x_), y(y_), z(z_) {}

    bool operator==(const Point3D& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Point3D& other) const
    {
        return !(*this == other);
    }

    Point3D operator+(const Point3D& other) const
    {
        return { x + other.x, y + other.y, z + other.z };
    }

    Point3D operator-(const Point3D& other) const
    {
        return { x - other.x, y - other.y, z - other.z };
    }

    Point3D operator*(coord_t s) const
    {
        return { x * s, y * s, z * s };
    }

    Point3D operator/(coord_t s) const
    {
        return { x / s, y / s, z / s };
    }

    Point3D& operator+=(const Point3D& other)
    {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    Point3D& operator-=(const Point3D& other)
    {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    /// Squared distance to another point
    coord_t distSq(const Point3D& other) const
    {
        coord_t dx = x - other.x;
        coord_t dy = y - other.y;
        coord_t dz = z - other.z;
        return dx * dx + dy * dy + dz * dz;
    }

    /// Euclidean distance to another point (in microns)
    double dist(const Point3D& other) const
    {
        return std::sqrt(static_cast<double>(distSq(other)));
    }
};

using Points3D = std::vector<Point3D>;

// ============================================================================
// 2D Bounding Box
// ============================================================================

struct BoundingBox2D
{
    Point min;
    Point max;

    BoundingBox2D() :
        min(std::numeric_limits<coord_t>::max(), std::numeric_limits<coord_t>::max()),
        max(std::numeric_limits<coord_t>::min(), std::numeric_limits<coord_t>::min())
    {}

    BoundingBox2D(const Point& min_, const Point& max_) : min(min_), max(max_) {}

    void extend(const Point& p)
    {
        if (p.x < min.x) min.x = p.x;
        if (p.y < min.y) min.y = p.y;
        if (p.x > max.x) max.x = p.x;
        if (p.y > max.y) max.y = p.y;
    }

    coord_t width()  const { return max.x - min.x; }
    coord_t height() const { return max.y - min.y; }

    Point center() const
    {
        return { (min.x + max.x) / 2, (min.y + max.y) / 2 };
    }

    bool contains(const Point& p) const
    {
        return p.x >= min.x && p.x <= max.x
            && p.y >= min.y && p.y <= max.y;
    }

    bool valid() const
    {
        return min.x <= max.x && min.y <= max.y;
    }
};

// ============================================================================
// 3D Bounding Box
// ============================================================================

struct BoundingBox3D
{
    Point3D min;
    Point3D max;

    BoundingBox3D() :
        min(std::numeric_limits<coord_t>::max(),
            std::numeric_limits<coord_t>::max(),
            std::numeric_limits<coord_t>::max()),
        max(std::numeric_limits<coord_t>::min(),
            std::numeric_limits<coord_t>::min(),
            std::numeric_limits<coord_t>::min())
    {}

    BoundingBox3D(const Point3D& min_, const Point3D& max_) : min(min_), max(max_) {}

    void extend(const Point3D& p)
    {
        if (p.x < min.x) min.x = p.x;
        if (p.y < min.y) min.y = p.y;
        if (p.z < min.z) min.z = p.z;
        if (p.x > max.x) max.x = p.x;
        if (p.y > max.y) max.y = p.y;
        if (p.z > max.z) max.z = p.z;
    }

    coord_t width()  const { return max.x - min.x; }
    coord_t height() const { return max.y - min.y; }
    coord_t depth()  const { return max.z - min.z; }

    Point3D center() const
    {
        return { (min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2 };
    }

    Point3D size() const
    {
        return { width(), height(), depth() };
    }

    bool contains(const Point3D& p) const
    {
        return p.x >= min.x && p.x <= max.x
            && p.y >= min.y && p.y <= max.y
            && p.z >= min.z && p.z <= max.z;
    }

    bool valid() const
    {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }
};

} // namespace slicing

#endif // SLICINGCORE_COORD_HPP
