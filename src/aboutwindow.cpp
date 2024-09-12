#include "aboutwindow.h"
#include "constants.h"
#include "libtorrent/version.hpp"
#include "ui_aboutwindow.h"
#include "utils.h"
#include <QDesktopServices>
#include <QStringLiteral>
#include <cpr/cprver.h>
#include <cpr/bearer.h>
#include <string>

AboutWindow::AboutWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::AboutWindow) {
  ui->setupUi(this);

  QString txt = "<html><head/><body><p><span style=\" font-size:18pt;\">";
  txt += "GayTor.rent Upload Utility v" + QString::number(VERSION) +
         "</span></p></body></html>";
  ui->version->setText(txt);

  connect(ui->updateCheck, &QPushButton::pressed, this,
          []() { utils::checkForUpdates(true); });
  connect(ui->ghRepo, &QPushButton::pressed, this, []() {
    QDesktopServices::openUrl(
        QUrl("https://github.com/vancer0/guu-cpp", QUrl::TolerantMode));
  });

  ui->platform->setText(qApp->platformName());
  ui->guuVersion->setText(QString::number(VERSION));
  ui->ltVer->setText(QString::fromLatin1(lt::version()));
  ui->qtVer->setText(QStringLiteral(QT_VERSION_STR));
  ui->cprVer->setText(QStringLiteral(CPR_VERSION));
  ui->curlVer->setText(QString(LIBCURL_VERSION));

  this->layout()->setSizeConstraint(QLayout::SetFixedSize);
  this->setFixedSize(this->minimumSizeHint());
}

AboutWindow::~AboutWindow() { delete ui; }
