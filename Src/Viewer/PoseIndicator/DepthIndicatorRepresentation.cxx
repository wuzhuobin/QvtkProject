#include "DepthIndicatorRepresentation.h"

#include "vtkActor2D.h"
#include "vtkCellArray.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataMapper.h"
#include "vtkPropCollection.h"
#include "vtkProperty2D.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkUnsignedCharArray.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "vtkRenderer.h"


const double PROGRESS_BAR_WIDTH = 1.5;
const double PROGRESS_BAR_HEIGHT = 20;

vtkStandardNewMacro(DepthIndicatorRepresentation);

DepthIndicatorRepresentation::DepthIndicatorRepresentation()
{
	//Set default depth------------------------------------------------------

	m_CurrentDepth = 0;
	m_RemindingDepth = 2;
	m_FinalDepth = 11;
	m_WarningDepth = 2;

	m_ProgressRate = 1 - (m_CurrentDepth-m_WarningDepth) / m_WarningDepth;

	//Set bar color yellow-----------------------------------------------------
	m_ProgressBarColor[0] = 1;
	m_ProgressBarColor[1] = 1;
	m_ProgressBarColor[2] = 0;

	m_BackgroundColor[0] = 1;
	m_BackgroundColor[1] = 1;
	m_BackgroundColor[2] = 1;
	m_DrawBackground = false;

	double barlen = m_WarningDepth + m_FinalDepth;

	double currentPoint = 0.2 + ((barlen - m_CurrentDepth) / barlen) * (PROGRESS_BAR_HEIGHT - 0.2);
	double remindingPoint = 0.2 + ((m_WarningDepth+m_RemindingDepth) / barlen)*(PROGRESS_BAR_HEIGHT - 0.2);
	double finalPoint= 0.2 + ((barlen - m_FinalDepth) / barlen)*(PROGRESS_BAR_HEIGHT - 0.2);


	// Set up the geometry-----------------------------------------------------
	double size[2];
	this->GetSize(size);
	this->Position2Coordinate->SetValue(0.04*size[0], 0.04*size[1]);
	this->ProportionalResizeOff();
	this->Moving = 1;
	this->SetShowBorder(vtkBorderRepresentation::BORDER_OFF);

	// Create the geometry in canonical coordinates--------------------------
	m_Points = vtkSmartPointer<vtkPoints>::New();
	m_Points->SetDataTypeToDouble();
	m_Points->SetNumberOfPoints(8);
	m_Points->SetPoint(0, 0.2, 0.2, 0.0);
	m_Points->SetPoint(1, 0.2, PROGRESS_BAR_HEIGHT, 0.0);
	m_Points->SetPoint(2, PROGRESS_BAR_WIDTH,
		PROGRESS_BAR_HEIGHT, 0.0);
	m_Points->SetPoint(3, PROGRESS_BAR_WIDTH, 0.2, 0.0);



	//Progress Bar 
	m_Points->SetPoint(4, 0.2, currentPoint, 0.0);
	m_Points->SetPoint(5, 0.2, PROGRESS_BAR_HEIGHT, 0.0);
	m_Points->SetPoint(6, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, 0.0);
	m_Points->SetPoint(7, PROGRESS_BAR_WIDTH,  currentPoint, 0.0);

	// Frame
	vtkNew<vtkCellArray> lines;
	vtkIdType linesIds[5] = { 0, 1, 2, 3, 0 };
	lines->InsertNextCell(5, linesIds);


	// Progress bar
	vtkNew<vtkCellArray> polys;
	vtkIdType polysIds[4] = { 4, 5, 6, 7 };
	polys->InsertNextCell(4, polysIds);

	vtkNew<vtkPolyData> polydata;
	polydata->SetPoints(m_Points);
	polydata->SetLines(lines.Get());
	polydata->SetPolys(polys.Get());

	// Create cell data to color cells
	m_ProgressBarData = vtkUnsignedCharArray::New();
	m_ProgressBarData->SetName("Color");
	m_ProgressBarData->SetNumberOfComponents(3);
	m_ProgressBarData->SetNumberOfTuples(8);
	polydata->GetPointData()->SetScalars(m_ProgressBarData);

	// Add a transform to position progress bar
	// and a mapper and actor
	vtkNew<vtkTransformPolyDataFilter> transformFilter;
	transformFilter->SetTransform(this->BWTransform);
	transformFilter->SetInputData(polydata.Get());
	vtkNew<vtkPolyDataMapper2D> mapper;
	mapper->SetInputConnection(
		transformFilter->GetOutputPort());
	m_Property = vtkProperty2D::New();
	m_Actor = vtkSmartPointer<vtkActor2D>::New();
	m_Actor->SetMapper(mapper.Get());
	m_Actor->SetProperty(m_Property);

	// Background cell
	vtkNew<vtkCellArray> background;
	background->InsertNextCell(4, linesIds);

	// Background polydata
	vtkNew<vtkPolyData> backgroundPolydata;
	backgroundPolydata->SetPoints(m_Points);
	backgroundPolydata->SetPolys(background.Get());

	// Create cell data to color background
	m_BackgroundData = vtkUnsignedCharArray::New();
	m_BackgroundData->SetNumberOfComponents(3);
	m_BackgroundData->SetNumberOfTuples(4);
	backgroundPolydata->GetPointData()->SetScalars(m_BackgroundData);

	// Add transform, mapper and actor------------------------------------------------------------
	vtkNew<vtkTransformPolyDataFilter> backgroundTransformFilter;
	backgroundTransformFilter->SetTransform(this->BWTransform);
	backgroundTransformFilter->SetInputData(backgroundPolydata.Get());
	vtkNew<vtkPolyDataMapper2D> backgroundMapper;
	backgroundMapper->SetInputConnection(
		backgroundTransformFilter->GetOutputPort());
	m_BackgroundActor = vtkSmartPointer<vtkActor2D>::New();
	m_BackgroundActor->SetMapper(backgroundMapper.Get());

	//RemindingDepth line and Zero line-----------------------------------------------------------
	m_LineActor = vtkSmartPointer<vtkActor2D>::New();
	vtkNew<vtkPolyData> linePolydata;

	m_LinePoints = vtkSmartPointer<vtkPoints>::New();
	m_LinePoints->InsertNextPoint(0.2, remindingPoint, 0);
	m_LinePoints->InsertNextPoint(PROGRESS_BAR_WIDTH, remindingPoint, 0);
	m_LinePoints->InsertNextPoint(PROGRESS_BAR_WIDTH, finalPoint, 0);
	m_LinePoints->InsertNextPoint(0.2, finalPoint, 0);
	vtkNew<vtkCellArray> lines2;
	vtkIdType linesIds2[5] = { 0, 1,2,3,0 };
	lines2->InsertNextCell(5, linesIds2);
	linePolydata->SetPoints(m_LinePoints);
	linePolydata->SetLines(lines2.Get());
	vtkNew<vtkTransformPolyDataFilter> lineTransformFilter;

	lineTransformFilter->SetTransform(this->BWTransform);
	lineTransformFilter->SetInputData(linePolydata.Get());


	vtkNew<vtkPolyDataMapper2D> lineMapper;
	lineMapper->SetInputConnection(lineTransformFilter->GetOutputPort());
	m_LineActor->SetMapper(lineMapper.Get());
}

