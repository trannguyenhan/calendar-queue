#include<stdio.h>
#include<stdlib.h>
#include "Event.c"

typedef struct node {
    //enum TypesOfEvent type; //type of event

    //int idElementInGroup;//id of element in group of hosts or switches
    //int portID;
    double endTime;
    struct node *left;
    struct node *right;
    struct node *parent;
}node;

node* new_node(double priority){
    node* tmp = malloc(sizeof(node));
    tmp->left = NULL;
    tmp->right = NULL;
    tmp->parent = NULL;
    tmp->endTime = priority;
    return tmp;
}
