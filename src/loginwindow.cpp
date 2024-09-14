#include "loginwindow.h"
#include "ui_loginwindow.h"

#include "types.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginWindow) {
  ui->setupUi(this);
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);
  this->setFixedSize(this->minimumSizeHint());
  ui->loginBtn->setShortcut(tr("Return"));
  connect(ui->loginBtn, &QPushButton::pressed, this, &LoginWindow::login);
}

void LoginWindow::setData(API *api, Settings *cfg) {
  Api = api;
  Cfg = cfg;
}

void LoginWindow::login() {
  if (Api == nullptr || Cfg == nullptr)
    return;

  String username = ui->username->text().toStdString();
  String password = ui->password->text().toStdString();
  ui->password->clear();

  qInfo() << "Sending login info";

  if (username.empty() || password.empty())
    return;

  if (Api->login(username, password)) {
    this->hide();
    ui->username->clear();
    if (ui->credSave->isChecked()) {
      Cfg->saveLogin = true;
      Cfg->gtUsername = username;
      Cfg->gtPassword = password;
      Cfg->save();
    }
    qInfo() << "Logged in as" << QString::fromStdString(username);
    emit loggedIn();
  } else {
    qWarning() << "Could not login:" << Api->getLastStatusCode()
               << Api->getLastError().message;

    QString msg = "Could not login. ";
    if (Api->getLastStatusCode() == 404)
      msg += "Please check your credentials and try again.";
    else
      msg += "Check the logs for more information";
    QMessageBox::warning(this, "GUU - Error", msg);
  }
}

LoginWindow::~LoginWindow() { delete ui; }
