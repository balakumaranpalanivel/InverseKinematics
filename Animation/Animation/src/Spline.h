#pragma once
#include <vector>

// define 4 2D control points

#define N 4 // number of control points
#define D 3 // number of dimensions
#define T 100 // number of u subintervals

//static GLfloat ctrlPoints[N][3] = { { 100, 100, 1 },{ 200, 350, 2 },{ 400,300, 1 },{ 480,100, 1} };
//static GLfloat ctrlPoints[N][3] = { { 1, 1, 1 },{ 2, 2, 2 },{ 3, 3, 1 },{ 2,2, 1 } };
static GLfloat ctrlPoints[N][3] = { { 1.297, 1.669, -0.0001},
									{ -3.5764, 1.4332, 0.3310},
									{ -0.7525, 0.1254, 0.331},
									{ -0.752, -1.508, 1.331} };

GLUnurbsObj *theNurb;

int typeMode = 2;	 // 1 = control polyline
					 // 2 = interpolating cubic 
					 // 3 = hermite cubic (with specific endpoint tangents)
					 // 4 = bezier cubic (Bernstein)
					 // 5 = bezier cubic (OpenGL)
					 // 6 = uniform (nonrational) B-spline cubic
					 // 7 = NURBS cubic
					 // 8 = exit

					 // window size
int ww = 1000;
int wh = 800;

// remember the moving control name
int MOVENAME = -1;

// set up pick radius for detecting movement of a control point
int pickRadius = 50;

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::vector<glm::vec3> points_to_travel;

void display2DControlPolyline()
{
	glLineWidth(2.0);
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < N; i++)
	{
		glVertex3i(ctrlPoints[i][0], ctrlPoints[i][1], ctrlPoints[i][2]);
	}
	glEnd();
	glFlush();
}

void display2DControlPoints()
{
	glPointSize(3.0);
	glColor3f(0.0f, 1.0f, 0.0f);

	glBegin(GL_POINTS);
	for (int i = 0; i < N; i++)
	{
		glVertex3i(ctrlPoints[i][0], ctrlPoints[i][1], ctrlPoints[i][2]);
	}
	glEnd();
	glFlush();
}

void interpolate()
{
	printf("\nCalculating Interpolating Form...\n");

	//glLineWidth(4.0);
	//glColor3f(1.0f, 1.0f, 1.0f);

	GLfloat bu[N];

	//glBegin(GL_LINE_STRIP);

	for (int uInt = 0; uInt <= T; uInt++)
	{
		GLfloat u = uInt / (GLfloat)T;
		bu[0] = (-9.0 / 2.0)*(u - (1.0 / 3.0))*(u - (2.0 / 3.0))*(u - 1.0);
		bu[1] = (27.0 / 2.0)*u*(u - (2.0 / 3.0))*(u - 1.0);
		bu[2] = (-27.0 / 2.0)*u*(u - (1.0 / 3.0))*(u - 1.0);
		bu[3] = (9.0 / 2.0)*u*(u - (1.0 / 3.0))*(u - (2.0 / 3.0));

		GLfloat x = 0.0;
		GLfloat y = 0.0;
		GLfloat z = 0.0;

		for (int i = 0; i < N; i++)
		{
			x += bu[i] * ctrlPoints[i][0];
			y += bu[i] * ctrlPoints[i][1];
			z += bu[i] * ctrlPoints[i][2];
		}
		printf("x=%g  y=%g z=%g\n", x, y, z);

		//glVertex3i(x, y, z);
		points_to_travel.push_back(glm::vec3(x, y, z));
	}

	//glEnd();
	//glFlush();
}

void hermite()
{
	// Calculate Hermite form with
	//	p0 = ctrlPoints[0]
	//  p1 = ctrlPoints[3]
	//  tangent "weights" chosen to show that curve can exit control polygon's convex hull
	//  tangent at p0 = 3*(ctrlPoints[1]-ctrlPoints[0])
	//  tangent at p1 = 6*(ctrlPoints[3]-ctrlPoints[2])

	printf("\nCalculating Hermite Form with specific endpoint tangents...\n");

	//glLineWidth(4.0);
	//glColor3f(1.0f, 1.0f, 1.0f);

	GLfloat fu[N];

	//glBegin(GL_LINE_STRIP);

	for (int uInt = 0; uInt <= T; uInt++)
	{
		GLfloat u = uInt / (GLfloat)T;
		GLfloat u2 = u * u;
		GLfloat u3 = u2 * u;
		fu[0] = 2.0*u3 - 3.0*u2 + 1.0;
		fu[1] = -2.0*u3 + 3.0*u2;
		fu[2] = u3 - 2.0*u2 + u;
		fu[3] = u3 - u2;

		GLfloat x = 0.0;
		GLfloat y = 0.0;
		GLfloat z = 0.0;

		// p0 = ctrlPoints[0]
		x += fu[0] * ctrlPoints[0][0];
		y += fu[0] * ctrlPoints[0][1];
		z += fu[0] * ctrlPoints[0][2];

		// p1 = ctrlPoints[3]
		x += fu[1] * ctrlPoints[3][0];
		y += fu[1] * ctrlPoints[3][1];
		z += fu[1] * ctrlPoints[3][2];

		//  tangent at p0 = ctrlPoints[1]-ctrlPoints[0]
		x += fu[2] * (3.0*(ctrlPoints[1][0] - ctrlPoints[0][0]));
		y += fu[2] * (3.0*(ctrlPoints[1][1] - ctrlPoints[0][1]));
		z += fu[2] * (3.0*(ctrlPoints[1][2] - ctrlPoints[0][2]));

		//  tangent at p1 = ctrlPoints[3]-ctrlPoints[2]
		x += fu[3] * (6.0*(ctrlPoints[3][0] - ctrlPoints[2][0]));
		y += fu[3] * (6.0*(ctrlPoints[3][1] - ctrlPoints[2][1]));
		z += fu[3] * (6.0*(ctrlPoints[3][2] - ctrlPoints[2][2]));

		printf("x=%g  y=%g z=%g\n", x, y, z);
		points_to_travel.push_back(glm::vec3(x, y, z));

	}

	//glEnd();
	//glFlush();
}

void drawCurve()
{
	switch (typeMode) {
	case 1:
		display2DControlPolyline();
		display2DControlPoints();
		break;
	case 2:
		//display2DControlPolyline();
		//display2DControlPoints();
		interpolate();
		break;
	case 3:
		hermite();
		break;
	case 4:
		//bezier();
		break;
	case 5:
		//bezierOpenGL();
		break;
	case 6:
		//bSpline();
		break;
	case 7:
		//NURBS();
		break;
	default:
		exit(0);
	}

}

void determine_control_points()
{
	//display2DControlPolyline();
	//display2DControlPoints();
	drawCurve();
}