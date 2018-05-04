//// me
//#include "SCBLandmarkTransformRMS.h"
//#include "ui_SCBLandmarkTransformRMS.h"
//#include "vtkLandmarkTransformRMS.h"
//
//// vtk 
//#include <vtkPoints.h>
//
//SCBLandmarkTransformRMS::SCBLandmarkTransformRMS(QWidget * parent)
//	:SCBAbstractFilter(parent)
//{
//	this->m_landmarkTransformRMS = vtkLandmarkTransformRMS::New();
//	this->LoadUI();
//	this->ConnectUI();
//}
//
//SCBLandmarkTransformRMS::~SCBLandmarkTransformRMS()
//{
//	this->m_landmarkTransformRMS->Delete();
//	delete this->m_ui;
//}
//
//void SCBLandmarkTransformRMS::Update()
//{
//	if (this->ErrorCheck()) {
//		qCritical("ErrorCheck failed");
//		return;
//	}
//	this->m_landmarkTransformRMS->Update();
//	vtkPoints* points;
//	points = this->m_landmarkTransformRMS->GetSourceLandmarks();
//	this->m_ui->tableWidgetSource->clearContents();
//	this->m_ui->tableWidgetSource->setRowCount(points->GetNumberOfPoints());
//	for (vtkIdType id = 0; id < points->GetNumberOfPoints(); ++id) {
//		this->m_ui->tableWidgetSource->item(id, 0)->setText(QString::number(id));
//		this->m_ui->tableWidgetSource->item(id, 0)->setText(QString::number(points->GetPoint(id)[0]));
//		this->m_ui->tableWidgetSource->item(id, 1)->setText(QString::number(points->GetPoint(id)[1]));
//		this->m_ui->tableWidgetSource->item(id, 2)->setText(QString::number(points->GetPoint(id)[2]));
//	}
//	points = this->m_landmarkTransformRMS->GetTargetLandmarks();
//	this->m_ui->tableWidgetTarget->clearContents();
//	this->m_ui->tableWidgetTarget->setRowCount(points->GetNumberOfPoints());
//	for (vtkIdType id = 0; id < points->GetNumberOfPoints(); ++id) {
//		this->m_ui->tableWidgetTarget->item(id, 0)->setText(QString::number(id));
//		this->m_ui->tableWidgetTarget->item(id, 0)->setText(QString::number(points->GetPoint(id)[0]));
//		this->m_ui->tableWidgetTarget->item(id, 1)->setText(QString::number(points->GetPoint(id)[1]));
//		this->m_ui->tableWidgetTarget->item(id, 2)->setText(QString::number(points->GetPoint(id)[2]));
//	}
//	this->m_ui->doubleSpinBoxRMS->setValue(this->m_landmarkTransformRMS->GetRMS());
//}
//
//void SCBLandmarkTransformRMS::LoadUI()
//{
//	this->m_ui = new Ui::SCBLandmarkTransformRMS;
//	this->m_ui->setupUi(this);
//}
//
//int SCBLandmarkTransformRMS::ConnectUI()
//{
//	connect(this->m_ui->comboBoxMode, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), 
//		this, [this](int i) {
//		switch (i)
//		{
//		case 0:
//			this->m_landmarkTransformRMS->SetModeToRigidBody();
//			break;
//		case 1:
//			this->m_landmarkTransformRMS->SetModeToAffine();
//			break;
//		case 2:
//			this->m_landmarkTransformRMS->SetModeToSimilarity();
//			break;
//		default:
//			break;
//		}});
//	connect(this->m_ui->pushButtonUpdate, &QPushButton::toggled,
//		this, [this](bool flag)->void {
//		if (!flag) {
//			return;
//		}
//		this->m_landmarkTransformRMS->Update();
//	});
//	return 0;
//}
//
////bool SCBLandmarkTransformRMS::ErrorCheck()
////{
////	if (!SCBAbstractFilter::ErrorCheck()) {
////		return false;
////	}
////	this->m_landmarkTransformRMS->Update();
////}
