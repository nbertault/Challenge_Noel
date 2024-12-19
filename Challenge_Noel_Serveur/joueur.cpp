#include "joueur.h"

Joueur::Joueur(QObject *parent)
    : QObject(parent)
{}

void Joueur::setPseudo(const QString &newPseudo)
{
    pseudo = newPseudo;
}

QString Joueur::getPseudo() const
{
    return pseudo;
}

void Joueur::setPersonnage(const QString &newPersonnage)
{
    personnage = newPersonnage;
}

QString Joueur::getPersonnage() const
{
    return personnage;
}

void Joueur::setAdresseIp(const QString &newAdresseIp)
{
    adresseIp = newAdresseIp;
}

QString Joueur::getAdresseIp() const
{
    return adresseIp;
}

void Joueur::EnvoyerMessage(QChar _commande, QString _donnees)
{
    QJsonObject obj;

    obj.insert("commande",QJsonValue(_commande));
    obj.insert("donnees",QJsonValue(_donnees));

    QJsonDocument doc(obj);

    client->sendTextMessage(doc.toJson(QJsonDocument::Compact).toStdString().c_str());
}

void Joueur::setClient(QWebSocket *newClient)
{
    client = newClient;
}

QWebSocket *Joueur::getClient() const
{
    return client;
}

void Joueur::setIndex(int newIndex)
{
    index = newIndex;
}

int Joueur::getIndex() const
{
    return index;
}
