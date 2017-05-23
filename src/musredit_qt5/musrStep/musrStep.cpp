#include <iostream>
#include <cstring>

#include <QApplication>

#include <QtDebug>

#include "git-revision.h"
#include "PMusrStep.h"

//-------------------------------------------------------------------------
void musrStep_syntax()
{
  std::cout << std::endl;
  std::cout << "usage: musrStep <fln> | [-v | --version] | [-h | --help]" << std::endl;
  std::cout << "       <fln>: msr-file name." << std::endl;
  std::cout << "       -v, --version: print the current git-version to the stdout" << std::endl;
  std::cout << "       -h, --help: print this help" << std::endl;
  std::cout << std::endl << std::endl;
}

//-------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  char fln[1024];

  if (argc != 2) {
    musrStep_syntax();
    return 1;
  } else {
    if (!strcmp(argv[1], "--version") || (!strcmp(argv[1], "-v"))) {
      std::cout << std::endl << "musrStep git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
      return 0;
    } else if (!strcmp(argv[1], "--help") || (!strcmp(argv[1], "-h"))) {
      musrStep_syntax();
      return 0;
    } else {
      strncpy(fln, argv[1], sizeof(fln));
    }
  }

  Q_INIT_RESOURCE(musrStep);

  QApplication app(argc, argv);

  PMusrStep musrStep(fln);
  if (!musrStep.isValid())
    return 1;

  musrStep.show();

  app.exec();

  int result = musrStep.result();

  return result;
}
