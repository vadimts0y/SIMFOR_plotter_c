#include "plotter_core.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace plotter_core
{
namespace
{
bool inside(double value, double lower, double upper)
{
	return std::abs(lower - upper) >= 1e-8 && lower < value && value < upper;
}

bool excluded(double value, int axis, const std::vector<ExclusionInterval> &exclusions)
{
	for (const ExclusionInterval &interval : exclusions)
		if (inside(value, interval.lower[axis], interval.upper[axis]))
			return true;
	return false;
}
}

int sample_count(double start, double end, double step, const char *axis)
{
	if (!std::isfinite(start) || !std::isfinite(end) || start > end)
		throw std::invalid_argument(std::string("Invalid ") + axis + " range");
	if (!std::isfinite(step) || step <= 0.)
		throw std::invalid_argument(std::string("Step for ") + axis + " must be finite and positive");

	const double count = std::floor((end - start) / step) + 1.;
	if (!std::isfinite(count) || count < 1. ||
	    count > static_cast<double>(std::numeric_limits<int>::max() - 1))
		throw std::invalid_argument(std::string("Too many samples on ") + axis);
	return static_cast<int>(count);
}

std::vector<Point> sample_function(double start, double step, int count,
	Function1D y_function, Function1D z_function,
	const std::vector<ExclusionInterval> &exclusions)
{
	if (count <= 0 || y_function == nullptr || !std::isfinite(start) ||
	    !std::isfinite(step) || step <= 0.)
		throw std::invalid_argument("Function sampling requires finite start, positive step/count, and Y function");

	std::vector<Point> points(static_cast<std::size_t>(count));
	for (int i = 0; i < count; ++i)
	{
		Point &point = points[static_cast<std::size_t>(i)];
		const double x = start + step * i;
		if (!std::isfinite(x) || excluded(x, 0, exclusions))
		{
			point.valid = false;
			continue;
		}
		point.x = x;

		const double y = y_function(x);
		if (!std::isfinite(y) || excluded(y, 1, exclusions))
		{
			point.valid = false;
			continue;
		}
		point.y = y;

		if (z_function != nullptr)
		{
			const double z = z_function(x);
			point.z = z;
			if (!std::isfinite(z) || excluded(z, 2, exclusions))
			{
				point.valid = false;
				continue;
			}
		}
	}
	return points;
}

SurfaceSamples sample_surface(double x_start, double y_start,
	double step_x, double step_y, int rows, int columns, Function2D function)
{
	if (rows <= 0 || columns <= 0 || function == nullptr ||
	    !std::isfinite(x_start) || !std::isfinite(y_start) ||
	    !std::isfinite(step_x) || step_x <= 0. || !std::isfinite(step_y) || step_y <= 0.)
		throw std::invalid_argument("Surface sampling requires finite starts, positive steps/dimensions, and a function");
	const std::size_t row_count = static_cast<std::size_t>(rows);
	const std::size_t column_count = static_cast<std::size_t>(columns);
	if (row_count > std::numeric_limits<std::size_t>::max() / column_count ||
	    row_count * column_count > std::numeric_limits<std::size_t>::max() / sizeof(Point))
		throw std::invalid_argument("Surface grid is too large");

	SurfaceSamples result;
	result.points.resize(row_count * column_count);
	result.minimum = std::numeric_limits<double>::infinity();
	result.maximum = -std::numeric_limits<double>::infinity();
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < columns; ++j)
		{
			Point &point = result.points[static_cast<std::size_t>(i) * column_count + static_cast<std::size_t>(j)];
			point.x = x_start + step_x * j;
			point.y = y_start + step_y * i;
			const double value = function(point.x, point.y);
			if (!std::isfinite(value))
			{
				point.z = 0.;
				continue;
			}
			point.z = -value;
			result.minimum = std::min(result.minimum, value);
			result.maximum = std::max(result.maximum, value);
		}
	if (!std::isfinite(result.minimum))
		result.minimum = result.maximum = 0.;
	return result;
}
}
