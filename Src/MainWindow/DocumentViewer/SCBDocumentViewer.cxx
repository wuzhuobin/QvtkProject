#include "SCBDocumentViewer.h"
#include "ui_scbdocumentviewer.h"
#include <stdio.h>
#include <QDebug>
#include <QFileDialog>
#include <QPixmap>
#include <QLabel>
#include <QScrollBar>
#include <QPalette>
#include <QPlainTextEdit.h>
#include <QCollator.h>

SCBDocumentViewer::SCBDocumentViewer(QWidget *parent) :
	QDialog(parent,Qt::FramelessWindowHint),
   ui(new Ui::SCBDocumentViewer)
{
	ui->setupUi(this);

	ui->Close->setStyleSheet( "QPushButton{background-color: #398fcc;color: rgb(255, 255, 255);}" \
		"QPushButton:hover{ background-color:#2fa2f4; color: white; }"\
		"QPushButton:pressed{ background-color: #223555;}");
	ui->LastPage->setStyleSheet("QPushButton{background-color: #398fcc;color: rgb(255, 255, 255);}" \
		"QPushButton:hover{ background-color:#2fa2f4; color:  white; }"\
		"QPushButton:pressed{ background-color: #223555;}");
	ui->NextPage->setStyleSheet("QPushButton{background-color: #398fcc;color: rgb(255, 255, 255);}" \
		"QPushButton:hover{ background-color:#2fa2f4; color: white; }"\
		"QPushButton:pressed{ background-color: #223555;}");
	ui->scrollArea->setStyleSheet({ "background-color:#f1f1f1;" });
	ui->widget->setStyleSheet({ "background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #0c172a, stop: 0.2 #292f5f,stop: 0.8 #292f5f, stop: 1.0 #0c172a);;" });
	ui->PageNumber->setStyleSheet({ "background-color: rgb(255, 255, 255) ;" });

	connect(ui->Close, 								SIGNAL(clicked()), 						this, SLOT(close_current_window()));
	connect(ui->scrollArea->verticalScrollBar(), 	SIGNAL(valueChanged(int)), 				this, SLOT(valueChanged(int)));
	connect(ui->LastPage, 							SIGNAL(clicked()), 						this, SLOT(LastPagechanged()));
	connect(ui->NextPage, 							SIGNAL(clicked()), 						this, SLOT(NextPagechanged()));
	connect(ui->PageNumber, 						SIGNAL(textChanged(const QString &)), 	this, SLOT(TargetPage(const QString &)));//reference 


}

SCBDocumentViewer::~SCBDocumentViewer()
{
    delete ui;
}

void SCBDocumentViewer::showDocument(QString directory)
{

	//QString imagefile = QFileDialog::getExistingDirectory(NULL, "Select Directory", ".jpg");
	QDir dir(directory);

	if (!dir.exists())
	{
		return;
	}

	//QStringList infolist = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);

	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	dir.setSorting(QDir::NoSort);  // will sort manually with std::sort

	auto infolist = dir.entryList();

	QCollator collator;   
	collator.setNumericMode(true);
	std::sort(infolist.begin(), infolist.end(), collator); //put all files in order. exp : 1, 2, 3 ..instead of 1 ,10,11,...
	
	QWidget* widget = new QWidget(this);
	QVBoxLayout* vboxLayout = new QVBoxLayout();

	for (int i = 0; i < infolist.size(); i++)
	{
		QPixmap* pixmap = new QPixmap(directory + "\\" + infolist.at(i));
		QLabel * label = new QLabel(this);
		label->setPixmap(*pixmap);             //put images into the label
		vboxLayout->addWidget(label);
		vboxLayout->setAlignment(widget, Qt::AlignCenter);
		vboxLayout->update();
	}


	widget->setLayout(vboxLayout);     //set the layout

	ui->scrollArea->setWidget(widget); //set the widget
}

void SCBDocumentViewer::close_current_window()
{
	ui->Close->window()->close();
}

void SCBDocumentViewer::valueChanged(int)
{
	int barWidth = ui->scrollArea->verticalScrollBar()->width();
	int max = ui->scrollArea->verticalScrollBar()->maximum();
	//qDebug() << "max" << max;
	value = ui->scrollArea->verticalScrollBar()->value();//obtain the current postion of scrollbar
	//qDebug() << "value"<<value;
	area = value / 1684 + 1;
	//qDebug() << "area"<<area;
	ui->PageNumber->setText(tempStr.setNum(area));
}

void SCBDocumentViewer::LastPagechanged()
{
	for (int i = 1; i <= max_pages; i++) 
	{
		if ((ui->scrollArea->verticalScrollBar()->value() / 1684 + 1) == i) 
		{
			ui->scrollArea->verticalScrollBar()->setSliderPosition((i - 2) * 1684);
			break;
		}
	}
}

void SCBDocumentViewer::NextPagechanged()
{
	//qDebug() << "value" << value;
	//qDebug() << "area+1" << area+1;
	for (int i = 1; i <= max_pages; i++) 
	{
		if ((ui->scrollArea->verticalScrollBar()->value() / 1684 + 1) == i) 
		{
			ui->scrollArea->verticalScrollBar()->setSliderPosition(i * 1684);
			break;
		}
	}
}

void SCBDocumentViewer::TargetPage(const QString &)
{   
	bool ok;
	QString valueStr=ui->PageNumber->text();
    int valueInt = valueStr.toInt(&ok);
    ui->scrollArea->verticalScrollBar()->setSliderPosition((valueInt-1) * 1684);
}
