#pragma once
#include <stdlib.h>
#include <iostream>
#include <GL/freeglut.h>
#include <cmath>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include "plotter_core.h"


// ------------------------------ НАСТРОЙКИ ------------------------------

//ЭКРАН - передаются при инициализации
extern int window_width, window_height;         //Высота
extern double aspect;             //Отношение сторон


// ЦВЕТА по умолчанию
extern double color_axis[3];      //Ось
extern double color_text[3];      //Текст
extern double color_array[3];     //Динамический цвет
extern int *color_table;          //Список цветов по умолчанию
extern double *surface_colormap;

//Здесь хранятся информация о осях для каждой поверхности
//- для корректной закраски
extern double **surface_start_end;

//Флаги освещения
extern bool lighting_activate, lighting_deactivate, lighting_status;

//Тип окрашивания поверхности
extern bool color_range;


//РАЗМЕРЫ
extern double axis_point_size;      //Точек оси
extern double axis_line_size;       //Ось
extern double net_line_size;        //Сетка

extern double graphic_point_size;   //Точек графика
extern double graphic_line_size;    //Линий графика

extern double surface_point_size;    //Поверхность
extern double surface_line_size;    //Сеть поверхности
extern double surface_triangle_volume; //Толщина поверхности

extern double axis_text_size, marks_text_size;    //Размер засечек
extern double axis_mark_line_size;  //Размер засечек
extern double text_size;            //Толщина линии текста
extern double text_point_size;      //Размер точек текста
extern double def_text_size_big, def_text_size_normal, def_text_size_small, def_text_size_very_small;
extern double def_point_size_big, def_point_size_normal, def_point_size_small;
extern double def_line_thick, def_line_normal, def_line_thin;



//ТЕКСТ
extern std::string axis_ox_text, axis_oy_text, axis_oz_text;   //Ось z
extern std::string *object_name;        //Надписи легенды.
extern std::string color_name;


//ПАРАМЕТРЫ ИЗОБРАЖЕНИЯ
extern bool simplify3d;         //Только оси
extern bool legend_need;        //Нужна ли легенда?
extern bool net_need;           //Отображать сетку?
extern bool mm;                 //Миллиметровый режим
extern bool antialiasing;       //Сглаживание
extern double zero_x;            //С какой позиции идёт легенда - от - 1 до 1
extern bool axis_text_need;      //Показ подписей осей
extern double diapason_x_start, diapason_x_end, diapason_y_start, diapason_y_end, diapason_z_start, diapason_z_end;
extern bool build_z_marks;      //Строить засечки для оси Z ?
extern double diapason_width, diapason_height, diapason_depth;           //Глубина


//ВЫЧИСЛЕНИЯ (передаются при инициализации в разных видах)
extern int point_number_x, point_number_y, point_number_z;

extern int number_of_sets, number_of_intervals;  //Число интервалов
extern double step_x, step_y;


//ОТОБРАЖЕНИЕ
extern int current_set;          //Текущий отображаемый набор
extern bool all_sets;        //Изображать все наборы
extern double x_x_position, y_y_position, z_z_position;

//Рисуем - поверхность?
extern bool is_surface;

//Тип графика линий
//1 - точечный
//2 - линии
//3 - линии с точками
extern int line_type;

//Тип поверхности
//1 - точки
//2 - линии и точки
//3 - линии
//4 - поверхность
extern int surface_type;


//КАМЕРА
extern int window_zoom, x_shift, y_shift, z_shift;
extern int add_x_canvas, add_y_canvas, add_z_canvas;
extern double factor, new_factor;
extern double x_degree, y_degree, z_degree;


extern double *surface_tesseract;
extern double(**input_f)(double);

// -------------------------------------------------------------------------------


using Point = plotter_core::Point;
extern Point *points_array;


class bound
{
public:
	Point left, right;

	bool eqv(int coord)
	{
		double temp;
		switch (coord)
		{
		case 0:
			temp = left.x - right.x;
			break;
		case 1:
			temp = left.y - right.y;
			break;
		case 2:
			temp = left.z - right.z;
			break;
		default:
			temp = 1;
		}
		temp = abs(temp);
		return temp < 0.00000001;
	}
};
extern bound **exclusion_intervals;


