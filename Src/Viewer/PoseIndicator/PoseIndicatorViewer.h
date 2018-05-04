#ifndef __POSEINDICATORVIEWER_H__
#define __POSEINDICATORVIEWER_H__

//Qt
#include <QWidget>

//vtk
#include <vtkSmartPointer.h>
class vtkMatrix4x4;
class vtkRenderWindowInteractor;
class vtkRenderer;
class vtkGenericOpenGLRenderWindow;
class vtkInteractorStyle;
class QVTKInteractor;

//me
class PositionIndicator;
class DepthIndicatorRepresentation;
class DepthIndicatorWidget;

//me
namespace Ui { class PoseIndicatorViewer; }

class PoseIndicatorViewer : public QWidget
{
	Q_OBJECT
public:
	explicit PoseIndicatorViewer(double depth, double remindingDepth, 
		double warningDepth, double nailLength, QWidget* parent = NULL);
	explicit PoseIndicatorViewer(QWidget* parent = NULL);
	~PoseIndicatorViewer();

	void setDepth(double depth);
	void setRemindingDepth(double depth);
	void setWarningDepth(double depth);
	void setNailLength(double length);

public slots:
	void setTargetPose(vtkMatrix4x4* mat);
    void setTrackerPosition(vtkMatrix4x4*);
	void vtkRender();

private:
	PositionIndicator* m_PIViewer;
	DepthIndicatorRepresentation* m_Representation;
	DepthIndicatorWidget* m_DIWidget;
	vtkSmartPointer<vtkRenderer> m_renderer;
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
	QVTKInteractor* m_interactor;
	vtkSmartPointer<vtkInteractorStyle> m_interactorStyle;

	//vtkSmartPointer<QVTKInteractor> m_interactor;

	double m_targetPosition[3] = { 0,0,0 };
	double m_targetOrientation[3] = { 0,0,1 };

	double m_depth;
	double m_remindingDepth;
	double m_warningDepth;
	double m_nailLength;

	Ui::PoseIndicatorViewer* m_ui;
};

#endif //