// Taken from http://www.gamedev.net/reference/articles/article605.asp
// Written by Jeff Hill
#include "stdafx.h"

#include "math3d.h"
#include <cassert>


namespace Math3D {

// From code in Graphics Gems; p. 766




scalar_t Determinant (const Matrix4& m) {
	scalar_t a1 = m[0][0];	scalar_t a2 = m[1][0];	scalar_t a3 = m[2][0];	scalar_t a4 = m[3][0];
	scalar_t b1 = m[0][1];	scalar_t b2 = m[1][1];	scalar_t b3 = m[2][1];	scalar_t b4 = m[3][1];
	scalar_t c1 = m[0][2];	scalar_t c2 = m[1][2];	scalar_t c3 = m[2][2];	scalar_t c4 = m[3][2];
	scalar_t d1 = m[0][3];	scalar_t d2 = m[1][3];	scalar_t d3 = m[2][3];	scalar_t d4 = m[3][3];

	return 	a1 * det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4)
		- 	b1 * det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4)
		+	c1 * det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4)
		-	d1 * det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

Matrix4 Adjoint (const Matrix4& m) {
	scalar_t a1 = m[0][0];	scalar_t a2 = m[0][1];	scalar_t a3 = m[0][2];	scalar_t a4 = m[0][3];
	scalar_t b1 = m[1][0];	scalar_t b2 = m[1][1];	scalar_t b3 = m[1][2];	scalar_t b4 = m[1][3];
	scalar_t c1 = m[2][0];	scalar_t c2 = m[2][1];	scalar_t c3 = m[2][2];	scalar_t c4 = m[2][3];
	scalar_t d1 = m[3][0];	scalar_t d2 = m[3][1];	scalar_t d3 = m[3][2];	scalar_t d4 = m[3][3];

	// Adjoint(x,y) = -1^(x+y) * a(y,x)
	// Where a(i,j) is the 3x3 determinant of m with row i and col j removed
	Matrix4 retVal;
	retVal(0)[0] = det3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
	retVal(0)[1] = -det3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
	retVal(0)[2] = det3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
	retVal(0)[3] = -det3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

	retVal(1)[0] = -det3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
	retVal(1)[1] = det3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
	retVal(1)[2] = -det3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
	retVal(1)[3] = det3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

	retVal(2)[0] = det3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
	retVal(2)[1] = -det3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
	retVal(2)[2] = det3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
	retVal(2)[3] = -det3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

	retVal(3)[0] = -det3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
	retVal(3)[1] = det3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
	retVal(3)[2] = -det3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
	retVal(3)[3] = det3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3);

	return retVal;
}