bool bounds_check(bound interval, double evalue, int coord);
bool infinity_check(double evalue);
bool nan_check(double evalue);


class Graphic
{
public:
	std::unique_ptr<Point[]> points_storage;
	Point *points;
	double(*fun_y)(double);
	double(*fun_z)(double);
	
	Graphic(double(*new_fun)(double))
		: points_storage(new Point[point_number_x + 1]), points(points_storage.get())
	{
		fun_y = new_fun;
		fun_z = new_fun;
		Comp();
	}

	Graphic()
		: points_storage(new Point[point_number_x + 1]), points(points_storage.get())
	{
		fun_y = nullptr;
		fun_z = nullptr;
	}

	//Расчёт точек поверхности
	void Comp()
	{
		std::vector<plotter_core::ExclusionInterval> exclusions;
		if (number_of_intervals < 0)
			throw std::invalid_argument("Number of exclusion intervals must not be negative");
		if (number_of_intervals > 0)
		{
			if (exclusion_intervals == nullptr || exclusion_intervals[current_set] == nullptr)
				throw std::invalid_argument("Exclusion interval data must not be null");
			for (int i = 0; i < number_of_intervals; ++i)
			{
				const bound &interval = exclusion_intervals[current_set][i];
				plotter_core::ExclusionInterval converted = {
					{interval.left.x, interval.left.y, interval.left.z},
					{interval.right.x, interval.right.y, interval.right.z}
				};
				exclusions.push_back(converted);
			}
		}
		const std::vector<Point> samples = plotter_core::sample_function(
			diapason_x_start, step_x, point_number_x, fun_y, fun_z, exclusions);
		std::copy(samples.begin(), samples.end(), points);
	}
};
extern Graphic *function_array;


class Surface
{
public:
	//Поверхность
	double(*fun_surface)(double, double);
	std::unique_ptr<Point[]> point_storage;
	std::vector<Point*> point_rows;
	Point **point;
	
	double surface_start, surface_end;
	
	Surface(double(*new_f)(double, double))
		: point_storage(new Point[static_cast<std::size_t>(point_number_x) * static_cast<std::size_t>(point_number_y)]),
		  point_rows(point_number_x), point(point_rows.data())
	{
		fun_surface = new_f;
		for (int i = 0; i < point_number_x; i++)
			point_rows[i] = point_storage.get() + i * point_number_y;

	}

	Surface()
		: point_storage(new Point[static_cast<std::size_t>(point_number_x) * static_cast<std::size_t>(point_number_y)]()),
		  point_rows(point_number_x), point(point_rows.data())
	{
		for (int i = 0; i < point_number_x; i++)
			point_rows[i] = point_storage.get() + i * point_number_y;
		fun_surface = nullptr;
	}

	void compute_surface()
	{
		const plotter_core::SurfaceSamples samples = plotter_core::sample_surface(
			diapason_x_start, diapason_y_start, step_x, step_y,
			point_number_x, point_number_y, fun_surface);
		for (int i = 0; i < point_number_x; ++i)
			std::copy(samples.points.begin() + static_cast<std::size_t>(i) * point_number_y,
			          samples.points.begin() + static_cast<std::size_t>(i + 1) * point_number_y,
			          point[i]);

		surface_start = samples.minimum;
		surface_end = samples.maximum;

		diapason_z_start = std::min(samples.minimum, diapason_z_start);
		diapason_z_start = floor(diapason_z_start);
		diapason_z_end = std::max(samples.maximum, diapason_z_end);
		diapason_z_end = ceil(diapason_z_end);
	}
};
extern Surface *surf_array;


//Вращение, смещение, факторизация
void factorization();

//Получить цвет по номеру
std::string color_map(int numer);
//Получение цвета по названию
void color_select(std::string color_name);
//Фон
void background();
//Общая инициализация, передаются размеры окна
void initialization_general(int i_window_width, int i_window_height);

// Параметры линии
void line_settings(double line_width, double color_r, double color_b, double  color_g);
//Отрисовка линии
void draw_line(double x_start, double y_start, double z_start,
               double x_end, double y_end, double z_end);

//Параметры точек
void point_settings(double point_size, double color_r, double color_b, double color_g);
//Отрисовка точек
void draw_point(double x_pos, double y_pos, double z_pos);

