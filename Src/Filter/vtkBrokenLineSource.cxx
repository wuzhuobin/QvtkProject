// me
#include "vtkBrokenLineSource.h"
// vtk
#include <vtkObjectFactory.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
vtkStandardNewMacro(vtkBrokenLineSource);
void vtkBrokenLineSource::PrintSelf(ostream & os, vtkIndent indent)
{
	Superclass::PrintSelf(os, indent);
	os << indent << "Solid: " << this->Solid << "\n";
	os << indent << "Empty: " << this->Empty << "\n";
}

vtkBrokenLineSource::vtkBrokenLineSource()
{
	this->Solid = 0;
	this->Empty = 0;
}

vtkBrokenLineSource::~vtkBrokenLineSource()
{
}

int vtkBrokenLineSource::RequestData(vtkInformation * info, vtkInformationVector ** input, vtkInformationVector * outputVector)
{
	// Reject meaningless parameterizations
	vtkIdType nSegments = this->Points ? this->Points->GetNumberOfPoints() - 1 : 1;
	if (nSegments < 1)
	{
		vtkWarningMacro(<< "Cannot define a broken line with given input.");
		return 0;
	}

	// get the info object
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// get the ouptut
	vtkPolyData *output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()) > 0)
	{
		return 1;
	}

	// Create and allocate lines
	vtkIdType numLines = nSegments * this->Resolution;
	vtkCellArray *newLines = vtkCellArray::New();
	newLines->Allocate(newLines->EstimateSize(numLines, 2));

	// Create and allocate points
	vtkIdType numPts = numLines + 1;
	vtkPoints *newPoints = vtkPoints::New();

	// Set the desired precision for the points in the output.
	if (this->OutputPointsPrecision == vtkAlgorithm::DOUBLE_PRECISION)
	{
		newPoints->SetDataType(VTK_DOUBLE);
	}
	else
	{
		newPoints->SetDataType(VTK_FLOAT);
	}

	newPoints->Allocate(numPts);

	// Create and allocate texture coordinates
	vtkFloatArray *newTCoords = vtkFloatArray::New();
	newTCoords->SetNumberOfComponents(2);
	newTCoords->Allocate(2 * numPts);
	newTCoords->SetName("Texture Coordinates");

	// Allocate convenience storage
	double x[3], tc[3], v[3];

	// Generate points and texture coordinates
	if (this->Points)
	{
		// Create storage for segment endpoints
		double point1[3];
		double point2[3];

		// Point index offset for fast insertion
		vtkIdType offset = 0;

		// Iterate over segments
		for (vtkIdType s = 0; s < nSegments; ++s)
		{
			// Get coordinates of endpoints
			this->Points->GetPoint(s, point1);
			this->Points->GetPoint(s + 1, point2);

			// Calculate segment vector
			for (int i = 0; i < 3; ++i)
			{
				v[i] = point2[i] - point1[i];
			}

			// Generate points along segment
			tc[1] = 0.;
			tc[2] = 0.;
			for (vtkIdType i = 0; i < this->Resolution; ++i, ++offset)
			{
				tc[0] = static_cast<double>(i) / this->Resolution;
				for (int j = 0; j < 3; ++j)
				{
					x[j] = point1[j] + tc[0] * v[j];
				}
				newPoints->InsertPoint(offset, x);
				newTCoords->InsertTuple(offset, tc);
			}
		} // s

		  // Generate last endpoint
		newPoints->InsertPoint(numLines, point2);
		tc[0] = 1.;
		newTCoords->InsertTuple(numLines, tc);

	} // if ( this->Points )
	else
	{
		// Calculate segment vector
		for (int i = 0; i < 3; ++i)
		{
			v[i] = this->Point2[i] - this->Point1[i];
		}

		// Generate points along segment
		tc[1] = 0.;
		tc[2] = 0.;
		for (vtkIdType i = 0; i < numPts; ++i)
		{
			tc[0] = static_cast<double>(i) / this->Resolution;
			for (int j = 0; j < 3; ++j)
			{
				x[j] = this->Point1[j] + tc[0] * v[j];
			}
			newPoints->InsertPoint(i, x);
			newTCoords->InsertTuple(i, tc);
		}
	} // else

	//  Generate lines
	if (0 == this->Solid + this->Empty || 
		numPts <= this->Solid ) {
		newLines->InsertNextCell(numPts);
		for (vtkIdType i = 0; i < numPts; ++i)
		{
			newLines->InsertCellPoint(i);
		}
	}
	else {
		// making multiple cell lines. 
		int step = this->Solid + this->Empty;
		for (vtkIdType i = 0; i < numPts; ++i) {
			unsigned int remainder = i % step;
			if (remainder == 0) {
				unsigned int difference = numPts - step * newLines->GetNumberOfCells();
				if (difference >= this->Solid) {
					newLines->InsertNextCell(this->Solid);
				}
				else
				{
					newLines->InsertNextCell(difference);
				}
			}
			if (remainder < this->Solid) {
				newLines->InsertCellPoint(i);
			}
		}
	}


	// Update ourselves and release memory
	output->SetPoints(newPoints);
	newPoints->Delete();

	output->GetPointData()->SetTCoords(newTCoords);
	newTCoords->Delete();

	output->SetLines(newLines);
	newLines->Delete();

	return 1;

}
