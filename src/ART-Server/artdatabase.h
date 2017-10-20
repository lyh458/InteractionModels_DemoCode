#ifndef ARTDATABASE_H
#define ARTDATABASE_H

#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>
#include <QtCore/QStringList>

class ARTRingBuffer;
class ARTDatabase : public QObject
{
    Q_OBJECT

  public:
    ARTDatabase(QObject* parent = 0);
    inline bool isDatabaseOpen() { return databaseopen; }

  signals:
    void signalDisplayLogMessage(QString);
    void signalBufferFinished(bool);
    void signalSendFrame(QString);
    void signalSetTableList(QStringList);

  public slots:
    void slotFillBuffer(QStringList tables);
    void slotClearBuffer();
    void slotResetBuffer();
    void slotSendFrame();
    void slotConnectDatabase(QString host, QString database, QString user, QString password);
    void slotGetAllTables();

  private:
    QSqlDatabase sqldatabase;
    bool databaseopen;
    ARTRingBuffer* buffer;
};

#endif // ARTDATABASE_H
