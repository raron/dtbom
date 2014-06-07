#include "loadform.h"
#include "ui_loadform.h"

LoadForm::LoadForm(QSqlDatabase _db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadForm),
    db(_db),
    query(db)
{
    ui->setupUi(this);
}

LoadForm::~LoadForm()
{
    delete ui;
}

void LoadForm::setQuery(QString q)
{
    query.exec(q);
    model.setQuery(query);
    ui->partlist->setModel(&model);
}

void LoadForm::setStandardButtons(QDialogButtonBox::StandardButtons b)
{
    ui->buttonBox->setStandardButtons(b);
}
