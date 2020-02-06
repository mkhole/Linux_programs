#include <stdio.h>
#include "template.h"

void CarStep1(void){ printf("Car Step1!\n"); }
void CarStep2(void){ printf("Car Step2\n"); }
void CarStep3(void){ printf("Car Step3\n"); }
void CarStep4(void){ printf("Car Step4\n"); }

void PlaneStep1(void){ printf("Plane Step1\n"); }
void PlaneStep2(void){ printf("Plane Step2\n"); }
void PlaneStep3(void){ printf("Plane Step3\n"); }
void PlaneStep4(void){ printf("Plane Step4\n"); }

void DriveAutomobile(Drive drive)
{
    drive.one(); drive.two(); drive.three(); drive.four();
}

int main()
{
    Drive car = {
		.one = CarStep1,
		.two = CarStep2,
		.three = CarStep3,
		.four = CarStep4
	};
	
	Drive plane = {
		.one = PlaneStep1,
		.two = PlaneStep2,
		.three = PlaneStep3,
		.four = PlaneStep4
	};

    printf("This is Car!!\n");
    DriveAutomobile(car);

    printf("This is Plane!!\n");
    DriveAutomobile(plane);
    return 0;
}
