#ifndef HELPHELP_H
#define HELPHELP_H
#ifdef Q_OS_WIN
#define snprintf _snprintf
#endif
#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class HelpHelp;
}

class HelpHelp : public QDialog
{
    Q_OBJECT

public:
    explicit HelpHelp(QWidget *parent = 0);
    ~HelpHelp();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::HelpHelp *ui;
};

#endif // HELPHELP_H
