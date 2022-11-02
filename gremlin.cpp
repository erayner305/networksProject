
/* Gremlins takes a character buffer, user selected corruption chance number between 0 and 1, 
 * and user selected packet loss chance number between 0 and 1
 *
 * Returns:
 * -1 - Error in input
 *  0 - Normal operation, no adjustment to packet
 *  1 - Packet loss occurs, do not 
 */
int gremlins(char buffer[], double corruptionChance, double lossChance){
	double randomNum;
	srand(time(NULL));

	//Error Checking.
	if (corruptionChance > 1 || corruptionChance < 0 || lossChance > 1 || lossChance < 0) { 
		return -1;
	} 


	if(rand()/RAND_MAX < lossChance){ //Checks for loss of packet
		return 1;
	}
	else if (rand()/RAND_MAX < corruptionChance) { //Checks for corruption of packet
		randomNum = rand()/RAND_MAX;
		if(randomNum <= 0.7){ //70% only one packet is affected
			buffer[8] = '\0';
		}
		
		if(randomNum <= 0.2){ //20% chance two packets are affected
			buffer[9] = '\0';
		}

		if(randomNum <= 0.1){ //10% chance three packets are affected
			buffer[10] = '\0';
		}
		return 2;
	}

	return 0;
 
}