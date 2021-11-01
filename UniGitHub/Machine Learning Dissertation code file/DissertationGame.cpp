#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>

// Controls players stats 
class Player {
private:
	// Initial stats
	float attack = 1;
	float defence = 1;
	float magicAttack = 1;
	float magicDefence = 1;
	float speed = 1;
	float timeMagic = 1;
	float accuracy = 1;
	float evasion = 1;
	float statTotal = 8;
	float HP = (statTotal / 8) * 100;

public:
	float currentSpeed = speed;
	char speedType = 'n';

	~Player() {};

	// Increase chosen stat (validity is proven prior to this being called)
	void IncreaseStat(std::string stat) {
		if (stat == "attack") {
			attack++;
		}
		else if (stat == "defence") {
			defence++;
		}
		else if (stat == "magic attack") {
			magicAttack++;
		}
		else if (stat == "magic defence") {
			magicDefence++;
		}
		else if (stat == "speed") {
			speed++;
		}
		else if (stat == "time magic") {
			timeMagic++;
		}
		else if (stat == "accuracy") {
			accuracy++;
		}
		else if (stat == "evasion") {
			evasion++;
		}
		statTotal++;
		HP = ceil(((float)statTotal / 8) * 100);
	}

	// Return the chosen stat 
	float GetStat(std::string stat) {
		if (stat == "attack") {
			return attack;
		}
		else if (stat == "defence") {
			return defence;
		}
		else if (stat == "magic attack") {
			return magicAttack;
		}
		else if (stat == "magic defence") {
			return magicDefence;
		}
		else if (stat == "speed") {
			return speed;
		}
		else if (stat == "time magic") {
			return timeMagic;
		}
		else if (stat == "accuracy") {
			return accuracy;
		}
		else if (stat == "evasion") {
			return evasion;
		}
		else if (stat == "HP") {
			return HP;
		}
		else if (stat == "total") {
			return statTotal;
		}
		else {
			std::cout << "That's not a stat!\n";
			return -66;
		}
	}

	// Reduce current HP value
	void DamagePlayer(int damage) {
		HP -= damage;
	}

	// Change the speed type to haste, raising the current speed
	void ActivateHaste() {
		speedType = 'h';
		currentSpeed = speed + (timeMagic / 2.0f);
	}

	// Change the speed type to slow, lowering the current speed
	void ActivateSlow(int otherTimeMagic) {
		speedType = 's';
		currentSpeed = (speed * speed) / (speed + (otherTimeMagic / 2.0f));
	}

	// When a new battle begins, reset the speed type, updated HP value and current speed
	void Reset() {
		currentSpeed = speed;
		HP = ceil(((float)statTotal / 8) * 100);
		speedType = 'n';
	}

	// Resets the player completely after a simulation
	void FullReset() {
		attack = 1;
		magicAttack = 1;
		defence = 1;
		magicDefence = 1;
		speed = 1;
		timeMagic = 1;
		accuracy = 1;
		evasion = 1;
		statTotal = 8;
		HP = 100;
		speedType = 'n';
		currentSpeed = 1;
	}
};

// This is the Neural Network for enemy stat raising training
class Network {
public:
	~Network() {};

	// Initialise the NN
	void Init(int inputNum, int hiddenNum, int outputNum, std::string loadOrGen) {
		if (loadOrGen == "gen") {
			std::ofstream originalFile;
			originalFile.open("OriginalWeightValues.txt");

			for (int i = 0; i < hiddenNum; i++) {
				std::vector<float> set;
				for (int j = 0; j < inputNum; j++) {
					set.push_back(static_cast<float>((float)rand() / RAND_MAX));
					originalFile << set[j] << "\n";
				}
				hiddenWeights.push_back(set);
			}
			network.push_back(hiddenWeights);

			for (int i = 0; i < outputNum; i++) {
				std::vector<float> set;
				for (int j = 0; j < hiddenNum; j++) {
					set.push_back(static_cast<float>((float)rand() / RAND_MAX));
					originalFile << set[j] << "\n";
				}
				outputWeights.push_back(set);
			}
			network.push_back(outputWeights);

			originalFile.close();
		}
		else if (loadOrGen == "load") {
			float weightValue;
			std::string weightValueString;
			std::ifstream weightFile;
			std::vector<float> allWeights;
			weightFile.open("TrainedWeightValues.txt");

			while (std::getline(weightFile, weightValueString)) {
				weightValue = std::stof(weightValueString);
				allWeights.push_back(weightValue);
			}
			weightFile.close();

			int index = 0;
			for (int i = 0; i < hiddenNum; i++) {
				std::vector<float> set;
				for (int j = 0; j < inputNum; j++) {
					set.push_back(allWeights[index]);
					index++;
				}
				hiddenWeights.push_back(set);
			}
			network.push_back(hiddenWeights);

			for (int i = 0; i < outputNum; i++) {
				std::vector<float> set;
				for (int j = 0; j < hiddenNum; j++) {
					set.push_back(allWeights[index]);
				}
				outputWeights.push_back(set);
			}
			network.push_back(outputWeights);
		}
		std::cout << "Network initialised." << std::endl;
	}

	// Activation function for each of the neurons
	float Activate(std::vector<float> weights, std::vector<float> inputs) {
		float activation = bias, total = 0;

		for (unsigned int i = 0; i < weights.size(); i++) {
			activation += weights[i] * inputs[i];
			total += inputs[i];
		}
		activation = activation / total;

		activation = 1.0f / (1.0f + exp(-activation));
		activation = (activation - 0.5f) * 2;
		// This needs to change, gives values between 0.99 and 1
		// The lowest initial is gonna be around 10 ish, giving 0.99 ish
		// Variable turn number creates a problem due to a variable maximum value for input layer sum
		// Turn number could be infinite (player and enemy only use temporal spells)
		
		return activation;
	}

	// Create the hidden layer through forward propagation
	std::vector<float> ForwardPropagate(std::vector<float> inputLayer) {
		std::vector<float> inputs = inputLayer;
		for (unsigned int i = 0; i < network.size(); i++) {
			std::vector<float> newInputs;
			std::vector<std::vector<float>> tempWeights = network[i];
			for (unsigned int j = 0; j < tempWeights.size(); j++) {
				float activation = Activate(tempWeights[j], inputs);
				newInputs.push_back(activation);
			}
			inputs = newInputs;
			if (i != network.size() - 1) {
				hiddenNodes = inputs;
			}
			else {
				outputNodes = inputs;
			}
		}
		return inputs;
	}

	// SoftMax function for setting probability distribution of stats to be raised.
	float SoftMax(float total, float initial) {
		return (exp(initial) / total);
	}

	// Generate the chosen stat to be raised.
	std::string GetChosenStat(std::vector<float> possibleOutputs, Player enemy) {
		std::string stats[8];
		stats[0] = "attack";
		stats[1] = "defence";
		stats[2] = "magic attack";
		stats[3] = "magic defence";
		stats[4] = "speed";
		stats[5] = "time magic";
		stats[6] = "accuracy";
		stats[7] = "evasion";
		bool chosen = false;

		// Distribute the outputs for following probability
		std::vector<float> distributedOutputs;
		float total = 0;
		for (unsigned int i = 0; i < possibleOutputs.size(); i++) {
			total += exp(possibleOutputs[i]);
		}
		for (unsigned int i = 0; i < possibleOutputs.size(); i++) {
			distributedOutputs.push_back(SoftMax(total, possibleOutputs[i]));
		}

		std::vector<int> totalProbabilities;
		for (unsigned int i = 0; i < distributedOutputs.size(); i++) {
			int probability = (int)(distributedOutputs[i] * 100);
			for (int j = 0; j < probability; j++) {
				totalProbabilities.push_back(i + 1);
			}
		}

		// Loop until a valid stat is chosen
		while (!chosen) {
			int randomIndex = rand() % totalProbabilities.size();
			int randomStatNumber = totalProbabilities[randomIndex];
			std::string chosenStat = stats[randomStatNumber - 1];
			previouslyRaisedStat = randomStatNumber - 1;

			if (enemy.GetStat(chosenStat) >= 10) {
				// Do not raise stat
				chosen = false;
			}
			else {
				// Raise stat
				chosen = true;
				return chosenStat;
			}
		}

		return "";
	}

	// Returns the derivative of an output value
	float GetDerivative(float output) {
		return (output * (1.0f - output));
	}

