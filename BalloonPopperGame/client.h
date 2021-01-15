#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <messagetype.h>
#include <QList>
#include "playerdata.h"

class Client : public QTcpSocket
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr);
    virtual ~Client();
    void resetPlayerData();

    // Get functions
    QString getPlayerName(int i = 0)const{return players[i]->name;}   // grab a player name
    int getLobbySize() const{return lobbySize;}                     // Returns lobby size
    int getPlayerScore(int i = 0)const{return players[i]->score;}    // grab a players score
    int getServerTime() const{return gameTimeLimit;}                // returns time set by server
    int getMyColor() const{return myColor;}                         // returns myColor
    int getPlayerColor(int i) const{return players[i]->color;}      // returns a certain players color
    int getMyID() const{return myId;}                               // returns client-side ID
    int getPlayerID(int i) const{return players[i]->id;}            // returns the ID of player at index i

    // Value checks
    bool isServerAtLimit() const{return serverLimitReached;}        // returns true if server is full
    bool isHost() const{return amHost;}                             // returns true if client is host
    bool isColorAvailabe(int i);                                    // returns true if color is available


    // Set functions
    void setHost(bool tf){amHost = tf;}
    void setName(QString name){clientName = name;}
    void setColor(int);

    // Send a byte array
    void SendByteArray(QByteArray &dataToSend);
    void SendFinalScoresSignal(QTextStream& ts){emit SendFinalScores(ts);}

private:
   QString clientName = "unkown";
   QString playerNames[5];
   QString highScores[5];
   unsigned short lobbySize = 0;
   bool serverLimitReached = false;
   bool amHost = false;
   bool first_message = true;

   // Variables for game info
   int myColor = static_cast<int>(Color::NullColor);
   int myId = 0;
   int gameTimeLimit = 60;
   QList<Player*> players; // Used to store all of the players info
   QList<int> availableColors;

   // Variable for choosing data processing option
   unsigned short dataOption = 0;

   // Data Processing functions
   void ProcessPlayerData(QTextStream &incomingData);        // Processes lobby names


signals:
   void nameChanges();              // emitted when lobby names have changed
   void enoughPlayersJoined(bool);  // emitted when enough players to start game have joined
   void startGame();                // emitted when a player has started the game
   void colorsUpdated();            // Used to update color lists
   void PlayerPosChanged(QTextStream&); //emitted when the server indicates a player has moved
   void PelletAdded(QTextStream&);           // Signal is emitted when another client has added a pellet
   void InitializeSchedule(QTextStream&);          // Signal is emitted when server sends spawn request
   void UpdateScores(QTextStream&);         // Update scores on game window
   void SendFinalScores(QTextStream&);      // Send game results to gameenddialog

private slots:
   void DataReceival();
   void sendName();
   void sendColorRequest();
   void updateColorList();
   void resetPlayerList();
};

#endif // CLIENT_H
