#ifndef JOUEUR_H
#define JOUEUR_H

#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>

class Joueur : public QObject
{
  Q_OBJECT

public:
    explicit Joueur(QObject *parent = nullptr);

    void setPseudo(const QString &newPseudo);

    QString getPseudo() const;

    void setPersonnage(const QString &newPersonnage);

    QString getPersonnage() const;

    void setAdresseIp(const QString &newAdresseIp);

    QString getAdresseIp() const;

    void EnvoyerMessage(QChar _commande, QString _donnees);

    void setClient(QWebSocket *newClient);

    QWebSocket *getClient() const;

    void setIndex(int newIndex);

    int getIndex() const;

private:
    QString pseudo;
    QString personnage;
    QString adresseIp;
    int index;
    QWebSocket *client;
};

#endif // JOUEUR_H
