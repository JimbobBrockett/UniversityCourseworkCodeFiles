using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TilePointCount : MonoBehaviour
{

    private int pointCount;

    // Start is called before the first frame update
    void Start()
    {
        if (gameObject.CompareTag("Tile1Point"))
        {
            pointCount = 1;
        }
        else if (gameObject.CompareTag("Tile2Point"))
        {
            pointCount = 2;
        }
        else if (gameObject.CompareTag("Tile3Point"))
        {
            pointCount = 3;
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public int GetPoints()
    {
        return pointCount;
    }
}
