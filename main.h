#pragma once
#include <iostream>
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define Duration(a) std::chrono::duration_cast<std::chrono::microseconds>(a).count()
#define TimeNow() std::chrono::high_resolution_clock::now()

enum Mode {
  TEST,
  RANDOM,
  USERINPUT,
  PRINT80
};

// #define ADJECENTLISTPRINT 0
// #define COSTLISTPRINT 0

Mode mode = USERINPUT;
int SIZE_X = 4;
int SIZE_Y = 4;
int WEIGHTRANGE = 998;
bool PRINTMODE = false;
int HINDERPROB = 10;

struct Node {
  unsigned id;
  int x;
  int y;
  int G;
  int H;
  int weight;
  struct Node *parent;
  bool hinder;

  Node(Node *n) {
    id = n->id;
    x = n->x;
    y = n->y;
    G = n->G;
    H = n->H;
    weight = n->weight;
    parent = n->parent;
    hinder = n->hinder;
  }
  Node(int _id, int _x, int _y, bool _hinder) :
  id(_id),
  x(_x),
  y(_y),
  G(99999),
  H(0),
  weight(99999),
  parent(0),
  hinder(_hinder)
  {}
  Node() : id(-1), x(0), y(0), G(99999), H(99999), weight(99999) {}
};
typedef struct Node node;

node *start;
node *goal;

