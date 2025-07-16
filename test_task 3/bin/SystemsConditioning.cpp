#include "SystemsConditioning.h"
#include "TemperatureWidget.h"
#include "PressureWidget.h"
#include "HumidityWidget.h"
#include <QXmlStreamWriter>
#include <QFile>

SystemsConditioning::SystemsConditioning(const Parametrs &parametr)
    : parametr(parametr), temper_widget(nullptr) {}

SystemsConditioning::SystemsConditioning(Parametrs &&parametr)
    : parametr(std::move(parametr)), temper_widget(nullptr) {}

SystemsConditioning::SystemsConditioning(const SystemsConditioning &other)
    : parametr(other.parametr), temper_widget(nullptr) {}

void SystemsConditioning::Set_Widget(TemperatureWidget *temper_widget, PressureWidget *pressure_widget, HumidityWidget *humid_widget)
{
    this->temper_widget = temper_widget;
    this->pressure_widget = pressure_widget;
    this->humid_widget = humid_widget;
}

SystemsConditioning &SystemsConditioning::operator=(const SystemsConditioning &other)
{
    if (this != &other)
    {
        parametr = other.parametr;
    }
    return *this;
}

void SystemsConditioning::ON()
{
    status_ = Status::on;
}

void SystemsConditioning::OFF()
{
    status_ = Status::Off;
}

void SystemsConditioning::Update_Dimension(TempScale newscale)
{
    parametr.convert(newscale);
    qDebug() << "System: Temperature scale changed to"
             << parametr.temperature_.Get_Dimension().c_str();
}
void SystemsConditioning::Update_Dimension(PressureUnit newscale)
{
    parametr.convert(newscale);
    qDebug() << "System: Temperature scale changed to"
             << parametr.pressure_.Get_Dimension().c_str() << " " << static_cast<int>(newscale);
}

void SystemsConditioning::Change_Parametrs(const Parametrs &prmtr)
{
    parametr = prmtr;
    if (temper_widget)
    {
        temper_widget->update(parametr.temperature_);
    }
    if (pressure_widget)
    {
        pressure_widget->update(parametr.pressure_);
    }
    if (humid_widget)
    {
        humid_widget->update(parametr.humidity_);
    }
    qDebug() << "System: " << parametr.temperature_.value;
}

template <>
Parametrs SystemsConditioning::Get_Parametr<Parametrs>() const
{
    return parametr;
}

template <>
Temperature SystemsConditioning::Get_Parametr<Temperature>() const
{
    return parametr.temperature_;
}

template <>
Pressure SystemsConditioning::Get_Parametr<Pressure>() const
{
    return parametr.pressure_;
}

template <>
Humidity SystemsConditioning::Get_Parametr<Humidity>() const
{
    return parametr.humidity_;
}

void SystemsConditioning::SaveXml(const QString &filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file for writing:" << filename;
        return;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement("SystemConditioning");

    xml.writeTextElement("Status", status_ == Status::on ? "on" : "off");

    xml.writeStartElement("Blocks");
    for (size_t i = 0; i < block.size(); ++i)
    {
        xml.writeStartElement("Block");
        xml.writeAttribute("id", QString::number(i));
        xml.writeCharacters(block[i] ? "true" : "false");
        xml.writeEndElement(); 
    }
    xml.writeEndElement();

    xml.writeStartElement("Parameters");

    xml.writeStartElement("Temperature");
    xml.writeAttribute("unit", QString::fromStdString(parametr.temperature_.Get_Dimension()));
    xml.writeCharacters(QString::number(parametr.temperature_.value));
    xml.writeEndElement(); 

    xml.writeStartElement("Pressure");
    xml.writeAttribute("unit", QString::fromStdString(parametr.pressure_.Get_Dimension()));
    xml.writeCharacters(QString::number(parametr.pressure_.value));
    xml.writeEndElement();

    xml.writeStartElement("Humidity");
    xml.writeCharacters(QString::number(parametr.humidity_.value));
    xml.writeEndElement();

    xml.writeEndElement(); 

    xml.writeEndElement(); 
    xml.writeEndDocument();

    file.close();
}

void SystemsConditioning::LoadXml(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw std::runtime_error("Failed to open file for reading: " + filename.toStdString());
    }
    
    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument)
            continue;

        if (token == QXmlStreamReader::StartElement && xml.name() == "SystemConditioning")
        {
            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == "SystemConditioning"))
            {
                xml.readNext();

                if (xml.name() == "Status")
                {
                    QString status = xml.readElementText();
                    status_ = (status == "on") ? Status::on : Status::Off;
                }
                else if (xml.name() == "Blocks")
                {
                    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                             xml.name() == "Blocks"))
                    {
                        xml.readNext();

                        if (xml.tokenType() == QXmlStreamReader::StartElement &&
                            xml.name() == "Block")
                        {
                            int id = xml.attributes().value("id").toInt();
                            QString state = xml.readElementText();

                            if (id >= 0 && id < static_cast<int>(block.size()))
                            {
                                block[id] = (state == "true");
                            }
                        }
                    }
                }
                else if (xml.name() == "Parameters")
                {
                    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                             xml.name() == "Parameters"))
                    {
                        xml.readNext();

                        if (xml.name() == "Temperature")
                        {
                            QString unit = xml.attributes().value("unit").toString();
                            double value = xml.readElementText().toDouble();

                            parametr.temperature_.Set_Dimension(unit.toStdString());
                            parametr.temperature_.value = value;
                        }
                        else if (xml.name() == "Pressure")
                        {
                            QString unit = xml.attributes().value("unit").toString();
                            double value = xml.readElementText().toDouble();

                            parametr.pressure_.Set_Dimension(unit.toStdString());
                            parametr.pressure_.value = value;
                        }
                        else if (xml.name() == "Humidity")
                        {
                            double value = xml.readElementText().toDouble();
                            parametr.humidity_.value = value;
                        }
                    }
                }
            }
        }
    }   
    
    if (xml.hasError())
    {
        throw std::runtime_error("XML parsing error: " + xml.errorString().toStdString());
    }

    file.close();

    if (temper_widget)
        temper_widget->update(parametr.temperature_);
    if (pressure_widget)
        pressure_widget->update(parametr.pressure_);
    if (humid_widget)
        humid_widget->update(parametr.humidity_);
    
}
