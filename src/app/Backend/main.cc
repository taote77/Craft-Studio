#include <qbus/application.h>

#include <QDebug>
#include <QtCore>
#include <dlfcn.h>
#include <qglobal.h>

/*******************************************************************************************
 *   nm  -D   ~/Qt/5.15.2/gcc_64/lib/libQt5Core.so.5.15  | grep  toString
 *  0000000000188b50 T _ZNK9QDateTime8toStringE11QStringView@@Qt_5
 *  0000000000188ae0 T _ZNK9QDateTime8toStringE11QStringView9QCalendar@@Qt_5
 *  000000000018e740 T _ZNK9QDateTime8toStringEN2Qt10DateFormatE@@Qt_5
 *  0000000000188ba0 T _ZNK9QDateTime8toStringERK7QString@@Qt_5
 *  0000000000188bf0 T _ZNK9QDateTime8toStringERK7QString9QCalendar@@Qt_5
 *  000000000012efc0 T _ZN7QLocale6systemEv@@Qt_5
 *  0000000000183f20 T _ZNK5QDate8toStringE11QStringView@@Qt_5
 *  0000000000183eb0 T _ZNK5QDate8toStringE11QStringView9QCalendar@@Qt_5
 *  0000000000183ac0 T _ZNK5QDate8toStringEN2Qt10DateFormatE@@Qt_5
 *  0000000000183cb0 T _ZNK5QDate8toStringEN2Qt10DateFormatE9QCalendar@@Qt_5
 *  0000000000183f70 T _ZNK5QDate8toStringERK7QString@@Qt_5
 *  0000000000183fc0 T _ZNK5QDate8toStringERK7QString9QCalendar@@Qt_5
 *  0000000000184af0 T _ZNK5QTime8toStringE11QStringView@@Qt_5
 *  0000000000184940 T _ZNK5QTime8toStringEN2Qt10DateFormatE@@Qt_5
 *  0000000000184b60 T _ZNK5QTime8toStringERK7QString@@Qt_5
 *
 ********************************************************************************************/

#define TIME_FUNC(CLASS, NAME, CL, FL, PL) _ZNK##CL##CLASS##FL##NAME##ERK##PL##QString
#define LOCAL_FUNC(CLASS, NAME, CL, FL) _ZN##CL##CLASS##FL##NAME##Ev
#define LIB_QT_CORE_INL(major, minor, patch) "libQt" #major "Core.so." #major "." #minor "." #patch
#define LIB_QT_CORE_OUT(major, minor, patch) LIB_QT_CORE_INL(major, minor, patch)
#define LIB_QT_CORE LIB_QT_CORE_OUT(QT_VERSION_MAJOR, QT_VERSION_MINOR, QT_VERSION_PATCH)

#include <QDateTime>

static QRecursiveMutex& getMutex()
{
  static QRecursiveMutex mutex;
  qDebug() << "==QTime::toString ========";
  return mutex;
}

#define FUNCTION_BODY(X, ...)                                                                      \
  static X pFunc = (X)dlsym(dlopen(LIB_QT_CORE, RTLD_LAZY), __FUNCTION__);                         \
  getMutex().lock();                                                                               \
  auto res = pFunc(__VA_ARGS__);                                                                   \
  getMutex().unlock();                                                                             \
  return res;

extern "C" QString TIME_FUNC(QDateTime, toString, 9, 8, 7)(
  QDateTime* pDateTime, const QString& format)
{
  using DT2S = QString (*)(QDateTime*, const QString&);
  FUNCTION_BODY(DT2S, pDateTime, format)

  qDebug() << "QDateTime::toString" << format;
}

extern "C" QString TIME_FUNC(QTime, toString, 5, 8, 7)(QTime* pTime, const QString& format)
{
  using T2S = QString (*)(QTime*, const QString&);
  FUNCTION_BODY(T2S, pTime, format)

  qDebug() << "QTime::toString" << format;
}

extern "C" QString TIME_FUNC(QDate, toString, 5, 8, 7)(QDate* pDate, const QString& format)
{
  using D2S = QString (*)(QDate*, const QString&);
  FUNCTION_BODY(D2S, pDate, format)

  qDebug() << "QDate::toString" << format;
}

extern "C" QLocale LOCAL_FUNC(QLocale, system, 7, 6)()
{
  using LS = QLocale (*)();
  FUNCTION_BODY(LS)

  qDebug() << "QDate::QLocale";
}

int main(int argc, char** argv)
{
  qbus::Application app(argc, argv);

  QDateTime now = QDateTime::currentDateTime();

  now.toString("yyyy-MM-dd hh:mm:ss");

  qDebug() << now.toString("yyyy-MM-dd hh:mm:ss");

  qDebug() << "QT_VERSION" << QT_VERSION << QT_VERSION_MAJOR << QT_VERSION_MINOR
           << QT_VERSION_PATCH;

  return app.exec();
}