	// Train the network through backpropagation and weight updating. Returns the indexes of the weights being observed.
	void TrainNetwork(std::vector<float> expectedValues) {
		std::ofstream errorDocument;
		errorDocument.open("ErrorValues.txt", std::fstream::app);
		hiddenChanges.clear();
		outputChanges.clear();
		for (int i = network.size() - 1; i >= 0; i--) {
			std::vector<float> errors;
			if (i != network.size() - 1) {
				for (unsigned int j = 0; j < hiddenNodes.size(); j++) {
					float error = 0.0f;
					std::vector<float> tempWeights;
					for (unsigned int k = 0; k < outputNodes.size(); k++) {
						std::vector<float> temptemp = outputWeights[k];
						tempWeights.push_back(temptemp[j]);
					}
					for (unsigned int k = 0; k < outputNodes.size(); k++) {
						error += tempWeights[k] * outputChanges[k];
					}
					errors.push_back(error);
				}
			}
			else {
				for (unsigned int j = 0; j < outputNodes.size(); j++) {
					float node = outputNodes[j];
					errors.push_back(expectedValues[j] - node);
				}
			}
			for (unsigned int j = 0; j < errors.size(); j++) {
				errorDocument << errors[j] << '\n';
				if (i != network.size() - 1) {
					hiddenChanges.push_back(errors[j] * GetDerivative(hiddenNodes[j]));
				}
				else {
					outputChanges.push_back(errors[j] * GetDerivative(outputNodes[j]));
				}
			}
		}
		errorDocument.close();
	}

	// Updates the weights passed in by index
	void UpdateWeights(std::vector<float> inputLayer, Network& networkArg) {
		for (unsigned int i = 0; i < networkArg.network.size(); i++) {
			std::vector<float> inputs = inputLayer;
			if (i == 1) {
				inputs = hiddenNodes;
			}
			std::vector<std::vector<float>>* temp = &networkArg.network[i];
			for (unsigned int j = 0; j < temp->size(); j++) {
				std::vector<float>* tempTwo = &(*temp)[j];
				for (unsigned int k = 0; k < inputs.size(); k++) {
					float* tempThree = &(*tempTwo)[k];
					*tempThree = *tempThree + (learningRate * hiddenChanges[j] * inputs[k]);
				}
			}
		}
	}

	// Prints the trained weights to a txt file for checking
	void PrintToFile() {
		std::ofstream trainedFile;
		trainedFile.open("TrainedWeightValues.txt");

		for (unsigned int i = 0; i < network.size(); i++) {
			std::vector<std::vector<float>> temp = network[i];
			for (unsigned int j = 0; j < temp.size(); j++) {
				std::vector<float> tempTwo = temp[j];
				for (unsigned int k = 0; k < tempTwo.size(); k++) {
					trainedFile << tempTwo[k] << "\n";
				}
			}
		}

		trainedFile.close();
	}

private:
	std::vector<std::vector<std::vector<float>>> network;
	std::vector<std::vector<float>> hiddenWeights, outputWeights;
	std::vector<float> hiddenNodes, outputNodes, hiddenChanges, outputChanges;
	int previouslyRaisedStat = 0;
	float bias = 1.0f, learningRate = 0.1f;
};

// Randomises which players turn it is
float characterTurn(Player one, Player two, float turnNum) {
	float speedTotal;

	// Fixed turn based, player takes actions equal to speed value 
	speedTotal = round(one.currentSpeed) + round(two.currentSpeed);
	if (turnNum > speedTotal) {
		while (turnNum > speedTotal) {
			turnNum -= speedTotal;
		}
	}
	if (turnNum <= round(one.currentSpeed)) {
		return 1;
	}
	else {
		return 2;
	}

	// Luck based turns using formula from before
	/*float randomValue;
	speedTotal = one.currentSpeed + two.currentSpeed;
	randomValue = (float)(rand() % (int)ceil(speedTotal));
	if (randomValue < one.currentSpeed) {
		return 1;
	}
	else {
		return 2;
	}*/
}

// Controls the input/output for the player turn
std::string playerTurn() {
	std::string choice;
	bool chosen = false;

	// Loop until valid move is chosen
	std::cout << "------------------------------------------------------------\n";
	std::cout << "Choose your move:\n Attack\n Magic Attack\n Haste\n Slow\n :";
	while (!chosen) {
		choice = "";
		std::getline(std::cin, choice);
		for (unsigned int i = 0; i < choice.length(); i++) {
			choice[i] = tolower((char)choice[i]); // Turn the entire input to lower case letters
		}
		if (choice == "attack") {
			return "attack";
			chosen = true;
		}
		else if (choice == "magic attack") {
			return "magic attack";
			chosen = true;
		}
		else if (choice == "haste") {
			return "haste";
			chosen = true;
		}
		else if (choice == "slow") {
			return "slow";
			chosen = true;
		}
		else {
			std::cout << "That is not a valid choice. Choose again\n :";
		}
	}

	return "";
}

// Controls the actions taken by the players
float moveController(std::string move, Player one, Player two, bool training) {
	float hitChance, accurate, evade, hitValue;
	if (move == "attack") {
		// Check if the move hits
		accurate = (float)one.GetStat("accuracy");
		evade = (float)two.GetStat("evasion");
		hitChance = ((accurate / (2 * evade) + ((accurate / 2) - 0.1f))) / accurate;
		hitValue = (float)rand() / RAND_MAX;
		if (hitValue < hitChance) {
			// Calculate the damage
			float damage = (one.GetStat("attack") - two.GetStat("defence")) + 10;
			int randomAdded = rand() % 7 - 3;
			damage += randomAdded;
			if (!training) {
				std::cout << "Attack hit for " << damage << " damage!\n";
			}
			return damage;
		}
		else {
			if (!training) {
				std::cout << "Attack missed!\n";
			}
			return 0;
		}
	}
	else if (move == "magic attack") {
		// Check if the move hits
		accurate = (float)one.GetStat("accuracy");
		evade = (float)two.GetStat("evasion");
		hitChance = ((accurate / (2 * evade) + ((accurate / 2) - 0.1f))) / accurate;
		hitValue = (float)rand() / RAND_MAX;
		if (hitValue < hitChance) {
			// Calculate the damage
			float damage = (one.GetStat("magic attack") - two.GetStat("magic defence")) + 10;
			int randomAdded = rand() % 6;
			damage += randomAdded;
			if (!training) {
				std::cout << "Magic attack hit for " << damage << " damage!\n";
			}
			return damage;
		}
		else {
			if (!training) {
				std::cout << "Magic attack missed!\n";
			}
			return 0;
		}
	}
	else if (move == "haste") {
		// Inflict haste on themeselves
		if (one.speedType != 'h') {
			return 1;
		}
		else {
			if (!training) {
				std::cout << "You are already under the effects of haste!\n";
			}
			return 0;
		}
	}
	else if (move == "slow") {
		// Inflict slow on the opponent
		if (two.speedType != 's') {
			return 1;
		}
		else {
			if (!training) {
				std::cout << "They are already under the effects of slow!\n";
			}
			return 0;
		}
	}
	else {
		// In no case should it be possible to reach this. If you do, just scream. I would.
		std::cout << "This shouldn't be possible...\n";
	}

	return 0;
}

