bool setupRelay() {
  for(size_t i=0; i<NUMRELAYS; i++){
    pinMode(relayChannels[i], OUTPUT);
  }
  return true;
}

bool setRelayToOn(const uint8_t i) {
  digitalWrite(relayChannels[i], HIGH);  
  return true;
}

bool setRelayToOff(const uint8_t i) {
  digitalWrite(relayChannels[i], LOW);  
  return true;
}


bool setRelaysToOn(){
    for (int i=0; i< NUMRELAYS; ++i){
      if (setRelayToOn(i) == false){
        printAlarm("Cannot operate on Relay "+(i+'0'));
        Serial.println("Cannot operate on Relay "+(i+'0'));
        return false;
      }
    }
    return true;
}
bool setRelaysToOff(){
    for (int i=0; i< NUMRELAYS; ++i){
      if (setRelayToOff(i) == false){
        printAlarm("Cannot operate on Relay "+(i+'0'));
        return false;
      }
    }
    return true;
}
