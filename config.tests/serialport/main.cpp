#include <stdio.h>
#include <stdlib.h>

#include <QSerialPort>

int main(void)
{

	auto serialPort = new QSerialPort();

	delete serialPort;

	return 0;
}

