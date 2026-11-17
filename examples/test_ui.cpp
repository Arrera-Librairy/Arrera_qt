#include "test_ui.h"
#include "ui_test_ui.h"

test_ui::test_ui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::test_ui)
{
    ui->setupUi(this);
}

test_ui::~test_ui()
{
    delete ui;
}