Matrix4 Inverse (const Matrix4& m) {
	Matrix4 retVal = Adjoint(m);
	scalar_t det = Determinant(m);
	assert(det);

	for (int i=0;i<4;++i) {
		for (int j=0;j<4;++j) {
			retVal(i)[j] /= det;
		}
	}

	return retVal;
}
//http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/index.htm
void toMatrix(float correct[3][3], float roll, float yaw ,float pitch)
{
 	float cos_x = cos(roll*PI/180);
 	float cos_y = cos(yaw*PI/180);
 	float cos_z = cos(pitch*PI/180);

 	float sin_x = sin(roll*3.1416/180);
 	float sin_y = sin(yaw*3.1416/180);
 	float sin_z = sin(pitch*3.1416/180);
 	correct[0][0] =   cos_z * cos_y;
 	correct[0][1] =   sin_z;
 	correct[0][2] =   -cos_z * sin_y;

 	correct[1][0] =   -sin_z * cos_y * cos_x + sin_x * sin_y;
 	correct[1][1] =   cos_z * cos_x;
 	correct[1][2] =   sin_z * sin_y * cos_x + sin_x * cos_y;

 	correct[2][0] =   sin_z * cos_y * sin_x + cos_x * sin_y;
 	correct[2][1] =   -cos_z * sin_x;
 	correct[2][2] =   -sin_z * sin_y * sin_x + cos_y * cos_x;
}
//http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
void toEuler(float &roll, float &yaw, float &pitch, float correct[3][3])
{
	/*if (correct[0][1] > 1)
	{
		pitch = 90;
	}
	else if (correct[0][1] < -1)
	{
		pitch = -90;
	}
	else
	{
		pitch = asin(correct[0][1]) * 180 / PI;
		if (pitch >= 0)
			pitch = (float)((int)((pitch + 0.005 )*100))/100;
		else
			pitch = (float)((int)((pitch - 0.005 )*100))/100;
	}
	if (fabs(correct[0][1]) > 0.999)
	{
		roll = 0;
		yaw = atan2(correct[2][0], correct[2][2])* 180 / PI;
		if (yaw >= 0)
			yaw = (float)((int)((yaw + 0.005 )*100))/100;
		else
			yaw = (float)((int)((yaw - 0.005 )*100))/100;
	}
	else
	{
		yaw = -atan2(correct[0][2], correct[0][0])* 180 / PI;
		if (yaw >= 0)
			yaw = (float)((int)((yaw + 0.005 )*100))/100;
		else
			yaw = (float)((int)((yaw - 0.005 )*100))/100;
		roll = -atan2(correct[2][1], correct[1][1])* 180 / PI;
		if (roll >= 0)
			roll = (float)((int)((roll + 0.005 )*100))/100;
		else
			roll = (float)((int)((roll - 0.005 )*100))/100;
	}*/

	if (correct[0][1] > 0.9998) { // singularity at north pole
		yaw = atan2(correct[2][0],correct[2][2])/3.14159265*180;
		pitch = 90;
		roll = 0;
		return;
	}
	if (correct[0][1] < -0.9998) { // singularity at south pole
		yaw = atan2(correct[2][0],correct[2][2])/3.14159265*180;
		pitch = -90;
		roll = 0;
		return;
	}
	yaw = atan2(-correct[0][2],correct[0][0])/3.14159265*180;
	roll = atan2(-correct[2][1],correct[1][1])/3.14159265*180;
	pitch = asin(correct[0][1])/3.14159265*180;
}

void toEulerDouble(double &roll, double &yaw, double &pitch, double correct[3][3])
{
	if (correct[0][1] > 1)
	{
		pitch = 90;
	}
	else if (correct[0][1] < -1)
	{
		pitch = -90;
	}
	else
	{
		pitch = asin(correct[0][1]) * 180 / PI;
		if (pitch >= 0)
			pitch = (double)((int)((pitch + 0.005 )*100))/100;
		else
			pitch = (double)((int)((pitch - 0.005 )*100))/100;
	}
	if (fabs(correct[0][1]) > 0.999)
	{
		roll = 0;
		yaw = atan2(correct[2][0], correct[2][2])* 180 / PI;
		if (yaw >= 0)
			yaw = (double)((int)((yaw + 0.005 )*100))/100;
		else
			yaw = (double)((int)((yaw - 0.005 )*100))/100;
	}
	else
	{
		yaw = -atan2(correct[0][2], correct[0][0])* 180 / PI;
		if (yaw >= 0)
			yaw = (double)((int)((yaw + 0.005 )*100))/100;
		else
			yaw = (double)((int)((yaw - 0.005 )*100))/100;
		roll = -atan2(correct[2][1], correct[1][1])* 180 / PI;
		if (roll >= 0)
			roll = (double)((int)((roll + 0.005 )*100))/100;
		else
			roll = (double)((int)((roll - 0.005 )*100))/100;
	}

	//if (correct[0][1] > 0.998) { // singularity at north pole
	//	yaw = atan2(correct[2][0],correct[2][2])/3.14159265*180;
	//	pitch = 90;
	//	roll = 0;
	//	return;
	//}
	//if (correct[0][1] < -0.998) { // singularity at south pole
	//	yaw = atan2(correct[2][0],correct[2][2])/3.14159265*180;
	//	pitch = -90;
	//	roll = 0;
	//	return;
	//}
	//yaw = atan2(-correct[0][2],correct[0][0])/3.14159265*180;
	//roll = atan2(-correct[2][1],correct[1][1])/3.14159265*180;
	//pitch = asin(correct[0][1])/3.14159265*180;
}


