// Copyright Patrick Haubner


#include "Player/AuraPlayerState.h"


AAuraPlayerState::AAuraPlayerState()
{
	//Replication settings for the player state 
	NetUpdateFrequency = 100.f; // Set the frequency of network updates to 100 times per second
}
