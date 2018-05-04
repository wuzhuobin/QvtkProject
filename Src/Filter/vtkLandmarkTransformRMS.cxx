
#include "vtkLandmarkTransformRMS.h"
#include <vtkPoints.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkLandmarkTransformRMS);

vtkLandmarkTransformRMS::vtkLandmarkTransformRMS() {

}

vtkLandmarkTransformRMS::~vtkLandmarkTransformRMS() {

}

void vtkLandmarkTransformRMS::PrintSelf(ostream & os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << "RMS: " << this->RMS << '\n';

}

void  vtkLandmarkTransformRMS::Calculate_RMS() {

	//cout << "Fuction Calculate_RMS() is called" << endl;
	int n = this->SourceLandmarks->GetNumberOfPoints();//number of points
	//cout << "Number of points for calculating the RMS = " << n << endl;
	double s[4];
	double t[4];
	double sourcePoints_2[4] = { 0 };
	double targetPoints_2[4] = { 0 };
	double out[4];
	double dist = 0;


	for (int i = 0; i < n; i++) {

		this->SourceLandmarks->GetPoint(i, s);
		this->TargetLandmarks->GetPoint(i, t);

		sourcePoints_2[0] = s[0];
		sourcePoints_2[1] = s[1];
		sourcePoints_2[2] = s[2];
		sourcePoints_2[3] = 1;
		targetPoints_2[0] = t[0];
		targetPoints_2[1] = t[1];
		targetPoints_2[2] = t[2];
		targetPoints_2[3] = 1;
		//v[3] to v[4]
		//std::cout << "sourcePoints_2 " << i << " : (" << sourcePoints_2[0] << " " << sourcePoints_2[1] << " " << sourcePoints_2[2] << " " << sourcePoints_2[3] << ")" << std::endl;
		//std::cout << "targetPoints_2 " << i << " : (" << targetPoints_2[0] << " " << targetPoints_2[1] << " " << targetPoints_2[2] << " " << targetPoints_2[3] << ")" << std::endl;


		// compute the points after multiply the transform matrix
		this->Matrix->MultiplyPoint(sourcePoints_2, out);

		//std::cout << "outPoints " << i << " : (" << out[0] << " " << out[1] << " " << out[2] << " " << out[3] << ")" << std::endl;


		for (int j = 0; j < 4; j++) {
			double tmp = out[j] - targetPoints_2[j];
			dist += tmp * tmp;
		}
		dist /= n;

	}
	//calculate RMS
	RMS = sqrt(dist);
	//cout << "The result of RMS is: " << RMS << endl;
}