#define CLAMP(x , min , max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : x))

void EulertoQuat(float quat[4], float rot_x, float rot_y ,float rot_z)
{
	float fCosHRoll = cos(rot_z * .5f);
	float fSinHRoll = sin(rot_z * .5f);
	float fCosHPitch = cos(rot_x * .5f);
	float fSinHPitch = sin(rot_x * .5f);
	float fCosHYaw = cos(rot_y * .5f);
	float fSinHYaw = sin(rot_y * .5f);
	float x, y, z, w;

	/// Cartesian coordinate System
	//w = fCosHRoll * fCosHPitch * fCosHYaw + fSinHRoll * fSinHPitch * fSinHYaw;
	//x = fSinHRoll * fCosHPitch * fCosHYaw - fCosHRoll * fSinHPitch * fSinHYaw;
	//y = fCosHRoll * fSinHPitch * fCosHYaw + fSinHRoll * fCosHPitch * fSinHYaw;
	//z = fCosHRoll * fCosHPitch * fSinHYaw - fSinHRoll * fSinHPitch * fCosHYaw;
	w = fCosHRoll * fCosHPitch * fCosHYaw + fSinHRoll * fSinHPitch * fSinHYaw;
	x = fCosHRoll * fSinHPitch * fCosHYaw + fSinHRoll * fCosHPitch * fSinHYaw;
	y = fCosHRoll * fCosHPitch * fSinHYaw - fSinHRoll * fSinHPitch * fCosHYaw;
	z = fSinHRoll * fCosHPitch * fCosHYaw - fCosHRoll * fSinHPitch * fSinHYaw;

	quat[0] = x, quat[1] = y, quat[2] = z, quat[3] = w;
}
void QuattoEuler(float &rot_x,float &rot_y, float &rot_z, float quat[4])
{
	/// Cartesian coordinate System 
	//ea.m_fRoll  = atan2(2 * (w * x + y * z) , 1 - 2 * (x * x + y * y));
	//ea.m_fPitch = asin(2 * (w * y - z * x));
	//ea.m_fYaw   = atan2(2 * (w * z + x * y) , 1 - 2 * (y * y + z * z));
	float x, y, z, w;
	x = quat[0], y = quat[1], z = quat[2], w = quat[3];

	rot_z  = atan2(2 * (w * z + x * y) , 1 - 2 * (z * z + x * x));
	rot_x = asin(CLAMP(2 * (w * x - y * z) , -1.0f , 1.0f));
	rot_y   = atan2(2 * (w * y + z * x) , 1 - 2 * (x * x + y * y));
}

void MatrixxMatrix(float result[3][3], float v1[3][3], float  v2[3][3])
{
	for (int i=0; i < 3;i++)
		for(int j=0; j< 3; j++)
		{
			result[i][j] = v1[i][0] * v2[0][j] + v1[i][1] * v2[1][j] + v1[i][2] * v2[2][j];
//			result[i][j] = (float)((int)((result[i][j] + 0.00005 )*10000))/10000;
		}
}
void MatrixxMatrixDouble(double result[3][3], double v1[3][3], double  v2[3][3])
{
	for (int i=0; i < 3;i++)
		for(int j=0; j< 3; j++)
		{
			result[i][j] = v1[i][0] * v2[0][j] + v1[i][1] * v2[1][j] + v1[i][2] * v2[2][j];
//			result[i][j] = (float)((int)((result[i][j] + 0.00005 )*10000))/10000;
		}
}
void MatrixInverse(float result[3][3], float v[3][3])
{
	for (int i=0; i < 3;i++)
		for(int j=0; j< 3; j++)
		{
			result[i][j] = v[j][i];
		}
}
void MatrixInverseDouble(double result[3][3], double v[3][3])
{
	for (int i=0; i < 3;i++)
		for(int j=0; j< 3; j++)
		{
			result[i][j] = v[j][i];
		}
}
}



