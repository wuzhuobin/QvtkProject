#ifndef DepthIndicatorRepresentation_h
#define DepthIndicatorRepresentation_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkBorderRepresentation.h"
#include "vtkSmartPointer.h"
#include "vtkVectorText.h"

class vtkActor2D;
class vtkPoints;
class vtkPolyData;
class vtkProperty2D;
class vtkUnsignedCharArray;
class vtkTextActor;


class  DepthIndicatorRepresentation : public vtkBorderRepresentation
{
public:
	static DepthIndicatorRepresentation *New();

	vtkTypeMacro(DepthIndicatorRepresentation, vtkBorderRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent);
	//@{
	/**
	* By obtaining this property you can specify the properties of the
	* representation.
	*/
	vtkGetObjectMacro(m_Property, vtkProperty2D);
	//@}

	//@{
	/**
	* Set/Get the progress rate of the progress bar, between 0 and 1
	* default is 0
	*/
	vtkSetClampMacro(m_ProgressRate, double, 0, 1);
	vtkGetMacro(m_ProgressRate, double);
	//@}

	//@{
	/**
	* Set/Get the progress bar color
	* Default is pure green
	*/
	vtkSetVector3Macro(m_ProgressBarColor, double);
	vtkGetVector3Macro(m_ProgressBarColor, double);
	//@}

	//@{
	/**
	* Set/Get the background color
	* Default is white
	*/
	vtkSetVector3Macro(m_BackgroundColor, double);
	vtkGetVector3Macro(m_BackgroundColor, double);
	//@}

	//@{
	/**
	* Set/Get background visibility
	* Default is off
	*/
	vtkSetMacro(m_DrawBackground, bool);
	vtkGetMacro(m_DrawBackground, bool);
	vtkBooleanMacro(m_DrawBackground, bool);
	//@}

	//@{
	/**
	* Satisfy the superclasses' API.
	*/
	virtual void BuildRepresentation();
	virtual void GetSize(double size[2]);
	//@}

	//@{
	/**
	* These methods are necessary to make this representation behave as
	* a vtkProp.
	*/
	virtual void GetActors2D(vtkPropCollection*);
	virtual void ReleaseGraphicsResources(vtkWindow*);
	virtual int RenderOverlay(vtkViewport*);
	virtual int RenderOpaqueGeometry(vtkViewport*);
	virtual int RenderTranslucentPolygonalGeometry(vtkViewport*);
	virtual int HasTranslucentPolygonalGeometry();
	//@}

	void SetFinalDepth(double depth);
	void SetCurrentDepth(double depth);
	void SetRemindingDepth(double depth);
	void SetWarningDepth(double depth);




protected:
	DepthIndicatorRepresentation();
	~DepthIndicatorRepresentation();

	double m_ProgressRate;
	double m_ProgressBarColor[3];
	double m_BackgroundColor[3];

	double m_FinalDepth;
	double m_CurrentDepth;
	double m_RemindingDepth;
	double m_WarningDepth;

	bool m_DrawBackground;


	vtkSmartPointer<vtkPoints>      m_Points;
	vtkSmartPointer<vtkPoints>      m_LinePoints;
	vtkSmartPointer<vtkUnsignedCharArray>  m_BackgroundData;
	vtkSmartPointer<vtkUnsignedCharArray>  m_ProgressBarData;
	vtkProperty2D  *m_Property;
	vtkSmartPointer<vtkActor2D>     m_Actor;
	vtkSmartPointer<vtkActor2D>     m_BackgroundActor;

	vtkSmartPointer<vtkActor2D> m_LineActor;
	vtkSmartPointer<vtkActor2D>  m_TitleTextActor;

	vtkSmartPointer<vtkActor2D> m_FinalTextActor;
	vtkSmartPointer<vtkActor2D> m_CurrentTextActor;
	vtkSmartPointer<vtkActor2D> m_RemindingTextActor;
	vtkSmartPointer<vtkActor2D> m_ZeroTextActor;
	vtkSmartPointer<vtkActor2D> m_WarningTextActor;



private:
	DepthIndicatorRepresentation(const DepthIndicatorRepresentation&) ;
	void operator=(const DepthIndicatorRepresentation&) ;

};



#endif