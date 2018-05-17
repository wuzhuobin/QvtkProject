#ifndef __QVTK_NEURAL_TUBE_H__
#define __QVTK_NEURAL_TUBE_H__
#pragma once 
// me 
#include "QvtkPolyData.h"
// vtk
class vtkTubeFilter;
class vtkLineSource;
/**
* @class	SCBNeural
* @brief	A data type of tube for Nerual. 
* @author	WUZHUOBIN
* @date		May.03.2018	
* @code
* // this is a sample code. 
*	tube->setColor(1, 1, 1);
*	tube->setEndPoint(0, 0, 0);
*	tube->setIntermediatePoint(1, 1, 1);
*	tube->setTubeLength(1000);
*	tube->setTubeRadius(2);
*	tube->readData("..");
*	SCBPolyDataActor* actor = new SCBPolyDataActor;
*	actor->setRenderDataSet(tube);
*	nonPlanarViewer->AddProp(actor);
*	SCBPolyDataActor2D *actor2D = new SCBPolyDataActor2D;
*	actor2D->setRenderDataSet(tube);
*	actor2D->setPlanarOrientation(planarViewer->GetOrientation());
*	actor2D->setPlanarOrigin(
*		planarViewer->GetCursorPosition()[0],
*		planarViewer->GetCursorPosition()[1],
*		planarViewer->GetCursorPosition()[2]);
*	planarViewer->AddProp(actor2D);
* @endcode
*/
namespace Q {
	namespace vtk {

class NeuralTube : public PolyData {
	Q_OBJECT;
	Q_PROPERTY(
		double	tubeRadius
		READ	getTubeRadius
		WRITE	setTubeRadius
		NOTIFY	tubeRadiusChanged);
	Q_PROPERTY(
		double	tubeLength
		READ	getTubeLength
		WRITE	setTubeLength
		NOTIFY	tubeLengthChanged);
	Q_PROPERTY(
		QList<double>	intermediatePoint
		READ			getIntermediatePoint
		WRITE			setIntermediatePoint
		NOTIFY			intermediatePointChanged);
	Q_PROPERTY(
		QList<double>	endPoint
		READ			getEndPoint
		WRITE			setEndPoint
		NOTIFY			endPointChanged);
	Q_VTK_DATA_H(
		NeuralTube,
		Q_VTK_KEY(TubeRadius)
		Q_VTK_KEY(TubeLength)
		Q_VTK_KEY(IntermediatePoint)
		Q_VTK_KEY(EndPoint)
	);
public:
	NeuralTube();
	virtual ~NeuralTube() override;
	virtual void readXML(const QDomElement &xml, QString directoryPath = QString()) override;
	virtual void writeXML(QDomElement &xml, QString directoryPath) const override;
	virtual bool readData(QString rootDirectory) override;
	virtual double getTubeRadius() const;
	virtual double getTubeLength() const { return this->m_tubeLength; }
	virtual const double* getIntermediatePoint() const { return this->m_intermediatePoint; }
	virtual QList<double> getIntermediatePoint() { return QList<double>{this->m_intermediatePoint[0], this->m_intermediatePoint[1], this->m_intermediatePoint[2]}; }
	virtual const double* getEndPoint() const;
	virtual QList<double> getEndPoint();
public Q_SLOTS:
	virtual void setTubeLength(double length);
	virtual void setTubeRadius(double radius); 
	virtual void setIntermediatePoint(double x, double y, double z);
	virtual void setIntermediatePoint(const double point[3]) { this->setIntermediatePoint(point[0], point[1], point[2]); }
	virtual void setIntermediatePoint(QList<double> point) { this->setIntermediatePoint(point[0], point[1], point[2]); }
	virtual void setEndPoint(double x, double y, double z);
	virtual void setEndPoint(const double point[3]) { this->setEndPoint(point[0], point[1], point[2]); }
	virtual void setEndPoint(QList<double> point) { this->setEndPoint(point[0], point[1], point[2]); }
Q_SIGNALS:
	void tubeRadiusChanged(double radius) const;
	void tubeLengthChanged(double length) const;
	void intermediatePointChanged(const double *point) const;
	void intermediatePointChanged(QList<double> point) const;
	void endPointChanged(const double *point) const;
	void endPointChanged(QList<double> point) const;
protected:
	virtual Data* newInstance() const override { return new NeuralTube; }
	vtkLineSource *m_lineSource;
	vtkTubeFilter *m_tubeFilter;
private:
	double m_tubeLength;
	double m_intermediatePoint[3];
};
	}
}
#endif // !__QVTK_NEURAL_TUBE_H__