//**************************************
// Testing from here on.
//**************************************
#ifdef TEST_MATH3D
#include <iostream>

using namespace Math3D;
using namespace std;

static int failures = 0;

void ReportFailure (const char* className, const char* testName, bool passed) {
	cout << className;
	if (passed)
		 cout << " passed test ";
	else {
		cout << " FAILED test ";
		++failures;
	}
	cout << testName << "." << endl;
}

const char* Vector4Name = "Vector4";
const char* Matrix4Name = "Matrix4";

void TestVector4Constructors(void) {
	// Default ctor... just make sure it compiles
	Vector4 defaultCtorTest;

	// Initializer ctor test (3 param)
	Vector4 initCtorTest(1, 2, 3);
	ReportFailure (Vector4Name, "initialized ctor (3 parameter version)", (initCtorTest[0] == 1 && initCtorTest[1] == 2 && initCtorTest[2] == 3 && initCtorTest[3] == 1));

	// Initializer ctor test (4 param)
	Vector4 initCtorTest2 (1,2,3,4);
	ReportFailure (Vector4Name, "initialized ctor (4 parameter version)", (initCtorTest2[0] == 1 && initCtorTest2[1] == 2 && initCtorTest2[2] == 3 && initCtorTest2[3] == 4));

	scalar_t initArray[] = { 1, 2, 3, 4};
	Vector4 initCtorArrayTest3 (initArray);
	ReportFailure (Vector4Name, "array initialized ctor (3 parameter version)", (initCtorArrayTest3[0] == 1 && initCtorArrayTest3[1] == 2 && initCtorArrayTest3[2] == 3 && initCtorArrayTest3[3] == 1));

	Vector4 initCtorArrayTest4 (initArray, 4);
	ReportFailure (Vector4Name, "array initialized ctor (4 parameter version)", (initCtorArrayTest4[0] == 1 && initCtorArrayTest4[1] == 2 && initCtorArrayTest4[2] == 3 && initCtorArrayTest4[3] == 4));

	// Copy ctor test
	Vector4 copyCtorTest(initCtorTest2);
	ReportFailure (Vector4Name, "copy ctor", (copyCtorTest[0] == 1 && copyCtorTest[1] == 2 && copyCtorTest[2] == 3 && copyCtorTest[3] == 4));
}

void TestVector4Comparison(void) {
	Vector4 alpha(1,1,1);
	Vector4 beta(alpha);
	Vector4 gamma(2,3,4);

	ReportFailure (Vector4Name, "equivalence operator test 1", (alpha==beta));
	ReportFailure (Vector4Name, "equivalence operator test 2", (!(alpha == gamma)));
	ReportFailure (Vector4Name, "comparison operator test 1", !(alpha < beta));
	ReportFailure (Vector4Name, "comparison operator test 2", (alpha < gamma));
	ReportFailure (Vector4Name, "comparison operator test 3", !(gamma < beta));
}

void TestVector4Assignment (void) {
	Vector4 alpha(1,1,1,1);
	Vector4 beta(10,10,10,10);
	alpha = beta;
	ReportFailure (Vector4Name, "assignment operator", (alpha == beta));
}