//-------------------------------------------------------------------------
DepthIndicatorRepresentation::~DepthIndicatorRepresentation()
{
	m_Property->Delete();
}

//-------------------------------------------------------------------------
void DepthIndicatorRepresentation::BuildRepresentation()
{
	m_ProgressRate = 1 - (m_CurrentDepth - m_WarningDepth) / m_WarningDepth;

	// Reposition progress bar points
	double barlen = m_WarningDepth + m_FinalDepth;

	double currentPoint = 0.2 + ((barlen - m_CurrentDepth) / barlen) * (PROGRESS_BAR_HEIGHT - 0.2);
	double remindingPoint = 0.2 + ((m_WarningDepth + m_RemindingDepth) / barlen)*(PROGRESS_BAR_HEIGHT - 0.2);
	double finalPoint = 0.2 + ((barlen - m_FinalDepth) / barlen)*(PROGRESS_BAR_HEIGHT - 0.2);
	m_Points->SetPoint(4, 0.2,  currentPoint, 0.0);
	m_Points->SetPoint(7, PROGRESS_BAR_WIDTH, currentPoint, 0.0);
	m_Points->Modified();

	m_LinePoints->SetPoint(0, 0.2, remindingPoint, 0);
	m_LinePoints->SetPoint(1, PROGRESS_BAR_WIDTH, remindingPoint, 0);
	m_LinePoints->SetPoint(2, PROGRESS_BAR_WIDTH, finalPoint, 0);
	m_LinePoints->SetPoint(3, 0.2, finalPoint, 0);
	m_LinePoints->Modified();

	// Set color
	double backgroundColor[3] = { m_BackgroundColor[0] * 255,
		m_BackgroundColor[1] * 255, m_BackgroundColor[2] * 255 };
	if (m_CurrentDepth<m_FinalDepth-m_RemindingDepth)
	{
		//yellow
		m_ProgressBarColor[0] = 1;
		m_ProgressBarColor[1] = 1;
		m_ProgressBarColor[2] = 0;
	}
	else if (m_CurrentDepth<= m_FinalDepth)
	{
		//green
		m_ProgressBarColor[0] = 0;
		m_ProgressBarColor[1] = 1;
		m_ProgressBarColor[2] = 0;
	} 
	else
	{
		//red
		m_ProgressBarColor[0] = 1;
		m_ProgressBarColor[1] = 0;
		m_ProgressBarColor[2] = 0;
	}

	double progressBarColor[3] = { m_ProgressBarColor[0] * 255,
		m_ProgressBarColor[1] * 255, m_ProgressBarColor[2] * 255 };
	for (int i = 0; i < 4; i++)
	{
		m_ProgressBarData->SetTuple(i, backgroundColor);
		m_BackgroundData->SetTuple(i, backgroundColor);
		m_ProgressBarData->SetTuple(i + 4, progressBarColor);
	}


	//Text Actor-----------------------------------------------------------

	//title
	m_TitleTextActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkVectorText>titleTextPolydata = vtkSmartPointer<vtkVectorText>::New();
	//number to string
	char titleText[100];
	if (m_FinalDepth - m_CurrentDepth > 0)
		sprintf(titleText, "+%.1f mm", m_FinalDepth - m_CurrentDepth);
	else
		sprintf(titleText, "%.1f mm", m_FinalDepth - m_CurrentDepth);
	titleTextPolydata->SetText(titleText);
	titleTextPolydata->Update();
	vtkSmartPointer<vtkTransform> titleTransformer = vtkSmartPointer<vtkTransform>::New();
	titleTransformer->Translate(0, PROGRESS_BAR_HEIGHT + 0.5, 0);
	titleTransformer->Scale(0.5, 0.5, 1);

	vtkSmartPointer<vtkTransformPolyDataFilter> titleScaleTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	titleScaleTransformFilter->SetTransform(titleTransformer);
	titleScaleTransformFilter->SetInputData(titleTextPolydata->GetOutput());
	titleScaleTransformFilter->Update();

	vtkNew<vtkTransformPolyDataFilter> titleWidgetTransformFilter;
	titleWidgetTransformFilter->SetTransform(this->BWTransform);
	titleWidgetTransformFilter->SetInputData(titleScaleTransformFilter->GetOutput());

	vtkSmartPointer<vtkPolyDataMapper2D> titleTextMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	titleTextMapper->SetInputConnection(titleWidgetTransformFilter->GetOutputPort());
	m_TitleTextActor->SetMapper(titleTextMapper);


	//final
	m_FinalTextActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkVectorText> finalTextPolydata = vtkSmartPointer<vtkVectorText>::New();
	sprintf(titleText, "%.1fmm", m_FinalDepth);
	finalTextPolydata->SetText(titleText);
	finalTextPolydata->Update();
	vtkSmartPointer<vtkTransform> finalTransformer = vtkSmartPointer<vtkTransform>::New();
	finalTransformer->Translate(PROGRESS_BAR_WIDTH + 0.3, finalPoint - 0.2, 0);
	finalTransformer->Scale(0.35, 0.35, 1);

	vtkSmartPointer<vtkTransformPolyDataFilter> finalScaleTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	finalScaleTransformFilter->SetTransform(finalTransformer);
	finalScaleTransformFilter->SetInputData(finalTextPolydata->GetOutput());
	finalScaleTransformFilter->Update();

	vtkNew<vtkTransformPolyDataFilter> finalWidgetTransformFilter;
	finalWidgetTransformFilter->SetTransform(this->BWTransform);
	finalWidgetTransformFilter->SetInputData(finalScaleTransformFilter->GetOutput());

	vtkSmartPointer<vtkPolyDataMapper2D> finalTextMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	finalTextMapper->SetInputConnection(finalWidgetTransformFilter->GetOutputPort());
	m_FinalTextActor->SetMapper(finalTextMapper);

	//current
	m_CurrentTextActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkVectorText> currentTextPolydata = vtkSmartPointer<vtkVectorText>::New();
	sprintf(titleText, "%.1fmm", m_CurrentDepth);
	currentTextPolydata->SetText(titleText);
	currentTextPolydata->Update();
	//vtkSmartPointer<vtkTransform> currentTransformer = vtkSmartPointer<vtkTransform>::New();
	auto currentTransformer = vtkSmartPointer<vtkTransform>::New();

	//currentTransformer->Translate(PROGRESS_BAR_WIDTH + 0.3, PROGRESS_BAR_HEIGHT-progressPoint , 0);
	currentTransformer->Translate(-2.0, currentPoint, 0);
	currentTransformer->Scale(0.35, 0.35, 1);

	vtkSmartPointer<vtkTransformPolyDataFilter> currentScaleTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	currentScaleTransformFilter->SetTransform(currentTransformer);
	currentScaleTransformFilter->SetInputData(currentTextPolydata->GetOutput());
	currentScaleTransformFilter->Update();

	vtkNew<vtkTransformPolyDataFilter> currentWidgetTransformFilter;
	currentWidgetTransformFilter->SetTransform(this->BWTransform);
	currentWidgetTransformFilter->SetInputData(currentScaleTransformFilter->GetOutput());

	vtkSmartPointer<vtkPolyDataMapper2D> currentTextMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	currentTextMapper->SetInputConnection(currentWidgetTransformFilter->GetOutputPort());
	m_CurrentTextActor->SetMapper(currentTextMapper);

	//reminding
	m_RemindingTextActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkVectorText> remindingTextPolydata = vtkSmartPointer<vtkVectorText>::New();
	sprintf(titleText, "%.1fmm", m_FinalDepth - m_RemindingDepth);
	remindingTextPolydata->SetText(titleText);
	remindingTextPolydata->Update();
	vtkSmartPointer<vtkTransform> remindingTransformer = vtkSmartPointer<vtkTransform>::New();
	remindingTransformer->Translate(PROGRESS_BAR_WIDTH + 0.3, remindingPoint - 0.2, 0);
	remindingTransformer->Scale(0.35, 0.35, 1);

	vtkSmartPointer<vtkTransformPolyDataFilter> remindingScaleTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	remindingScaleTransformFilter->SetTransform(remindingTransformer);
	remindingScaleTransformFilter->SetInputData(remindingTextPolydata->GetOutput());
	remindingScaleTransformFilter->Update();

	vtkNew<vtkTransformPolyDataFilter> remindingWidgetTransformFilter;
	remindingWidgetTransformFilter->SetTransform(this->BWTransform);
	remindingWidgetTransformFilter->SetInputData(remindingScaleTransformFilter->GetOutput());

	vtkSmartPointer<vtkPolyDataMapper2D> remindingTextMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	remindingTextMapper->SetInputConnection(remindingWidgetTransformFilter->GetOutputPort());
	m_RemindingTextActor->SetMapper(remindingTextMapper);
	m_RemindingTextActor->GetProperty()->SetColor(0.7, 0.7, 0.7);
	//zero
	m_ZeroTextActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkVectorText> zeroTextPolydata = vtkSmartPointer<vtkVectorText>::New();

	zeroTextPolydata->SetText("0");
	zeroTextPolydata->Update();
	vtkSmartPointer<vtkTransform> zeroTransformer = vtkSmartPointer<vtkTransform>::New();
	zeroTransformer->Translate(PROGRESS_BAR_WIDTH + 0.3, PROGRESS_BAR_HEIGHT - 0.2, 0);
	zeroTransformer->Scale(0.35, 0.35, 1);

	vtkSmartPointer<vtkTransformPolyDataFilter> zeroScaleTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	zeroScaleTransformFilter->SetTransform(zeroTransformer);
	zeroScaleTransformFilter->SetInputData(zeroTextPolydata->GetOutput());
	zeroScaleTransformFilter->Update();

	vtkNew<vtkTransformPolyDataFilter> zeroWidgetTransformFilter;
	zeroWidgetTransformFilter->SetTransform(this->BWTransform);
	zeroWidgetTransformFilter->SetInputData(zeroScaleTransformFilter->GetOutput());

	vtkSmartPointer<vtkPolyDataMapper2D> zeroTextMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	zeroTextMapper->SetInputConnection(zeroWidgetTransformFilter->GetOutputPort());
	m_ZeroTextActor->SetMapper(zeroTextMapper);
	m_ZeroTextActor->GetProperty()->SetColor(0.7, 0.7, 0.7);
	//warning
	m_WarningTextActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkVectorText> WarningTextPolydata = vtkSmartPointer<vtkVectorText>::New();
	sprintf(titleText, "%.1fmm", m_FinalDepth + m_WarningDepth);
	WarningTextPolydata->SetText(titleText);
	WarningTextPolydata->Update();
	vtkSmartPointer<vtkTransform> WarningTransformer = vtkSmartPointer<vtkTransform>::New();
	WarningTransformer->Translate(PROGRESS_BAR_WIDTH + 0.3, 0, 0);
	WarningTransformer->Scale(0.35, 0.35, 1);

	vtkSmartPointer<vtkTransformPolyDataFilter> WarningScaleTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	WarningScaleTransformFilter->SetTransform(WarningTransformer);
	WarningScaleTransformFilter->SetInputData(WarningTextPolydata->GetOutput());
	WarningScaleTransformFilter->Update();

	vtkNew<vtkTransformPolyDataFilter> WarningWidgetTransformFilter;
	WarningWidgetTransformFilter->SetTransform(this->BWTransform);
	WarningWidgetTransformFilter->SetInputData(WarningScaleTransformFilter->GetOutput());

	vtkSmartPointer<vtkPolyDataMapper2D> WarningTextMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	WarningTextMapper->SetInputConnection(WarningWidgetTransformFilter->GetOutputPort());
	m_WarningTextActor->SetMapper(WarningTextMapper);
	m_WarningTextActor->GetProperty()->SetColor(0.7, 0.7, 0.7);


	// Note that the transform is updated by the superclass
	this->Superclass::BuildRepresentation();
}

