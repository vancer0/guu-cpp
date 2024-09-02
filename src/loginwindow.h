#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QPushButton>
#include <QWidget>

#include <string>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget {
  Q_OBJECT

public:
  explicit LoginWindow(QWidget *parent = nullptr);
  QPushButton *getButton();
  std::string getUsername();
  std::string getPassword();
  bool shouldSave();
  ~LoginWindow();

private:
  Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
