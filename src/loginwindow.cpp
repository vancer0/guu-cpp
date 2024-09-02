#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginWindow) {
  ui->setupUi(this);
  this->resize(1, 1);
  this->setMaximumSize(this->size());
  ui->loginBtn->setShortcut(tr("Return"));
}

QPushButton *LoginWindow::getButton() { return ui->loginBtn; }

std::string LoginWindow::getUsername() {
  std::string s = ui->username->text().toStdString();
  ui->username->clear();
  return s;
}

std::string LoginWindow::getPassword() {
  std::string s = ui->password->text().toStdString();
  ui->password->clear();
  return s;
}

bool LoginWindow::shouldSave() {
  bool s = ui->credSave->isChecked();
  ui->credSave->setChecked(false);
  return s;
}

LoginWindow::~LoginWindow() { delete ui; }