//-------------------------------------------------------------------------
void DepthIndicatorRepresentation::SetFinalDepth(double depth)
{
	m_FinalDepth = depth;
}

void DepthIndicatorRepresentation::SetCurrentDepth(double depth)
{
	m_CurrentDepth = depth;
}

void DepthIndicatorRepresentation::SetRemindingDepth(double depth)
{
	m_RemindingDepth = depth;
}

void DepthIndicatorRepresentation::SetWarningDepth(double depth)
{
	m_WarningDepth = depth;
}

//-------------------------------------------------------------------------
void DepthIndicatorRepresentation::GetSize(double size[2])
{
	size[0] = PROGRESS_BAR_WIDTH + 0.2;
	size[1] = PROGRESS_BAR_HEIGHT + 0.2;
}

//-------------------------------------------------------------------------
void DepthIndicatorRepresentation::GetActors2D(vtkPropCollection *pc)
{
	if (m_DrawBackground)
	{
		pc->AddItem(m_BackgroundActor);
	}
	pc->AddItem(m_Actor);
	pc->AddItem(m_LineActor);

	pc->AddItem(m_TitleTextActor);
	pc->AddItem(m_FinalTextActor);
	pc->AddItem(m_CurrentTextActor);
	pc->AddItem(m_RemindingTextActor);
	pc->AddItem(m_ZeroTextActor);
	pc->AddItem(m_WarningTextActor);
	this->Superclass::GetActors2D(pc);
}