void TestVector4UnaryOps (void) {
	Vector4 alpha(10,10,10,10);
	Vector4 beta(-10,-10,-10,-10);
	alpha = -alpha;
	ReportFailure(Vector4Name, "negation operator", (alpha==beta));

	ReportFailure(Vector4Name, "length squared 3 element version", LengthSquared3(alpha) == 300);
	ReportFailure(Vector4Name, "length 3 element version", Length3(alpha) == SQRT_FUNCTION(300));
	ReportFailure(Vector4Name, "length squared 4 element version", LengthSquared4(alpha) == 400);
	ReportFailure(Vector4Name, "length 4 element version", Length4(alpha) == SQRT_FUNCTION(400));

	// Manually normalize beta
	// Done without /= on Vector4, as we want to be independant of failure of /=
	// Earlier failures should be resolved before later ones (just like C++)
	beta = alpha;
	for (int i=0;i<3;++i) beta(i) /= SQRT_FUNCTION(300);
	beta(3) = 1;
	ReportFailure(Vector4Name, "normalize 3 element version", Normalize3(alpha) == beta);

	beta = alpha;
	for (int i=0;i<4;++i) beta(i) /= SQRT_FUNCTION(400);
	ReportFailure(Vector4Name, "normalize 4 element version", Normalize4(alpha) == beta);
}

void TestVector4BinaryOps (void) {
	// Vector * Matrix is tested in TestMatrix4BinaryOps
	Vector4 testVec(1,1,1,1);
	Vector4 deltaVec(1,2,3,4);
	Vector4 crossVec(1, -2, 1, 1);	// testVec x deltaVec

	Vector4 factorVec(10,10,10,10);
	Vector4 sumVec(2,3,4,5);
	Vector4 difVec(0,-1,-2,-3);
	Vector4 testVec2;

	ReportFailure(Vector4Name, "scalar multiply 1", (testVec * 10) == factorVec);
	ReportFailure(Vector4Name, "scalar multiply 2", (10 * testVec) == factorVec);
	testVec2 = testVec;
	ReportFailure(Vector4Name, "scalar multiply and store", (testVec2 *= 10) == factorVec);

	ReportFailure(Vector4Name, "scalar divide", (factorVec / 10) == testVec);
	testVec2 = factorVec;
	ReportFailure(Vector4Name, "scalar divide and store", (testVec2 /= 10) == testVec);

	ReportFailure(Vector4Name, "vector addition", (testVec + deltaVec) == sumVec);
	testVec2 = testVec;
	ReportFailure(Vector4Name, "vector addition and store", (testVec2 += deltaVec) == sumVec);

	ReportFailure(Vector4Name, "vector subtraction", (testVec - deltaVec) == difVec);
	testVec2 = testVec;
	ReportFailure(Vector4Name, "vector subtraction and store", (testVec2 -= deltaVec) == difVec);

	ReportFailure(Vector4Name, "3 element dot product", 6 == DotProduct3(testVec, deltaVec));
	ReportFailure(Vector4Name, "4 element dot product", 10 == DotProduct4(testVec, deltaVec));

	ReportFailure(Vector4Name, "cross product", crossVec == CrossProduct(testVec, deltaVec));
}

void TestVector4 (void) {
	// Accessors cannot be tested effectively...
	// They are really trivial, and so don't really need testing,
	// but more importantly, how do you test the ctors without assuming
	// the accessors work?  Conversely, how do you test the acccessors
	// without assuming the ctors work?  Chicken and egg problem, and I
	// decided on testing the ctors, not the accessors.
	TestVector4Constructors();
	TestVector4Comparison();
	TestVector4Assignment();
	TestVector4UnaryOps();
	TestVector4BinaryOps();
}

void TestMatrix4Constructors (void) {
	// Check if default ctor compiles
	Matrix4 defaultTest;

	scalar_t initArray[] = { 	0, 1, 2, 3,
								4, 5, 6, 7,
								8, 9, 10, 11,
								12, 13, 14, 15 };
	Matrix4 arrayTest;
	arrayTest.C_Matrix(initArray);
	bool passedTest = true;
	for (int x=0;x<4;++x) for (int y=0;y<4;++y)
		if (arrayTest[x][y] != initArray[(y<<2) + x]) passedTest = false;
	ReportFailure(Matrix4Name, "array constructor", passedTest);

	Matrix4 copyTest(arrayTest);
	passedTest = true;
	for (int x=0;x<4;++x) for (int y=0;y<4;++y)
		if (arrayTest[x][y] != copyTest[x][y]) passedTest = false;
	ReportFailure(Matrix4Name, "copy constructor", passedTest);
}

