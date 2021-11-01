using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AI_Script : MonoBehaviour
{

    private GameObject player;
    private GameObject[] tilesOne, tilesTwo, tilesThree, tiles, surroundingTiles;
    private int waitTime;
    private Vector3 up = new Vector3(0, 0, -10);
    public string enemyType;

    // Start is called before the first frame update
    void Start()
    {
        surroundingTiles = new GameObject[4];
        player = GameObject.FindGameObjectWithTag("Player");
        tilesOne = GameObject.FindGameObjectsWithTag("Tile1Point");
        tilesTwo = GameObject.FindGameObjectsWithTag("Tile2Point");
        tilesThree = GameObject.FindGameObjectsWithTag("Tile3Point");
        tiles = new GameObject[tilesOne.Length + tilesTwo.Length + tilesThree.Length];
        tilesOne.CopyTo(tiles, 0);
        tilesTwo.CopyTo(tiles, tilesOne.Length);
        tilesThree.CopyTo(tiles, tilesOne.Length + tilesTwo.Length);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void NavigateToPlayer()
    {
        Vector3 target = player.transform.position;
        waitTime--;

        if (waitTime <= 0)
        {
            if (enemyType == "easier")
            {
                float xDist = Mathf.Abs(transform.position.x - target.x);
                float yDist = Mathf.Abs(transform.position.y - target.y);
                if (xDist > yDist)
                {
                    if (transform.position.x > target.x)
                    {
                        transform.position = new Vector3(transform.position.x - 2, transform.position.y, transform.position.z);
                        transform.LookAt(new Vector3(transform.position.x - 2, transform.position.y, transform.position.z), up);
                    }
                    else
                    {
                        transform.position = new Vector3(transform.position.x + 2, transform.position.y, transform.position.z);
                        transform.LookAt(new Vector3(transform.position.x + 2, transform.position.y, transform.position.z), up);
                    }
                }
                else
                {
                    if (transform.position.y > target.y)
                    {
                        transform.position = new Vector3(transform.position.x, transform.position.y - 2, transform.position.z);
                        transform.LookAt(new Vector3(transform.position.x, transform.position.y - 2, transform.position.z), up);
                    }
                    else
                    {
                        transform.position = new Vector3(transform.position.x, transform.position.y + 2, transform.position.z);
                        transform.LookAt(new Vector3(transform.position.x, transform.position.y + 2, transform.position.z), up);
                    }
                }
            }
            else if (enemyType == "harder")
            {
                IntelligentNavigate();
            }
            waitTime = GetWaitTime();
        }
        if (transform.position == player.transform.position)
        {
            player.GetComponent<PlayerMove>().PlayerDeath();
        }
    }

    private void IntelligentNavigate()
    {
        int tilesIndex = 0;
        for (int i = 0; i < tiles.Length; i++)
        {
            if (tiles[i].transform.position.x - 2 == transform.position.x && tiles[i].transform.position.y == transform.position.y)
            {
                surroundingTiles[tilesIndex] = tiles[i];
                tilesIndex++;
            }
            if (tiles[i].transform.position.x + 2 == transform.position.x && tiles[i].transform.position.y == transform.position.y)
            {
                surroundingTiles[tilesIndex] = tiles[i];
                tilesIndex++;
            }
            if (tiles[i].transform.position.y - 2 == transform.position.y && tiles[i].transform.position.x == transform.position.x)
            {
                surroundingTiles[tilesIndex] = tiles[i];
                tilesIndex++;
            }
            if (tiles[i].transform.position.y + 2 == transform.position.y && tiles[i].transform.position.x == transform.position.x)
            {
                surroundingTiles[tilesIndex] = tiles[i];
                tilesIndex++;
            }
        }
        float[] newTileValues = new float[tilesIndex];
        for (int i = 0; i < newTileValues.Length; i++)
        {
            newTileValues[i] = surroundingTiles[i].GetComponent<TilePointCount>().GetPoints();
            newTileValues[i] += Vector3.Distance(surroundingTiles[i].transform.position, GameObject.FindGameObjectWithTag("Player").transform.position);
        }
        newTileValues = SortValues(newTileValues);
        if (surroundingTiles[0].transform.position.x > transform.position.x && surroundingTiles[0].transform.position.y == transform.position.y)
        {
            transform.LookAt(new Vector3(transform.position.x + 2, transform.position.y, transform.position.z), up);
        }
        else if (surroundingTiles[0].transform.position.x < transform.position.x && surroundingTiles[0].transform.position.y == transform.position.y)
        {
            transform.LookAt(new Vector3(transform.position.x - 2, transform.position.y, transform.position.z), up);
        }
        else if (surroundingTiles[0].transform.position.x == transform.position.x && surroundingTiles[0].transform.position.y > transform.position.y)
        {
            transform.LookAt(new Vector3(transform.position.x, transform.position.y + 2, transform.position.z), up);
        }
        else if (surroundingTiles[0].transform.position.x == transform.position.x && surroundingTiles[0].transform.position.y < transform.position.y)
        {
            transform.LookAt(new Vector3(transform.position.x, transform.position.y - 2, transform.position.z), up);
        }
        transform.position = surroundingTiles[0].transform.position;
    }

    private float[] SortValues(float[] values)
    {
        int length = values.Length;
        float temp = values[0];
        for (int i = 0; i < length; i++)
        {
            for (int j = i+1; j < length; j++)
            {
                if (values[i] > values[j])
                {
                    temp = values[i];
                    values[i] = values[j];
                    values[j] = temp;

                    GameObject tempTile = surroundingTiles[i];
                    surroundingTiles[i] = surroundingTiles[j];
                    surroundingTiles[j] = tempTile;
                }
            }
        }
        return values;
    }

    private int GetWaitTime()
    {
        for (int i = 0; i < tiles.Length; i++)
        {
            if (transform.position.x == tiles[i].transform.position.x && transform.position.y == tiles[i].transform.position.y)
            {
                return tiles[i].GetComponent<TilePointCount>().GetPoints();
            }
        }
        return 10000;
    }

    public void KillZombie()
    {
        Destroy(gameObject);
    }
}