//-------------------------------------------------------------------------
void DepthIndicatorRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
	if (m_DrawBackground)
	{
		m_BackgroundActor->ReleaseGraphicsResources(w);
	}
	m_Actor->ReleaseGraphicsResources(w);
	m_LineActor->ReleaseGraphicsResources(w);
	m_TitleTextActor->ReleaseGraphicsResources(w);
	m_FinalTextActor->ReleaseGraphicsResources(w);
	m_CurrentTextActor->ReleaseGraphicsResources(w);
	m_RemindingTextActor->ReleaseGraphicsResources(w);
	m_ZeroTextActor->ReleaseGraphicsResources(w);
	m_WarningTextActor->ReleaseGraphicsResources(w);
	this->Superclass::ReleaseGraphicsResources(w);
}

//-------------------------------------------------------------------------
int DepthIndicatorRepresentation::RenderOverlay(vtkViewport *w)
{
	int count = this->Superclass::RenderOverlay(w);
	if (m_DrawBackground)
	{
		count += m_BackgroundActor->RenderOverlay(w);
	}
	count += m_Actor->RenderOverlay(w);
	count += m_LineActor->RenderOverlay(w);
	count += m_TitleTextActor->RenderOverlay(w);
	count += m_FinalTextActor->RenderOverlay(w);
	count += m_CurrentTextActor->RenderOverlay(w);
	count += m_RemindingTextActor->RenderOverlay(w);
	count += m_ZeroTextActor->RenderOverlay(w);
	count += m_WarningTextActor->RenderOverlay(w);
	return count;
}