void TestMatrix4Comparison (void) {
	scalar_t initArray[] = 	{	0, 1, 2, 3,
								4, 5, 6, 7,
								8, 9, 10, 11,
								12, 13, 14, 15 };
	scalar_t initArray2[] = {	1, 1, 1, 1,
								1, 1, 1, 1,
								1, 1, 1, 1,
								1, 1, 1, 1 	};
	Matrix4 alpha, beta, gamma;
	alpha.C_Matrix(initArray);
	beta.C_Matrix(initArray);
	gamma.C_Matrix(initArray2);

	ReportFailure(Matrix4Name, "equality test 1", alpha == beta);
	ReportFailure(Matrix4Name, "equality test 2", !(alpha == gamma));
	ReportFailure(Matrix4Name, "comparison test 1", alpha < gamma);
	ReportFailure(Matrix4Name, "comparison test 2", !(gamma < alpha));
	ReportFailure(Matrix4Name, "comparison test 3", !(alpha < beta));
}

void TestMatrix4BinaryOps (void) {
	scalar_t initVector[] = { 0, 1, 2, 3 };
	scalar_t initMatrix[] = { 	0, 1, 2, 3,
								4, 5, 6, 7,
								8, 9, 10, 11,
								12, 13, 14, 15 };
	scalar_t resultVector[] = { 0 * 0 + 1 * 1 + 2 * 2 + 3 * 3,
								0 * 4 + 1 * 5 + 2 * 6 + 3 * 7,
								0 * 8 + 1 * 9 + 2 * 10 + 3 * 11,
								0 * 12 + 1 * 13 + 2 * 14 + 3 * 15 };

	Vector4 vector1 (initVector, 4);
	Matrix4 matrix1;
	matrix1.C_Matrix(initMatrix);
	Vector4 vectorTest (resultVector, 4);
	ReportFailure (Matrix4Name, "matrix * vector", vectorTest == matrix1 * vector1);

	scalar_t initMatrix2[] = {	15, 14, 13, 12,
								11, 10, 9, 8,
								7, 6, 5, 4,
								3, 2, 1, 0 };

	Matrix4 matrix2;
	matrix2.C_Matrix(initMatrix2);
	Matrix4 resultMatrix;
	for (int x=0;x<4;++x) for (int y=0;y<4;++y) {
		resultMatrix(x)[y] = 0;
		for (int i=0;i<4;++i) resultMatrix(x)[y] += matrix1[i][y] * matrix2[x][i];
	}
	ReportFailure (Matrix4Name, "matrix * matrix", resultMatrix == matrix1 * matrix2);
}

void TestMatrix4 (void) {
	TestMatrix4Constructors();
	TestMatrix4Comparison();
	TestMatrix4BinaryOps();
}

int main (int, char*[]) {
	int vectorFailures = 0;
	int matrixFailures = 0;
	TestVector4();
	vectorFailures = failures;
	failures = 0;

	TestMatrix4();
	matrixFailures = failures;

	cout << endl << "****************************************" << endl;
	cout << "*                                      *" << endl;
	if (vectorFailures + matrixFailures == 0)
		cout <<     "*    No failures detected in Math3D    *" << endl;
	else {
		cout <<     "*     FAILURES DETECTED IN MATH3D!     *" << endl;
		cout <<     "*     Total Vector4 failures: " << vectorFailures << "        *" << endl;
		cout << 	"*     Total Matrix4 failures: " << matrixFailures << "        *" << endl;
		cout << 	"*   Total Failures in Math3D: " << vectorFailures + matrixFailures << "        *" << endl;

	}
	cout << "*                                      *" << endl;
	cout << "****************************************" << endl;

	return 0;
}
#endif
