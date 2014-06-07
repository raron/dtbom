#ifndef LOADFORM_H
#define LOADFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDialogButtonBox>

namespace Ui {
class LoadForm;
}

class LoadForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoadForm(QSqlDatabase _db, QWidget *parent = 0);
    ~LoadForm();

public slots:
    void setQuery(QString q);
    void setStandardButtons(QDialogButtonBox::StandardButtons b);

private:
    Ui::LoadForm *ui;
    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel model;
};

#endif // LOADFORM_H