// Controls the enemies turn. This is where fuzzy logic will be implemented.
std::string enemyTurn(Player player, Player enemy) {
	std::string moves[4], chosenMove;
	moves[0] = "attack";
	moves[1] = "magic attack";
	moves[2] = "haste";
	moves[3] = "slow";

	float playerAttack = player.GetStat("attack");
	float playerDefence = player.GetStat("defence");
	float playerMagicA = player.GetStat("magic attack");
	float playerMagicD = player.GetStat("magic defence");
	float playerSpeed = player.GetStat("speed");
	float playerTime = player.GetStat("time magic");
	float playerEvasion = player.GetStat("evasion");
	float playerAccuracy = player.GetStat("accuracy");
	float enemyAttack = enemy.GetStat("attack");
	float enemyDefence = enemy.GetStat("defence");
	float enemyMagicA = enemy.GetStat("magic attack");
	float enemyMagicD = enemy.GetStat("magic defence");
	float enemySpeed = enemy.GetStat("speed");
	float enemyTime = enemy.GetStat("time magic");
	float enemyEvasion = enemy.GetStat("evasion");
	float enemyAccuracy = enemy.GetStat("accuracy");
	char playerSpeedType = player.speedType;
	char enemySpeedType = enemy.speedType;

	float attackEffectiveness = enemyAttack - playerDefence;
	float magicEffectiveness = enemyMagicA = playerMagicD;
	float whoIsFaster = playerSpeed - enemySpeed;
	float isPlayerHasted, isEnemyHasted;
	if (playerSpeedType == 'h') {
		isPlayerHasted = 1;
	}
	else if (playerSpeedType == 'n') {
		isPlayerHasted = 0.5f;
	}
	else {
		isPlayerHasted = 0;
	}
	
	if (enemySpeedType == 'h') {
		isEnemyHasted = 0;
	}
	else if (enemySpeedType == 'n') {
		isEnemyHasted = 0.5f;
	}
	else {
		isEnemyHasted = 1;
	}

	float attackRemap = (attackEffectiveness + 10) / 20.0f;
	float magicRemap = (magicEffectiveness + 10) / 20.0f;
	float speedRemap = (whoIsFaster + 10) / 20.0f;

	float shouldIHaste, shouldISlow, whichAttack, hasteOrSlow;
	shouldIHaste = std::min(speedRemap, isEnemyHasted);
	shouldISlow = std::min(speedRemap, isPlayerHasted);
	whichAttack = std::max(attackRemap, magicRemap);
	hasteOrSlow = std::max(shouldIHaste, shouldISlow);
	
	if (hasteOrSlow > whichAttack) {
		if (shouldIHaste > shouldISlow) {
			return "haste";
		}
		else {
			return "slow";
		}
	}
	else {
		if (attackRemap > magicRemap) {
			return "attack";
		}
		else {
			return "magic attack";
		}
	}
}

// Player gets to raise a stat.
std::string raiseStat(Player player) {
	bool chosen = false;
	std::string choice;

	// Loop until a valid stat is chosen
	std::cout << "------------------------------------------------------------\n";
	std::cout << "Choose a stat to raise:\n Attack\n Magic Attack\n Defence\n Magic Defence\n Speed\n Time Magic\n Accuracy\n Evasion\n :";
	while (!chosen) {
		choice = "";
		std::getline(std::cin, choice);
		for (unsigned int i = 0; i < choice.length(); i++) {
			choice[i] = tolower((char)choice[i]);
		}
		if (choice == "attack" || choice == "magic attack" || choice == "defence" || choice == "magic defence" || choice == "speed" || choice == "time magic" || choice == "accuracy" || choice == "evasion") {
			if (player.GetStat(choice) >= 10) {
				// Do not raise stat
				std::cout << "That stat is already maxed out. Choose again\n :";
			}
			else {
				// Stat raises
				chosen = true;
				return choice;
			}
		}
		else {
			std::cout << "That is not a valid choice. Choose again\n :";
		}
	}

	return "";
}

// The computer raises a stat. This is a machine learning part!
std::string enemyRaiseStat(Player enemy, Player player, Network& network, float winner, float playerStatRaised, float playerFavouriteMove, float enemyFavouriteMove, float playerFirstMove, float enemyFirstMove, float turnNumber, int enemyType, bool training) {
	std::string chosenStat;
	
	if (enemyType == 0) {
		// Build the input layer for the NN
		std::vector<float> inputLayer;
		inputLayer.push_back(player.GetStat("attack"));
		inputLayer.push_back(player.GetStat("magic attack"));
		inputLayer.push_back(player.GetStat("defence"));
		inputLayer.push_back(player.GetStat("magic defence"));
		inputLayer.push_back(player.GetStat("speed"));
		inputLayer.push_back(player.GetStat("time magic"));
		inputLayer.push_back(player.GetStat("accuracy"));
		inputLayer.push_back(player.GetStat("evasion"));
		inputLayer.push_back(enemy.GetStat("attack"));
		inputLayer.push_back(enemy.GetStat("magic attack"));
		inputLayer.push_back(enemy.GetStat("defence"));
		inputLayer.push_back(enemy.GetStat("magic defence"));
		inputLayer.push_back(enemy.GetStat("speed"));
		inputLayer.push_back(enemy.GetStat("time magic"));
		inputLayer.push_back(enemy.GetStat("accuracy"));
		inputLayer.push_back(enemy.GetStat("evasion"));
		inputLayer.push_back(winner);
		inputLayer.push_back(playerStatRaised);
		inputLayer.push_back(playerFavouriteMove);
		inputLayer.push_back(enemyFavouriteMove);
		inputLayer.push_back(playerFirstMove);
		inputLayer.push_back(enemyFirstMove);
		inputLayer.push_back(turnNumber);

		// Create the expected output values
		std::vector<float> expectedValues;
		float total = 0;
		total += exp(player.GetStat("attack"));
		total += exp(player.GetStat("defence"));
		total += exp(player.GetStat("magic attack"));
		total += exp(player.GetStat("magic defence"));
		total += exp(player.GetStat("speed"));
		total += exp(player.GetStat("time magic"));
		total += exp(player.GetStat("accuracy"));
		total += exp(player.GetStat("evasion"));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("attack")));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("defence")));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("magic attack")));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("magic defence")));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("speed")));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("time magic")));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("accuracy")));
		expectedValues.push_back(1.0f - network.SoftMax(total, player.GetStat("evasion")));

		// Run the NN here
		std::vector<float> possibleOutputs = network.ForwardPropagate(inputLayer);
		if (training) {
			network.TrainNetwork(expectedValues);
			network.UpdateWeights(inputLayer, network);
		}
		chosenStat = network.GetChosenStat(possibleOutputs, enemy);
	}
	else {
		std::string stats[8];
		stats[0] = "attack";
		stats[1] = "defence";
		stats[2] = "magic attack";
		stats[3] = "magic defence";
		stats[4] = "speed";
		stats[5] = "time magic";
		stats[6] = "accuracy";
		stats[7] = "evasion";
		bool chosen = false;

		while (!chosen) {
			chosenStat = stats[rand() % 8];
			if (enemy.GetStat(chosenStat) >= 10) {
				chosen = false;
			}
			else {
				chosen = true;
			}
		}
	}

	if (!training) {
		std::cout << "Enemy raised " << chosenStat << "!\n";
	}
	return chosenStat;
}

