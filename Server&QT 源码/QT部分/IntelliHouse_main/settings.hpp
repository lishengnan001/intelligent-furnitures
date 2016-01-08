#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QObject>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
public:
    QString getValue(const QString& key);
    static Settings* getSettings();
    static void releaseSettings();
private:
    explicit Settings(QObject *parent = 0);
    explicit Settings(const Settings& s);
    Settings& operator=(const Settings& s);
    ~Settings();
    static Settings *pSetting;
    QSettings* pSettings;
    
signals:
    
public slots:
    
};

#endif // SETTINGS_HPP
