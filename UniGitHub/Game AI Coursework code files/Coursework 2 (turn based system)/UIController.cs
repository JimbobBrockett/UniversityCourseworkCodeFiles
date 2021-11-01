using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIController : MonoBehaviour
{

    public Button attackButton, superAttackButton, stealButton, itemButton, playerButton, allyButton, attackStatButton, defenceStatButton, hpStatButton, speedStatButton, blueButton, redButton, yellowButton, greenButton;
    public Text playerHPText, allyHPText, enemyHPText, currentMoveText, scoreText, destructProbText, defendProbText, playerProbText, allyProbText, healProbText;
    private bool isCharging = false, itemUse = false, moveChosen = false, statRaising = false, colourChoosing = false;
    private int score = 0;
    private float destructProb = 0, defendProb = 0, playerProb = 0, allyProb = 0, healProb = 0;
    private GameObject ally, player;

    // Start is called before the first frame update
    void Start()
    {
        ally = GameObject.FindGameObjectWithTag("Ally");
        player = GameObject.FindGameObjectWithTag("Player");

        // Set up the button functions.
        attackButton.onClick.AddListener(AttackEnemy);
        superAttackButton.onClick.AddListener(ChargeAttack);
        stealButton.onClick.AddListener(Steal);
        itemButton.onClick.AddListener(UseItem);
        playerButton.onClick.AddListener(UseItemOnPlayer);
        allyButton.onClick.AddListener(UseItemOnAlly);
        attackStatButton.onClick.AddListener(IncreaseAttackStat);
        defenceStatButton.onClick.AddListener(IncreaseDefenceStat);
        hpStatButton.onClick.AddListener(IncreaseHPStat);
        speedStatButton.onClick.AddListener(IncreaseSpeedStat);
        blueButton.onClick.AddListener(MakePlayerBlue);
        redButton.onClick.AddListener(MakePlayerRed);
        yellowButton.onClick.AddListener(MakePlayerYellow);
        greenButton.onClick.AddListener(MakePlayerGreen);
        playerButton.enabled = false;
        playerButton.image.enabled = false;
        playerButton.GetComponentInChildren<Text>().enabled = false;
        allyButton.enabled = false;
        allyButton.image.enabled = false;
        allyButton.GetComponentInChildren<Text>().enabled = false;
        attackStatButton.enabled = false;
        attackStatButton.image.enabled = false;
        attackStatButton.GetComponentInChildren<Text>().enabled = false;
        defenceStatButton.enabled = false;
        defenceStatButton.image.enabled = false;
        defenceStatButton.GetComponentInChildren<Text>().enabled = false;
        hpStatButton.enabled = false;
        hpStatButton.image.enabled = false;
        hpStatButton.GetComponentInChildren<Text>().enabled = false;
        speedStatButton.enabled = false;
        speedStatButton.image.enabled = false;
        speedStatButton.GetComponentInChildren<Text>().enabled = false;
        blueButton.enabled = false;
        blueButton.image.enabled = false;
        blueButton.GetComponentInChildren<Text>().enabled = false;
        redButton.enabled = false;
        redButton.image.enabled = false;
        redButton.GetComponentInChildren<Text>().enabled = false;
        yellowButton.enabled = false;
        yellowButton.image.enabled = false;
        yellowButton.GetComponentInChildren<Text>().enabled = false;
        greenButton.enabled = false;
        greenButton.image.enabled = false;
        greenButton.GetComponentInChildren<Text>().enabled = false;
        currentMoveText.text = "";
    }

    // Update is called once per frame
    void Update()
    { 
        if (player == null)
        {
            ReFindPlayer();
        }

        if (ally == null)
        {
            ReFindAlly();
        }

        // HP text updates
        if (GameObject.FindGameObjectWithTag("Player"))
        {
            playerHPText.text = "Player HP: " + player.GetComponent<PlayerController>().GetHP();
        }
        else
        {
            playerHPText.text = "Player HP: ";
        }

        if (GameObject.FindGameObjectWithTag("Ally"))
        {
            allyHPText.text = "Ally HP: " + ally.GetComponent<PlayerAllyController>().GetHP();
        }
        else
        {
            allyHPText.text = "Ally HP: ";
        }

        if (GameObject.FindGameObjectWithTag("Enemy"))
        {
            enemyHPText.text = "Enemy HP: " + GameObject.FindGameObjectWithTag("Enemy").GetComponent<EnemyController>().GetHP();
        }
        else
        {
            enemyHPText.text = "Enemy HP: ";
        }

        if (player.GetComponent<PlayerController>().GetHP() <= 0 && ally.GetComponent<PlayerAllyController>().GetHP() <= 0)
        {
            Debug.Log("You all died");
            Application.Quit();
        }

        scoreText.text = "Score: " + score;

        // Enemy choice probability updates
        destructProbText.text = "Self-Destruct Probability: " + destructProb;
        defendProbText.text = "Defend Probability: " + defendProb;
        playerProbText.text = "Player Attack Probability: " + playerProb;
        allyProbText.text = "Ally Attack Probability: " + allyProb;
        healProbText.text = "Heal Probability: " + healProb;

        // Button updates, sets them to active when it's the players go. Also checks if the player is
        // charging a super attack, and performs that instead.
        if (gameObject.GetComponent<BattleController>().GetTurnNum() == 1 && !player.GetComponent<PlayerController>().dead && !moveChosen && !statRaising && !colourChoosing)
        {
            if (!GameObject.FindGameObjectWithTag("Enemy") && !isCharging && !itemUse)
            {
                attackButton.enabled = false;
                attackButton.image.enabled = false;
                attackButton.GetComponentInChildren<Text>().enabled = false;
                superAttackButton.enabled = false;
                superAttackButton.image.enabled = false;
                superAttackButton.GetComponentInChildren<Text>().enabled = false;
                stealButton.enabled = false;
                stealButton.image.enabled = false;
                stealButton.GetComponentInChildren<Text>().enabled = false;
                itemButton.enabled = false;
                itemButton.image.enabled = false;
                itemButton.GetComponentInChildren<Text>().enabled = false;
                playerButton.enabled = false;
                playerButton.image.enabled = false;
                playerButton.GetComponentInChildren<Text>().enabled = false;
                allyButton.enabled = false;
                allyButton.image.enabled = false;
                allyButton.GetComponentInChildren<Text>().enabled = false;
                attackStatButton.enabled = false;
                attackStatButton.image.enabled = false;
                attackStatButton.GetComponentInChildren<Text>().enabled = false;
                defenceStatButton.enabled = false;
                defenceStatButton.image.enabled = false;
                defenceStatButton.GetComponentInChildren<Text>().enabled = false;
                hpStatButton.enabled = false;
                hpStatButton.image.enabled = false;
                hpStatButton.GetComponentInChildren<Text>().enabled = false;
                speedStatButton.enabled = false;
                speedStatButton.image.enabled = false;
                speedStatButton.GetComponentInChildren<Text>().enabled = false;
                blueButton.enabled = false;
                blueButton.image.enabled = false;
                blueButton.GetComponentInChildren<Text>().enabled = false;
                redButton.enabled = false;
                redButton.image.enabled = false;
                redButton.GetComponentInChildren<Text>().enabled = false;
                yellowButton.enabled = false;
                yellowButton.image.enabled = false;
                yellowButton.GetComponentInChildren<Text>().enabled = false;
                greenButton.enabled = false;
                greenButton.image.enabled = false;
                greenButton.GetComponentInChildren<Text>().enabled = false;
            }
            else if (isCharging && GameObject.FindGameObjectWithTag("Enemy"))
            {
                currentMoveText.text = "Super Attack";
                player.GetComponent<PlayerController>().SuperAttackEnemy();
                isCharging = false;
                moveChosen = true;
                ally.GetComponent<PlayerAllyController>().boolSet.playerIsCharging = false;
                GoToNextTurn();
            }
            else if (isCharging && !GameObject.FindGameObjectWithTag("Enemy"))
            {
                attackButton.enabled = false;
                attackButton.image.enabled = false;
                attackButton.GetComponentInChildren<Text>().enabled = false;
                superAttackButton.enabled = false;
                superAttackButton.image.enabled = false;
                superAttackButton.GetComponentInChildren<Text>().enabled = false;
                stealButton.enabled = false;
                stealButton.image.enabled = false;
                stealButton.GetComponentInChildren<Text>().enabled = false;
                itemButton.enabled = false;
                itemButton.image.enabled = false;
                itemButton.GetComponentInChildren<Text>().enabled = false;
                playerButton.enabled = false;
                playerButton.image.enabled = false;
                playerButton.GetComponentInChildren<Text>().enabled = false;
                allyButton.enabled = false;
                allyButton.image.enabled = false;
                allyButton.GetComponentInChildren<Text>().enabled = false;
                attackStatButton.enabled = false;
                attackStatButton.image.enabled = false;
                attackStatButton.GetComponentInChildren<Text>().enabled = false;
                defenceStatButton.enabled = false;
                defenceStatButton.image.enabled = false;
                defenceStatButton.GetComponentInChildren<Text>().enabled = false;
                hpStatButton.enabled = false;
                hpStatButton.image.enabled = false;
                hpStatButton.GetComponentInChildren<Text>().enabled = false;
                speedStatButton.enabled = false;
                speedStatButton.image.enabled = false;
                speedStatButton.GetComponentInChildren<Text>().enabled = false;
                blueButton.enabled = false;
                blueButton.image.enabled = false;
                blueButton.GetComponentInChildren<Text>().enabled = false;
                redButton.enabled = false;
                redButton.image.enabled = false;
                redButton.GetComponentInChildren<Text>().enabled = false;
                yellowButton.enabled = false;
                yellowButton.image.enabled = false;
                yellowButton.GetComponentInChildren<Text>().enabled = false;
                greenButton.enabled = false;
                greenButton.image.enabled = false;
                greenButton.GetComponentInChildren<Text>().enabled = false;
            }
            else if (itemUse)
            {
                attackButton.enabled = false;
                attackButton.image.enabled = false;
                attackButton.GetComponentInChildren<Text>().enabled = false;
                superAttackButton.enabled = false;
                superAttackButton.image.enabled = false;
                superAttackButton.GetComponentInChildren<Text>().enabled = false;
                stealButton.enabled = false;
                stealButton.image.enabled = false;
                stealButton.GetComponentInChildren<Text>().enabled = false;
                itemButton.enabled = false;
                itemButton.image.enabled = false;
                itemButton.GetComponentInChildren<Text>().enabled = false;
                playerButton.enabled = true;
                playerButton.image.enabled = true;
                playerButton.GetComponentInChildren<Text>().enabled = true;
                allyButton.enabled = true;
                allyButton.image.enabled = true;
                allyButton.GetComponentInChildren<Text>().enabled = true;
                attackStatButton.enabled = false;
                attackStatButton.image.enabled = false;
                attackStatButton.GetComponentInChildren<Text>().enabled = false;
                defenceStatButton.enabled = false;
                defenceStatButton.image.enabled = false;
                defenceStatButton.GetComponentInChildren<Text>().enabled = false;
                hpStatButton.enabled = false;
                hpStatButton.image.enabled = false;
                hpStatButton.GetComponentInChildren<Text>().enabled = false;
                speedStatButton.enabled = false;
                speedStatButton.image.enabled = false;
                speedStatButton.GetComponentInChildren<Text>().enabled = false;
                blueButton.enabled = false;
                blueButton.image.enabled = false;
                blueButton.GetComponentInChildren<Text>().enabled = false;
                redButton.enabled = false;
                redButton.image.enabled = false;
                redButton.GetComponentInChildren<Text>().enabled = false;
                yellowButton.enabled = false;
                yellowButton.image.enabled = false;
                yellowButton.GetComponentInChildren<Text>().enabled = false;
                greenButton.enabled = false;
                greenButton.image.enabled = false;
                greenButton.GetComponentInChildren<Text>().enabled = false;
            }
            else
            {
                attackButton.enabled = true;
                attackButton.image.enabled = true;
                attackButton.GetComponentInChildren<Text>().enabled = true;
                superAttackButton.enabled = true;
                superAttackButton.image.enabled = true;
                superAttackButton.GetComponentInChildren<Text>().enabled = true;
                stealButton.enabled = true;
                stealButton.image.enabled = true;
                stealButton.GetComponentInChildren<Text>().enabled = true;
                itemButton.enabled = true;
                itemButton.image.enabled = true;
                itemButton.GetComponentInChildren<Text>().enabled = true;
                playerButton.enabled = false;
                playerButton.image.enabled = false;
                playerButton.GetComponentInChildren<Text>().enabled = false;
                allyButton.enabled = false;
                allyButton.image.enabled = false;
                allyButton.GetComponentInChildren<Text>().enabled = false;
                attackStatButton.enabled = false;
                attackStatButton.image.enabled = false;
                attackStatButton.GetComponentInChildren<Text>().enabled = false;
                defenceStatButton.enabled = false;
                defenceStatButton.image.enabled = false;
                defenceStatButton.GetComponentInChildren<Text>().enabled = false;
                hpStatButton.enabled = false;
                hpStatButton.image.enabled = false;
                hpStatButton.GetComponentInChildren<Text>().enabled = false;
                speedStatButton.enabled = false;
                speedStatButton.image.enabled = false;
                speedStatButton.GetComponentInChildren<Text>().enabled = false;
                blueButton.enabled = false;
                blueButton.image.enabled = false;
                blueButton.GetComponentInChildren<Text>().enabled = false;
                redButton.enabled = false;
                redButton.image.enabled = false;
                redButton.GetComponentInChildren<Text>().enabled = false;
                yellowButton.enabled = false;
                yellowButton.image.enabled = false;
                yellowButton.GetComponentInChildren<Text>().enabled = false;
                greenButton.enabled = false;
                greenButton.image.enabled = false;
                greenButton.GetComponentInChildren<Text>().enabled = false;
            }
        }
        else if (statRaising)
        {
            attackButton.enabled = false;
            attackButton.image.enabled = false;
            attackButton.GetComponentInChildren<Text>().enabled = false;
            superAttackButton.enabled = false;
            superAttackButton.image.enabled = false;
            superAttackButton.GetComponentInChildren<Text>().enabled = false;
            stealButton.enabled = false;
            stealButton.image.enabled = false;
            stealButton.GetComponentInChildren<Text>().enabled = false;
            itemButton.enabled = false;
            itemButton.image.enabled = false;
            itemButton.GetComponentInChildren<Text>().enabled = false;
            playerButton.enabled = false;
            playerButton.image.enabled = false;
            playerButton.GetComponentInChildren<Text>().enabled = false;
            allyButton.enabled = false;
            allyButton.image.enabled = false;
            allyButton.GetComponentInChildren<Text>().enabled = false;
            attackStatButton.enabled = true;
            attackStatButton.image.enabled = true;
            attackStatButton.GetComponentInChildren<Text>().enabled = true;
            defenceStatButton.enabled = true;
            defenceStatButton.image.enabled = true;
            defenceStatButton.GetComponentInChildren<Text>().enabled = true;
            hpStatButton.enabled = true;
            hpStatButton.image.enabled = true;
            hpStatButton.GetComponentInChildren<Text>().enabled = true;
            speedStatButton.enabled = true;
            speedStatButton.image.enabled = true;
            speedStatButton.GetComponentInChildren<Text>().enabled = true;
            blueButton.enabled = false;
            blueButton.image.enabled = false;
            blueButton.GetComponentInChildren<Text>().enabled = false;
            redButton.enabled = false;
            redButton.image.enabled = false;
            redButton.GetComponentInChildren<Text>().enabled = false;
            yellowButton.enabled = false;
            yellowButton.image.enabled = false;
            yellowButton.GetComponentInChildren<Text>().enabled = false;
            greenButton.enabled = false;
            greenButton.image.enabled = false;
            greenButton.GetComponentInChildren<Text>().enabled = false;
        }
        else if (colourChoosing)
        {
            attackButton.enabled = false;
            attackButton.image.enabled = false;
            attackButton.GetComponentInChildren<Text>().enabled = false;
            superAttackButton.enabled = false;
            superAttackButton.image.enabled = false;
            superAttackButton.GetComponentInChildren<Text>().enabled = false;
            stealButton.enabled = false;
            stealButton.image.enabled = false;
            stealButton.GetComponentInChildren<Text>().enabled = false;
            itemButton.enabled = false;
            itemButton.image.enabled = false;
            itemButton.GetComponentInChildren<Text>().enabled = false;
            playerButton.enabled = false;
            playerButton.image.enabled = false;
            playerButton.GetComponentInChildren<Text>().enabled = false;
            allyButton.enabled = false;
            allyButton.image.enabled = false;
            allyButton.GetComponentInChildren<Text>().enabled = false;
            attackStatButton.enabled = false;
            attackStatButton.image.enabled = false;
            attackStatButton.GetComponentInChildren<Text>().enabled = false;
            defenceStatButton.enabled = false;
            defenceStatButton.image.enabled = false;
            defenceStatButton.GetComponentInChildren<Text>().enabled = false;
            hpStatButton.enabled = false;
            hpStatButton.image.enabled = false;
            hpStatButton.GetComponentInChildren<Text>().enabled = false;
            speedStatButton.enabled = false;
            speedStatButton.image.enabled = false;
            speedStatButton.GetComponentInChildren<Text>().enabled = false;
            blueButton.enabled = true;
            blueButton.image.enabled = true;
            blueButton.GetComponentInChildren<Text>().enabled = true;
            redButton.enabled = true;
            redButton.image.enabled = true;
            redButton.GetComponentInChildren<Text>().enabled = true;
            yellowButton.enabled = true;
            yellowButton.image.enabled = true;
            yellowButton.GetComponentInChildren<Text>().enabled = true;
            greenButton.enabled = true;
            greenButton.image.enabled = true;
            greenButton.GetComponentInChildren<Text>().enabled = true;
        }
        else if (player.GetComponent<PlayerController>().dead)
        {
            gameObject.GetComponent<BattleController>().NextTurn();
        }
        else
        {
            attackButton.enabled = false;
            attackButton.image.enabled = false;
            attackButton.GetComponentInChildren<Text>().enabled = false;
            superAttackButton.enabled = false;
            superAttackButton.image.enabled = false;
            superAttackButton.GetComponentInChildren<Text>().enabled = false;
            stealButton.enabled = false;
            stealButton.image.enabled = false;
            stealButton.GetComponentInChildren<Text>().enabled = false;
            itemButton.enabled = false;
            itemButton.image.enabled = false;
            itemButton.GetComponentInChildren<Text>().enabled = false;
            playerButton.enabled = false;
            playerButton.image.enabled = false;
            playerButton.GetComponentInChildren<Text>().enabled = false;
            allyButton.enabled = false;
            allyButton.image.enabled = false;
            allyButton.GetComponentInChildren<Text>().enabled = false;
            attackStatButton.enabled = false;
            attackStatButton.image.enabled = false;
            attackStatButton.GetComponentInChildren<Text>().enabled = false;
            defenceStatButton.enabled = false;
            defenceStatButton.image.enabled = false;
            defenceStatButton.GetComponentInChildren<Text>().enabled = false;
            hpStatButton.enabled = false;
            hpStatButton.image.enabled = false;
            hpStatButton.GetComponentInChildren<Text>().enabled = false;
            speedStatButton.enabled = false;
            speedStatButton.image.enabled = false;
            speedStatButton.GetComponentInChildren<Text>().enabled = false;
            blueButton.enabled = false;
            blueButton.image.enabled = false;
            blueButton.GetComponentInChildren<Text>().enabled = false;
            redButton.enabled = false;
            redButton.image.enabled = false;
            redButton.GetComponentInChildren<Text>().enabled = false;
            yellowButton.enabled = false;
            yellowButton.image.enabled = false;
            yellowButton.GetComponentInChildren<Text>().enabled = false;
            greenButton.enabled = false;
            greenButton.image.enabled = false;
            greenButton.GetComponentInChildren<Text>().enabled = false;
        }
    }

    // Finds the player again after a new one spawns
    public void ReFindPlayer()
    {
        player = GameObject.FindGameObjectWithTag("Player");
    }

    // Finds the ally again after a new one spawns
    public void ReFindAlly()
    {
        ally = GameObject.FindGameObjectWithTag("Ally");
    }

    // Runs the players attack enemy procedure and then moves the turn to the ally
    private void AttackEnemy()
    {
        currentMoveText.text = "Attack";
        moveChosen = true;
        player.GetComponent<PlayerController>().AttackEnemy();
        GoToNextTurn();
    }

    // Runs the players super attack procedure and then moves the turn to the ally
    private void ChargeAttack()
    {
        currentMoveText.text = "Charging";
        isCharging = true;
        moveChosen = true;
        ally.GetComponent<PlayerAllyController>().boolSet.playerIsCharging = true;
        GoToNextTurn();
    }

    // Runs the players steal procedure and then moves the turn to the ally
    private void Steal()
    {
        currentMoveText.text = "Steal";
        moveChosen = true;
        player.GetComponent<PlayerController>().StealFromEnemy();
        GoToNextTurn();
    }

    // Runs the players use item procedure and then moves the turn to the ally
    private void UseItem()
    {
        if (gameObject.GetComponent<BattleController>().item != "")
        {
            itemUse = true;
        }
        else
        {
            Debug.Log("No item here!");
        }
    }

    // Runs the players self-use item procedure then moves the turn to the ally
    private void UseItemOnPlayer()
    {
        currentMoveText.text = "Item";
        player.GetComponent<PlayerController>().UseItemOnSelf();
        gameObject.GetComponent<BattleController>().item = "";
        itemUse = false;
        moveChosen = true;
        ally.GetComponent<PlayerAllyController>().boolSet.itemIsInBag = false;
        GoToNextTurn();
    }

    // Runs the players ally-use item procedure then moves the turn to the ally
    private void UseItemOnAlly()
    {
        currentMoveText.text = "Item";
        player.GetComponent<PlayerController>().UseItemOnAlly();
        gameObject.GetComponent<BattleController>().item = "";
        itemUse = false;
        moveChosen = true;
        GoToNextTurn();
    }

    // The turn goes to the ally, as long as the player did not just kill the enemy.
    private void GoToNextTurn()
    {
        if (GameObject.FindGameObjectWithTag("Enemy").GetComponent<EnemyController>().GetHP() > 0)
        {
            StartCoroutine(NextTurnWaiter());
        }
    }

    // Increases the score for the player
    public void IncreaseScore()
    {
        score++;
    }

    // Returns the score value
    public int GetScore()
    {
        return score;
    }

    // Returns if the player is charging an attack or not, mainly for use by the Player Controller
    public bool GetIsCharging()
    {
        return isCharging;
    }

    // Sets the stat raising bool to be true so the correct UI appears
    public void StartStatRaising()
    {
        statRaising = true;
        isCharging = false;
        ally.GetComponent<PlayerAllyController>().ResetCharge();
        gameObject.GetComponent<BattleController>().SwitchStatRaising();
    }

    // Calls the players attack increase procedure
    private void IncreaseAttackStat()
    {
        player.GetComponent<PlayerController>().IncreaseAttackStat();
        statRaising = false;
        ally.GetComponent<PlayerAllyController>().IncreaseStat();
        gameObject.GetComponent<BattleController>().IncreaseEnemyStat("Attack");
        colourChoosing = true;
    }

    // Calls the players defence increase procedure
    private void IncreaseDefenceStat()
    {
        player.GetComponent<PlayerController>().IncreaseDefenceStat();
        statRaising = false;
        ally.GetComponent<PlayerAllyController>().IncreaseStat();
        gameObject.GetComponent<BattleController>().IncreaseEnemyStat("Defence");
        colourChoosing = true;
    }

    // Calls the players HP increase procedure 
    private void IncreaseHPStat()
    {
        player.GetComponent<PlayerController>().IncreaseHPStat();
        statRaising = false;
        ally.GetComponent<PlayerAllyController>().IncreaseStat();
        gameObject.GetComponent<BattleController>().IncreaseEnemyStat("HP");
        colourChoosing = true;
    }

    private void IncreaseSpeedStat()
    {
        player.GetComponent<PlayerController>().IncreaseSpeedStat();
        statRaising = false;
        ally.GetComponent<PlayerAllyController>().IncreaseStat();
        gameObject.GetComponent<BattleController>().IncreaseEnemyStat("Speed");
        colourChoosing = true;
    }

    // Spawns a blue version of the player 
    private void MakePlayerBlue()
    {
        colourChoosing = false;
        gameObject.GetComponent<BattleController>().CreateNewPlayer("Blue");
        gameObject.GetComponent<BattleController>().SwitchStatRaising();
        gameObject.GetComponent<BattleController>().NewEnemyNeeded();
    }

    // Spawns a red version of the player 
    private void MakePlayerRed()
    {
        colourChoosing = false;
        gameObject.GetComponent<BattleController>().CreateNewPlayer("Red");
        gameObject.GetComponent<BattleController>().SwitchStatRaising();
        gameObject.GetComponent<BattleController>().NewEnemyNeeded();
    }

    // Spawns a yellow version of the player 
    private void MakePlayerYellow()
    {
        colourChoosing = false;
        gameObject.GetComponent<BattleController>().CreateNewPlayer("Yellow");
        gameObject.GetComponent<BattleController>().SwitchStatRaising();
        gameObject.GetComponent<BattleController>().NewEnemyNeeded();
    }

    // Spawns a green version of the player 
    private void MakePlayerGreen()
    {
        colourChoosing = false;
        gameObject.GetComponent<BattleController>().CreateNewPlayer("Green");
        gameObject.GetComponent<BattleController>().SwitchStatRaising();
        gameObject.GetComponent<BattleController>().NewEnemyNeeded();
    }

    // Resets the main bools for choosing which UI buttons appear
    public void ResetUIBools()
    {
        isCharging = false;
        moveChosen = false;
        statRaising = false;
        colourChoosing = false;
    }

    // Sets the enemy move probabilities shown on-screen
    public void SetProbabilities(float destruct, float defend, float playerP, float allyP, float heal)
    {
        destructProb = destruct;
        defendProb = defend;
        playerProb = playerP;
        allyProb = allyP;
        healProb = heal;
    }

    // Waits 3 seconds, then resets the move choice text, resets the move chosen bool, and goes to the next turn
    IEnumerator NextTurnWaiter()
    {
        yield return new WaitForSeconds(3);
        currentMoveText.text = "";
        moveChosen = false;
        gameObject.GetComponent<BattleController>().NextTurn();
        StopCoroutine(NextTurnWaiter());
    }

    // A separate enumerator for when the enemy self destructs 
    public IEnumerator EnemySelfDestructed()
    {
        yield return new WaitForSeconds(3);
        currentMoveText.text = "";
        StopCoroutine(EnemySelfDestructed());
    }
}
