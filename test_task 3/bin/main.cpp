#include <QApplication>  


#include "SystemsConditioning.h"
#include"window.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    Pressure initialPres;
    initialPres.value = 101325.0f;
    initialPres.dimension_ = PressureUnit::Pascal;
    
    Temperature initialTemp;
    initialTemp.value = 20.0f;
    initialTemp.dimension_ = TempScale::Celsius;
    
    Humidity initialHum;
    initialHum.value = 50.0f;
    
    Parametrs initialParams{initialPres, initialTemp, initialHum};
    
    SystemsConditioning system(initialParams);
    MainWindow main(&system);
    main.show();
    
    return app.exec();
}