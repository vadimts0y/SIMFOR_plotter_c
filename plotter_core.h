#pragma once

#include <vector>

namespace plotter_core
{
// OpenGL-free sampled data and numerical helpers for plotter consumers.
struct Point
{
	bool valid = true;
	double x = 0.;
	double y = 0.;
	double z = 0.;
	double static_color[3] = {0., 0., 0.};
	double dynamic_color[3] = {0., 0., 0.};
};

struct Coordinates
{
	double x;
	double y;
	double z;

	Coordinates(double x_value = 0., double y_value = 0., double z_value = 0.)
		: x(x_value), y(y_value), z(z_value) {}
};

struct ExclusionInterval
{
	double lower[3];
	double upper[3];
};

struct SurfaceSamples
{
	std::vector<Point> points;
	double minimum;
	double maximum;
};

using Function1D = double (*)(double);
using Function2D = double (*)(double, double);

int sample_count(double start, double end, double step, const char *axis);
std::vector<Point> sample_function(double start, double step, int count,
	Function1D y_function, Function1D z_function,
	const std::vector<ExclusionInterval> &exclusions);
SurfaceSamples sample_surface(double x_start, double y_start,
	double step_x, double step_y, int rows, int columns, Function2D function);
}
