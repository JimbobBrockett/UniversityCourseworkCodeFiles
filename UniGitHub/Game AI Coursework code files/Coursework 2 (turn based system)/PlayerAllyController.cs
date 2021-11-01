using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// Contains the Booleans for the ChooseMove Behaviour Tree
public struct MoveChoiceBools
{
    public bool itemIsInBag, enemyHasItem, enemyHPAboveHalf, playerHPBelowThirtyPercent, HPBelowThirtyPercent, playerIsCharging;
}

public class PlayerAllyController : MonoBehaviour
{

    private int MaxHP = 20, attack = 1, defence = 1, speed = 1, currentHP;
    private bool isCharging = false, dead = false;
    private GameObject controller, player;
    public MoveChoiceBools boolSet;

    // Start is called before the first frame update
    void Start()
    {
        controller = GameObject.FindGameObjectWithTag("GameController");
        player = GameObject.FindGameObjectWithTag("Player");

        if (GameObject.FindGameObjectsWithTag("Ally").Length > 1)
        {
            PlayerAndAllyStats statsGrab = controller.GetComponent<PlayerAndAllyStats>();
            MaxHP = statsGrab.GetAllyHP();
            attack = statsGrab.GetAllyAttack();
            defence = statsGrab.GetAllyDefence();
            speed = statsGrab.GetAllySpeed();
            boolSet.itemIsInBag = statsGrab.GetAllyItemInBag();
            boolSet.enemyHasItem = statsGrab.GetAllyEnemyHasItem();
            boolSet.enemyHPAboveHalf = statsGrab.GetAllyEnemyHP();
            boolSet.playerHPBelowThirtyPercent = statsGrab.GetAllyPlayerHPState();
            boolSet.HPBelowThirtyPercent = statsGrab.GetAllyHPState();
            boolSet.playerIsCharging = statsGrab.GetAllyPlayerCharging();
        }
        else
        {
            boolSet.itemIsInBag = false;
            boolSet.enemyHasItem = false;
            boolSet.enemyHPAboveHalf = false;
            boolSet.playerHPBelowThirtyPercent = false;
            boolSet.HPBelowThirtyPercent = false;
            boolSet.playerIsCharging = false;
        }

        controller.GetComponent<UIController>().ReFindAlly();
        player.GetComponent<PlayerController>().ReFindAlly();

        currentHP = MaxHP;
    }

    // Finds the player again after a new ally spawns
    public void ReFindPlayer()
    {
        player = GameObject.FindGameObjectWithTag("Player");
    }

    // Chooses the ally move using a strict Behaviour Tree.
    public void ChooseMove()
    {
        if (!dead)
        {
            GameObject enemy = GameObject.FindGameObjectWithTag("Enemy");
            if (!isCharging)
            {
                if (!boolSet.itemIsInBag)
                {
                    if (boolSet.enemyHasItem)
                    {
                        controller.GetComponent<UIController>().currentMoveText.text = "Steal";
                        enemy.GetComponent<EnemyController>().ItemStolen();
                    }
                    else
                    {
                        AttackEnemy(enemy);
                    }
                }
                else
                {
                    if (boolSet.playerHPBelowThirtyPercent)
                    {
                        if (boolSet.itemIsInBag)
                        {
                            controller.GetComponent<UIController>().currentMoveText.text = "Item";
                            UseItemOnPlayer();
                            controller.GetComponent<BattleController>().item = "";
                        }
                        else
                        {
                            AttackEnemy(enemy);
                        }
                    }
                    else
                    {
                        if (boolSet.HPBelowThirtyPercent)
                        {
                            if (boolSet.itemIsInBag)
                            {
                                controller.GetComponent<UIController>().currentMoveText.text = "Item";
                                UseItemOnSelf();
                                controller.GetComponent<BattleController>().item = "";
                            }
                            else
                            {
                                AttackEnemy(enemy);
                            }
                        }
                        else
                        {
                            if (boolSet.playerIsCharging)
                            {
                                controller.GetComponent<UIController>().currentMoveText.text = "Charging";
                                isCharging = true;
                            }
                            else
                            {
                                AttackEnemy(enemy);
                            }
                        }
                    }
                }
            }
            else
            {
                controller.GetComponent<UIController>().currentMoveText.text = "Super Attack";
                int damage = Random.Range(6, 10) * attack;
                damage = CalculateDamage(enemy, damage);
                enemy.GetComponent<EnemyController>().DamageEnemy(damage);
                isCharging = false;
            }
            if (enemy.GetComponent<EnemyController>().GetHP() > 0)
            {
                StartCoroutine(NextTurnWaiter());
            }
        }
        else
        {
            controller.GetComponent<BattleController>().NextTurn();
        }
    }

