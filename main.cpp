#include "main.h"
#include "BinaryHeap.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using std::vector;
using std::pair;
using std::string;
using std::cout;
using std::cerr;
using std::endl;

#ifdef _WIN32
  const string ALIVE(1, (char)254u);
  const string DEAD(1, (char)250u);
#else
  const string ALIVE = "\u25A0";
  const string DEAD = "\u00B7";
#endif


bool isHinder() {
  int r = rand() % 100 + 1;
  return (r < HINDERPROB);
}


// Initializes the initial problem, which consists of:
// - The grid that represents the 'problem'.
// - The adjecentNodesList used by the A* algoritm.
vector<node*> initProblem(vector<vector<pair<node*, int>>>& a) {
  int size = SIZE_X*SIZE_Y;
  a.resize(size);
  vector<node*> grid(size);
  for (int i = 0; i < SIZE_Y; ++i) {
    for (int j = 0; j < SIZE_X; ++j) {
      grid[i*SIZE_X + j] = new node(i*SIZE_X+j, i, j, isHinder());
    }
  }

  for (int x = 0; x < a.size(); ++x) {
    if (x+1 < size && (x % SIZE_X !=SIZE_X-1))
      a[x].push_back(pair<node*, int>(grid[x+1], rand() % WEIGHTRANGE + 1));
    if (x+SIZE_X < size)
      a[x].push_back(pair<node*, int>(grid[x+SIZE_X], rand() % WEIGHTRANGE + 1));
    if (x-1 >= 0  && (x % SIZE_X != 0)){
      int i;
      for (i = 0; i < a[x-1].size(); ++i)
        if (a[x-1][i].first->id == x)
          break;
      if (i < a[x-1].size())
        a[x].push_back(pair<node*, int>(grid[x-1], a[x-1][i].second));
    }
    if (x-SIZE_X >= 0) {
      int i;
      for (i = 0; i < a[x-SIZE_X].size(); ++i)
        if (a[x-SIZE_X][i].first->id == x)
          break;
      if (i < a[x-SIZE_X].size())
        a[x].push_back(pair<node*, int>(grid[x-SIZE_X], a[x-SIZE_X][i].second));
    }
  }
  return grid;
}

// Naive approach to the heuristic cost estimate.
inline int heuristicCostEstimate(node *f, node *t) {
  return abs(f->x - t->x) + abs(f->y - t->y);
}

// The A* algoritm
// Parameters:
// - start: The start node that you start from.
// - goal: The goal node that you want to get to using the optimal path.
// - adjecentList: A list of lists. These lists contains pairs of neighbours and the cost to travel to that neigbour.
pair<int, vector<node*>> astar(node *start, node *goal, vector<vector<pair<node*, int>>> adjecentList) {
  // G: Cost from start to a node.
  // H: Estimated distance from a node to goal.
  // weight (F): Value based on G and H. The lower the better.

  // A MinBinaryHeap is used because most wanted functions can be achieved in constant time or O(log(n)),
  // where n is the number of nodes in the MinBinaryHeap.
  BinaryHeap<node*> closedSet;
  BinaryHeap<node*> openSet;

  start->G = 0;
  start->H = heuristicCostEstimate(start, goal);
  start->weight = start->G + start->H;
  start->parent = 0;
  openSet.Insert(start);
  node *currentNode;
  // Continue until the open list is empy...
  while(openSet.PeekAtTop()) {
    currentNode = openSet.PeekAtTop();
    // ...or until the goal node has been reached.
    if (currentNode->id == goal->id)
      break;

    // Remove the current node from the open list and...
    openSet.Remove(currentNode->id);
    // ...insert it in to the closed list.
    closedSet.Insert(currentNode);
    for (std::vector<pair<node*, int>>::iterator it = adjecentList[currentNode->id].begin(); it != adjecentList[currentNode->id].end(); ++it) {
      node *neighbour = (*it).first;
      if (closedSet.Search(neighbour->id))
        continue;

      if (neighbour->hinder) {
        closedSet.Insert(neighbour);
        continue;
      }

      // Calculate the parameters for the neighbour.
      int H = heuristicCostEstimate(neighbour, goal);
      int cost = (*it).second;
      int G = currentNode->G + cost;
      int weight = H + G;

      // Set the neighbours parameters.
      neighbour->H = H;
      neighbour->G = G;
      neighbour->weight = weight;
      neighbour->parent = currentNode;

      if (openSet.Search(neighbour->id)) {
        node *existingNode = openSet.GetNode(neighbour->id);
        // If the node exists already but is beaten by the current one,
        // update the existing one with the current parameters.
        if (neighbour->G < existingNode->G) {
          existingNode->G = neighbour->G;
          int newWeight = existingNode->G + heuristicCostEstimate(existingNode, goal);
          existingNode->parent = currentNode;
          openSet.DecreaseKey(existingNode->id, newWeight);
        }
        // Do nothing if it exists but is worse.
        continue;
      }
      openSet.Insert(neighbour);
    }
  }
  // If path was found.
  int G = -1;
  if (currentNode->id == goal->id) {
    G = currentNode->G;
  }

  // Reconstruct the path using the node parameter parent.
  std::vector<node*> path;
  while(currentNode->parent != 0) {
    path.push_back(currentNode);
    currentNode = currentNode->parent;
  }
  path.push_back(start);
  // Path is from goal to start, so reverse it to get the proper path.
  std::reverse(path.begin(), path.end());
  return pair<int, vector<node*>>(G, path);
}