//Отрисовка текста - подготовка
void draw_text_preparation(double coord_x, double coord_y, double coord_z, double size_coeff);
//Пишем текст
void draw_string_stroke(std::string input);
void draw_string_3d(std::string input, double coord_x, double coord_y, double coord_z, double size_coeff);
//Полный текст
void draw_string(std::string input, double coord_x, double coord_y, double size_coeff);

// Отрисовка осей
void draw_axis();
//Получение порядка числа
int GetMagnitudeOfInteger(double x);
//Отрисовка засечек
void draw_axis_marks();
//Легенда
void draw_legend();
//Сетка
void draw_net();

//Задание размеров изображение после вычисления
void set_bounds();
//Задание размеров изображение после вычисления функции
void set_f_bounds();
//Задание размеров изображения графика для точеченого графика
void initialization_bounds();

//Инициализация для функций
void initialization_function(double i_diapason_x_start, double i_diapason_x_end,
                             double(**i_functions)(double), int i_functions_number);
//Инициализация для поверхностей
void initialization_surface(double i_diapason_x_start, double i_diapason_x_end,
                            double i_diapason_y_start, double i_diapason_y_end,
                            double(**i_surface_functions)(double, double),
                            int i_surface_number);

//Цвет поверхности
//Преобразуем точку y в точку между 0 и 1
//На основе этого вычисляем цвет
void surface_coloring(double axis_start, double axis_end, double current_axis);


//Отрисовка треугольника
void draw_triangle(Point first_point, Point second_point, Point third_point, double t_size);

void draw_surface(int t);


//Начальные настройки графики
void graphic_initialization();
void draw_graphic();

//Отрисовка графика
void draw();

// Отрисовка кадра
void display();

//Изменение размера
void OnRedraw(GLint w, GLint h);
//Замыкание круга
double degree_circling(double x);

//Обработчик клавиатуры. 
void keyboard(unsigned char key, GLint x, GLint y);
// Отрисовка
void OpenGL();


//Инициализация наборов точек
void initialization_points(const std::vector<std::vector<plotter_core::Coordinates>> &point_sets);
// Совместимый интерфейс: ось -> точка -> набор.
void initialization_p_many(double ***i_points_xyz, int i_point_number_x, int i_axis_number, int i_sets_number);


// ПОЛЬЗОВАТЕЛЬСКИЕ НАСТРОЙКИ

//Оси
void Set_Axis_X_Text(std::string new_text); // Текст по оси x
void Set_Axis_Y_Text(std::string new_text); // Текст по оси y
void Set_Axis_Z_Text(std::string new_text); // Текст по оси z
void Set_Axis_Text_Size(double new_size);   //Размер шрифта текста
void Set_Text_Line_Size(double new_size);   //Размер линии текста
void Set_Marks_Text_Size(double new_size);  //Размер шрифта текста засечек
void Show_Z_Marks(bool boolean);            // Показать засечки по оси Z
void Set_Axis_Point_Size(double new_size);  // Размер точек осей
void Set_Axis_Line_Size(double new_size);   // Размер линий осей
void Set_Marks_Line_Size(double new_size);  // Размер засечек осей 

// График
void surface_mode(bool new_mode);
void Set_Step_X(double new_step);             // Шаг по оси x
void Set_Step_Y(double new_step);             // Шаг по оси y
void Set_Graphic_Line_Size(double new_size);  // Размер линий графика
void Set_Graphic_Point_Size(double new_size); // Размер точек графика
// Поверхность
void Set_Surface_Line_Size(double new_size);
void Set_Surface_Point_Size(double new_size);
void Set_Surface_Width(double new_size);


//Сетка
void Set_Net(bool flag);                    // Нужна сетка?
void Set_Net_Line_Size(double new_size);    // Размер линий сетки
void tenth_mode(bool flag);                 // Режим миллиметра

// Легенда
void Set_Legend_Text(std::string* new_text); // Текст легенды
void Set_Legend_Text(std::string* new_text, int text_count);
void Set_Legend_Start_Pos_X(double posit);   // Позиция легенды
void Show_Legend(bool flag);                 // Показать легенду?
