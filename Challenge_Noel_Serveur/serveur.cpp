#include "serveur.h"
#include "ui_serveur.h"


/* Liste des bugs


 */
Serveur::Serveur(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Serveur)
    , serveur(new QWebSocketServer("Serveur WebSocket Nolhan", QWebSocketServer::NonSecureMode,this))
{
    ui->setupUi(this);

    tour = 0;
    index = 0;

    /* BDD */
    qDebug() << QSqlDatabase::drivers();
    db = QSqlDatabase::addDatabase("QMARIADB");
    db.setHostName("172.18.58.7");
    db.setDatabaseName("FDLM_NMJI"); // Nom de la base de données
    db.setUserName("snir"); // Nom d'utilisateur
    db.setPassword("snir"); // Mot de passe

    if(db.open())
        qDebug() << "Connexion réussie à la base de données.";
    else
        qDebug() << "Erreur lors de la connexion :" << db.lastError().text();

    etat = ATTENTE;
}

Serveur::~Serveur()
{
    delete ui;
    serveur->close();
    qDeleteAll(lesJoueurs);
}

void Serveur::on_pushButtonConnexion_clicked()
{
    if (!serveur->listen(QHostAddress::Any, ui->lineEditPort->text().toInt())) {
        //serveur->close();
        QMessageBox::warning(this, "Erreur au lancement", serveur->errorString());
    }

    if (ui->pushButtonConnexion->text() == "Connexion") {
        connect(serveur, &QWebSocketServer::newConnection, this, &Serveur::onQWebSocketServerNewConnexion);
        qDebug() << "Serveur WebSocket démarré sur le port" << serveur->serverPort();
        ui->pushButtonConnexion->setText("Déconnexion");
    } else {
        ui->pushButtonConnexion->setText("Connexion");
        //serveur->close();
    }
}

void Serveur::onQWebSocketServerNewConnexion()
{
    qDebug() << "Nouveau client connecté.";

    QWebSocket* client = serveur->nextPendingConnection();

    joueur = new Joueur;

    joueur->setAdresseIp(client->peerAddress().toString());
    joueur->setClient(client);
    joueur->setIndex(index);
    lesJoueurs.append(joueur);

    connect(client, &QWebSocket::textMessageReceived,this, &Serveur::onQWebSocketTextMessageReceived);
    connect(client, &QWebSocket::disconnected,this, &Serveur::onQWebSocketDisconnected);

    if (lesJoueurs.count() < 0) {
        foreach (auto tousJoueurs , lesJoueurs) {
            tousJoueurs->EnvoyerMessage('A', "Attente");
        }
    }
    index++;
}

void Serveur::onQWebSocketDisconnected()
{
    QWebSocket* client=qobject_cast<QWebSocket*>(sender());
    int index;
    foreach (Joueur *j, lesJoueurs) {
        if (j->getClient()==client)
        {
            //deleteLater(j->client);
            lesJoueurs.removeOne(j);
        }
    }
    qDebug() << "Client Déconnecté";
}

void Serveur::on_comboBoxDatabase_currentTextChanged(const QString &arg1)
{
    QSqlTableModel* model = new QSqlTableModel(this);

    model->setTable(arg1);
    model->select();

    ui->tableViewDataBase->setModel(model);
    ui->tableViewDataBase->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Serveur::onQWebSocketTextMessageReceived(const QString &msg)
{
    QRandomGenerator gen;
    QStringList message = msg.split(";");

    qDebug() << "Message Reçu" << msg << message.at(1) << message.at(2);


    if(message.at(1) == 'C') {
        bool pseudo = false;

        foreach (auto var,lesJoueurs) {
            if(var->getPseudo() == message.at(2)) {
                pseudo = true;
            }
        }

        if(pseudo == false)
            lesJoueurs.last()->setPseudo(message.at(2));
        else {
            QMessageBox::warning(this, "Erreur", "Pseudo invalide");
            lesJoueurs.last()->EnvoyerMessage('E',"Pseudo invalide");
        }

        qDebug() << "Pseudo entré";

        foreach (auto var,lesJoueurs) {
            qDebug() << var->getPseudo();
        }
    }

    if (lesJoueurs.count() > 0 && etat == ATTENTE) {
        qDebug() << "Début De Partie";

        QRandomGenerator gen;
        QList <int>listeIdPersonnage;
        QSqlQuery idmaxCommande("SELECT MAX(id_nom) as max FROM Personnage");
        int valeur;
        int idmax;

        foreach (auto personnage , lesJoueurs) {
            idmaxCommande.exec();
            idmaxCommande.next();
            idmax = idmaxCommande.value("max").toInt();

            for (int i = 0; i < lesJoueurs.count(); i++) {
                do {
                    valeur= gen.bounded(idmax);

                }while(listeIdPersonnage.contains(valeur));
                listeIdPersonnage.append(valeur);
            }

            QSqlQuery commande;
            int valeur = gen.bounded(15);

            qDebug() << "Numéro du personnage : " << valeur;

            commande.prepare("SELECT nom, description FROM Personnages WHERE id = :id");
            commande.bindValue(":id", valeur);
            commande.exec();
            commande.next();

            qDebug() << "Personnage Envoyé :" << commande.value("nom").toString();

            personnage->EnvoyerMessage('P', commande.value("nom").toString());
            personnage->setPersonnage(commande.value("nom").toString());
            listePersonnage.append('|' + commande.value("nom").toString());
            commande.clear();
            etat = DEDUCTION;
        }
    }


    if (etat == DEDUCTION && tour < 4 && message.at(1) == 'P') {

        //tant que la liste ne contient pas tous les mots
        while (listeMot.count() != lesJoueurs.count()) {
            //ajout du mot dans la lsite
            listeMot.append(message.at(2));
            qDebug() << "Ajout à la liste de mot" << message.at(2);

            //pour chaque joueur envoyer le mot correspondant
            foreach (auto personnage , lesJoueurs) {
                personnage->EnvoyerMessage('M', listeMot.at(personnage->getIndex()));
            }
        }
        tour++;
    }

    if (tour == 4)
    {
        etat = FIN;
    }

    if(etat == FIN) {
        foreach (auto var,lesJoueurs) {
            var->EnvoyerMessage('A', "Fin");
        }
    }


    /* Machine d'état (non fonctionnel)*/
    // switch (etat) {
    // case ATTENTE: {
    //     if(message.at(1) == 'C'){
    //         joueur = new Joueur;
    //         joueur->setPseudo(message.at(2));
    //     }

    //     if(lesJoueurs.count() > 3) {
    //         etat = DISTRIBUTION;
    //     } else
    //         EnvoyerMessage('A', "Attente");

    //     qDebug() << "Attente";
    //     break;
    // }
    // case DISTRIBUTION: {
    //     for (int i = 0; i < lesJoueurs.count(); i++){
    //         int valeur = gen.generate();
    //         QSqlQuery commande;
    //         commande.prepare("SELECT nom FROM Personnages WHERE id = :id");
    //         commande.bindValue(":id", valeur);
    //         commande.exec();
    //         joueur->setPersonnage(commande.value("nom").toString());
    //         qDebug() << commande.value("nom").toString();
    //     }
    //     qDebug() << "Distribution";
    //     etat = ECHANGE_PROPOSITION;
    //     break;
    // }
    // case FIN:
    //     break;
    // case ECHANGE_PROPOSITION:
    //     if(message.at(1) == 'P') {

    //     }
    //     break;
    // case DEDUCTION:
    //         EnvoyerMessage('D', "Attente");
    //     break;
    // case CALCUL_SCORE:
    //     break;
    // case RECONNEXION:
    //     break;
    // }
}