vector<node*> initMain(vector<vector<pair<node*, int>>>& a) {
  int startNodeID, goalNodeID;
  switch(mode) {
    case TEST:
      cout << "Test mode selected." << "\n";
      SIZE_X = 4;
      SIZE_Y = 4;
    break;
    case RANDOM:
      cout << "Random mode selected." << "\n";
      SIZE_X = rand() % 498 + 2;
      SIZE_Y = rand() % 498 + 2;
    break;
    case USERINPUT: {
      string userChoice = "";
      cout << "User input mode selected." << "\n";
      cout << "Enter grid X size: ";

      std::getline(std::cin, userChoice);
      SIZE_X = std::stoi(userChoice);
      cout << "Enter grid Y size: ";
      std::getline(std::cin, userChoice);
      SIZE_Y = std::stoi(userChoice);
      cout << "Enter the probability that a node is a hinder (1-100): ";
      std::getline(std::cin, userChoice);
      HINDERPROB = std::stoi(userChoice);
      cout << "Choose a start node by id(id = x*gridSizeX + y)\nMax ID is: " << SIZE_X*SIZE_Y-1 << endl;
      std::getline(std::cin, userChoice);
      startNodeID = std::stoi(userChoice);
      cout << "Choose a goal node(id = x*gridSizeX + y)\nMax ID is: " << SIZE_X*SIZE_Y-1 << endl;
      std::getline(std::cin, userChoice);
      goalNodeID = std::stoi(userChoice);
      if (SIZE_X < 100 && SIZE_Y < 100) {
        cout << "Print the final path through the graph?" << endl;
        cout << "(1) Yes." << endl;
        cout << "(2) No." << endl;
        std::getline(std::cin, userChoice);
        if (std::stoi(userChoice) == 1) {
          PRINTMODE = true;
        }
      }
      else {
        cout << "Printing not available for chosen dimensions." << endl << endl;
      }
    }
    break;
    case PRINT80:
      SIZE_X = 80;
      SIZE_Y = 80;
      PRINTMODE = true;
    break;
  }
  cout << "Starting initialization...";
  cout.flush();
  vector<node*> grid = initProblem(a);
  if(mode == USERINPUT) {
    start = grid[startNodeID];
    goal = grid[goalNodeID];
  }
  else{
    start = grid[0];
    goal = grid[grid.size()-1];
  }

  if(mode == TEST){
    for (unsigned i = 0; i < a.size(); ++i) {
      for (unsigned j = 0; j < a[i].size(); ++j) {
        a[i][j].second = 99999;
      }
    }

    a[0][1].second = 5;
    a[4][0].second = 5;
    a[5][3].second = 5;
    a[1][0].second = 5;
    a[2][1].second = 5;
    a[6][1].second = 5;
    a[10][0].second = 5;
    a[11][0].second = 5;
  }
  cout << " done.\n==========================" << endl;
  start->hinder = false;
  goal->hinder = false;

  return grid;
}

