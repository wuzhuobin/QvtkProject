#ifndef __QVTK_DENTAL_NERVE_SOURCE_H
#define __QVTK_DENTAL_NERVE_SOURCE_H

#include "QvtkAnnotationPolyData.h"
namespace Q {
	namespace vtk {

class DentalNerveSource : public AnnotationPolyData
{
	Q_OBJECT;
	Q_VTK_DATA_H(
		DentalNerveSource,
		Q_VTK_KEY(NerveRadius)
		Q_VTK_KEY(NerveResolution)
		Q_VTK_KEY(NerveId)
	);

public:
	DentalNerveSource();
	virtual ~DentalNerveSource() override;
	virtual void printSelf() const override;

	// data io
	virtual bool readData(QString rootDirectory=QString()) override;
	virtual bool writeData(QString rootDirectory=QString()) const override;

	// nerve data info
	virtual void setNerveId(unsigned int nerveId);
	virtual unsigned int getNerveId();

	// control point manipulation
	void addPoint(double point[3]);
	void removePoint(unsigned int id);
	void setPoint(unsigned int id, double point[3]);
	void clear();
	void getPoints(vtkPoints* points);
	virtual vtkPoints* getPoints();
	virtual void getPolyData(vtkPolyData* nerve);
	virtual vtkPolyData* getPolyData();

	// spline manipulation
	void setRadius(double radius);
	void setResolution(double resolution);
	double getRadius();
	double getResolution();

public slots:
	void update();

signals:
	void radiusChanged(double);
	void resolutionChanged(double);

	void updateComplete();

protected:
	virtual Data* newInstance() const override = 0;

private:
	vtkPoints* m_splinePoints;
	double m_radius;
	vtkPolyData* m_nervePolyData;
	unsigned int m_nerveId;
	QString m_pointsPath;
	unsigned int m_resolution;

	void compute();

};

	}
}
#endif