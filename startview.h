#ifndef STARTVIEW_H
#define STARTVIEW_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QWidget>

namespace Ui {
class StartView;
}

class StartView : public QWidget
{
    Q_OBJECT

public:
    explicit StartView(QWidget *parent = 0);
    ~StartView();

private:
    Ui::StartView *ui;
};

#endif // STARTVIEW_H
