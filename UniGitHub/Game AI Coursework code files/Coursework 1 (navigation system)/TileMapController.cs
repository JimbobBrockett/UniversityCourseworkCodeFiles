using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TileMapController : MonoBehaviour
{

    private int[,] tileGrid = new int[11,5];
    private int gridHeight = 5;
    private int gridWidth = 11;
    private GameObject player;
    private GameObject[] zombies;

    // Start is called before the first frame update
    void Start()
    {
        player = GameObject.FindGameObjectWithTag("Player");
        zombies = GameObject.FindGameObjectsWithTag("Zombie");

        for (int i = 0; i < gridWidth; i++)
        {
            for (int j = 0; j < gridHeight; j++)
            {
                tileGrid[i, j] = 0;
            }
        }

        UpdateGrid();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void UpdateGrid()
    {
        // Reset grid
        for (int i = 0; i < gridWidth; i++)
        {
            for (int j = 0; j < gridHeight; j++)
            {
                tileGrid[i, j] = 0;
            }
        }

        // Set player position
        tileGrid[((int)player.transform.position.x + 10) / 2, ((int)player.transform.position.y + 4) / 2] = 1;
        
        // Set enemy positions
        for (int i = 0; i < zombies.Length; i++)
        {
            tileGrid[((int)zombies[i].transform.position.x + 10) / 2, ((int)zombies[i].transform.position.y + 4) / 2] = 2;
        }
    }
}
