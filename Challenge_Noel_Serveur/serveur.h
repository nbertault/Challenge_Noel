#ifndef SERVEUR_H
#define SERVEUR_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QTcpServer>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QRandomGenerator>
#include <QBuffer>
#include "joueur.h"

#define NOMBRE_PERSONNAGES 15
#define NOMBRE_DE_TOURS 4

enum ETAT {
    ATTENTE,
    DISTRIBUTION,
    RECONNEXION,
    FIN,
    CALCUL_SCORE,
    DEDUCTION,
    ECHANGE_PROPOSITION
};

QT_BEGIN_NAMESPACE
namespace Ui {
class Serveur;
}
QT_END_NAMESPACE

class Serveur : public QWidget
{
    Q_OBJECT

public:
    Serveur(QWidget *parent = nullptr);
    ~Serveur();
    void LancerServeur();

private slots:

    void on_pushButtonConnexion_clicked();
    void on_comboBoxDatabase_currentTextChanged(const QString &arg1);
    void onQWebSocketServerNewConnexion();
    void onQWebSocketDisconnected();
    void onQWebSocketTextMessageReceived(const QString &msg);
private:
    Ui::Serveur *ui;
    QSqlDatabase db;
    QSqlTableModel model;
    QWebSocketServer *serveur;
    QList<Joueur*> lesJoueurs;
    Joueur *joueur;
    ETAT etat;
    QStringList listePersonnage;
    QStringList listeMot;
    int tour, index;
};
#endif // SERVEUR_H
