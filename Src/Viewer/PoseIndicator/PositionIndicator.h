#ifndef POSITIONINDICATORVIEWER_H
#define POSITIONINDICATORVIEWER_H

//vtk
#include <vtkSmartPointer.h>
class vtkVectorText;
class vtkImageActor;
class vtkActor2D;
//class vtkPNGReader;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkInteractorStyleImage;


#include <QObject>


class PositionIndicator : public QObject
{
	Q_OBJECT
public:
	explicit PositionIndicator(QObject* parent = NULL);
	~PositionIndicator();

	void SetTargetPosition(double *);
	void SetCurrentPosition(double *);
	void SetTargetOrientation(double *);
	void SetCurrentOrientation(double *);

	void InitialPosition(double* targetPos, double* targetOri);
	void UpdatePosition(double* currentPos, double* currentOri);

//private:
	double m_targetPosition[3];
	double m_currentPosition[3];
	double m_targetOrientation[3];
	double m_currentOrientation[3];

	double m_drillOffset[3]= { 257,23,0 };
	double m_crossOffset[3]={ 258,259,0 };

	double m_scale = 20;        //20 piexls==1mm

	double m_drillDistance = 500;

	char m_titleText[100];

	inline void showArray(double *array, char * name);
	void CalculatePosition(double *input, double *output);


	vtkSmartPointer<vtkVectorText> m_text;

	//Actor
	vtkSmartPointer<vtkImageActor> m_drillActor;
	vtkSmartPointer<vtkImageActor> m_backgroundActor;
	vtkSmartPointer<vtkImageActor> m_crossActor;
	vtkSmartPointer<vtkActor2D> m_textActor;


	//Image Reader
	//vtkSmartPointer<vtkPNGReader> m_drillReader;
	//vtkSmartPointer<vtkPNGReader> m_backgroundReader;
	//vtkSmartPointer<vtkPNGReader> m_crossReader;

	//Renderer
	//vtkSmartPointer<vtkRenderer> m_renderer;
	//vtkSmartPointer<vtkRenderWindow> m_renderWindow;
	//vtkSmartPointer<vtkRenderWindowInteractor> m_interactor;
	//vtkSmartPointer<vtkInteractorStyleImage> m_style;

public slots:
	void setPositionChanged(double*);

signals:
	void positionChanged(double*);
};


#endif