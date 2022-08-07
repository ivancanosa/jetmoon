#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "core/definitions.hpp"

struct SplineCoefficients{
	float a, b, c, d;

	glm::vec4 getGLM(){
		return {a, b, c, d};
	}
};

//Range of [0., points.size()-1]
struct Spline2D{
	std::vector<Vec2> points;
	std::vector<SplineCoefficients> coeficientsX;
	std::vector<SplineCoefficients> coeficientsY;

	Eigen::MatrixXf A;
	Eigen::VectorXf B;
	Eigen::VectorXf MX;
	Eigen::VectorXf MY;
	Eigen::ConjugateGradient<Eigen::MatrixXf> solver;

	Vec2 eval(float t){
		int size = coeficientsX.size()-1;
		int piece = std::min((int)std::floor(t), size);
		auto[ax, bx, cx, dx] = coeficientsX[piece];
		auto[ay, by, cy, dy] = coeficientsY[piece];
		return {ax*t*t*t + bx*t*t + cx*t + dx,
			    ay*t*t*t + by*t*t + cy*t + dy};
	}

	float getArcLenght(int piece, int segments=10){
		Vec2 p0 = eval(piece);
		Vec2 p1;
		float length = 0;
		for(int i=1; i<=segments; i++){
			float value = piece + (float)i/segments;
			p1 = eval(value);
			length += std::sqrt((p1.x-p0.x)*(p1.x-p0.x) + (p1.y-p0.y)*(p1.y-p0.y));
			p0 = p1;
		}
		return length;
	}

	void generate(){
		coeficientsX.clear();
		coeficientsY.clear();
		float h = 1.;
		auto fX = [&](int p0, int p1, int p2){
			return (points[p2].x - points[p1].x) / ((h)*(2*h)) -
				(points[p1].x - points[p0].x) / ((h)*(2*h));
		};
		auto fY = [&](int p0, int p1, int p2){
			return (points[p2].y - points[p1].y) / ((h)*(2*h)) -
				(points[p1].y - points[p0].y) / ((h)*(2*h));
		};
		auto dX = [&](int i){
			return 6 * fX(i-1, i, i+1);
		};
		auto dY = [&](int i){
			return 6 * fY(i-1, i, i+1);
		};
		int n = points.size();

		A.resize(n, n);
		B.resize(n);
		MX.resize(n);
		MY.resize(n);

		for(int i=0; i<n; i++){
			B(i) = 0.;
			for(int j=0; j<n; j++){
				A(i, j) = 0.;
			}
		}

		A(0,0) = 2;
		A(0,1) = 0;
		B(0) = 0;
		for(int i=1; i < n-1; i++){
			A(i, i-1) = h/(h+h);
			A(i, i) = 2;
			A(i, i+1) = h/(h+h);
			B(i) = dX(i);
		}
		A(n-1, n-2) = 0;
		A(n-1, n-1) = 2;
		B(n-1) = 0;
		solver.compute(A);
		MX = solver.solve(B);
		for(int i=1; i < n-1; i++){
			B(i) = dY(i);
		}
		MY = solver.solve(B);

		for(int i=1; i<n; i++){
			float M_0 = MX(i-1), M_1=MX(i), x_0=i-1, x_1=i, y_0=points[i-1].x, y_1=points[i].x, g=h;
			float a, b, c, d;
			a = (M_1/(6*g) - M_0/(6*g));
			b = ((M_0*x_1)/(2*g) - (M_1*x_0)/(2*g));
			c = -(y_0 - (g*g*M_0)/6)/g + (y_1 - (g*g*M_1)/6)/g - (M_0*x_1*x_1)/(2*g) + (M_1*x_0*x_0)/(2*g);
			d = (x_1*(y_0 - (g*g*M_0)/6))/g - (x_0*(y_1 - (g*g*M_1)/6))/g + (M_0*x_1*x_1*x_1)/(6*g) - (M_1*x_0*x_0*x_0)/(6*g);
			coeficientsX.push_back({a, b, c, d});
		}
		for(int i=1; i<n; i++){
			float M_0 = MY(i-1), M_1=MY(i), x_0=i-1, x_1=i, y_0=points[i-1].y, y_1=points[i].y, g=h;
			float a, b, c, d;
			a = (M_1/(6*g) - M_0/(6*g));
			b = ((M_0*x_1)/(2*g) - (M_1*x_0)/(2*g));
			c = -(y_0 - (g*g*M_0)/6)/g + (y_1 - (g*g*M_1)/6)/g - (M_0*x_1*x_1)/(2*g) + (M_1*x_0*x_0)/(2*g);
			d = (x_1*(y_0 - (g*g*M_0)/6))/g - (x_0*(y_1 - (g*g*M_1)/6))/g + (M_0*x_1*x_1*x_1)/(6*g) - (M_1*x_0*x_0*x_0)/(6*g);
			coeficientsY.push_back({a, b, c, d});
		}
	}
};
