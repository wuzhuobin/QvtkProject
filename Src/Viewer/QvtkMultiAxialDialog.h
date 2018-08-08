#ifndef __QVTK_MULTI_AXIAL_DIALOG_H__
#define __QVTK_MULTI_AXIAL_DIALOG_H__
#pragma once
// me
#include "viewer_export.h"
// qt
#include <QDialog>
namespace Q {
	namespace vtk {
		class PlanarViewer;
		namespace Ui {
			class MultiAxialDialog;
		}
	}
}
namespace Q {
	namespace vtk {
		class VIEWER_EXPORT MultiAxialDialog : public QDialog
		{
			Q_OBJECT

		public:
			explicit MultiAxialDialog(unsigned int row = 5, unsigned int col = 2, QWidget *parent = 0);
			explicit MultiAxialDialog(QWidget *parent);
			~MultiAxialDialog();
			PlanarViewer *getViewer(unsigned int row, unsigned int col);
		public Q_SLOTS:
			void updateAxialViewer();
		private:
			unsigned int row;
			unsigned int col;
			QList<PlanarViewer*> viewers;
			Ui::MultiAxialDialog *ui;
		};
	}
}

#endif // __QVTK_MULTI_AXIAL_DIALOG_H__
