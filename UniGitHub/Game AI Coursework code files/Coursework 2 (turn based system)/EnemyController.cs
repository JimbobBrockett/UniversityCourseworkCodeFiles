using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyController : MonoBehaviour
{

    private int HP = 10;
    private string item = "potion";
    private bool stolen = false, defending = false;
    private GameObject ally, controller, player;

    // Start is called before the first frame update
    void Start()
    {
        ally = GameObject.FindGameObjectWithTag("Ally");
        controller = GameObject.FindGameObjectWithTag("GameController");
        player = GameObject.FindGameObjectWithTag("Player");

        HP = controller.GetComponent<BattleController>().enemyBaseHP;
        ally.GetComponent<PlayerAllyController>().boolSet.enemyHasItem = true;
        ally.GetComponent<PlayerAllyController>().boolSet.enemyHPAboveHalf = true;
    }

    // Chooses a move using fuzzy logic 
    public void ChooseMove()
    {
        defending = false;

        // Get the fuzzy inputs between 0 and 1
        float playerHPFactor = (float)player.GetComponent<PlayerController>().GetHP() / (float)player.GetComponent<PlayerController>().GetMaxHP();
        float allyHpFactor = (float)ally.GetComponent<PlayerAllyController>().GetHP() / (float)ally.GetComponent<PlayerAllyController>().GetMaxHP();
        float enemyHPFactor = (float)HP / (float)controller.GetComponent<BattleController>().enemyBaseHP;
        float playerChargingFactor = 0;
        if (player.GetComponent<PlayerController>().GetIsCharging())
        {
            playerChargingFactor = 1;
        }
        float allyChargingFactor = 0;
        if (ally.GetComponent<PlayerAllyController>().GetIsCharging())
        {
            allyChargingFactor = 1;
        }

        // Compute the logic using Zadeh operators: AND -> Mathf.Min(), OR -> Mathf.Max()
        float shouldISelfDestruct = 1 - Mathf.Max(Mathf.Max(playerHPFactor, allyHpFactor), enemyHPFactor);
        float shouldIDefend = Mathf.Max(playerChargingFactor, allyChargingFactor);
        float shouldIAttackThePlayer = Mathf.Min(allyHpFactor, 1 - playerHPFactor);
        float shouldIAttackTheAlly = Mathf.Min(playerHPFactor, 1 - allyHpFactor);
        float shouldIHeal = Mathf.Min(Mathf.Min(playerHPFactor, allyHpFactor), enemyHPFactor);

        // Make sure the probability is never 0
        if (shouldISelfDestruct <= 0)
        {
            shouldISelfDestruct = 0.01f;
        }
        if (shouldIDefend <= 0)
        {
            shouldIDefend = 0.01f;
        }
        if (shouldIAttackThePlayer <= 0)
        {
            shouldIAttackThePlayer = 0.01f;
        }
        if (shouldIAttackTheAlly <= 0)
        {
            shouldIAttackTheAlly = 0.01f;
        }
        if (shouldIHeal <= 0)
        {
            shouldIHeal = 0.01f;
        }

        // Run all of the values through the SoftMax function, creating a probability distribution
        float destructVal, defendVal, playerVal, allyVal, healVal, totalVal;
        totalVal = Mathf.Exp(shouldISelfDestruct) + Mathf.Exp(shouldIDefend) + Mathf.Exp(shouldIAttackThePlayer) + Mathf.Exp(shouldIAttackTheAlly) + Mathf.Exp(shouldIHeal);
        destructVal = SoftMaxFunction(totalVal, shouldISelfDestruct);
        defendVal = SoftMaxFunction(totalVal, shouldIDefend);
        playerVal = SoftMaxFunction(totalVal, shouldIAttackThePlayer);
        allyVal = SoftMaxFunction(totalVal, shouldIAttackTheAlly);
        healVal = SoftMaxFunction(totalVal, shouldIHeal);

        // Send the probabilities to the UI controller
        controller.GetComponent<UIController>().SetProbabilities(destructVal, defendVal, playerVal, allyVal, healVal);

        // Create a list of the moves, each occurs the number of times it is likely to happen relative to the other options
        List<int> listOfMoves = new List<int>();
        int destructProbability = Mathf.RoundToInt(destructVal * 100);
        int defendProbability = Mathf.RoundToInt(defendVal * 100);
        int playerProbability = Mathf.RoundToInt(playerVal * 100);
        int allyProbability = Mathf.RoundToInt(allyVal * 100);
        int healProbability = Mathf.RoundToInt(healVal * 100);
        int index = 0;

        for (int i = 0; i < destructProbability; i++)
        {
            listOfMoves.Add(0);
        }
        index = destructProbability;
        for (int i = index; i < index + defendProbability; i++)
        {
            listOfMoves.Add(1);
        }
        index += defendProbability;
        for (int i = index; i < index + playerProbability; i++)
        {
            listOfMoves.Add(2);
        }
        index += playerProbability;
        for (int i = index; i < index + allyProbability; i++)
        {
            listOfMoves.Add(3);
        }
        index += allyProbability;
        for (int i = index; i < index + healProbability; i++)
        {
            listOfMoves.Add(4);
        }
        index += healProbability;

        // Randomly choose the move number and then execute the appropriate action
        int moveNum = listOfMoves[Random.Range(0, listOfMoves.Count)];
        if (moveNum == 0)
        {
            int damage = Random.Range(1, 4) * controller.GetComponent<BattleController>().enemyAttack;
            controller.GetComponent<UIController>().currentMoveText.text = "Attack";
            damage = CalculateDamage(ally, damage);
            ally.GetComponent<PlayerAllyController>().DamageAlly(damage);
        }
        else if (moveNum == 1)
        {
            int damage = Random.Range(1, 4) * controller.GetComponent<BattleController>().enemyAttack;
            controller.GetComponent<UIController>().currentMoveText.text = "Attack";
            damage = CalculateDamage(player, damage);
            player.GetComponent<PlayerController>().DamagePlayer(damage);
        }
        else if (moveNum == 2)
        {
            controller.GetComponent<UIController>().currentMoveText.text = "Defend";
            defending = true;
        }
        else if (moveNum == 3)
        {
            controller.GetComponent<UIController>().currentMoveText.text = "Heal";
            HP += 5;
            if (HP > controller.GetComponent<BattleController>().enemyBaseHP / 2)
            {
                ally.GetComponent<PlayerAllyController>().boolSet.enemyHPAboveHalf = true;
            }
            if (HP > controller.GetComponent<BattleController>().enemyBaseHP)
            {
                HP = controller.GetComponent<BattleController>().enemyBaseHP;
            }
        }
        else if (moveNum == 4)
        {
            controller.GetComponent<UIController>().currentMoveText.text = "Self-Destruct";
            player.GetComponent<PlayerController>().DamagePlayer(CalculateDamage(player, 7));
            ally.GetComponent<PlayerAllyController>().DamageAlly(CalculateDamage(ally, 7));
            OnDead();
        }
        StartCoroutine(NextTurnWaiter());
    }

    // Calculates the SoftMax value for each of the move probabilities
    private float SoftMaxFunction(float total, float initial)
    {
        return (Mathf.Exp(initial) / total);
    }

    // Calculates initial damage, checks for type weaknesses and doubles the damage if necessary, then returns it
    private int CalculateDamage(GameObject target, int damage)
    {
        if ((gameObject.name == "BlueCharacter(Clone)" && target.name == "GoodRed(Clone)") ||
            (gameObject.name == "RedCharacter(Clone)" && target.name == "GoodGreen(Clone)") ||
            (gameObject.name == "GreenCharacter(Clone)" && target.name == "GoodYellow(Clone)") ||
            (gameObject.name == "YellowCharacter(Clone)" && target.name == "GoodBlue(Clone)") ||
            (gameObject.name == "BlueCharacter(Clone)" && target.name == "AllyRed(Clone)") ||
            (gameObject.name == "RedCharacter(Clone)" && target.name == "AllyGreen(Clone)") ||
            (gameObject.name == "GreenCharacter(Clone)" && target.name == "AllyYellow(Clone)") ||
            (gameObject.name == "YellowCharacter(Clone)" && target.name == "AllyBlue(Clone)"))
        {
            damage *= 2;
        }

        return damage;
    }

    // Reduces the enemy's HP based on the amount passed in, and checks for enemy death 
    public void DamageEnemy(int damage)
    {
        damage -= controller.GetComponent<BattleController>().enemyDefence;
        if (defending)
        {
            damage -= 3;
        }
        if (damage < 1)
        {
            damage = 1;
        }
        HP -= damage;
        if (HP <= 0)
        {
            OnDead();
        }
        else if (HP <= controller.GetComponent<BattleController>().enemyBaseHP / 2)
        {
            ally.GetComponent<PlayerAllyController>().boolSet.enemyHPAboveHalf = false;
        }
    }

    // If the enemy is dead, a new enemy is spawned and the current one is destroyed
    private void OnDead()
    {
        controller.GetComponent<UIController>().IncreaseScore();
        controller.GetComponent<BattleController>().SetPlayerAndAllyHPs();
        if (controller.GetComponent<UIController>().GetScore()%5 != 0)
        {
            controller.GetComponent<BattleController>().NewEnemyNeeded();
        }
        else
        {
            controller.GetComponent<UIController>().StartStatRaising();
        }
        Destroy(gameObject);
    }

    // Removes the item from the enemy and puts in the "bag", assuming there is an item to steal
    public void ItemStolen()
    {
        if (!stolen)
        {
            controller.GetComponent<BattleController>().item = item;
            Debug.Log(item + " was stolen!");
            item = "";
            stolen = true;
            ally.GetComponent<PlayerAllyController>().boolSet.itemIsInBag = true;
            ally.GetComponent<PlayerAllyController>().boolSet.enemyHasItem = false;
        }
        else
        {
            Debug.Log("Item is already stolen");
        }
    }

    // Returns the enemy's HP, mainly for use by the UIController
    public int GetHP()
    {
        return HP;
    }

    // Waits 3 seconds, then clears the move name text and moves to the next turn
    IEnumerator NextTurnWaiter()
    {
        yield return new WaitForSeconds(3);
        controller.GetComponent<UIController>().currentMoveText.text = "";
        controller.GetComponent<BattleController>().NextTurn();
        StopCoroutine(NextTurnWaiter());
    }
}
