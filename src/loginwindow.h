#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QPushButton>
#include <QWidget>

#include "api.h"
#include "settings.h"
#include <string>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget {
  Q_OBJECT

public:
  explicit LoginWindow(QWidget *parent = nullptr);
  void setData(API *api, Settings *cfg);

  void login();
  ~LoginWindow();

private:
  Ui::LoginWindow *ui;
  API *Api = nullptr;
  Settings *Cfg = nullptr;

  signals:
  void loggedIn();
};

#endif // LOGINWINDOW_H
