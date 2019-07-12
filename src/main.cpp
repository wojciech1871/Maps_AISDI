#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>
#include <ctime>
#include <random>

#include "TreeMap.h"
#include "HashMap.h"

using namespace std;

namespace
{
  chrono::high_resolution_clock::time_point tickTime()
  {
    return chrono::high_resolution_clock::now();
  }

  void generateTab(size_t* table, size_t numOfItems)
  {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<> d{50000, 40000};

    size_t check, key;
    for (size_t i=0; i<numOfItems; i++)
    {
      do
      {
        check=0;
        key = abs(std::round(d(gen)));
        for (size_t j = 0; j < i; ++j)
        {
          if (table[j] == key)
          {
            check=1;
            break;
          }
        }
      } while(check==1);
      table[i]=key;
    }
    return;
  }

  template<typename K, typename V>
  void perfomTest(size_t numOfItems, size_t* table)
  {
      aisdi::TreeMap<K,V> testTree;
      aisdi::HashMap<K,V> testMap;

//********************* APPENDING ITEMS *******************
      size_t i=0;
      size_t numOfElToAppend=100;
      while (i<numOfItems-numOfElToAppend)
      {
        testTree[table[i]] = "QWERTY";
        i++;
      }
      auto startT=tickTime();
      while (i<numOfElToAppend)
      {
        testTree[table[i]] = "QWERTY";
        i++;
      }
      auto endT=tickTime();
      auto timeOfAppendToTree=(endT-startT).count();

      i=0;
      while (i<numOfItems-numOfElToAppend)
      {
        testMap[table[i]] = "QWERTY";
        i++;
      }
      auto startM=tickTime();
      while (i<numOfElToAppend)
      {
        testMap[table[i]] = "QWERTY";
        i++;
      }
      auto endM=tickTime();
      auto timeOfAppendToMap=(endM-startM).count();

// ********************** FINDING ITEM *********************************************************
      size_t numOfElToFind=100;
      auto startFT=tickTime();
      for(i=0; i<numOfElToFind; i++)
      {
        auto findedElementT = testTree.find(table[i]);
        (void)findedElementT;
      }
      auto endFT=tickTime();
      auto timeOfFindingsize_tree=(endFT-startFT).count();

      auto startFM=tickTime();
      for(i=0; i<numOfElToFind; i++)
      {
        auto findedElementM = testMap.find(table[i]);
        (void)findedElementM;
      }
      auto endFM=tickTime();
      auto timeOfFindingInMap=(endFM-startFM).count();

// ********************** REMOVING ITEMS *********************************************************
      size_t numOfElTORem=10;
      auto startRT=tickTime();
      for(i=0; i<numOfElTORem; i++)
      {
        testTree.remove(table[i]);
      }
      auto endRT=tickTime();
      auto timeOfRemoveFromTree=(endRT-startRT).count();
      auto startRM =tickTime();
      for(i=0; i<numOfElTORem;  i++)
      {
        testMap.remove(table[i]);
      }
      auto endRM=tickTime();
      auto timeOfRemoveFromMap=(endRM-startRM).count();

//  ******************************* RESULTS: *******************************************************
      cout <<"Time of appending " << numOfElToAppend << " elements to the tree: \t" <<timeOfAppendToTree << endl;
      cout <<"Time of appending " << numOfElToAppend << " elements to the map: \t" <<timeOfAppendToMap << endl<<endl;
      cout <<"Time of finding " << numOfElToFind <<" elements in the tree: \t" <<timeOfFindingsize_tree << endl;
      cout <<"Time of finding " << numOfElToFind << " elements in the map: \t" <<timeOfFindingInMap << endl<<endl;
      cout <<"Time of removing " << numOfElTORem << " elements from the tree: \t" <<timeOfRemoveFromTree << endl;
      cout <<"Time of removing " << numOfElTORem << " elements from the map: \t" <<timeOfRemoveFromMap << endl<<endl;
  }
}

int main()
{
    size_t keyTab[100000];
    generateTab(keyTab,100000);
    cout <<"Collection size 1000" <<endl;
    perfomTest<size_t,string>(1000,keyTab);
    cout <<"Collection size 10000" <<endl;
    perfomTest<size_t,string>(10000,keyTab);
    cout <<"Collection size 100000" <<endl;
    perfomTest<size_t,string>(100000,keyTab);
    return 0;
}