//-------------------------------------------------------------------------
int DepthIndicatorRepresentation::RenderOpaqueGeometry(vtkViewport *w)
{
	int count = this->Superclass::RenderOpaqueGeometry(w);
	if (m_DrawBackground)
	{
		count += m_BackgroundActor->RenderOpaqueGeometry(w);
	}
	count += m_Actor->RenderOpaqueGeometry(w);
	count += m_LineActor->RenderOpaqueGeometry(w);
	count += m_TitleTextActor->RenderOpaqueGeometry(w);
	count += m_FinalTextActor->RenderOpaqueGeometry(w);
	count += m_CurrentTextActor->RenderOpaqueGeometry(w);
	count += m_RemindingTextActor->RenderOpaqueGeometry(w);
	count += m_ZeroTextActor->RenderOpaqueGeometry(w);
	count += m_WarningTextActor->RenderOpaqueGeometry(w);
	return count;
}

//-----------------------------------------------------------------------------
int DepthIndicatorRepresentation::RenderTranslucentPolygonalGeometry(
	vtkViewport *w)
{
	int count = this->Superclass::RenderTranslucentPolygonalGeometry(w);
	if (m_DrawBackground)
	{
		count += m_BackgroundActor->RenderTranslucentPolygonalGeometry(w);
	}
	count += m_Actor->RenderTranslucentPolygonalGeometry(w);
	count += m_LineActor->RenderTranslucentPolygonalGeometry(w);
	count += m_TitleTextActor->RenderTranslucentPolygonalGeometry(w);
	count += m_FinalTextActor->RenderTranslucentPolygonalGeometry(w);
	count += m_CurrentTextActor->RenderTranslucentPolygonalGeometry(w);
	count += m_RemindingTextActor->RenderTranslucentPolygonalGeometry(w);
	count += m_ZeroTextActor->RenderTranslucentPolygonalGeometry(w);
	count += m_WarningTextActor->RenderTranslucentPolygonalGeometry(w);
	return count;
}

