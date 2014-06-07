#include "importcsv.h"
#include "ui_importcsv.h"

ImportCSV::ImportCSV(QFile *file, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportCSV),
    stream(file)
{
    ui->setupUi(this);
    reRead();
}

ImportCSV::~ImportCSV()
{
    delete ui;
}

int ImportCSV::getDataCount() const
{
    return data.count();
}

ImportCSV::Info ImportCSV::getData(int i)
{
    Info info;
    info.refdes = data[i][ui->refdes->value()];

    if ((ui->part->value() >= 0) && (ui->part->value() < header.size()))
    {
        info.part = data[i][ui->part->value()];
    }
    if ((ui->type->value() >= 0) && (ui->type->value() < header.size()))
    {
        info.type = data[i][ui->type->value()];
    }
    if ((ui->value->value() >= 0) && (ui->value->value() < header.size()))
    {
        info.value = data[i][ui->value->value()];
    }
    if ((ui->pattern->value() >= 0) && (ui->pattern->value() < header.size()))
    {
        info.pattern = data[i][ui->pattern->value()];
    }
    if ((ui->shop->value() >= 0) && (ui->shop->value() < header.size()))
    {
        info.shop = data[i][ui->shop->value()];
    }
    if ((ui->shopid->value() >= 0) && (ui->shopid->value() < header.size()))
    {
        info.shopid = data[i][ui->shopid->value()];
    }

    return info;
}

void ImportCSV::reRead()
{
    data.clear();
    header.clear();
    maxFileldNo = 0;

    int skiplines = ui->skiplines->value();

    stream.seek(0);
    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        if (skiplines > 0)
        {
            skiplines--;
            continue;
        }
        if (ui->skipEmptyLines->isChecked() && line.isEmpty())
        {
            continue;
        }

        QStringList list = parseLine(line, ui->fieldseparator->text(), ui->stringseparator->text());
        if (list.size() > maxFileldNo)
            maxFileldNo = list.size();
        data << list;
    }


    if (data.size() > 0)
    {
        if (ui->header->isChecked())
        {
            header = data[0];
            data.removeFirst();

            ui->refdes->setValue(header.indexOf(QRegExp("refdes", Qt::CaseInsensitive)));
            ui->part->setValue(header.indexOf(QRegExp("part", Qt::CaseInsensitive)));
            ui->type->setValue(header.indexOf(QRegExp("type", Qt::CaseInsensitive)));
            ui->value->setValue(header.indexOf(QRegExp("value", Qt::CaseInsensitive)));
            ui->pattern->setValue(header.indexOf(QRegExp("pattern", Qt::CaseInsensitive)));
            ui->shop->setValue(header.indexOf(QRegExp("shop", Qt::CaseInsensitive)));
            ui->shopid->setValue(header.indexOf(QRegExp("shopid", Qt::CaseInsensitive)));

        }
        ui->table->clear();
        ui->table->setRowCount(data.size());
        ui->table->setColumnCount(maxFileldNo);

        for(int i = 0; i < data.size(); i++)
        {
            for(int j = 0; j < data[i].size(); j++)
            {
                ui->table->setItem(i, j, new QTableWidgetItem(data[i][j]));
            }
        }
        updateHeader();
    }
}

void ImportCSV::updateHeader()
{
    QStringList showheader;

    for(int i = 0; i < maxFileldNo; i++)
    {
        QString head;
        if (i < header.size())
            head = header[i];
        if (i == ui->refdes->value())
        {
            showheader << "RefDes" + QString(" [%1] (%2)").arg(head).arg(i);
        }
        else if (i == ui->part->value())
        {
            showheader << "Part" + QString(" [%1] (%2)").arg(head).arg(i);
        }
        else if (i == ui->type->value())
        {
            showheader << "Type" + QString(" [%1] (%2)").arg(head).arg(i);
        }
        else if (i == ui->value->value())
        {
            showheader << "Value" + QString(" [%1] (%2)").arg(head).arg(i);
        }
        else if (i == ui->pattern->value())
        {
            showheader << "Pattern" + QString(" [%1] (%2)").arg(head).arg(i);
        }
        else if (i == ui->shop->value())
        {
            showheader << "Shop" + QString(" [%1] (%2)").arg(head).arg(i);
        }
        else if (i == ui->shopid->value())
        {
            showheader << "ShopId" + QString(" [%1] (%2)").arg(head).arg(i);
        }
        else if (ui->header->isChecked())
        {
            showheader << QString(" [%1] (%2)").arg(head).arg(i);
        }
        else
        {
            showheader << QString("(%1)").arg(i);
        }
    }
    ui->table->setHorizontalHeaderLabels(showheader);
    ui->table->resizeColumnsToContents();
}

void ImportCSV::done(int r)
{
    if (r == ImportCSV::Accepted)
    {
        if ((ui->refdes->value() >= 0) && (ui->refdes->value() < header.size()))
        {
            QDialog::done(r);
        }
        else
        {
            QMessageBox::warning(this, "szar", "szar a refdes", QMessageBox::Ok);
        }
    }
    else
    {
        QDialog::done(r);
    }
}

QStringList ImportCSV::parseLine(QString line, QString fieldsep, QString stringsep)
{
    QStringList list;
    QStringList tmpList = line.split(QRegExp(stringsep)); // Split by "
    bool inside = false;
    list.clear();
    foreach (QString s, tmpList)
    {
        if (inside)
        { // If 's' is inside quotes ...
            list.append(s); // ... get the whole string
        }
        else
        { // If 's' is outside quotes ...
            list.append(s.split(QRegExp(fieldsep), QString::SkipEmptyParts)); // ... get the splitted string
        }
        inside = !inside;
    }
    return list;
}

void ImportCSV::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;
    QDialog::keyPressEvent(event);
}

void ImportCSV::on_reread_clicked()
{
    reRead();
}
