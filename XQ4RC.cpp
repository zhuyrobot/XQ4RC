﻿#include "XQ4IO.h"
#include <QtGui/QtGui>
#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include <QtSerialPort/QtSerialPort>

class XQ4RC : public QWidget
{
public:
	XQ4IO ioXQ;

public:
	static void RunMe(int argc = 0, char** argv = 0) { QApplication app(argc, argv); XQ4RC me; me.show(); app.exec(); }
	XQ4RC(QWidget* parent = 0) : QWidget(parent)
	{
		//0.Basic settting
		this->setWindowTitle("Super Cube");
		this->setMinimumSize(QSize(1280, 720));
		this->setFont(QFont("", 15, QFont::Thin));

		//1.Group1 setting
		gridLayoutMain->addWidget(comboBoxPorts, 0, 0, 1, 2);
		gridLayoutMain->addWidget(pushButtonOpen, 0, 2, 1, 1);
		gridLayoutMain->addWidget(spinBoxLinearSpeed, 1, 0);
		gridLayoutMain->addWidget(spinBoxAngularSpeed, 1, 1);
		gridLayoutMain->addWidget(spinBoxBrakeSpeed, 1, 2);
		gridLayoutMain->addWidget(pushButtonMoveHead, 2, 1);
		gridLayoutMain->addWidget(pushButtonTurnLeft, 3, 0);
		gridLayoutMain->addWidget(comboBoxWorkMode, 3, 1);
		gridLayoutMain->addWidget(pushButtonTurnRight, 3, 2);
		gridLayoutMain->addWidget(pushButtonMoveBack, 4, 1);
		gridLayoutMain->addWidget(pushButtonCalibIMU, 5, 1, 4, 1);
		gridLayoutMain->addWidget(pushButtonEnableIR, 5, 0);
		gridLayoutMain->addWidget(pushButtonDisableIR, 5, 2);
		gridLayoutMain->addWidget(pushButtonPlusMotor1, 6, 0);
		gridLayoutMain->addWidget(pushButtonMinusMotor1, 6, 2);
		gridLayoutMain->addWidget(pushButtonPlusMotor2, 7, 0);
		gridLayoutMain->addWidget(pushButtonMinusMotor2, 7, 2);
		gridLayoutMain->addWidget(pushButtonPlusMotors, 8, 0);
		gridLayoutMain->addWidget(pushButtonMinusMotors, 8, 2);
		gridLayoutMain->addWidget(pushButtonEnableROS2, 9, 0, 1, 3);
		gridLayoutMain->addWidget(plainTextEditCarStatus, 0, 3, 10, 1);
		{
			//1.GetAllSerPorts
			QList<QSerialPortInfo> listSerialPortInfo = QSerialPortInfo::availablePorts();
			for (int k = 0; k < listSerialPortInfo.size(); ++k) comboBoxPorts->addItem(listSerialPortInfo[k].portName());

			//2.FillOptions
			spinBoxLinearSpeed->setRange(0, 100); spinBoxLinearSpeed->setValue(10);
			spinBoxAngularSpeed->setRange(0, 100); spinBoxAngularSpeed->setValue(10);
			spinBoxBrakeSpeed->setRange(0, 100); spinBoxBrakeSpeed->setValue(10);
			comboBoxWorkMode->addItems(QStringList() << "DBG" << "Run" << "Reset");

			//3.AdjustGUI
			QList<QWidget*> children = this->findChildren<QWidget*>();
			for (int k = 0; k < children.size(); ++k) children[k]->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
			for (int k = 2; k < 5; ++k) gridLayoutMain->setRowStretch(k, 1);
			gridLayoutMain->setColumnStretch(3, 1);
			connect(pushButtonMoveHead, &QPushButton::pressed, [this]()->void { ioXQ.runCar('f', char(spinBoxLinearSpeed->value())); });
			connect(pushButtonMoveBack, &QPushButton::pressed, [this]()->void { ioXQ.runCar('b', char(spinBoxLinearSpeed->value())); });
			connect(pushButtonTurnLeft, &QPushButton::pressed, [this]()->void { ioXQ.runCar('c', char(spinBoxAngularSpeed->value())); });
			connect(pushButtonTurnRight, &QPushButton::pressed, [this]()->void { ioXQ.runCar('d', char(spinBoxAngularSpeed->value())); });
			connect(pushButtonMoveHead, &QPushButton::released, [this]()->void { ioXQ.runCar('s', char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonMoveBack, &QPushButton::released, [this]()->void { ioXQ.runCar('s', char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonTurnLeft, &QPushButton::released, [this]()->void { ioXQ.runCar('s', char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonTurnRight, &QPushButton::released, [this]()->void { ioXQ.runCar('s', char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonCalibIMU, &QPushButton::pressed, [this]()->void { ioXQ.runSensor(0); });
			connect(pushButtonEnableIR, &QPushButton::pressed, [this]()->void { ioXQ.runSensor(1); });
			connect(pushButtonDisableIR, &QPushButton::pressed, [this]()->void { ioXQ.runSensor(2); });
			connect(pushButtonPlusMotor1, &QPushButton::pressed, [this]()->void { ioXQ.runMotor('F', 'S', char(spinBoxLinearSpeed->value()), char(spinBoxAngularSpeed->value())); });
			connect(pushButtonMinusMotor1, &QPushButton::pressed, [this]()->void { ioXQ.runMotor('B', 'S', char(spinBoxLinearSpeed->value()), char(spinBoxAngularSpeed->value())); });
			connect(pushButtonPlusMotor2, &QPushButton::pressed, [this]()->void { ioXQ.runMotor('S', 'F', char(spinBoxLinearSpeed->value()), char(spinBoxAngularSpeed->value())); });
			connect(pushButtonMinusMotor2, &QPushButton::pressed, [this]()->void { ioXQ.runMotor('S', 'B', char(spinBoxLinearSpeed->value()), char(spinBoxAngularSpeed->value())); });
			connect(pushButtonPlusMotors, &QPushButton::pressed, [this]()->void { ioXQ.runMotor('F', 'F', char(spinBoxLinearSpeed->value()), char(spinBoxAngularSpeed->value())); });
			connect(pushButtonMinusMotors, &QPushButton::pressed, [this]()->void { ioXQ.runMotor('B', 'B', char(spinBoxLinearSpeed->value()), char(spinBoxAngularSpeed->value())); });
			connect(pushButtonPlusMotor1, &QPushButton::released, [this]()->void { ioXQ.runMotor('S', 'S', char(spinBoxBrakeSpeed->value()), char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonMinusMotor1, &QPushButton::released, [this]()->void { ioXQ.runMotor('S', 'S', char(spinBoxBrakeSpeed->value()), char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonPlusMotor2, &QPushButton::released, [this]()->void { ioXQ.runMotor('S', 'S', char(spinBoxBrakeSpeed->value()), char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonMinusMotor2, &QPushButton::released, [this]()->void { ioXQ.runMotor('S', 'S', char(spinBoxBrakeSpeed->value()), char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonPlusMotors, &QPushButton::released, [this]()->void { ioXQ.runMotor('S', 'S', char(spinBoxBrakeSpeed->value()), char(spinBoxBrakeSpeed->value())); });
			connect(pushButtonMinusMotors, &QPushButton::released, [this]()->void { ioXQ.runMotor('S', 'S', char(spinBoxBrakeSpeed->value()), char(spinBoxBrakeSpeed->value())); });
			connect(comboBoxWorkMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)->void { char modes[3] = { 'T', 'R', 'I' }; ioXQ.setMode(modes[index]); });
			connect(pushButtonOpen, &QPushButton::clicked, [this]()->void
				{
					if (ioXQ.opened())
					{
						timerCarStatus->stop();
						ioXQ.close();
						pushButtonOpen->setText("Open");
						spdlog::info("{} closed with success", ioXQ.name());
					}
					else
					{
						if (ioXQ.open(comboBoxPorts->currentText().toStdString()))
						{
							pushButtonOpen->setText("Close");
							spdlog::info("{} openned with success", ioXQ.name());
							timerCarStatus->start(200);
						}
						else
						{
							spdlog::error("{} openned with failure", ioXQ.name());
							QMessageBox::information(this, "", ioXQ.name().c_str() + QString(" openned with failure"));
						}
					}
				});
			connect(timerCarStatus, &QTimer::timeout, [this]()->void 
				{
					XQ4IO::XQFrame* msg = 0;
					ioXQ.getStatus(&msg);
					if (msg != 0) plainTextEditCarStatus->setPlainText(msg->print().c_str());
					else plainTextEditCarStatus->setPlainText(fmt::format("No data received {}", tsms).c_str());
				});
		}
	}

public:
	QGridLayout* gridLayoutMain = new QGridLayout(this);
	QComboBox* comboBoxPorts = new QComboBox(this);
	QPushButton* pushButtonOpen = new QPushButton("Open", this);
	QSpinBox* spinBoxLinearSpeed = new QSpinBox(this);
	QSpinBox* spinBoxAngularSpeed = new QSpinBox(this);
	QSpinBox* spinBoxBrakeSpeed = new QSpinBox(this);
	QComboBox* comboBoxWorkMode = new QComboBox(this);
	QPushButton* pushButtonMoveHead = new QPushButton("Move head", this);
	QPushButton* pushButtonMoveBack = new QPushButton("Move back", this);
	QPushButton* pushButtonTurnLeft = new QPushButton("Turn left", this);
	QPushButton* pushButtonTurnRight = new QPushButton("Turn right", this);
	QPushButton* pushButtonCalibIMU = new QPushButton("Calib IMU", this);
	QPushButton* pushButtonEnableIR = new QPushButton("Enable IR", this);
	QPushButton* pushButtonDisableIR = new QPushButton("Disable IR", this);
	QPushButton* pushButtonPlusMotor1 = new QPushButton("Plus motorR", this);
	QPushButton* pushButtonMinusMotor1 = new QPushButton("Minus motorR", this);
	QPushButton* pushButtonPlusMotor2 = new QPushButton("Plus motorL", this);
	QPushButton* pushButtonMinusMotor2 = new QPushButton("Minus motorL", this);
	QPushButton* pushButtonPlusMotors = new QPushButton("Plus motors", this);
	QPushButton* pushButtonMinusMotors = new QPushButton("Minus motors", this);
	QPushButton* pushButtonEnableROS2 = new QPushButton("Start ROS2", this);
	QPlainTextEdit* plainTextEditCarStatus = new QPlainTextEdit("", this);
	QTimer* timerCarStatus = new QTimer(this);
};

int main(int argc, char** argv) { XQ4RC::RunMe(argc, argv); return 0; }