#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#include <QDialog>

namespace Ui {
class SystemInformation;
}

class SystemInformation : public QDialog
{
    Q_OBJECT

public:
    explicit SystemInformation(QWidget *parent = 0);
    ~SystemInformation();

private:
    Ui::SystemInformation *ui;
};

#endif // SYSTEMINFORMATION_H