    // Chooses the type of attack, abstracted from ChooseMove() for code optimisation
    private void AttackEnemy(GameObject enemy)
    {
        if (boolSet.enemyHPAboveHalf)
        {
            controller.GetComponent<UIController>().currentMoveText.text = "Charging";
            isCharging = true;
        }
        else
        {
            controller.GetComponent<UIController>().currentMoveText.text = "Attack";
            int damage = Random.Range(1, 4) * attack;
            damage = CalculateDamage(enemy, damage);
            enemy.GetComponent<EnemyController>().DamageEnemy(damage);
        }
    }

    // Calculates initial damage, checks for type weaknesses and doubles the damage if necessary, then returns it
    private int CalculateDamage(GameObject enemy, int damage)
    {
        if ((gameObject.name == "AllyBlue(Clone)" && enemy.name == "RedCharacter(Clone)") ||
            (gameObject.name == "AllyRed(Clone)" && enemy.name == "GreenCharacter(Clone)") ||
            (gameObject.name == "AllyGreen(Clone)" && enemy.name == "YellowCharacter(Clone)") ||
            (gameObject.name == "AllyYellow(Clone)" && enemy.name == "BlueCharacter(Clone)"))
        {
            damage *= 2;
        }

        return damage;
    }

    // Uses the current held item on themself, assuming there is one to be used
    public void UseItemOnSelf()
    {
        currentHP += 5;
        if (currentHP > (MaxHP / 10) * 3)
        {
            boolSet.HPBelowThirtyPercent = false;
        }
        if (currentHP > MaxHP)
        {
            currentHP = MaxHP;
        }
        boolSet.itemIsInBag = false;
        controller.GetComponent<BattleController>().item = "";
        dead = false;
    }

    // Uses the current held item on the player, assuming there is one to be used
    public void UseItemOnPlayer()
    {
        player.GetComponent<PlayerController>().UseItemOnSelf();
        controller.GetComponent<BattleController>().item = "";
        boolSet.itemIsInBag = false;
    }

    // Increases a random stat for the ally
    public void IncreaseStat()
    {
        int randomStatNum = Random.Range(1, 5);
        if (randomStatNum == 1)
        {
            attack++;
        }
        else if (randomStatNum == 2)
        {
            defence++;
        }
        else if (randomStatNum == 3)
        {
            MaxHP += 10;
        }
        else if (randomStatNum == 4)
        {
            speed++;
        }
        currentHP = MaxHP;
        dead = false;
        isCharging = false;
    }

    // Reduces the ally's HP by the amount passed in subtract their defence, is capped at a minimum of 1
    public void DamageAlly(int damage)
    {
        damage -= defence;
        if (damage < 1)
        {
            damage = 1;
        }
        currentHP -= damage;
        if (currentHP <= 0)
        {
            Debug.Log("Ally died!");
            dead = true;
        }
        else if (currentHP <= (MaxHP / 10) * 3)
        {
            boolSet.HPBelowThirtyPercent = true;
        }
        if (currentHP < 0)
        {
            currentHP = 0;
        }
    }

    // Resets the isCharging bool after an enemy is defeated
    public void ResetCharge()
    {
        isCharging = false;
    }

    // Returns the HP, mainly for use by the UIController.
    public int GetHP()
    {
        return currentHP;
    }

    // Returns the allys maximum HP, mainly for use by the Enemy Controller
    public int GetMaxHP()
    {
        return MaxHP;
    }

    // Returns the attack, mainly for use by the stat transfer script
    public int GetAttack()
    {
        return attack;
    }

    // Returns the defence, mainly for use by the stat transfer script
    public int GetDefence()
    {
        return defence;
    }

    // Returns the speed, mainly for use by the battle controller
    public int GetSpeed()
    {
        return speed;
    }

    // Returns if the ally is charging an attack or not, mainly for use by the Enemy Controller
    public bool GetIsCharging()
    {
        return isCharging;
    }

    // Sets the ally stats in the stat transfer script
    public void SetAllyStats()
    {
        controller.GetComponent<PlayerAndAllyStats>().SetAllyStats(MaxHP, attack, defence, speed, boolSet.itemIsInBag, boolSet.enemyHasItem, boolSet.enemyHPAboveHalf, boolSet.playerHPBelowThirtyPercent, boolSet.HPBelowThirtyPercent, boolSet.playerIsCharging);
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
