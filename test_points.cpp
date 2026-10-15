#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include "plotter.h"


using namespace std;


int main(int argc, char** argv)
{
    // Начальные настройки
    //Set_Axis_X_Text("Ось x");
    //Set_Axis_Y_Text("Ось y");
    //Set_Axis_Z_Text("Ось z");

    //Set_Axis_Text_Size(24.);
    //Set_Text_Line_Size(100.);
    //Set_Marks_Text_Size(24.);
    //Show_Z_Marks(true);

    //Set_Axis_Point_Size(24.);       //Работает
    //Set_Axis_Line_Size(12.);        //Работает
    //Set_Marks_Line_Size(10.);       //Работает

    // График
    //surface_mode(true);
    //Set_Step_X(1);                  //Работает           
    //Set_Step_Y(1);                  //Работает
    //Set_Graphic_Line_Size(6.);      //Работает
    //Set_Graphic_Point_Size(24.);    //Работает

    //Сетка
    //Set_Net(true);                  //Работает                   
    //Set_Net_Line_Size(10);          //Работает
    //tenth_mode(true);               //Работает  

    //Set_Legend_Start_Pos_X(-0.01);  
    Show_Legend(false);              //Работает

    glutInit(&argc, argv); // Инициализация FreeGLUT             
	
    // Легенда
    std::vector<std::string> legend_text = {"points1", "points2"};
    Set_Legend_Text(legend_text.data(), static_cast<int>(legend_text.size()));

    // Размер окна
    initialization_general(800, 800);

    // Наборы точек: внешний вектор задаёт набор, внутренний — его точки.
    std::vector<std::vector<plotter_core::Coordinates>> points = {
        {
            {-4.5, 0.0, 0.0}, {-4.0, 1.5, 0.0}, {-3.6, -3.0, 0.0},
            {-3.2, 1.7, 0.0}, {-2.9, 2.2, 0.0}, {-2.3, -0.5, 0.0},
            {0.0, 4.5, 0.0}, {1.5, 3.0, 0.0}, {3.9, 3.0, 0.0}, {4.9, 2.2, 0.0}
        },
        {
            {-5.0, 0.01, 0.0}, {-4.0, 0.9, 0.0}, {-3.0, 2.0, 0.0},
            {-2.0, 0.0, 0.0}, {-1.0, 4.0, 0.0}, {0.0, 3.0, 0.0},
            {1.0, 2.5, 0.0}, {2.0, 1.0, 0.0}, {3.0, -1.2, 0.0}, {4.0, 2.2, 0.0}
        }
    };

    Set_Graphic_Point_Size(15);
    color_select("Blue");

	initialization_points(points);

    OpenGL();

    return 0;
}