bool isInPath(int id, vector<node*> path) {
  for (int i = 0; i < path.size(); ++i) {
    if (path[i]->id == id)
      return true;
  }
  return false;
}

int main(int argc, char const *argv[]) {
  srand(time(NULL));
  vector<vector<pair<node*, int>>> aList;
  string userChoice;
  cout << "Choose start mode." << "\n";
  cout << "(1) Test mode. Has a predetermined best path." << "\n";
  cout << "(2) Random mode. Random size of the grid and random weights on every edge." << "\n";
  cout << "(3) User input mode. Set grid dimensions, weight and start/goal node manually." << "\n";
  cout << "(4) Print 80 mode. Grid 80x80 with printed path at the end of the execution." << "\n";
  std::getline(std::cin, userChoice);
  mode = static_cast<Mode>(std::stoi(userChoice)-1);

  vector<node*> grid =initMain(aList);

  cout << "Grid dimensions are: " << SIZE_X << ", " << SIZE_Y << endl;
  cout << "Start node: " << start->id << endl;
  cout << "Goal node: " << goal->id << endl;

  cout << "==========================\nRunning A*...";
  cout.flush();
  TimeVar t1 = TimeNow();
  pair<int, vector<node*>> pairGpath = astar(start, goal, aList);
  auto d = Duration(TimeNow()-t1);
  cout << "done.\nIt took " << d << " µs." << endl;
  vector<node*> path = pairGpath.second;

  if (pairGpath.first == -1) {
    cout << "No path found." << endl;
  }
  else {
    cout << "Total cost: " << pairGpath.first << endl;
    cout << "Path length: " << path.size() << "\n";
  }

  if (PRINTMODE) {
    for (int i = 0; i < SIZE_Y; ++i) {
      for (int j = 0; j < SIZE_X; ++j) {
        int index = i*SIZE_X+j;
        int gridId = grid[index]->id;
        if (isInPath(gridId, path)) {
          if (gridId == start->id) {
            cout << "S";
          }
          else if (gridId == goal->id) {
            cout << "G";
          }
          else
            cout << ALIVE;
        }
        else if (grid[index]->hinder) {
          cout << "X";
        }
        else {
          cout << DEAD;
        }
      }
      cout << "\n";
    }
  }

  #ifdef COSTLISTPRINT
  cout << "[";
  for (int i = 0; i < path.size(); ++i) {
    int aindex = path[i]->id;
    if (i+1 < path.size()) {
      for (int j = 0; j < aList[aindex].size(); ++j) {
        if (aList[aindex][j].first->id == path[i+1]->id) {
          cout << aList[aindex][j].second << ((i!=path.size()-2)?", ":"");
          break;
        }
      }
    }
  }
  cout << "]\n";
  #endif

  #ifdef ADJECENTLISTPRINT
  // Print Adjecent-cost List //
  // ------------------------ //
  for (int i = 0; i < aList.size(); ++i) {
    cout << "ID: " << i << "\nNeighours: [";
    for (int j = 0; j < aList[i].size(); ++j) {
      cout << "ID: " << aList[i][j].first->id << ", cost: " << aList[i][j].second << (j!=aList[i].size()-1 ? ", " : "]");
    }
    cout << endl;
  }
  cout << endl;
  #endif

  // Delete all the allocated nodes.
  for (int i = 0; i < grid.size(); ++i) {
    delete grid[i];
  }

  return 0;
}

