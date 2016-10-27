#ifndef HELPBP_H
#define HELPBP_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QDialog>

namespace Ui {
class HelpBP;
}

class HelpBP : public QDialog
{
    Q_OBJECT

public:
    explicit HelpBP(QWidget *parent = 0);
    ~HelpBP();

private:
    Ui::HelpBP *ui;
};

#endif // HELPBP_H
