// c
#include <stdio.h>

// me
#include "vtkAluminiumMarkerPolyDataExtractionFilter.h"

// vtk
#include <vtkNew.h>
#include <vtkNIFTIImageReader.h>
#include <vtkPolyDataWriter.h>


int main(int argc, char* argv[])
{
	
	vtkNew<vtkNIFTIImageReader> reader;
	reader->SetFileName("tooth.nii.gz");
	reader->Update();


	vtkNew<vtkAluminiumMarkerPolyDataExtractionFilter> filer;
	filer->SetInputConnection(reader->GetOutputPort());
	filer->SetTargetVolume(3134);
	filer->SetVolumeTolerance(1700);
	filer->Update();

	vtkNew<vtkPolyDataWriter> writer;
	writer->SetInputConnection(filer->GetOutputPort());
	writer->SetFileName("output.vtk");
	writer->Write();

	cout << "Finish";

	getchar();
	return 0;
}