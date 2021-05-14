//hiiiiiii
//bloop
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"

MapResult mapPutIfContains(Map map, MapKeyElement keyElement, MapDataElement dataElement);
MapResult mapPutIfFirst(Map map, MapKeyElement keyElement, MapDataElement dataElement);
MapResult mapPutIfNotContains(Map map, MapKeyElement keyElement, MapDataElement dataElement);


typedef struct node{
    MapKeyElement key;
    MapDataElement data;
    struct node* next;
} *Node;

struct Map_t{
    struct node* head;
    struct node* i;
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
};

Node createNode(MapKeyElement key,MapDataElement data){
    if(key==NULL||data==NULL){
        return NULL;
    }
    Node ptr=malloc(sizeof(*ptr));
    if(!ptr){
        return NULL;
    }
    ptr->data=data;
    ptr->key=key;
    ptr->next=NULL;
    return ptr;
}
void destroyList(Node ptr) {
	while(ptr) {
		Node toDelete = ptr;
		ptr = ptr->next;
		free(toDelete);
	}
}

Node createList(int size){
    Node new_list=NULL;
    Node tail=NULL;
    for (int i=0;i<size;i++){
        Node temp=malloc(sizeof(*temp));
        if(temp==NULL){
            destroyList(new_list);
            return NULL;
        }
        temp->next=NULL;
        if(new_list==NULL){ 
            new_list=temp;
            tail=new_list;
        }
        else{
            tail->next=temp;
            tail=tail->next;
        }
    }
    return new_list;
}

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements){
                    if(copyDataElement==NULL||copyKeyElement==NULL||
                        freeDataElement==NULL||freeKeyElement==NULL||compareKeyElements==NULL) {
                            return NULL;
                    }
                    Map map=malloc(sizeof(*map));
                    if(map==NULL){
                        return NULL;
                    }
                    map->head=NULL;
                    map->i=NULL;
                    map->copyDataElement=copyDataElement;
                    map->copyKeyElement=copyKeyElement;
                    map->freeDataElement=freeDataElement;
                    map->freeKeyElement=freeKeyElement;
                    map->compareKeyElements=compareKeyElements;
                    return map;
}

int mapGetSize(Map map){
    if(map==NULL){
        return -1;
    }
    int count=0;
    mapGetFirst(map);
    while(map->i!=NULL){
        count++;
        mapGetNext(map);
    }
    return count;
}

MapKeyElement mapGetFirst(Map map){
    if(map==NULL){
        return NULL;
    }
    if(map->head==NULL){
        return NULL;
    }
    if(map->head->key==NULL){
        return NULL;
    }
    map->i=map->head;
    MapKeyElement copy=map->copyKeyElement(map->i->key);
    if (copy==NULL){
        return NULL;
    }
    return copy;
}

MapKeyElement mapGetNext(Map map){
    if(map==NULL){
        return NULL;
    }
    if (map->i==NULL){
        return NULL;
    }
    map->i=map->i->next;
    if(map->i==NULL){
        return NULL;
    }
    MapKeyElement copy=map->copyKeyElement(map->i->key);
    if (copy==NULL){
        return NULL;
    }
    return copy;
}

bool mapContains(Map map, MapKeyElement key)
{
    if(key==NULL  || map==NULL){
        return false;
    }
    MapKeyElement curr_key = mapGetFirst(map);
    if(curr_key==NULL){
        return false;
    }
    int map_len = mapGetSize(map);
    for(int i = 0; i<map_len; i++)
    {
        if(map->compareKeyElements(key, curr_key) == 0){
            return true;
        }
        //the keys weren't identical
        curr_key = mapGetNext(map);
        if(curr_key == NULL){ //mapGetNext's alloction failed
                return false;
        }
    }
    return false;
}
Map mapCopy(Map map){
    if (map==NULL){
        return NULL;
    }
    Map copyMap=mapCreate(map->copyDataElement,
              map->copyKeyElement,
              map->freeDataElement,
              map->freeKeyElement,
              map->compareKeyElements);
    if(copyMap==NULL){
        return NULL;
    }
    int size=mapGetSize(map);
    copyMap->head=createList(size);
    if (copyMap->head==NULL){
        return NULL;
    }
    map->i=map->head;
    copyMap->i=copyMap->head;
    for (int i=0;i<size;i++){
        copyMap->i->data=map->copyDataElement(map->i->data);
        copyMap->i->key=map->copyKeyElement(map->i->key);
        copyMap->i=copyMap->i->next;
        map->i=map->i->next;
    }
    return copyMap;
}
MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    
    struct node* curr_node = map->head;
    if(curr_node == NULL)
        return NULL;

    while((map->compareKeyElements(keyElement, curr_node->key) != 0) && (curr_node!=NULL))
    {
        curr_node = curr_node->next;
    }
    
    if(curr_node==NULL)
        return NULL;
    return curr_node->data; 
}

