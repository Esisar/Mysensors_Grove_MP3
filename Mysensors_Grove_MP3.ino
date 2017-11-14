/**
 * Mysensors_Grove_MP3.ino
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0: GUILLOTON Laurent
 * Version 1.1 - 2017-11-10: Création du sketch initial
 *
 * DESCRIPTION
 *
 * Sketch gérant le contrôle d'un module Grove MP3
 *
 * For more information, please visit:
 * http://wiki.seeed.cc/Grove-MP3_v2.0/
 *
 */

// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24

// ID du noeud
//#define MY_NODE_ID 108

#include <SoftwareSerial.h>
#include <MP3Player_KT403A.h>
#include <MySensors.h>
#include <SPI.h>

SoftwareSerial mp3(2, 3);

byte COMMAND, EQUALIZER, VOLUME, FOLDER, TRACK;
bool COMMAND_CHANGE=0, EQUALIZER_CHANGE=0, VOLUME_CHANGE=0, FOLDER_CHANGE=0, TRACK_CHANGE=0;

#define CHILD_ID_CMD 0
#define CHILD_ID_EQUALIZER 1
#define CHILD_ID_VOLUME 2
#define CHILD_ID_FOLDER 3
#define CHILD_ID_TRACK 4

MyMessage msgCmd(CHILD_ID_CMD, V_VAR1);
MyMessage msgEqualizer(CHILD_ID_EQUALIZER, V_VAR1);
MyMessage msgVolume(CHILD_ID_VOLUME, V_PERCENTAGE);
MyMessage msgFolder(CHILD_ID_FOLDER, V_VAR1);
MyMessage msgTrack(CHILD_ID_TRACK, V_VAR1);

void presentation()
{
	// Send the sketch version information to the gateway
	sendSketchInfo("Mysensors_Grove_MP3", "1.1");

	// Register all sensors to gw (they will be created as child devices)
	present(CHILD_ID_CMD, S_CUSTOM);
	present(CHILD_ID_EQUALIZER, S_CUSTOM);
	present(CHILD_ID_VOLUME, S_DIMMER);
	present(CHILD_ID_FOLDER, S_CUSTOM);
	present(CHILD_ID_TRACK, S_CUSTOM);
}

/** Fonction setup() */
void setup() {

	/* Initialise le port série */
	mp3.begin(9600);
	mp3.setTimeout(60000);

	/* Initialise le module KT403A */
	resetPlayer();
	selectSourceDevice(DEVICE_SDCARD);
	// Récupération des données du contrôleur
	EQUALIZER=request(CHILD_ID_EQUALIZER, V_VAR1);
	VOLUME=request(CHILD_ID_VOLUME, V_PERCENTAGE);
	FOLDER=request(CHILD_ID_FOLDER, V_VAR1);
	TRACK=request(CHILD_ID_TRACK, V_VAR1);

	setVolume(VOLUME);
}

