using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAndAllyStats : MonoBehaviour
{

    private int playerHP, allyHP, playerAttack, allyAttack, playerDefence, allyDefence, playerSpeed, allySpeed;
    private bool itemIsInBag, enemyHasItem, enemyHPAboveHalf, playerHPBelowThirtyPercent, HPBelowThirtyPercent, playerIsCharging;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    // Set the player stats for stat transfer
    public void SetPlayerStats(int HP, int attack, int defence, int speed)
    {
        playerHP = HP;
        playerAttack = attack;
        playerDefence = defence;
        playerSpeed = speed;
    }

    // Set the ally stats for stat transfer
    public void SetAllyStats(int HP, int attack, int defence, int speed, bool itemBag, bool itemEnemy, bool enemyHP, bool playerHP, bool selfHP, bool playerCharge)
    {
        allyHP = HP;
        allyAttack = attack;
        allyDefence = defence;
        allySpeed = speed;
        itemIsInBag = itemBag;
        enemyHasItem = itemEnemy;
        enemyHPAboveHalf = enemyHP;
        playerHPBelowThirtyPercent = playerHP;
        HPBelowThirtyPercent = selfHP;
        playerIsCharging = playerCharge;
    }

    // Transfer player HP stat
    public int GetPlayerHP()
    {
        return playerHP;
    }

    // Transfer player attack stat
    public int GetPlayerAttack()
    {
        return playerAttack;
    }

    // Transfer player defence stat
    public int GetPlayerDefence()
    {
        return playerDefence;
    }

    // Transfer the player speed stat
    public int GetPlayerSpeed()
    {
        return playerSpeed;
    }

    // Transfer ally HP stat
    public int GetAllyHP()
    {
        return allyHP;
    }

    // Transfer ally attack stat
    public int GetAllyAttack()
    {
        return allyAttack;
    }

    // Transfer ally defence stat
    public int GetAllyDefence()
    {
        return allyDefence;
    }

    // Transfer the ally speed stat
    public int GetAllySpeed()
    {
        return allySpeed;
    }

    // Transfer ally bool for if an item is in the bag
    public bool GetAllyItemInBag()
    {
        return itemIsInBag;
    }

    // Transfer ally bool for it the enemy has an item
    public bool GetAllyEnemyHasItem()
    {
        return enemyHasItem;
    }

    // Transfer ally bool for the enemies current HP state
    public bool GetAllyEnemyHP()
    {
        return enemyHPAboveHalf;
    }

    // Transfer ally bool for the players current HP state
    public bool GetAllyPlayerHPState()
    {
        return playerHPBelowThirtyPercent;
    }

    // Transfer ally bool for allys current HP state
    public bool GetAllyHPState()
    {
        return HPBelowThirtyPercent;
    }

    // Transfer ally bool for if the player is charging an attack
    public bool GetAllyPlayerCharging()
    {
        return playerIsCharging;
    }
}
