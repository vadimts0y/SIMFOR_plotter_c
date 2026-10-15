#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include "plotter.h"


using namespace std;


// Гиперболический параболоид (седло)
double f1(double x, double y)
{
    return (x*x/0.9 - y*y/0.9);
}

// эллиптический параболоид
double f2(double x, double y)
{
    return (x*x/0.9 + y*y/0.9);
}

double f3(double x, double y)
{
    return sqrt(x*x/0.9 + y*y/0.9);
}


int main(int argc, char** argv)
{
    // Начальные настройки
    //Set_Axis_X_Text("Ось x");
    //Set_Axis_Y_Text("Ось y");
    //Set_Axis_Z_Text("Ось z");

    //Set_Axis_Text_Size(24.);
    //Set_Text_Line_Size(100.);
    //Set_Marks_Text_Size(24.);
    Show_Z_Marks(true);

    //Set_Axis_Point_Size(24.);       //Работает
    //Set_Axis_Line_Size(12.);        //Работает
    //Set_Marks_Line_Size(10.);       //Работает

    // График
    surface_mode(true);
    //Set_Step_X(1);                  //Работает           
    //Set_Step_Y(1);                  //Работает
    //Set_Graphic_Line_Size(6.);      //Работает
    //Set_Graphic_Point_Size(24.);    //Работает

    //Сетка
    //Set_Net(true);                  //Работает                   
    //Set_Net_Line_Size(10);          //Работает
    //tenth_mode(true);               //Работает  

    Set_Surface_Line_Size(2);         //Работает
    Set_Surface_Point_Size(1);        //???
    Set_Surface_Width(40);            //???

    Set_Legend_Start_Pos_X(-0.02);  
    Show_Legend(false);              //Работает

    glutInit(&argc, argv); // Инициализация FreeGLUT             
	
    // Легенда
    std::string legend_text[3];
	legend_text[0] = "x^2/0.9 - y^2/0.9";
	legend_text[1] = "x^2/0.9 + y^2/0.9";
    legend_text[2] = "sqrt(x^2/0.9 + y^2/0.9)";
    Set_Legend_Text(legend_text, 3);

    // Размер окна
    initialization_general(800, 800);

    double (*surface_functions[3])(double, double) = {f1, f2, f3};

    initialization_surface(-2, 2, -2, 2, surface_functions, 1);
    initialization_surface(-2, 2, -2, 2, surface_functions, 2);
    initialization_surface(-2, 2, -2, 2, surface_functions, 3);   

    OpenGL();

    return 0;
}
