using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class PlayerMove : MonoBehaviour
{

    private Vector3 up = new Vector3(0, 0, -10);
    private GameObject[] zombies;
    private int score = 0;
    private char lastDirection = 'w';

    // Start is called before the first frame update
    void Start()
    {
        zombies = GameObject.FindGameObjectsWithTag("Zombie");
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown("w") && transform.position.y < 4)
        {
            transform.position = new Vector3(transform.position.x, transform.position.y + 2, transform.position.z);
            transform.LookAt(new Vector3(transform.position.x, transform.position.y + 2, transform.position.z), up);
            score++;
            lastDirection = 'w';
            MoveEnemies();
        }
        else if (Input.GetKeyDown("a") && transform.position.x > -10)
        {
            transform.position = new Vector3(transform.position.x - 2, transform.position.y, transform.position.z);
            transform.LookAt(new Vector3(transform.position.x - 2, transform.position.y, transform.position.z), up);
            score++;
            lastDirection = 'a';
            MoveEnemies();
        }
        else if (Input.GetKeyDown("s") && transform.position.y > -4)
        {
            transform.position = new Vector3(transform.position.x, transform.position.y - 2, transform.position.z);
            transform.LookAt(new Vector3(transform.position.x, transform.position.y - 2, transform.position.z), up);
            score++;
            lastDirection = 's';
            MoveEnemies();
        }
        else if (Input.GetKeyDown("d") && transform.position.x < 10)
        {
            transform.position = new Vector3(transform.position.x + 2, transform.position.y, transform.position.z);
            transform.LookAt(new Vector3(transform.position.x + 2, transform.position.y, transform.position.z), up);
            score++;
            lastDirection = 'd';
            MoveEnemies();
        }
        else if (Input.GetKeyDown("space"))
        {
            if (lastDirection == 'w')
            {
                for (int i = 0; i < zombies.Length; i++)
                {
                    if (zombies[i] != null)
                    {
                        if (zombies[i].transform.position.x == transform.position.x && zombies[i].transform.position.y == transform.position.y + 2)
                        {
                            zombies[i].GetComponent<AI_Script>().KillZombie();
                            score += 10;
                            break;
                        }
                    }
                }
            }
            else if (lastDirection == 'a')
            {
                for (int i = 0; i < zombies.Length; i++)
                {
                    if (zombies[i] != null)
                    {
                        if (zombies[i].transform.position.x == transform.position.x - 2 && zombies[i].transform.position.y == transform.position.y)
                        {
                            zombies[i].GetComponent<AI_Script>().KillZombie();
                            score += 10;
                            break;
                        }
                    }
                }
            }
            else if (lastDirection == 's')
            {
                for (int i = 0; i < zombies.Length; i++)
                {
                    if (zombies[i] != null)
                    {
                        if (zombies[i].transform.position.x == transform.position.x && zombies[i].transform.position.y == transform.position.y - 2)
                        {
                            zombies[i].GetComponent<AI_Script>().KillZombie();
                            score += 10;
                            break;
                        }
                    }
                }
            }
            else if (lastDirection == 'd')
            {
                for (int i = 0; i < zombies.Length; i++)
                {
                    if (zombies[i] != null)
                    {
                        if (zombies[i].transform.position.x == transform.position.x + 2 && zombies[i].transform.position.y == transform.position.y)
                        {
                            zombies[i].GetComponent<AI_Script>().KillZombie();
                            score += 10;
                            break;
                        }
                    }
                }
            }
        }
        CheckWin();
    }

    void MoveEnemies()
    {
        for (int i = 0; i < zombies.Length; i++)
        {
            if (zombies[i] != null)
            {
                zombies[i].GetComponent<AI_Script>().NavigateToPlayer();
            }
        }
    }

    public void PlayerDeath()
    {
        Debug.Log("Player is dead");
        SceneManager.LoadScene(SceneManager.GetActiveScene().name, LoadSceneMode.Single);
    }

    public int GetScore()
    {
        return score;
    }

    private void CheckWin()
    {
        GameObject[] currentZombies = GameObject.FindGameObjectsWithTag("Zombie");
        if (currentZombies.Length <= 0)
        {
            SceneManager.LoadScene("YouWin", LoadSceneMode.Single);
        }
    }
}
