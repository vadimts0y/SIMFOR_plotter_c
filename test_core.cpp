#include "plotter_core.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{
void require(bool condition, const std::string &message)
{
	if (!condition)
		throw std::runtime_error(message);
}

template <typename Callable>
void require_invalid_argument(Callable callable, const std::string &message)
{
	try
	{
		callable();
	}
	catch (const std::invalid_argument &)
	{
		return;
	}
	throw std::runtime_error(message);
}

double identity(double x) { return x; }
double reciprocal(double x) { return 1. / x; }
double square_root(double x) { return std::sqrt(x); }
double plane(double x, double y) { return x + y; }
double non_finite_surface(double x, double y)
{
	return x == 1. && y == 1. ? std::numeric_limits<double>::infinity() : x - y;
}

void test_sample_count()
{
	require(plotter_core::sample_count(-1., 1., 0.5, "X") == 5, "inclusive sample count");
	require(plotter_core::sample_count(0., 1., 0.3, "X") == 4, "non-dividing step sample count");
	require_invalid_argument([] { plotter_core::sample_count(2., 1., 0.1, "X"); }, "reversed range accepted");
	require_invalid_argument([] { plotter_core::sample_count(0., 1., 0., "X"); }, "zero step accepted");
	require_invalid_argument([] { plotter_core::sample_count(0., 1., std::numeric_limits<double>::infinity(), "X"); }, "infinite step accepted");
	require_invalid_argument([] { plotter_core::sample_count(0., 1e100, 1e-100, "X"); }, "oversized grid accepted");
}

void test_function_sampling()
{
	const auto reciprocal_points = plotter_core::sample_function(-1., 1., 3, reciprocal, nullptr, {});
	require(reciprocal_points.size() == 3, "function sample array size");
	require(reciprocal_points[0].valid && !reciprocal_points[1].valid && reciprocal_points[2].valid,
	        "infinite function result was not marked invalid");

	const auto sqrt_points = plotter_core::sample_function(-1., 1., 3, square_root, nullptr, {});
	require(!sqrt_points[0].valid && sqrt_points[1].valid && sqrt_points[2].valid,
	        "NaN function result was not marked invalid");

	plotter_core::ExclusionInterval x_exclusion = {{0.5, 0., 0.}, {1.5, 0., 0.}};
	const auto excluded_x = plotter_core::sample_function(0., 1., 3, identity, nullptr, {x_exclusion});
	require(excluded_x[0].valid && !excluded_x[1].valid && excluded_x[2].valid,
	        "X exclusion interval was not applied");

	plotter_core::ExclusionInterval y_exclusion = {{0., 0.5, 0.}, {0., 1.5, 0.}};
	const auto excluded_y = plotter_core::sample_function(0., 1., 3, identity, nullptr, {y_exclusion});
	require(excluded_y[0].valid && !excluded_y[1].valid && excluded_y[2].valid,
	        "Y exclusion interval was not applied");

	plotter_core::ExclusionInterval z_exclusion = {{0., 0., 0.5}, {0., 0., 1.5}};
	const auto excluded_z = plotter_core::sample_function(0., 1., 3, identity, identity, {z_exclusion});
	require(excluded_z[0].valid && !excluded_z[1].valid && excluded_z[2].valid,
	        "Z exclusion interval was not applied");

	require_invalid_argument([] { plotter_core::sample_function(0., 1., 0, identity, nullptr, {}); },
	                         "zero-sized function array accepted");
	require_invalid_argument([] { plotter_core::sample_function(0., 1., 1, nullptr, nullptr, {}); },
	                         "null function accepted");
}

void test_surface_sampling()
{
	const auto surface = plotter_core::sample_surface(-1., 0., 1., 1., 2, 3, plane);
	require(surface.points.size() == 6, "surface array size");
	require(surface.points[0].x == -1. && surface.points[0].y == 0., "surface row-major coordinates");
	require(surface.points[4].x == 0. && surface.points[4].y == 1., "surface second row coordinates");
	require(surface.points[4].z == -1., "surface Z orientation");
	require(surface.minimum == -1. && surface.maximum == 2., "surface minimum/maximum");

	const auto non_finite = plotter_core::sample_surface(0., 0., 1., 1., 2, 2, non_finite_surface);
	require(non_finite.points[3].z == 0. && non_finite.points[3].valid, "non-finite surface result handling");

	require_invalid_argument([] { plotter_core::sample_surface(0., 0., 1., 1., 0, 2, plane); },
	                         "empty surface dimensions accepted");
	require_invalid_argument([] { plotter_core::sample_surface(0., 0., 1., 1., 1, 1, nullptr); },
	                         "null surface function accepted");
	require_invalid_argument([] {
		plotter_core::sample_surface(0., 0., 1., 1., std::numeric_limits<int>::max(),
		                             std::numeric_limits<int>::max(), plane);
	}, "oversized surface array accepted");
}
}

int main()
{
	try
	{
		test_sample_count();
		test_function_sampling();
		test_surface_sampling();
	}
	catch (const std::exception &error)
	{
		std::cerr << "Core test failed: " << error.what() << '\n';
		return 1;
	}

	std::cout << "Core tests passed\n";
	return 0;
}
