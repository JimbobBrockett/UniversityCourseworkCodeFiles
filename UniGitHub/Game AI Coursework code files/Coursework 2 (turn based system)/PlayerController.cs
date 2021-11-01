using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{

    private int MaxHP = 20, attack = 1, defence = 1, speed = 1, currentHP;
    private GameObject ally;
    public bool dead = false;

    // Start is called before the first frame update
    void Start()
    {
        ally = GameObject.FindGameObjectWithTag("Ally");

        if (GameObject.FindGameObjectsWithTag("Player").Length > 1)
        {
            PlayerAndAllyStats statsGrab = GameObject.FindGameObjectWithTag("GameController").GetComponent<PlayerAndAllyStats>();
            MaxHP = statsGrab.GetPlayerHP();
            attack = statsGrab.GetPlayerAttack();
            defence = statsGrab.GetPlayerDefence();
            speed = statsGrab.GetPlayerSpeed();
        }

        ally.GetComponent<PlayerAllyController>().ReFindPlayer();
        GameObject.FindGameObjectWithTag("GameController").GetComponent<UIController>().ReFindPlayer();

        currentHP = MaxHP;
    }

    // Finds the ally after a new player is spawned
    public void ReFindAlly()
    {
        ally = GameObject.FindGameObjectWithTag("Ally");
    }

    // Deals damage to the enemy based on attack and defence stats
    public void AttackEnemy()
    {
        GameObject enemy = GameObject.FindGameObjectWithTag("Enemy");
        int damage = Random.Range(1, 4) * attack;
        damage = CalculateDamage(enemy, damage);
        enemy.GetComponent<EnemyController>().DamageEnemy(damage);
    }

    // Deals charged damage to the enemy that ignores defence stats
    public void SuperAttackEnemy()
    {
        GameObject enemy = GameObject.FindGameObjectWithTag("Enemy");
        int damage = Random.Range(6, 10) * attack;
        damage = CalculateDamage(enemy, damage);
        enemy.GetComponent<EnemyController>().DamageEnemy(Random.Range(6, 10) * attack);
    }

    // Calculates initial damage, checks for type weaknesses and doubles the damage if necessary, then returns it
    private int CalculateDamage(GameObject enemy, int damage)
    {
        if ((gameObject.name == "GoodBlue(Clone)" && enemy.name == "RedCharacter(Clone)") ||
            (gameObject.name == "GoodRed(Clone)" && enemy.name == "GreenCharacter(Clone)") ||
            (gameObject.name == "GoodGreen(Clone)" && enemy.name == "YellowCharacter(Clone)") ||
            (gameObject.name == "GoodYellow(Clone)" && enemy.name == "BlueCharacter(Clone)"))
        {
            damage *= 2;
            Debug.Log("Damage doubled");
        }

        return damage;
    }

    // Steals the item the enemy is holding, assuming they are holding one
    public void StealFromEnemy()
    {
        GameObject.FindGameObjectWithTag("Enemy").GetComponent<EnemyController>().ItemStolen();
    }

    // Uses the current held item on themself, assuming there is one to be used
    public void UseItemOnSelf()
    {
        currentHP += 5;
        if (currentHP > (MaxHP / 10) * 3)
        {
            ally.GetComponent<PlayerAllyController>().boolSet.playerHPBelowThirtyPercent = false;
        }
        if (currentHP > MaxHP)
        {
            currentHP = MaxHP;
        }
        dead = false;
    }

    // Uses the current held item on the ally, assuming there is one to be used
    public void UseItemOnAlly()
    {
        ally.GetComponent<PlayerAllyController>().UseItemOnSelf();
    }

    // Reduces the player by the value passed in subtract the players defence, as capped at a minimum of 1
    public void DamagePlayer(int damage)
    {
        damage -= defence;
        if (damage < 1)
        {
            damage = 1;
        }
        currentHP -= damage;
        if (currentHP <= 0)
        {
            Debug.Log("You died");
            dead = true;
        }
        else if (currentHP <= (MaxHP / 10) * 3)
        {
            ally.GetComponent<PlayerAllyController>().boolSet.playerHPBelowThirtyPercent = true;
        }
        if (currentHP < 0)
        {
            currentHP = 0;
        }
    }

    // Increases the players attack by 1
    public void IncreaseAttackStat()
    {
        attack++;
        currentHP = MaxHP;
        dead = false;
    }

    // Increases the players defence by 1
    public void IncreaseDefenceStat()
    {
        defence++;
        currentHP = MaxHP;
        dead = false;
    }

    // Increases the players max HP by 10
    public void IncreaseHPStat()
    {
        MaxHP += 10;
        currentHP = MaxHP;
        dead = false;
    }

    // Increases the players speed by 1
    public void IncreaseSpeedStat()
    {
        speed++;
        currentHP = MaxHP;
        dead = false;
    }

    // Returns the players HP, mainly for use by the UIController
    public int GetHP()
    {
        return currentHP;
    }

    // Returns the players maximum HP, mainly for use by the Enemy Controller
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

    // Returns if the player is charging an attack or not, mainly for use by the Enemy Controller
    public bool GetIsCharging()
    {
        return GameObject.FindGameObjectWithTag("GameController").GetComponent<UIController>().GetIsCharging();
    }

    // Sets the player stats in the stat transfer script
    public void SetPlayerStats()
    {
        GameObject.FindGameObjectWithTag("GameController").GetComponent<PlayerAndAllyStats>().SetPlayerStats(MaxHP, attack, defence, speed);
    }
}
