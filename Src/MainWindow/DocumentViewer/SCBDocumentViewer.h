#ifndef SCBDOCUMENTVIEWER_H
#define SCBDOCUMENTVIEWER_H

#include <QWidget>
#include <QDialog>
#include <QProxyStyle>
#include "ui_scbdocumentviewer.h"

namespace Ui {
class SCBDocumentViewer;
}

class SCBDocumentViewer : public QDialog
{
    Q_OBJECT

public:
   explicit SCBDocumentViewer(QWidget *parent=0);
    ~SCBDocumentViewer();

	void showDocument(QString directory);

private slots:
	void close_current_window();
	void valueChanged(int);
	void LastPagechanged();
	void NextPagechanged();
	void TargetPage(const QString &);


private:
    Ui::SCBDocumentViewer *ui;

	QString tempStr;
    int imageWidth;
	int imageHeight;
	int value;
	int area;
	int max_pages 	= 1000; //max number pages 
};

#endif // SCBDOCUMENTVIEWER_h

