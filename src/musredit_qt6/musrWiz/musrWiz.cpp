#include <iostream>
#include <fstream>

#include <QApplication>
#include <QProcess>
#include <QMessageBox>
#include <QObject>

#ifdef HAVE_GIT_REV_H
#include "git-revision.h"
#endif

#include "musrWiz.h"
#include "PAdmin.h"
#include "PMusrWiz.h"

//-------------------------------------------------------------------
/**
 *
 */
void PParam::init()
{
  fName = QString("UnDef");
  fNumber = -1;
  fValue = 0.0;
  fStep = 0.0;
  fPosErr = QString("UnDef");
  fBoundLow = QString("UnDef");
  fBoundHigh = QString("UnDef");
}

//-------------------------------------------------------------------
/**
 *
 */
void musrWiz_syntax()
{
  std::cout << "usage: musrWiz [[--version | -v] | --debug [0|1|2] | --help]" << std::endl << std::endl;
  std::cout << "       --version | -v : print current git-version." << std::endl;
  std::cout << "       --debug 0 : dump's the instrument definition(s) at startup." << std::endl;
  std::cout << "       --debug 1 : dump's the musrfit functions at startup." << std::endl;
  std::cout << "       --debug 2 : dump's the musrfit functions and instrument definition(s) at startup." << std::endl;
  std::cout << "       --log     : writes a log-file 'musrWiz.log' which contains the path-file-name of" << std::endl;
  std::cout << "                   the created msr-file." << std::endl;
  std::cout << "       --help    : shows this help." << std::endl << std::endl;
}

//-------------------------------------------------------------------
/**
 *
 */
int main(int argc, char *argv[])
{
  int dump = -1;
  bool logFile = false;

  if (argc == 2) {
    if (!strcmp(argv[1], "--version") || (!strcmp(argv[1], "-v"))) {
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "musrWiz - git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#else
      std::cout << std::endl << "musrWiz - git-branch: unknown, git-rev: unknown." << std::endl << std::endl;
#endif
      return 0;
    } else if (!strcmp(argv[1], "--help")) {
      musrWiz_syntax();
      return 0;
    } else if (!strcmp(argv[1], "--log")) { // thought to be used from within musredit only
      logFile = true;
    } else {
      musrWiz_syntax();
      return 0;
    }
  } else if (argc == 3) {
    dump = atoi(argv[2]);
    if ((dump < 0) || (dump > 2)) {
      musrWiz_syntax();
      return 0;
    }
  }

  Q_INIT_RESOURCE(musrWiz);

  QApplication app(argc, argv);

  PAdmin *admin = new PAdmin();
  if (!admin->IsValid()) {
    delete admin;
    return 1;
  }
  admin->dump(dump);

  PMsrData *info = new PMsrData(); // this content will eventually be set by the xml-handler

  PMusrWiz wizard(admin, info);
  wizard.show();

  app.exec();

  int result = wizard.result();

  if (result == 1) { // if everything went fine up to this point
    // check if a log-file shall be written
    QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
    QString pathName = procEnv.value("HOME", "");
    pathName += "/.musrfit/musredit/musrWiz.log";
    if (logFile) {
      std::ofstream fout(pathName.toLatin1().data(), std::ofstream::out);
      fout << "path-file-name: " << info->getMsrPathName().toLatin1().data() << "/" << info->getMsrFileName().toLatin1().data() << std::endl;
      fout.close();
    }

    // check if musrt0 shall be called. If the option --log is set, only add musrt0 flag to in the musrWiz.log file
    if (info->getT0Tag() == T0_FROM_MUSR_T0) {
      if (logFile) {
        std::ofstream fout(pathName.toLatin1().data(), std::ofstream::app);
        fout << "musrt0-tag: yes" << std::endl;
        fout.close();
      } else {
        QString musrt0Path = procEnv.value("MUSRFITPATH", "??");
        if (musrt0Path == "??") { // not found hence try ROOTSYS
          musrt0Path = procEnv.value("ROOTSYS", "??");
          if (musrt0Path != "??") {
              musrt0Path += "/bin";
            }
        }

        if (musrt0Path != "??") {
          QString musrt0 = musrt0Path + "/musrt0";
          QString pathFln = QString("%1/%2").arg(info->getMsrPathName()).arg(info->getMsrFileName());
          QStringList arguments;
          arguments << pathFln;
          QProcess::startDetached(musrt0, arguments, "./");
        } else {
          QMessageBox::warning(nullptr, "WARNING", "Couldn't find musrt0 :-(.\n Only the msr-file has been generated.");
        }
      }
    }
  }

  if (info)
    delete info;
  if (admin)
    delete admin;

  return result;
}
