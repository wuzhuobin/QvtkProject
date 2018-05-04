#ifndef IMAGEREGISTRATION_H
#define IMAGEREGISTRATION_H

#include <QObject>

#include <itkImage.h>
#include <itkMatrix.h>
#include <itkVersorRigid3DTransform.h>
#include <itkAffineTransform.h>

class ImageRegistration : public QObject
{
	Q_OBJECT

public:
	typedef itk::Image< short, 3 > ImageType;
	//typedef itk::AffineTransform<double, 3> TransformType;
	typedef itk::VersorRigid3DTransform <double> TransformType;		//TransformType

	ImageRegistration(QObject* parent = NULL);
	~ImageRegistration();

	void SetInitialMatrix(itk::Matrix<double, 3,3>);
	void SetInitialRotation(double, double, double);
	void SetInitialTranslation(double, double, double);
	void SetInitialCenter(double, double, double);
	void SetFixedImage(ImageType::Pointer fixedImage);
	void SetMovingImage(ImageType::Pointer movingImage);

	bool Update();	
	TransformType::Pointer   GetOutputTransform();
	ImageType::DirectionType GetOutputDirection();
	ImageType::PointType	 GetOutputOrigin();

private:
	itk::Matrix<double, 3, 3> m_initialMatrix;
	itk::Vector<double, 3>	m_initialTranslation;
	itk::Vector<double, 3>	m_initialCenter;
	itk::Vector<double, 3> m_initialRotation;
	ImageType::Pointer	   m_fixedImage;
	ImageType::Pointer	   m_movingImage;
	ImageType::Pointer	   m_outputImage;
	TransformType::Pointer m_finalTransform;
};

#endif // IMAGEREGISTRATION_H