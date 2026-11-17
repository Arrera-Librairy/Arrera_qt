#ifndef TEST_UI_H
#define TEST_UI_H

#include <QMainWindow>

namespace Ui {
class test_ui;
}

class test_ui : public QMainWindow
{
    Q_OBJECT

public:
    explicit test_ui(QWidget *parent = nullptr);
    ~test_ui();

private:
    Ui::test_ui *ui;
};

#endif // TEST_UI_H
