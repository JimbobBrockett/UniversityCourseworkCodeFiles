using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BattleController : MonoBehaviour
{

    public List<GameObject> enemyTypes;
    public List<GameObject> playerTypes;
    public List<GameObject> allyTypes;
    private int turn = 1, turnsToDefeatEnemy = 0, averageNumberOfTurns = 0, latestPlayerHP = 0, latestAllyHP = 0;
    private int[] turnOrder;
    private bool statRaising = false;
    public string item = "";
    public int enemyAttack = 1, enemyDefence = 1, enemyBaseHP = 10, enemySpeed = 1;

    // Start is called before the first frame update
    void Start()
    {
        Random.InitState((int)System.DateTime.Now.Ticks);
        turnOrder = new int[3];

        // Spawn the player and ally, sets the intial turn order, then creates an enemy
        Instantiate(playerTypes[Random.Range(0, 4)], new Vector3(3, 1, -9), Quaternion.Euler(0, 0, 0));
        Instantiate(allyTypes[Random.Range(0, 4)], new Vector3(-3, 1, -9), Quaternion.Euler(0, 0, 0));
        ChangeTurnOrder();
        StartCoroutine(CreateEnemy());
    }

    private void Update()
    {
        if (!GameObject.FindGameObjectWithTag("Enemy"))
        {
            turn = 1;
            if (!statRaising)
            {
                gameObject.GetComponent<UIController>().ResetUIBools();
            }
        }
    }

    // Sets the turn back to the player and creates a new enemy
    public void NewEnemyNeeded()
    {
        gameObject.GetComponent<UIController>().ResetUIBools();
        GameObject.FindGameObjectWithTag("Ally").GetComponent<PlayerAllyController>().ResetCharge();
        turn = 1;
        StartCoroutine(gameObject.GetComponent<UIController>().EnemySelfDestructed());
        StartCoroutine(CreateEnemy());
    }

    // Waits for 3 seconds and then creates a random enemy, stops itself
    IEnumerator CreateEnemy()
    {
        yield return new WaitForSeconds(3);
        averageNumberOfTurns = (averageNumberOfTurns + turnsToDefeatEnemy) / 2;
        turnsToDefeatEnemy = 0;
        Instantiate(enemyTypes[Random.Range(0, 4)], new Vector3(0, 1, 0), Quaternion.Euler(0, 0, 0));
        gameObject.GetComponent<UIController>().ResetUIBools();
        StopCoroutine(CreateEnemy());
    }

    // Increases the enemy's stats according to what the player raised (passed in as a string)
    // Also accounts for the average number of turns to beat an enemy, latest player HP, and latest ally HP
    public void IncreaseEnemyStat(string nameOfStat)
    {
        int maxPlayer, maxAlly, playerQuater, playerHalf, playerThree, allyQuarter, allyHalf, allyThree;
        maxAlly = GameObject.FindGameObjectWithTag("Ally").GetComponent<PlayerAllyController>().GetMaxHP();
        maxPlayer = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>().GetMaxHP();
        playerQuater = Mathf.RoundToInt(maxPlayer * 0.25f);
        playerHalf = Mathf.RoundToInt(maxPlayer * 0.5f);
        playerThree = Mathf.RoundToInt(maxPlayer * 0.75f);
        allyQuarter = Mathf.RoundToInt(maxAlly * 0.25f);
        allyHalf = Mathf.RoundToInt(maxAlly * 0.5f);
        allyThree = Mathf.RoundToInt(maxAlly * 0.75f);

        if (averageNumberOfTurns <= 2)
        {
            enemyBaseHP += 10;
            enemyAttack++;
            enemyDefence++;
            enemySpeed++;
            if (latestPlayerHP > playerThree && latestAllyHP > allyThree)
            {
                enemyAttack++;
                enemySpeed++;
            }
            else if (latestPlayerHP > playerThree || latestAllyHP > allyThree)
            {
                enemyDefence++;
            }
            else if (latestPlayerHP < playerThree && latestAllyHP < allyThree)
            {
                enemyAttack--;
                enemyDefence--;
                enemySpeed--;
            }
        }
        else if (averageNumberOfTurns < 5 && averageNumberOfTurns > 2 && latestPlayerHP > playerQuater && latestAllyHP > allyQuarter)
        {
            if (nameOfStat == "Attack")
            {
                enemyBaseHP += 10;
            }
            else if (nameOfStat == "Defence")
            {
                enemyAttack++;
            }
            else if (nameOfStat == "HP")
            {
                enemySpeed++;
            }
            else if (nameOfStat == "Speed")
            {
                enemyDefence++;
            }
        }
        else if (averageNumberOfTurns > 8)
        {
            if (latestPlayerHP > playerThree && latestAllyHP > allyThree)
            {
                enemySpeed += 2;
            }
            else if ((latestPlayerHP < playerHalf && latestAllyHP < allyHalf) && (latestPlayerHP > playerQuater && latestAllyHP > allyQuarter))
            {
                enemyDefence--;
                enemyAttack--;
                if (enemyDefence <= 0)
                {
                    enemyDefence = 1;
                }
                if (enemyAttack <= 0)
                {
                    enemyAttack = 1;
                }
            }
            else if (latestPlayerHP < playerQuater && latestAllyHP < allyQuarter)
            {
                enemyDefence -= 2;
                enemyAttack--;
                enemySpeed--;
                if (enemyDefence <= 0)
                {
                    enemyDefence = 1;
                }
                if (enemyAttack <= 0)
                {
                    enemyAttack = 1;
                }
                if (enemySpeed <= 0)
                {
                    enemySpeed = 1;
                }
            }
        }
    }

    // This gets the player and ally HP values after an enemy dies
    public void SetPlayerAndAllyHPs()
    {
        latestPlayerHP = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>().GetHP();
        latestAllyHP = GameObject.FindGameObjectWithTag("Ally").GetComponent<PlayerAllyController>().GetHP();
    }

    // Sets the order in which the characters move in
    public void ChangeTurnOrder()
    {
        int playerSpeed = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>().GetSpeed();
        int allySpeed = GameObject.FindGameObjectWithTag("Ally").GetComponent<PlayerAllyController>().GetSpeed();

        if (playerSpeed >= allySpeed && playerSpeed >= enemySpeed)
        {
            turnOrder[0] = 1;
            if (allySpeed >= enemySpeed)
            {
                turnOrder[1] = 2;
                turnOrder[2] = 3;
            }
            else
            {
                turnOrder[1] = 3;
                turnOrder[2] = 2;
            }
        }
        else if (allySpeed >= playerSpeed && allySpeed >= enemySpeed)
        {
            turnOrder[0] = 2;
            if (playerSpeed >= enemySpeed)
            {
                turnOrder[1] = 1;
                turnOrder[2] = 3;
            }
            else
            {
                turnOrder[1] = 3;
                turnOrder[2] = 1;
            }
        }
        else if (enemySpeed >= playerSpeed && enemySpeed >= allySpeed)
        {
            turnOrder[0] = 3;
            if (playerSpeed >= allySpeed)
            {
                turnOrder[1] = 1;
                turnOrder[2] = 2;
            }
            else
            {
                turnOrder[1] = 2;
                turnOrder[2] = 1;
            }
        }
    }

    // Moves the turn number on, runs the move choosers for the ally and enemy, and loops the turns
    public void NextTurn()
    {
        turn++;
        if (turn > 3 || !GameObject.FindGameObjectWithTag("Enemy"))
        {
            turn = 1;
        }
        else if (turnOrder[turn - 1] == 2)
        {
            GameObject.FindGameObjectWithTag("Ally").GetComponent<PlayerAllyController>().ChooseMove();
        }
        else if (turnOrder[turn - 1] == 3 && GameObject.FindGameObjectWithTag("Enemy"))
        {
            GameObject.FindGameObjectWithTag("Enemy").GetComponent<EnemyController>().ChooseMove();
            turnsToDefeatEnemy++;
        }
    }

    // Returns the current turn number, mainly for use by the UIController
    public int GetTurnNum()
    {
        return turnOrder[turn - 1];
    }

    public void SwitchStatRaising()
    {
        statRaising = !statRaising;
    }

    // Spawns the correct colour player and destroys the previous one
    public void CreateNewPlayer(string colour)
    {
        GameObject playerTemp = GameObject.FindGameObjectWithTag("Player");
        playerTemp.GetComponent<PlayerController>().SetPlayerStats();
        if (colour == "Blue")
        {
            Instantiate(playerTypes[0], playerTemp.transform.position, playerTemp.transform.rotation);
            Destroy(playerTemp);
        }
        else if (colour == "Red")
        {
            Instantiate(playerTypes[2], playerTemp.transform.position, playerTemp.transform.rotation);
            Destroy(playerTemp);
        }
        else if (colour == "Yellow")
        {
            Instantiate(playerTypes[3], playerTemp.transform.position, playerTemp.transform.rotation);
            Destroy(playerTemp);
        }
        else if (colour == "Green")
        {
            Instantiate(playerTypes[1], playerTemp.transform.position, playerTemp.transform.rotation);
            Destroy(playerTemp);
        }
        ChangeTurnOrder();

        int randomAllyColourNum = Random.Range(1, 5);
        if (randomAllyColourNum == 1)
        {
            CreateNewAlly("Blue");
        }
        else if (randomAllyColourNum == 2)
        {
            CreateNewAlly("Green");
        }
        else if (randomAllyColourNum == 3)
        {
            CreateNewAlly("Red");
        }
        else if (randomAllyColourNum == 4)
        {
            CreateNewAlly("Yellow");
        }
    }

    // Spawns the correct colour ally and destroyes the previous one
    public void CreateNewAlly(string colour)
    {
        GameObject allyTemp = GameObject.FindGameObjectWithTag("Ally");
        allyTemp.GetComponent<PlayerAllyController>().SetAllyStats();
        if (colour == "Blue")
        {
            Instantiate(allyTypes[0], allyTemp.transform.position, allyTemp.transform.rotation);
            Destroy(allyTemp);
        }
        else if (colour == "Red")
        {
            Instantiate(allyTypes[2], allyTemp.transform.position, allyTemp.transform.rotation);
            Destroy(allyTemp);
        }
        else if (colour == "Yellow")
        {
            Instantiate(allyTypes[3], allyTemp.transform.position, allyTemp.transform.rotation);
            Destroy(allyTemp);
        }
        else if (colour == "Green")
        {
            Instantiate(allyTypes[1], allyTemp.transform.position, allyTemp.transform.rotation);
            Destroy(allyTemp);
        }
    }
}