/** Fonction loop() */
void loop() {

	/* Attends une commande */
	while(!mp3.available());


	if (COMMAND_CHANGE)
	{
		if (COMMAND==1) {  // OK (ok dans tt dossiers)
			playNextSong();
		}

		if (COMMAND==2){  // OK (ok dans tt dossiers)
			playPreviousSong();
		}

		if (COMMAND==3) {  // OK 1-65535 (lecture par ordre création tt dossiers confondu)
			long track_number = TRACK;
			playTrack(track_number & 0xFFFF);
		}

		if (COMMAND==4) {  // OK
			volumeUp();
		}

		if (COMMAND==5){  // OK
			volumeDown();
		}

		if (COMMAND==6){  // OK 0-30
			long volume = VOLUME;
			setVolume(volume & 0xFF);
		}

		if (COMMAND==7){  // OK
			long equalizer = EQUALIZER;
			setEqualizer(equalizer & 0xFF);
		}

		if (COMMAND==8){  // OK
			long track_number = TRACK;
			repeatSingleTrack(track_number & 0xFFFF);
		}

		if (COMMAND==10) {  // OK
			enterStandbyMode();
		}

		if (COMMAND==11)  { // OK (need select device aprés réveil)
			exitStandbyMode();
		}

		if (COMMAND==12)  {  // OK
			resetPlayer();
		}

		if (COMMAND==13)  {  // OK
			playResumeMusic();
		}

		if (COMMAND==14)  {  // OK (lecture suivant si pas musique)
			pauseMusic();
		}

		if (COMMAND==15) {  // OK
			long track_number = TRACK;
			long folder_number = FOLDER;
			selectSourceFolderAndTrack(folder_number & 0xFF, track_number & 0xFF);
		}

		if (COMMAND==16)  {  // OK
			loopAllMusic(false);
		}

		if (COMMAND==17)  {  // OK folder 00-99, track 000-255
			long track_number = TRACK;
			selectSourceTrackInMp3Directory(track_number & 0xFFFF);
		}

		if (COMMAND==18)  {  // OK folder 00-15, track 0000-9999
			long track_number = TRACK;
			insertSongFromAdvertDirectory(track_number & 0xFFFF);
		}

		if (COMMAND==19)  {  // OK 0000-9999
			long folder_number = FOLDER;
			long track_number = TRACK;
			selectSourceBigFolderAndTrack(folder_number & 0x0F, track_number & 0x0FFF);
		}

		if (COMMAND==20) {  // OK
			stopInsertedSongAndResumePlaying();
		}

		if (COMMAND==21)  {  // OK
			stopMusic();
		}

		if (COMMAND==22) {  // OK 00-99 (boucle sur les dates de création des fichiers, pas sur les index)
			long folder_number = FOLDER;
			loopFolder(folder_number & 0xFF);
		}

		if (COMMAND==23) {  // OK
			shufflePlay();
		}

		if (COMMAND==24) {  // OK 00-99 (boucle sur les dates de création des fichiers, pas sur les index)
			long folder_number = FOLDER;
			loopFolder(folder_number & 0xFF);
		}

		if (COMMAND==25)  {  // OK
			repeatCurrentTrack(true);
		}


		if (COMMAND==26)  {  // OK 0000-9999 (sur les dates de création des fichiers)
			long track_number = Serial.parseInt();
			repeatSingleTrack(track_number & 0xFFFF);
		}
		COMMAND_CHANGE=0;
	}
	else if (EQUALIZER_CHANGE)
	{
		if (EQUALIZER==0)
		{
			setEqualizer(0x00);
		}
		if (EQUALIZER==1)
		{
			setEqualizer(0x01);
		}
		if (EQUALIZER==2)
		{
			setEqualizer(0x02);
		}
		if (EQUALIZER==3)
		{
			setEqualizer(0x03);
		}
		if (EQUALIZER==4)
		{
			setEqualizer(0x04);
		}
		if (EQUALIZER==5)
		{
			setEqualizer(0x05);
		}
		EQUALIZER_CHANGE=0;
	}
	else if (VOLUME_CHANGE)
	{
		setVolume(VOLUME);
		VOLUME_CHANGE=0;
	}

}

void receive(const MyMessage &message)
{
	int Type=message.type;
	if (Type == V_VAR1)
	{
		if (message.sensor==CHILD_ID_CMD)
		{
			int val = atoi(message.data);
			COMMAND=val;
			COMMAND_CHANGE=1;
			send(msgCmd.set(COMMAND));
		}
		if (message.sensor==CHILD_ID_EQUALIZER)
		{
			int val = atoi(message.data);
			EQUALIZER=val;
			EQUALIZER_CHANGE=1;
			send(msgEqualizer.set(EQUALIZER));
		}
		if (message.sensor==CHILD_ID_FOLDER)
		{
			int val = atoi(message.data);
			FOLDER=val;
			FOLDER_CHANGE=1;
			send(msgFolder.set(FOLDER));
		}
		if (message.sensor==CHILD_ID_TRACK)
		{
			int val = atoi(message.data);
			TRACK=val;
			TRACK_CHANGE=1;
			send(msgTrack.set(TRACK));
		}
	}
	if(Type == V_PERCENTAGE)
	{
		int val = atoi(message.data);
		VOLUME=val;
		VOLUME_CHANGE=1;
		send(msgVolume.set(VOLUME));
	}
}
