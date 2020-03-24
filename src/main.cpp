#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <limits>

#include <random>
#include <unordered_map>
#include <chrono>

#include <cmath>

#include "Utils.hpp"
#include "PixelGrid.hpp"
#include "Screen.hpp"
#include "Mat4.hpp"
#include "Vec4.hpp"

void parse(std::string fileName, Screen &screen, Mat4 &edges, Mat4 &triangles, Mat4 &transform, Color color)
{
    std::ifstream infile(fileName);

    std::string line;

    while (std::getline(infile, line))
    {
        std::cout << "command: " << line << std::endl;
        if (line == "line")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v0, v1;
            iss >> v0 >> v1;
            edges.addEdge(v0, v1);
        }
        else if (line == "triangle")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v0, v1, v2;
            iss >> v0 >> v1 >> v2;
            triangles.addTriangle(v0, v1, v2);
        }
        else if (line == "ident")
        {
            transform = Mat4::identity();
        }
        else if (line == "scale")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            iss >> v;
            Mat4 const &temp = Mat4::scale(v);
            transform.multiplyMutate(temp);
        }
        else if (line == "move" || line == "translate")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            iss >> v;
            Mat4 const &temp = Mat4::translate(v);
            transform.multiplyMutate(temp);
        }
        else if (line == "rotate")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            char axis;
            double angle;
            iss >> axis >> angle;

            double PI = std::atan(1) * 4;

            angle *= PI / 180;
            Mat4 temp(0);
            temp.identity();
            if (axis == 'x')
            {
                temp = Mat4::rotX(angle);
            }
            else if (axis == 'y')
            {
                temp = Mat4::rotY(angle);
            }
            else if (axis == 'z')
            {
                temp = Mat4::rotZ(angle);
            }
            transform.multiplyMutate(temp);
        }
        else if (line == "apply")
        {
            edges.multiplyMutate(transform);
            triangles.multiplyMutate(transform);
        }
        else if (line == "display")
        {
            screen.clear({0, 0, 0, 255});
            screen.drawTriangles(triangles, color);
            screen.drawEdges(edges, color);
            screen.display();
        }
        else if (line == "save")
        {
            std::getline(infile, line);
            std::cout << "file name " << line << std::endl;
            screen.clear({0, 0, 0, 255});
            screen.drawTriangles(triangles, color);
            screen.drawEdges(edges, color);
            screen.toFileExtension(line);
        }
        else if (line == "circle")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            double r;
            iss >> v >> r;
            edges.addCircle(v, r, 100);
        }
        else if (line == "hermite")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            double x0, y0, x1, y1, rx0, ry0, rx1, ry1;
            iss >> x0 >> y0 >> x1 >> y1 >> rx0 >> ry0 >> rx1 >> ry1;
            edges.addCurve(x0, y0, x1, y1, rx0, ry0, rx1, ry1, 100, CurveType::Hermite);
        }
        else if (line == "bezier")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            double x0, y0, x1, y1, x2, y2, x3, y3;
            iss >> x0 >> y0 >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
            edges.addCurve(x0, y0, x1, y1, x2, y2, x3, y3, 100, CurveType::Bezier);
            //edges.addCurve(x0, y0, x3, y3, 3*(x1-x0), 3*(y1-y0), 3*(x3-x2), 3*(y3-y2), 50, CurveType::Hermite);
        }
        else if (line == "box")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v, dims;
            iss >> v >> dims;
            triangles.addBox(v, dims);
        }
        else if (line == "sphere")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            double r;
            iss >> v >> r;

            triangles.addSphere(v, r, 30, 15);
        }
        else if (line == "torus")
        {
            std::getline(infile, line);
            std::istringstream iss(line);

            Vec4 v;
            double r1, r2;
            iss >> v >> r1 >> r2;

            triangles.addTorus(v, r1, r2, 30, 12);
        }
        else if (line == "clear")
        {
            edges.clear();
            triangles.clear();
        }
        else
        {
            std::cout << "Unrecognized command!" << std::endl;
        }
    }
}

int main()
{
    Screen screen(500, 500);
    Mat4 edges(0);
    Mat4 triangles(0);
    Mat4 transform = Mat4::identity();

    parse("script_1", screen, edges, triangles, transform, {255, 127, 127, 255});

    return 0;
}