MapResult mapClear(Map map)
{
    if(map==NULL){
        return MAP_NULL_ARGUMENT;
    }
    if(map->head==NULL){
        return MAP_SUCCESS;
    }
    int map_len = mapGetSize(map);
    for(int i=0;i<map_len;i++){
        MapResult remove =mapRemove(map,mapGetFirst(map));
        if (remove!=MAP_SUCCESS){
            return MAP_ERROR;
        }
    }
    return MAP_SUCCESS;
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    if (map==NULL||keyElement==NULL){
        return MAP_NULL_ARGUMENT;
    }
    map->i=map->head;
    if (map->i==NULL)
    {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    
    Node ptr=map->i;
    if(map->compareKeyElements(map->i->key,keyElement)==0){
        map->head=map->head->next;
    }
    else {
        map->i=map->i->next;
    }
    while(map->compareKeyElements(map->i->key,keyElement)!=0){
        ptr=ptr->next;
        map->i=map->i->next;
    }
    if(map->i==NULL){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    ptr->next=map->i->next;
    map->freeKeyElement(map->i->key);
    map->freeDataElement(map->i->data);
    free(map->i);
    return MAP_SUCCESS;
}


MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if((map==NULL) || (keyElement==NULL) || (dataElement==NULL)){
        return MAP_NULL_ARGUMENT;
    }
    if(mapContains(map, keyElement)){
        //find it and change the data
        return mapPutIfContains(map, keyElement, dataElement); 
    }
    else {
        //find the location and create a new node
        if(map->head == NULL) // first node
        {
            return mapPutIfFirst(map, keyElement, dataElement);
        }
        return mapPutIfNotContains(map, keyElement, dataElement);
    }
}

MapResult mapPutIfContains(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    MapKeyElement curr_key = mapGetFirst(map);
    while(map->compareKeyElements(keyElement, curr_key) != 0) // key_element != curr_key
    {
        curr_key = mapGetNext(map); 
        if(curr_key == NULL){
            return MAP_ERROR;
        }
    }
    //found the node where key_element != curr_key
    if(map->i->data != NULL)
        map->freeDataElement(map->i->data);
    map->i->data = map->copyDataElement(dataElement); 
    return MAP_SUCCESS; 
}

MapResult mapPutIfFirst(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    struct node* new_node = createNode(keyElement, dataElement);
    if(new_node == NULL)
        return MAP_OUT_OF_MEMORY;
    new_node->next = NULL; 
    map->head = new_node;
    return MAP_SUCCESS;
}


MapResult mapPutIfNotContains(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    struct node* last_node = NULL;
    mapGetFirst(map); //iterator at the head node
    while( map->i != NULL) //until the iterator got to the last node in the list 
    {
        if(map->compareKeyElements(map->i->key, keyElement) > 0)// if the current key is larger than keyElement
        {
            struct node* new_node = createNode(keyElement, dataElement);
            if(new_node == NULL)
                return MAP_OUT_OF_MEMORY;
            if(last_node == NULL) //i==head
            {
                new_node->next = map->i;
            }
            else{
                new_node->next = last_node->next; 
                last_node->next = new_node;
            }
            return MAP_SUCCESS;  
        } 
        last_node = map->i;
        mapGetNext(map); // no need to check because even if the allocation failed the iterator was still changed 
    }
    //the new key was larger than all the keys in map
    struct node* new_node = createNode(keyElement, dataElement);
    if(new_node == NULL)
        return MAP_OUT_OF_MEMORY;
    new_node->next = NULL; 
    last_node->next = new_node; //lat_node can't be NULL here due to the loop's condition
    return MAP_SUCCESS;
}


void mapDestroy(Map map){
    if (map==NULL){
        return;
    }
    mapClear(map);
    free(map); 
}
