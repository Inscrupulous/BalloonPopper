#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

//this enum holds all the msg types for the server
//any file which needs to encode/decode a message just needs to include this file, and use STRICT CASTING (such as "int x = static_cast<int>(MessageType::Error)"), WELL-DEFINED SCOPE (eg. MessageType::HSRequest)
//Convert the type to an int, insert into a textstream, followed by whitespace, followed by the data section of the message.
//separate elements of a msg are separated by white space, sent via a QTextStream

//should it be an enum class? i think so? ;]
enum class MessageType {
    //-------- Client->Server Messages
    ColorChangeRequest,     //tells the server:  "hey, i want this color. it's mine" if the color is not already taken, the server should send all players a notice that that color is claimed.
    StartGameRequest,       //player1 should be the only one to press this. Upon receival, server initializes game
    Moved,                  //this msg tells the server that a player has moved the server will then update the player pos and send that info to all other players
    ShootPellet,            //tell server to create pellet obj on sender's current coords. Server sends this info to all clients (including the client who fired the pellet?)
    IncPlayerScore,         //tell server to increase a player's score
    RequestBalloonSchedule, //Ask server for balloon spawn schedule

    //-------- Server->Client Messages
    ColorChanged,           //player id, color type (colors enummed?)
    PlayerPosChange,        //player id, new coords
    PelletAdded,            //similar idea to balloonadded. upon receiving shootpellet, server creates pellet at player's coords, sends [pelletID, coords] to each client.
    UpdatePlayerScores,     //variable list: [numplayers, player1name, player2score, player3name, ..., player_n_scores]
    SendFinalScores,        //sends the final scores to gameenddialog
    LoadGame,               //signal players to create game object
    UpdatePlayerName,       //tell client to update name and id list
    SetId,                  //tell client their id
    BalloonSchedule,        //Server creates balloon spawn schedule and sends it to all clients for them to use to create balloons for the game
};

enum class Color{
    Red,
    Pink,
    Cyan,
    Yellow,
    Green,
    Blue,
    Purple,
    Black,
    NullColor,
};

#endif //MESSAGETYPE_H