// Strategies for NN training.
std::string StrategyOne(std::string moveOrStat, std::string move, float turnNum, Player playerOne, Player playerTwo) {
	if (moveOrStat == "move"){
		if (turnNum == 1) {
			return "haste";
		}
		else if (turnNum == 2) {
			return "slow";
		}
		else if (turnNum == 3) {
			return "magic attack";
		}
		else if (turnNum == 4) {
			return "attack";
		}
		else if (turnNum >= 5) {
			if (playerOne.speedType != 'h') {
				return "haste";
			}
			else if (playerTwo.speedType != 's') {
				return "slow";
			}
			else if (move == "attack") {
				return "attack";
			}
			else if (move == "magic attack") {
				return "magic attack";
			}
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("speed") < 10) {
			return "speed";
		}
		else if (move == "attack") {
			if (playerOne.GetStat("attack") < 10) {
				return "attack";
			}
			else {
				return "magic attack";
			}
		}
		else if (move == "magic attack") {
			if (playerOne.GetStat("magic attack") < 10) {
				return "magic attack";
			}
			else {
				return "attack";
			}
		}
		else {
			std::string stats[5], chosenStat;
			stats[0] = "defence";
			stats[1] = "magic defence";
			stats[2] = "time magic";
			stats[3] = "accuracy";
			stats[4] = "evasion";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 5];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

std::string StrategyTwo(std::string moveOrStat, Player playerOne) {
	if (moveOrStat == "move") {
		return "magic attack";
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("magic attack") < 10) {
			return "magic attack";
		}
		else if (playerOne.GetStat("speed") < 10) {
			return "speed";
		}
		else if (playerOne.GetStat("accuracy") < 10) {
			return "accuracy";
		}
		else {
			std::string stats[5], chosenStat;
			stats[0] = "attack";
			stats[1] = "defence";
			stats[2] = "magic defence";
			stats[3] = "time magic";
			stats[4] = "evasion";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 5];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

std::string StrategyThree(std::string moveOrStat, Player playerOne) {
	if (moveOrStat == "move") {
		return "attack";
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("evasion") < 10 || playerOne.GetStat("accuracy") < 10 || playerOne.GetStat("speed") < 10) {
			if (playerOne.GetStat("evasion") == playerOne.GetStat("accuracy") && playerOne.GetStat("evasion") == playerOne.GetStat("speed")) {
				return "evasion";
			}
			else if (playerOne.GetStat("evasion") <= playerOne.GetStat("accuracy") && playerOne.GetStat("evasion") <= playerOne.GetStat("speed")) {
				return "evasion";
			}
			else if (playerOne.GetStat("accuracy") <= playerOne.GetStat("evasion") && playerOne.GetStat("accuracy") <= playerOne.GetStat("speed")) {
				return "accuracy";
			}
			else if (playerOne.GetStat("speed") <= playerOne.GetStat("evasion") && playerOne.GetStat("speed") <= playerOne.GetStat("accuracy")) {
				return "speed";
			}
		}
		else if (playerOne.GetStat("attack") < 10) {
			return "attack";
		}
		else {
			std::string stats[4], chosenStat;
			stats[0] = "defence";
			stats[1] = "magic attack";
			stats[2] = "magic defence";
			stats[3] = "time magic";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 4];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

std::string StrategyFour(std::string moveOrStat, std::string move, float turnNum, float statNum, Player playerOne) {
	if (moveOrStat == "move") {
		if (turnNum == 1) {
			return "attack";
		}
		else if (turnNum == 2) {
			return "magic attack";
		}
		else if (turnNum >= 3) {
			if (move == "attack") {
				return "attack";
			}
			else if (move == "magic attack") {
				return "magic attack";
			}
		}
	}
	else if (moveOrStat == "stat") {
		if (statNum == 1) {
			return "attack";
		}
		else if (statNum == 2) {
			return "defence";
		}
		else if (statNum == 3) {
			return "magic attack";
		}
		else if (statNum == 4) {
			return "magic defence";
		}
		else if (statNum == 5) {
			return "speed";
		}
		else if (statNum == 6) {
			return "time magic";
		}
		else if (statNum == 7) {
			return "accuracy";
		}
		else if (statNum == 8) {
			return "evasion";
		}
	}

	return "";
}

std::string StrategyFive(std::string moveOrStat, float statNum, Player playerOne) {
	if (moveOrStat == "move") {
		return "attack";
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("attack") < 10 || playerOne.GetStat("defence") < 10) {
			if (playerOne.GetStat("attack") <= playerOne.GetStat("defence")) {
				return "attack";
			}
			else if (playerOne.GetStat("attack") > playerOne.GetStat("defence")) {
				return "defence";
			}
		}
		else {
			if (statNum == 1) {
				return "magic attack";
			}
			else if (statNum == 2) {
				return "magic defence";
			}
			else if (statNum == 3) {
				return "speed";
			}
			else if (statNum == 4) {
				return "time magic";
			}
			else if (statNum == 5) {
				return "accuracy";
			}
			else if (statNum == 6) {
				return "evasion";
			}
		}
	}

	return "";
}

std::string StrategySix(std::string moveOrStat, std::string enemyFavouriteMove, float statNum, Player playerOne, Player playerTwo) {
	if (moveOrStat == "move") {
		if (playerTwo.speedType == 'h') {
			return "slow";
		}
		else if (playerOne.speedType == 's') {
			return "haste";
		}
		else {
			return "attack";
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("defence") < 10) {
			if (enemyFavouriteMove == "attack") {
				return "defence";
			}
			else if (playerOne.GetStat("magic defence") < 10) {
				return "magic defence";
			}
			else {
				if (statNum == 1 && playerOne.GetStat("attack") < 10) {
					return "attack";
				}
				else if (statNum == 2 && playerOne.GetStat("magic attack") < 10) {
					return "magic attack";
				}
				else if (statNum == 3 && playerOne.GetStat("speed") < 10) {
					return "speed";
				}
				else if (statNum == 4 && playerOne.GetStat("time magic") < 10) {
					return "time magic";
				}
				else if (statNum == 5 && playerOne.GetStat("accuracy") < 10) {
					return "accuracy";
				}
				else if (statNum == 6 && playerOne.GetStat("evasion") < 10) {
					return "evasion";
				}
				else {
					if (playerOne.GetStat("attack") < 10) {
						return "attack";
					}
					else if (playerOne.GetStat("magic attack") < 10) {
						return "magic attack";
					}
					else if (playerOne.GetStat("defence") < 10) {
						return "defence";
					}
					else if (playerOne.GetStat("magic defence") < 10) {
						return "magic defence";
					}
					else if (playerOne.GetStat("speed") < 10) {
						return "speed";
					}
					else if (playerOne.GetStat("time magic") < 10) {
						return "time magic";
					}
					else if (playerOne.GetStat("accuracy") < 10) {
						return "accuracy";
					}
					else if (playerOne.GetStat("evasion") < 10) {
						return "evasion";
					}
				}
			}
		}
		else if (playerOne.GetStat("magic defence") < 10) {
			if (enemyFavouriteMove == "magic attack") {
				return "magic defence";
			}
			else {
				if (statNum == 1 && playerOne.GetStat("attack") < 10) {
					return "attack";
				}
				else if (statNum == 2 && playerOne.GetStat("magic attack") < 10) {
					return "magic attack";
				}
				else if (statNum == 3 && playerOne.GetStat("speed") < 10) {
					return "speed";
				}
				else if (statNum == 4 && playerOne.GetStat("time magic") < 10) {
					return "time magic";
				}
				else if (statNum == 5 && playerOne.GetStat("accuracy") < 10) {
					return "accuracy";
				}
				else if (statNum == 6 && playerOne.GetStat("evasion") < 10) {
					return "evasion";
				}
				else {
					if (playerOne.GetStat("attack") < 10) {
						return "attack";
					}
					else if (playerOne.GetStat("magic attack") < 10) {
						return "magic attack";
					}
					else if (playerOne.GetStat("defence") < 10) {
						return "defence";
					}
					else if (playerOne.GetStat("magic defence") < 10) {
						return "magic defence";
					}
					else if (playerOne.GetStat("speed") < 10) {
						return "speed";
					}
					else if (playerOne.GetStat("time magic") < 10) {
						return "time magic";
					}
					else if (playerOne.GetStat("accuracy") < 10) {
						return "accuracy";
					}
					else if (playerOne.GetStat("evasion") < 10) {
						return "evasion";
					}
				}
			}
		}
		else {
		if (statNum == 1 && playerOne.GetStat("attack") < 10) {
			return "attack";
		}
		else if (statNum == 2 && playerOne.GetStat("magic attack") < 10) {
			return "magic attack";
		}
		else if (statNum == 3 && playerOne.GetStat("speed") < 10) {
			return "speed";
		}
		else if (statNum == 4 && playerOne.GetStat("time magic") < 10) {
			return "time magic";
		}
		else if (statNum == 5 && playerOne.GetStat("accuracy") < 10) {
			return "accuracy";
		}
		else if (statNum == 6 && playerOne.GetStat("evasion") < 10) {
			return "evasion";
		}
		else {
			if (playerOne.GetStat("attack") < 10) {
				return "attack";
			}
			else if (playerOne.GetStat("magic attack") < 10) {
				return "magic attack";
			}
			else if (playerOne.GetStat("defence") < 10) {
				return "defence";
			}
			else if (playerOne.GetStat("magic defence") < 10) {
				return "magic defence";
			}
			else if (playerOne.GetStat("speed") < 10) {
				return "speed";
			}
			else if (playerOne.GetStat("time magic") < 10) {
				return "time magic";
			}
			else if (playerOne.GetStat("accuracy") < 10) {
				return "accuracy";
			}
			else if (playerOne.GetStat("evasion") < 10) {
				return "evasion";
			}
		}
		}
	}

	return "";
}

std::string StrategySeven(std::string moveOrStat, Player playerOne, Player playerTwo) {
	if (moveOrStat == "move") {
		if (playerOne.speedType != 'h') {
			return "haste";
		}
		else {
			if (playerTwo.GetStat("defence") <= playerTwo.GetStat("magic defence")) {
				return "attack";
			}
			else if (playerTwo.GetStat("defence") > playerTwo.GetStat("magic defence")) {
				return "magic attack";
			}
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("evasion") < 10) {
			return "evasion";
		}
		else if (playerOne.GetStat("accuracy") < 10) {
			return "accuracy";
		}
		else {
			std::string stats[6], chosenStat;
			stats[0] = "attack";
			stats[1] = "defence";
			stats[2] = "magic attack";
			stats[3] = "magic defence";
			stats[4] = "speed";
			stats[5] = "time magic";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 6];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

std::string StrategyEight(std::string moveOrStat, float turnNum, Player playerOne) {
	if (moveOrStat == "move") {
		if ((int)turnNum % 2 == 0) {
			return "attack";
		}
		else {
			return "magic attack";
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("speed") < 10) {
			return "speed";
		}
		else if (playerOne.GetStat("defence") < 10 || playerOne.GetStat("magic defence") < 10) {
			if (playerOne.GetStat("defence") <= playerOne.GetStat("magic defence")) {
				return "defence";
			}
			else {
				return "magic defence";
			}
		}
		else {
			std::string stats[5], chosenStat;
			stats[0] = "attack";
			stats[1] = "magic attack";
			stats[2] = "time magic";
			stats[3] = "accuracy";
			stats[4] = "evasion";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 5];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

std::string StrategyNine(std::string moveOrStat, float turnNum, Player playerOne) {
	if (moveOrStat == "move") {
		if ((int)turnNum % 2 == 0) {
			return "attack";
		}
		else {
			return "magic attack";
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("speed") < 10 || playerOne.GetStat("attack") < 10 || playerOne.GetStat("magic attack") < 10) {
			if (playerOne.GetStat("speed") <= playerOne.GetStat("attack") && playerOne.GetStat("speed") <= playerOne.GetStat("magic attack")) {
				return "speed";
			}
			else if (playerOne.GetStat("attack") <= playerOne.GetStat("speed") && playerOne.GetStat("attack") <= playerOne.GetStat("magic attack")) {
				return "attack";
			}
			else if (playerOne.GetStat("magic attack") <= playerOne.GetStat("speed") && playerOne.GetStat("magic attack") <= playerOne.GetStat("attack")) {
				return "magic attack";
			}
		}
		else {
			std::string stats[5], chosenStat;
			stats[0] = "defence";
			stats[1] = "magic defence";
			stats[2] = "time magic";
			stats[3] = "accuracy";
			stats[4] = "evasion";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 5];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

std::string StrategyTen(std::string moveOrStat, Player playerOne) {
	if (moveOrStat == "move") {
		if (playerOne.speedType != 'h') {
			return "haste";
		}
		else {
			return "attack";
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("attack") < 10 || playerOne.GetStat("defence") < 10 || playerOne.GetStat("speed") < 10 || playerOne.GetStat("magic defence") < 10) {
			if (playerOne.GetStat("attack") <= playerOne.GetStat("defence") && playerOne.GetStat("attack") <= playerOne.GetStat("speed") && playerOne.GetStat("attack") <= playerOne.GetStat("magic defence")) {
				return "attack";
			}
			else if (playerOne.GetStat("defence") <= playerOne.GetStat("attack") && playerOne.GetStat("defence") <= playerOne.GetStat("speed") && playerOne.GetStat("defence") <= playerOne.GetStat("magic defence")) {
				return "defence";
			}
			else if (playerOne.GetStat("speed") <= playerOne.GetStat("attack") && playerOne.GetStat("speed") <= playerOne.GetStat("defence") && playerOne.GetStat("speed") <= playerOne.GetStat("magic defence")) {
				return "speed";
			}
			else if (playerOne.GetStat("magic defence") <= playerOne.GetStat("attack") && playerOne.GetStat("magic defence") <= playerOne.GetStat("speed") && playerOne.GetStat("magic defence") <= playerOne.GetStat("defence")) {
				return "magic defence";
			}
		}
		else {
			std::string stats[4], chosenStat;
			stats[0] = "magic attack";
			stats[1] = "time magic";
			stats[2] = "accuracy";
			stats[3] = "evasion";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 4];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

std::string StrategyEleven(std::string moveOrStat, std::string move, float turnNum, Player playerOne, Player playerTwo) {
	if (moveOrStat == "move") {
		if (turnNum == 1) {
			return "haste";
		}
		else if (turnNum == 2) {
			return "slow";
		}
		else if (turnNum == 3) {
			return "magic attack";
		}
		else if (turnNum == 4) {
			return "attack";
		}
		else if (turnNum >= 5) {
			if (playerOne.speedType != 'h') {
				return "haste";
			}
			else if (playerTwo.speedType != 's') {
				return "slow";
			}
			else if (move == "attack") {
				return "attack";
			}
			else if (move == "magic attack") {
				return "magic attack";
			}
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("defence") < 5 || playerOne.GetStat("magic defence") < 5) {
			if (playerOne.GetStat("defence") <= playerOne.GetStat("magic defence")) {
				return "defence";
			}
			else {
				return "magic defence";
			}
		}
		else if (playerOne.GetStat("speed") < 5 || playerOne.GetStat("time magic") < 5) {
			if (playerOne.GetStat("speed") <= playerOne.GetStat("time magic")) {
				return "speed";
			}
			else {
				return "time magic";
			}
		}
		else if (playerOne.GetStat("attack") < 5 || playerOne.GetStat("magic attack") < 5) {
			if (playerOne.GetStat("attack") <= playerOne.GetStat("magic attack")) {
				return "attack";
			}
			else {
				return "magic attack";
			}
		}
		else if (playerOne.GetStat("accuracy") < 5 || playerOne.GetStat("evasion") < 5) {
			if (playerOne.GetStat("accuracy") <= playerOne.GetStat("evasion")) {
				return "accuracy";
			}
			else {
				return "evasion";
			}
		}
		else if (playerOne.GetStat("defence") < 10 || playerOne.GetStat("magic defence") < 10) {
			if (playerOne.GetStat("defence") <= playerOne.GetStat("magic defence")) {
				return "defence";
			}
			else {
				return "magic defence";
			}
		}
		else if (playerOne.GetStat("speed") < 10 || playerOne.GetStat("time magic") < 10) {
			if (playerOne.GetStat("speed") <= playerOne.GetStat("time magic")) {
				return "speed";
			}
			else {
				return "time magic";
			}
		}
		else if (playerOne.GetStat("attack") < 10 || playerOne.GetStat("magic attack") < 10) {
			if (playerOne.GetStat("attack") <= playerOne.GetStat("magic attack")) {
				return "attack";
			}
			else {
				return "magic attack";
			}
		}
		else if (playerOne.GetStat("accuracy") < 10 || playerOne.GetStat("evasion") < 10){
			if (playerOne.GetStat("accuracy") <= playerOne.GetStat("evasion")) {
				return "accuracy";
			}
			else {
				return "evasion";
			}
		}
	}

	return "";
}

std::string StrategyTwelve(std::string moveOrStat, Player playerOne) {
	if (moveOrStat == "move") {
		if (playerOne.speedType != 'h') {
			return "haste";
		}
		else {
			return "attack";
		}
	}
	else if (moveOrStat == "stat") {
		if (playerOne.GetStat("attack") < 10 || playerOne.GetStat("time magic") < 10) {
			if (playerOne.GetStat("attack") <= playerOne.GetStat("time magic")) {
				return "attack";
			}
			else {
				return "time magic";
			}
		}
		else {
			std::string stats[6], chosenStat;
			stats[0] = "defence";
			stats[1] = "magic attack";
			stats[2] = "magic defence";
			stats[3] = "speed";
			stats[4] = "accuracy";
			stats[5] = "evasion";
			bool chosen = false;

			while (!chosen) {
				chosenStat = stats[rand() % 6];
				if (playerOne.GetStat(chosenStat) >= 10) {
					// Do not raise stat
					chosen = false;
				}
				else {
					// Raise stat
					chosen = true;
					return chosenStat;
				}
			}
		}
	}

	return "";
}

// The loop for the training battle
int TrainAIBattleLoop(Player playerOne, Player playerTwo, int strategyNum, Network& network) {
	float simulationNumber = 1, winner = 0, simNumber = 0, turnNumber = 1, statNumber = 1, enemyAttackCounter = 0, enemyMagicAttackCounter = 0, enemyHasteCounter = 0, enemySlowCounter = 0, playerAttackCounter = 0, playerMagicAttackCounter = 0, playerHasteCounter = 0, playerSlowCounter = 0;
	float currentTurn, damage, playerFirstMove = 0, enemyFirstMove = 0, playerFavouriteMove = 0, enemyFavouriteMove = 0;
	int winNumber = 0;
	std::string moveChoice, statChoice;
	std::ofstream strategySpecificWins;
	std::cout << "Training against strategy number " << strategyNum << "...\n";

	if (strategyNum == 1) {
		strategySpecificWins.open("Strategy1Wins.txt");
	}
	else if (strategyNum == 2) {
		strategySpecificWins.open("Strategy2Wins.txt");
	}
	else if (strategyNum == 3) {
		strategySpecificWins.open("Strategy3Wins.txt");
	}
	else if (strategyNum == 4) {
		strategySpecificWins.open("Strategy4Wins.txt");
	}
	else if (strategyNum == 5) {
		strategySpecificWins.open("Strategy5Wins.txt");
	}
	else if (strategyNum == 6) {
		strategySpecificWins.open("Strategy6Wins.txt");
	}
	else if (strategyNum == 7) {
		strategySpecificWins.open("Strategy7Wins.txt");
	}
	else if (strategyNum == 8) {
		strategySpecificWins.open("Strategy8Wins.txt");
	}
	else if (strategyNum == 9) {
		strategySpecificWins.open("Strategy9Wins.txt");
	}
	else if (strategyNum == 10) {
		strategySpecificWins.open("Strategy10Wins.txt");
	}
	else if (strategyNum == 11) {
		strategySpecificWins.open("Strategy11Wins.txt");
	}
	else if (strategyNum == 12) {
		strategySpecificWins.open("Strategy12Wins.txt");
	}

	// Strategy specific variables needed
	float strategyAttackDamage = 0, strategyMagicAttackDamage = 0;

	while (simulationNumber < 100) {
		statNumber = 1;
		while (playerOne.GetStat("total") <= 80 && playerTwo.GetStat("total") <= 80) {
			turnNumber = 1;
			strategyAttackDamage = 0;
			strategyMagicAttackDamage = 0;
			while (playerOne.GetStat("HP") > 0 && playerTwo.GetStat("HP") > 0) {
				currentTurn = characterTurn(playerOne, playerTwo, turnNumber);
				if (currentTurn == 1) {
					// Planned strategy player turn
					if (strategyNum == 1) {
						if (turnNumber < 5) {
							moveChoice = StrategyOne("move", "", turnNumber, playerOne, playerTwo);
							damage = moveController(moveChoice, playerOne, playerTwo, true);
							if (moveChoice == "attack") {
								strategyAttackDamage = damage;
								playerTwo.DamagePlayer((int)damage);
							}
							else  if (moveChoice == "magic attack") {
								strategyMagicAttackDamage = damage;
								playerTwo.DamagePlayer((int)damage);
							}
							else if (moveChoice == "haste") {
								playerOne.ActivateHaste();
							}
							else if (moveChoice == "slow") {
								playerTwo.ActivateSlow((int)playerOne.GetStat("time magic"));
							}
						}
						else {
							if (strategyAttackDamage >= strategyMagicAttackDamage) {
								moveChoice = StrategyOne("move", "attack", turnNumber, playerOne, playerTwo);
							}
							else {
								moveChoice = StrategyOne("move", "magic attack", turnNumber, playerOne, playerTwo);
							}
							damage = moveController(moveChoice, playerOne, playerTwo, true);
							if (moveChoice == "attack" || moveChoice == "magic attack") {
								playerTwo.DamagePlayer((int)damage);
							}
							else if (moveChoice == "haste") {
								playerOne.ActivateHaste();
							}
							else if (moveChoice == "slow") {
								playerTwo.ActivateSlow((int)playerOne.GetStat("time magic"));
							}
						}
					}
					else if (strategyNum == 2) {
						moveChoice = StrategyTwo("move", playerOne); // Always returns "magic attack"
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						playerTwo.DamagePlayer((int)damage);
					}
					else if (strategyNum == 3) {
						moveChoice = StrategyThree("move", playerOne); // Always returns "attack"
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						playerTwo.DamagePlayer((int)damage);
					}
					else if (strategyNum == 4) {
						if (turnNumber < 3) {
							moveChoice = StrategyFour("move", "", turnNumber, 0, playerOne);
							damage = moveController(moveChoice, playerOne, playerTwo, true);
							if (moveChoice == "attack") {
								strategyAttackDamage = damage;
								playerTwo.DamagePlayer((int)damage);
							}
							else if (moveChoice == "magic attack") {
								strategyMagicAttackDamage = damage;
								playerTwo.DamagePlayer((int)damage);
							}
						}
						else {
							// Always returns either "attack" or "magic attack"
							if (strategyAttackDamage >= strategyMagicAttackDamage) {
								moveChoice = StrategyFour("move", "attack", turnNumber, 0, playerOne);
							}
							else if (strategyMagicAttackDamage > strategyAttackDamage) {
								moveChoice = StrategyFour("move", "magic attack", turnNumber, 0, playerOne);
							}
							damage = moveController(moveChoice, playerOne, playerTwo, true);
							playerTwo.DamagePlayer((int)damage);
						}
					}
					else if (strategyNum == 5) {
						moveChoice = StrategyFive("move", 0, playerOne); // Always returns "attack"
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						playerTwo.DamagePlayer((int)damage);
					}
					else if (strategyNum == 6) {
						moveChoice = StrategySix("move", "", 0, playerOne, playerTwo);
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						if (moveChoice == "attack") {
							playerTwo.DamagePlayer((int)damage);
						}
						else if (moveChoice == "haste") {
							playerOne.ActivateHaste();
						}
						else if (moveChoice == "slow") {
							playerTwo.ActivateSlow((int)playerOne.GetStat("time magic"));
						}
					}
					else if (strategyNum == 7) {
						moveChoice = StrategySeven("move", playerOne, playerTwo);
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						if (moveChoice == "attack" || moveChoice == "magic attack") {
							playerTwo.DamagePlayer((int)damage);
						}
						else if (moveChoice == "haste") {
							playerOne.ActivateHaste();
						}
					}
					else if (strategyNum == 8) {
						moveChoice = StrategyEight("move", turnNumber, playerOne); // Always returns either "attack" or "magic attack"
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						playerTwo.DamagePlayer((int)damage);
					}
					else if (strategyNum == 9) {
						moveChoice = StrategyNine("move", turnNumber, playerOne); // Always return either "attack" or "magic attack"
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						playerTwo.DamagePlayer((int)damage);
					}
					else if (strategyNum == 10) {
						moveChoice = StrategyTen("move", playerOne); 
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						if (moveChoice == "attack") {
							playerTwo.DamagePlayer((int)damage);
						}
						else if (moveChoice == "haste") {
							playerOne.ActivateHaste();
						}
					}
					else if (strategyNum == 11) {
						if (turnNumber < 5) {
							moveChoice = StrategyEleven("move", "", turnNumber, playerOne, playerTwo);
							damage = moveController(moveChoice, playerOne, playerTwo, true);
							if (moveChoice == "attack") {
								strategyAttackDamage = damage;
								playerTwo.DamagePlayer((int)damage);
							}
							else  if (moveChoice == "magic attack") {
								strategyMagicAttackDamage = damage;
								playerTwo.DamagePlayer((int)damage);
							}
							else if (moveChoice == "haste") {
								playerOne.ActivateHaste();
							}
							else if (moveChoice == "slow") {
								playerTwo.ActivateSlow((int)playerOne.GetStat("time magic"));
							}
						}
						else {
							if (strategyAttackDamage >= strategyMagicAttackDamage) {
								moveChoice = StrategyEleven("move", "attack", turnNumber, playerOne, playerTwo);
							}
							else {
								moveChoice = StrategyEleven("move", "magic attack", turnNumber, playerOne, playerTwo);
							}
							damage = moveController(moveChoice, playerOne, playerTwo, true);
							if (moveChoice == "attack" || moveChoice == "magic attack") {
								playerTwo.DamagePlayer((int)damage);
							}
							else if (moveChoice == "haste") {
								playerOne.ActivateHaste();
							}
							else if (moveChoice == "slow") {
								playerTwo.ActivateSlow((int)playerOne.GetStat("time magic"));
							}
						}
					}
					else if (strategyNum == 12) {
						moveChoice = StrategyTwelve("move", playerOne);
						damage = moveController(moveChoice, playerOne, playerTwo, true);
						if (moveChoice == "attack") {
							playerTwo.DamagePlayer((int)damage);
						}
						else if (moveChoice == "haste") {
							playerOne.ActivateHaste();
						}
					}

					if (moveChoice == "attack") {
						playerAttackCounter++;
						if (turnNumber == 1) {
							playerFirstMove = 1;
						}
					}
					else if (moveChoice == "magic attack") {
						playerMagicAttackCounter++;
						if (turnNumber == 1) {
							playerFirstMove = 2;
						}
					}
					else if (moveChoice == "haste") {
						playerHasteCounter++;
						if (turnNumber == 1) {
							playerFirstMove = 3;
						}
					}
					else if (moveChoice == "slow") {
						playerSlowCounter++;
						if (turnNumber == 1) {
							playerFirstMove = 4;
						}
					}
				}
				else if (currentTurn == 2) {
					moveChoice = enemyTurn(playerOne, playerTwo);
					damage = moveController(moveChoice, playerOne, playerTwo, true);
					if (moveChoice == "attack" || moveChoice == "magic attack") {
						playerOne.DamagePlayer((int)damage);
					}
					else if (moveChoice == "haste") {
						playerTwo.ActivateHaste();
					}
					else if (moveChoice == "slow") {
						playerOne.ActivateSlow((int)playerTwo.GetStat("time magic"));
					}

					if (moveChoice == "attack") {
						enemyAttackCounter++;
						if (turnNumber == 1) {
							enemyFirstMove = 1;
						}
					}
					else if (moveChoice == "magic attack") {
						enemyMagicAttackCounter++;
						if (turnNumber == 1) {
							enemyFirstMove = 2;
						}
					}
					else if (moveChoice == "haste") {
						enemyHasteCounter++;
						if (turnNumber == 1) {
							enemyFirstMove = 3;
						}
					}
					else if (moveChoice == "slow") {
						enemySlowCounter++;
						if (turnNumber == 1) {
							enemyFirstMove = 4;
						}
					}
				}
				turnNumber++;

				if (playerOne.GetStat("HP") < 0) {
					winner = 2;
					winNumber++;
					strategySpecificWins << winner << "\n";
				}
				else if (playerTwo.GetStat("HP") < 0) {
					winner = 1;
					strategySpecificWins << winner << "\n";
				}

				if (turnNumber > 200) {
					if (playerOne.GetStat("HP") >= playerTwo.GetStat("HP")) {
						winner = 1;
						strategySpecificWins << winner << "\n";
					}
					else {
						winner = 2;
						winNumber++;
						strategySpecificWins << winner << "\n";
					}
					break;
				}
			}

			if (playerOne.GetStat("total") == 80 && playerTwo.GetStat("total") == 80) {
				break;
			}

			// Stat raising 
			if (strategyNum == 1) {
				if (strategyAttackDamage >= strategyMagicAttackDamage) {
					statChoice = StrategyOne("stat", "attack", 0, playerOne, playerTwo);
				}
				else {
					statChoice = StrategyOne("stat", "magic attack", 0, playerOne, playerTwo);
				}
			}
			else if (strategyNum == 2) {
				statChoice = StrategyTwo("stat", playerOne);
			}
			else if (strategyNum == 3) {
				statChoice = StrategyThree("stat", playerOne);
			}
			else if (strategyNum == 4) {
				statChoice = StrategyFour("stat", "", 0, statNumber, playerOne);
			}
			else if (strategyNum == 5) {
				statChoice = StrategyFive("stat", statNumber, playerOne);
			}
			else if (strategyNum == 6) {
				if (enemyAttackCounter >= enemyMagicAttackCounter) {
					statChoice = StrategySix("stat", "attack", statNumber, playerOne, playerTwo);
				}
				else {
					statChoice = StrategySix("stat", "magic attack", statNumber, playerOne, playerTwo);
				}
			}
			else if (strategyNum == 7) {
				statChoice = StrategySeven("stat", playerOne, playerTwo);
			}
			else if (strategyNum == 8) {
				statChoice = StrategyEight("stat", 0, playerOne);
			}
			else if (strategyNum == 9) {
				statChoice = StrategyNine("stat", 0, playerOne);
			}
			else if (strategyNum == 10) {
				statChoice = StrategyTen("stat", playerOne);
			}
			else if (strategyNum == 11) {
				statChoice = StrategyEleven("stat", "", 0, playerOne, playerTwo);
			}
			else if (strategyNum == 12) {
				statChoice = StrategyTwelve("stat", playerOne);
			}
			playerOne.IncreaseStat(statChoice);

			statNumber++;
			if (strategyNum == 4) {
				if (statNumber > 8) {
					statNumber = 1;
				}
			}
			else if (strategyNum == 5 || strategyNum == 6) {
				if (statNumber > 6) {
					statNumber = 1;
				}
			}

			// Turn the stat the player just raised into a value
			float playerStatRaised = 0;
			if (statChoice == "attack") {
				playerStatRaised = 1;
			}
			else if (statChoice == "magic attack") {
				playerStatRaised = 2;
			}
			else if (statChoice == "defence") {
				playerStatRaised = 3;
			}
			else if (statChoice == "magic defence") {
				playerStatRaised = 4;
			}
			else if (statChoice == "speed") {
				playerStatRaised = 5;
			}
			else if (statChoice == "time magic") {
				playerStatRaised = 6;
			}
			else if (statChoice == "accuracy") {
				playerStatRaised = 7;
			}
			else if (statChoice == "evasion") {
				playerStatRaised = 8;
			}

			// Work out the players most favoured move this round
			if (playerAttackCounter >= playerMagicAttackCounter && playerAttackCounter >= playerHasteCounter && playerAttackCounter >= playerSlowCounter) {
				playerFavouriteMove = 1;
			}
			else if (playerMagicAttackCounter >= playerAttackCounter && playerMagicAttackCounter >= playerHasteCounter && playerMagicAttackCounter >= playerSlowCounter) {
				playerFavouriteMove = 2;
			}
			else if (playerHasteCounter >= playerAttackCounter && playerHasteCounter >= playerMagicAttackCounter && playerHasteCounter >= playerSlowCounter) {
				playerFavouriteMove = 3;
			}
			else {
				playerFavouriteMove = 4;
			}

			// Work out the enemies most favoured move this round
			if (enemyAttackCounter >= enemyMagicAttackCounter && enemyAttackCounter >= enemyHasteCounter && enemyAttackCounter >= enemySlowCounter) {
				enemyFavouriteMove = 1;
			}
			else if (enemyMagicAttackCounter >= enemyAttackCounter && enemyMagicAttackCounter >= enemyHasteCounter && enemyMagicAttackCounter >= enemySlowCounter) {
				enemyFavouriteMove = 2;
			}
			else if (enemyHasteCounter >= enemyAttackCounter && enemyHasteCounter >= enemyMagicAttackCounter && enemyHasteCounter >= enemySlowCounter) {
				enemyFavouriteMove = 3;
			}
			else if (enemySlowCounter >= enemyAttackCounter && enemySlowCounter >= enemyMagicAttackCounter && enemySlowCounter >= enemyHasteCounter) {
				enemyFavouriteMove = 4;
			}

			statChoice = enemyRaiseStat(playerTwo, playerOne, network, winner, playerStatRaised, playerFavouriteMove, enemyFavouriteMove, playerFirstMove, enemyFirstMove, turnNumber, 0, true);
			playerTwo.IncreaseStat(statChoice);
		}

		playerOne.FullReset();
		playerTwo.FullReset();

		simulationNumber++;
	}

	strategySpecificWins.close();
	return winNumber;
}

// The game loop is in here.
int main()
{
	srand((unsigned int)(time(NULL) * rand() * rand()));
	Player playerOne;
	Player playerTwo;
	Network network;
	float currentTurn, damage, winner = 0, winNumber = 0, strategyNum = 12, turnNumber = 1, playerFirstMove = 0, enemyFirstMove = 0, playerFavouriteMove = 0, enemyFavouriteMove = 0, enemyAttackCounter = 0, enemyMagicAttackCounter = 0, enemyHasteCounter = 0, enemySlowCounter = 0, playerAttackCounter = 0, playerMagicAttackCounter = 0, playerHasteCounter = 0, playerSlowCounter = 0;
	std::string moveChoice, statChoice, enemyType, trainOrNot;
	bool enemyTypeChosen = false, trainingChosen = false;
	int winNumberTraining = 0, enemyTypeNum;
	std::ofstream winNumberFile, errorDocumentClear;
	errorDocumentClear.open("ErrorValues.txt", std::fstream::out);
	errorDocumentClear.close();

	std::cout << "Would you like to fight the trained or untrained enemy?\n";
	while (!enemyTypeChosen) {
		enemyType = "";
		std::getline(std::cin, enemyType);
		for (unsigned int i = 0; i < enemyType.length(); i++) {
			enemyType[i] = tolower((char)enemyType[i]); // Turn the entire input to lower case letters
		}
		if (enemyType == "trained") {
			enemyTypeNum = 0;

			std::cout << "Would you like to train the enemy first?\n";
			while (!trainingChosen) {
				trainOrNot = "";
				std::getline(std::cin, trainOrNot);
				for (unsigned int i = 0; i < trainOrNot.length(); i++) {
					trainOrNot[i] = tolower((char)trainOrNot[i]); // Turn the entire input to lower case letters
				}
				if (trainOrNot == "yes") {
					network.Init(23, 16, 8, "gen");

					winNumberFile.open("WinNumbers.txt");

					// Run the AI training loop
					for (int i = 0; i < strategyNum; i++) {
						winNumberTraining = TrainAIBattleLoop(playerOne, playerTwo, i + 1, network);
						winNumberFile << winNumberTraining << "\n";
					}
					winNumberFile.close();
					network.PrintToFile();

					trainingChosen = true;
				}
				else if (trainOrNot == "no") {
					network.Init(23, 16, 8, "load");

					trainingChosen = true;
				}
				else {
					std::cout << "That is not a valid choice. Choose again\n";
				}
			}

			enemyTypeChosen = true;
		}
		else if (enemyType == "untrained") {
			enemyTypeNum = 1;
			enemyTypeChosen = true;
		}
		else {
			std::cout << "That is not a valid choice. Choose again\n";
		}
	}

	// Run the bot vs player game

	while (playerOne.GetStat("total") <= 80 && playerTwo.GetStat("total") <= 80) {
		turnNumber = 1;
		std::cout << "------------------------------------------------------------\n------------------------------------------------------------\n------------------------------------------------------------\n";
		while (playerTwo.GetStat("HP") > 0 && playerOne.GetStat("HP") > 0) {
			currentTurn = characterTurn(playerOne, playerTwo, turnNumber);
			if (currentTurn == 1) {
				moveChoice = playerTurn();
				damage = moveController(moveChoice, playerOne, playerTwo, false);
				if (moveChoice == "attack" || moveChoice == "magic attack") {
					playerTwo.DamagePlayer((int)damage);
				}
				else if (moveChoice == "haste" && damage == 1) {
					playerOne.ActivateHaste();
					std::cout << "Your new speed: " << playerOne.currentSpeed << std::endl;
				}
				else if (moveChoice == "slow" && damage == 1) {
					playerTwo.ActivateSlow((int)playerOne.GetStat("time magic"));
					std::cout << "Enemies new speed: " << playerTwo.currentSpeed << std::endl;
				}

				if (moveChoice == "attack") {
					playerAttackCounter++;
					if (turnNumber == 1) {
						playerFirstMove = 1;
					}
				}
				else if (moveChoice == "magic attack") {
					playerMagicAttackCounter++;
					if (turnNumber == 1) {
						playerFirstMove = 2;
					}
				}
				else if (moveChoice == "haste") {
					playerHasteCounter++;
					if (turnNumber == 1) {
						playerFirstMove = 3;
					}
				}
				else if (moveChoice == "slow") {
					playerSlowCounter++;
					if (turnNumber == 1) {
						playerFirstMove = 4;
					}
				}
			}
			else if (currentTurn == 2) {
				moveChoice = enemyTurn(playerOne, playerTwo);
				damage = moveController(moveChoice, playerTwo, playerOne, false);
				if (moveChoice == "attack" || moveChoice == "magic attack") {
					playerOne.DamagePlayer((int)damage);
				}
				else if (moveChoice == "haste" && damage == 1) {
					playerTwo.ActivateHaste();
					std::cout << "Enemies new speed: " << playerTwo.currentSpeed << std::endl;
				}
				else if (moveChoice == "slow" && damage == 1) {
					playerOne.ActivateSlow((int)playerTwo.GetStat("time magic"));
					std::cout << "Your new speed: " << playerOne.currentSpeed << std::endl;
				}

				if (moveChoice == "attack") {
					enemyAttackCounter++;
					if (turnNumber == 1) {
						enemyFirstMove = 1;
					}
				}
				else if (moveChoice == "magic attack") {
					enemyMagicAttackCounter++;
					if (turnNumber == 1) {
						enemyFirstMove = 2;
					}
				}
				else if (moveChoice == "haste") {
					enemyHasteCounter++;
					if (turnNumber == 1) {
						enemyFirstMove = 3;
					}
				}
				else if (moveChoice == "slow") {
					enemySlowCounter++;
					if (turnNumber == 1) {
						enemyFirstMove = 4;
					}
				}
			}
			std::cout << "Your HP: " << playerOne.GetStat("HP") << "\n";
			std::cout << "Enemy HP: " << playerTwo.GetStat("HP") << "\n";
			std::cout << "------------------------------------------------------------\n";
			turnNumber++;

			if (playerOne.GetStat("HP") <= 0) {
				winner = 2;
			}
			else if (playerTwo.GetStat("HP") <= 0) {
				winner = 1;
			}
		}

		if (winner == 1) {
			std::cout << "You win!\n";
			winNumber++;
		}
		else if (winner == 2) {
			std::cout << "You lose!\n";
		}

		if (playerOne.GetStat("total") >= 80 || playerTwo.GetStat("total") >= 80) {
			break;
		}

		statChoice = raiseStat(playerOne);
		playerOne.IncreaseStat(statChoice);

		// Turn the stat the player just raised into a value
		float playerStatRaised = 0;
		if (statChoice == "attack") {
			playerStatRaised = 1;
		}
		else if (statChoice == "magic attack") {
			playerStatRaised = 2;
		}
		else if (statChoice == "defence") {
			playerStatRaised = 3;
		}
		else if (statChoice == "magic defence") {
			playerStatRaised = 4;
		}
		else if (statChoice == "speed") {
			playerStatRaised = 5;
		}
		else if (statChoice == "time magic") {
			playerStatRaised = 6;
		}
		else if (statChoice == "accuracy") {
			playerStatRaised = 7;
		}
		else if (statChoice == "evasion") {
			playerStatRaised = 8;
		}

		// Work out the players most favoured move this round
		if (playerAttackCounter >= playerMagicAttackCounter && playerAttackCounter >= playerHasteCounter && playerAttackCounter >= playerSlowCounter) {
			playerFavouriteMove = 1;
		}
		else if (playerMagicAttackCounter >= playerAttackCounter && playerMagicAttackCounter >= playerHasteCounter && playerMagicAttackCounter >= playerSlowCounter) {
			playerFavouriteMove = 2;
		}
		else if (playerHasteCounter >= playerAttackCounter && playerHasteCounter >= playerMagicAttackCounter && playerHasteCounter >= playerSlowCounter) {
			playerFavouriteMove = 3;
		}
		else {
			playerFavouriteMove = 4;
		}

		// Work out the enemies most favoured move this round
		if (enemyAttackCounter >= enemyMagicAttackCounter && enemyAttackCounter >= enemyHasteCounter && enemyAttackCounter >= enemySlowCounter) {
			enemyFavouriteMove = 1;
		}
		else if (enemyMagicAttackCounter >= enemyAttackCounter && enemyMagicAttackCounter >= enemyHasteCounter && enemyMagicAttackCounter >= enemySlowCounter) {
			enemyFavouriteMove = 2;
		}
		else if (enemyHasteCounter >= enemyAttackCounter && enemyHasteCounter >= enemyMagicAttackCounter && enemyHasteCounter >= enemySlowCounter) {
			enemyFavouriteMove = 3;
		}
		else if (enemySlowCounter >= enemyAttackCounter && enemySlowCounter >= enemyMagicAttackCounter && enemySlowCounter >= enemyHasteCounter) {
			enemyFavouriteMove = 4;
		}

		statChoice = enemyRaiseStat(playerTwo, playerOne, network, winner, playerStatRaised, playerFavouriteMove, enemyFavouriteMove, playerFirstMove, enemyFirstMove, turnNumber, enemyTypeNum, false);
		playerTwo.IncreaseStat(statChoice);

		playerOne.Reset();
		playerTwo.Reset();
	}

	std::cout << "Game end. You won " << winNumber << " times!\n";
	std::cin;
}