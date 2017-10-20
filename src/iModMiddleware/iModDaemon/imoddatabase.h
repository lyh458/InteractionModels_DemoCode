/**
* @file iModDaemon/imoddatabase.h
* @brief This file contains implementation for the database communication
*/


#ifndef IMODDATABASE_H
#define IMODDATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlRecord>

class QSqlDatabase;
class iModUtile;
/**
 * @class iModDatabase
 * @brief This class connects to a database server and is able to load/store/playback recordings.
 */
class iModDatabase : public QObject
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of iModDatabase
     * @param _utile Pointer to utility class to be able to read data from the config file
     * @param parent Parent of this instance
     */
    explicit iModDatabase(iModUtile* _utile, QObject *parent = 0);
    ~iModDatabase();

    /**
     * @brief Load the categorys from the database and return the result.
     * @return Returns a list of all categorys.
     */
    QStringList getCategorys() { updateCategorys(); return categorylist; }
    /**
     * @brief Loads the table names from a certain category.
     * @param category Category to load from.
     * @return List of tables.
     */
    QStringList getTablesFromCategory(QString category);
    /**
     * @brief Loading a table into local memory.
     * @details The table is loaded into local memory to bypass network latency. If you want to replay with 60 FPS a sql-server may be too slow to provide the data that fast, therefore this function loads the data, creates an A.R.T. stream and stores it in the memory.
     * @warning Loading a table can take some time depending on the network latency and the size of the recording.
     * @param category Category to load from.
     * @param name Tablename inside the category.
     */
    void loadTable(QString category, QString name);
    /**
     * @brief Gets the next A.R.T. frame (without framenumber)
     * @details The buffer which stores the parsed recording is a ring buffer and will start from the beginning, if the recording played one time.
     * @warning This will return a 6d line, you are responsible to add a framenumber 'fr /number/\n' on your own.
     * @return Returns a 6d tracking string.
     */
    QString getNextFrame();
    /**
     * @brief Writes the recording to the database
     * @details This function starts an interaction and rerolls it, if something fails. It is recommendet to check, if this function realy stored your recording.
     * @warning Be aware, that different servers can have different sizes of a single SQL-statement and this function will fail when sending a statement. The current limit is 500.000 characters.
     * @bug The transaction is not working as expected.
     * @param category Desiret category. (Has to exist)
     * @param name Desired table name. (Is created if necessary)
     * @param data Contains a list of A.R.T. frames
     * @param art Set this to true, if you want to store an A.R.T. recording
     */
    void writeRecording(QString category, QString name, QStringList data, bool art);
    /**
     * @brief Returns, if the database is bussy or not.
     * @return Database state.
     */
    bool databaseBussy() { return databasebussy; }
    /**
     * @brief Gets the currently loaded category.
     * @return Returns category
     */
    QString getCurrentCategory() { return currentcategory; }
    /**
     * @brief Gets the current table.
     * @return Returns table
     */
    QString getCurrentTable() { return currenttable; }
    /**
     * @brief Returns the status of the database for the web interface
     * @return Status
     */
    QString getStatusString();

  signals:

  public slots:

  private:
    /**
     * @brief Loads the list of categorys from the database and stores in in categorylist
     */
    void updateCategorys();
    /**
     * @brief Reads a double from a bytearray and converts it to a string.
     * @param bytes The bytearray
     * @param offset Offset added to the bytearray
     * @return Returns the double as string.
     */
    QString readDouble(char* bytes, int* offset);

    /**
     * @brief SQL database driver
     */
    QSqlDatabase database;
    /**
     * @brief List of categorys
     */
    QStringList categorylist;
    /**
     * @brief When parsing data from the database, the 6d frames are stored in this list
     */
    QStringList frames;
    /**
     * @brief Holds a pointer to the utility class.
     */
    iModUtile* utile;
    /**
     * @brief Holds the current position the the framelist
     */
    int currentframe;
    /**
     * @brief Is set to true, if the database is bussy.
     */
    bool databasebussy;
    /**
     * @brief Name of the current category
     */
    QString currentcategory;
    /**
     * @brief Name of the current table.
     */
    QString currenttable;
};

#endif // IMODDATABASE_H