//-----------------------------------------------------------------------------
int DepthIndicatorRepresentation::HasTranslucentPolygonalGeometry()
{
	int result = this->Superclass::HasTranslucentPolygonalGeometry();
	if (m_DrawBackground)
	{
		result |= m_BackgroundActor->HasTranslucentPolygonalGeometry();
	}
	result |= m_Actor->HasTranslucentPolygonalGeometry();
	result |= m_LineActor->HasTranslucentPolygonalGeometry();
	result |= m_TitleTextActor->HasTranslucentPolygonalGeometry();
	result |= m_FinalTextActor->HasTranslucentPolygonalGeometry();
	result |= m_CurrentTextActor->HasTranslucentPolygonalGeometry();
	result |= m_RemindingTextActor->HasTranslucentPolygonalGeometry();
	result |= m_ZeroTextActor->HasTranslucentPolygonalGeometry();
	result |= m_WarningTextActor->HasTranslucentPolygonalGeometry();
	return result;
}

//-------------------------------------------------------------------------
void DepthIndicatorRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	if (m_Property)
	{
		os << indent << "Property:\n";
		m_Property->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "Property: (none)\n";
	}
	os << indent << "ProgressRate: " << m_ProgressRate << "\n";
	os << indent << "ProgressBarColor: " << m_ProgressBarColor[0] << " "
		<< m_ProgressBarColor[1] << " " << m_ProgressBarColor[2] << "\n";
	os << indent << "DrawBackground: " << m_DrawBackground << "\n";
	os << indent << "BackgroundColor: " << m_BackgroundColor[0] << " "
		<< m_BackgroundColor[1] << " " << m_BackgroundColor[2] << "\n";
}