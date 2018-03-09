#pragma once
#include <vector>
#include "Config.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// define 4 2D control points
#define N 4 // number of control points
#define D 3 // number of dimensions
#define T 100 // number of u subintervals

// Key Frame Positions
static GLfloat ctrlPoints[N][3] = 
								{ 
									{ 1.297, 1.669, -0.0001},
									{ -3.5764, 1.4332, 0.3310},
									{ -0.7525, 0.1254, 0.331},
									{ 1.752, -2.508, 1.331}
								};

GLUnurbsObj *theNurb;

int typeMode = CONFIG.SPLINE_CONFIG;

std::vector<glm::vec3> points_to_travel;

void interpolate()
{
	printf("\nCalculating Interpolating Form...\n");

	GLfloat bu[N];

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
		points_to_travel.push_back(glm::vec3(x, y, z));
	}
}

void hermite()
{
	// Calculate Hermite form with

	printf("\nCalculating Hermite Form with specific endpoint tangents...\n");

	GLfloat fu[N];

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
}

GLfloat bernstein(GLfloat u, int i, int n)
{
	GLfloat choose[N] = { 1, 3, 3, 1 };
	GLfloat nChoosei = choose[i];
	GLfloat ui = pow(u, i);
	GLfloat oneMinusu = pow(1.0 - u, n - i);

	return (nChoosei*ui*oneMinusu);
}

void bezier()
{
	printf("\nCalculating Bezier Form using Bernstein polynomials...\n");


	for (int uInt = 0; uInt <= T; uInt++)
	{
		GLfloat u = uInt / (GLfloat)T;

		GLfloat x = 0.0;
		GLfloat y = 0.0;
		GLfloat z = 0.0;

		for (int i = 0; i < N; i++)
		{
			GLfloat b = bernstein(u, i, N - 1);
			x += b * ctrlPoints[i][0];
			y += b * ctrlPoints[i][1];
			z += b * ctrlPoints[i][2];
		}
		printf("x=%g  y=%g z=%g\n", x, y, z);
		points_to_travel.push_back(glm::vec3(x, y, z));
	}
}

void bSpline()
{
	printf("\nCalculating uniform B-Spline Form...\n");

	GLfloat fu[N];

	glBegin(GL_LINE_STRIP);

	for (int uInt = 0; uInt <= T; uInt++)
	{
		GLfloat u = uInt / (GLfloat)T;
		GLfloat u2 = u * u;
		GLfloat u3 = u2 * u;
		fu[0] = -u3 / 6.0 + u2 / 2.0 - u / 2.0 + 1.0 / 6.0;
		fu[1] = u3 / 2.0 - u2 + 2.0 / 3.0;
		fu[2] = -u3 / 2.0 + u2 / 2.0 + u / 2.0 + 1.0 / 6.0;
		fu[3] = u3 / 6.0;

		GLfloat x = 0.0;
		GLfloat y = 0.0;
		GLfloat z = 0.0;

		for (int i = 0; i < N; i++)
		{
			x += fu[i] * ctrlPoints[i][0];
			y += fu[i] * ctrlPoints[i][1];
			z += fu[i] * ctrlPoints[i][2];
		}
		printf("x=%g  y=%g z%g\n", x, y, z);
		points_to_travel.push_back(glm::vec3(x, y, z));
	}
}

void drawCurve()
{
	switch (typeMode)
	{
		case 1:
			interpolate();
			break;
		case 2:
			hermite();
			break;
		case 3:
			bezier();
			break;
		case 4:
			bSpline();
			break;
		default:
			exit(0);
	}

}

void determine_control_points()
{
	drawCurve();
}