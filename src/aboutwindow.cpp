#include "aboutwindow.h"
#include "constants.h"
#include "ui_aboutwindow.h"
#include <string>

AboutWindow::AboutWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::AboutWindow) {
  ui->setupUi(this);
  this->resize(1, 1);
  this->setMaximumSize(this->size());
  std::string s = "<html><head/><body><p><span style=\" font-size:18pt;\">";
  QString ss = QString::fromStdString(s + "Gaytor.rent Upload Utility v" +
                                      std::to_string(VERSION) +
                                      "</span></p></body></html>");
  ui->version->setText(ss);
}

AboutWindow::~AboutWindow() { delete ui; }
