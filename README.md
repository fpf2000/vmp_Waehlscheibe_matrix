# vmp_Waehlscheibe_matrix_mqtt
Eine alte Vermittlungplatz Waehlscheibe mit einem 8x8 Matrix ueber die NodeMcu angeschaltet
Ziffer fuer Ziffer erfolgt hier die Eingabe der Zahlen
# BESCHREIBUNG:
Das System werdet die sogenannte Impulswahl über einen Öffner und einen Schließer aus.
Das ganze ist an D1 [Öffner] und D2 [Schließer] gegen GND verdrahtet.
Der Code zählt nun die Impulse an diesen Eingängen und gibt sie aus.
Die Ausgabe erfolgt ist nun auf LED Matrix[ MAX7219ENG].
Bei erfolgtem Internetzugang werden die Ziffern zusätzlich zu einem MQTT Broker gesendet.
Hier könnte man nun weitere Prozesse triggern.
Im WIKI auch ein BILD dazu !! https://github.com/fpf2000/vmp_Waehlscheibe_matrix/wiki
# Benutzt wurde
1 Arduino ESP8266 Nodemcu 
1 8x8 LED MATRIX MAX7219ENG
1 Eine Uralte Wählscheibe (finde die erst einmal :-) ) Es gehen natürlich auch Neuere !
