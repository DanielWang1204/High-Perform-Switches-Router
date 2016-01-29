#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unordered_map>

std::unordered_map<uint32_t,int> hashtable[33];

struct BtNode{
    BtNode  *left;      /* for 0 */
    BtNode  *right;     /* for 1 */
    int     verdict;
};

void insert_hash( uint32_t prefix, int prelen, int portnum)
{
    static std::unordered_map<uint32_t,int> HH[33];
    HH[prelen].insert(std::make_pair<uint32_t,int>(prefix,portnum));
    
}
BtNode* init_btnode(){
    BtNode *ret = (BtNode *)malloc(sizeof(BtNode));
    ret->left = NULL;
    ret->right = NULL;
    ret->verdict = -1;
    return ret;
}

/* Clean up binary trie */
void free_bt(BtNode *root){

    if(root->left != NULL){
        free_bt(root->left);
    }
    if(root->right != NULL){
        free_bt(root->right);
    }

    free(root);
}

void insert_Marker(int prelen,uint32_t prefix){
    int n = 0;
    int start = 0;
    int end = 32;
    int temp_mid = 16;
    int temp_prelen = prelen;
    uint32_t temp_prefix=prefix;
    int marker_portnum=-1;
    uint32_t marker[5];
    uint32_t mask[33];
    static std::unordered_map<uint32_t,int> HH[33];
    mask[0]=0x00000000;
    for(int k=1; k<=32;k++)
    {
        mask[1+k]=mask[k] || 0xffffffff<< (31-k);
    }
    for (int i = 0; i< 5;i++){

        if (prelen > temp_mid){
            marker[n] = temp_prefix & mask[temp_mid];
            temp_prelen = temp_mid;

            if (HH[temp_mid].find(marker[n])==HH[temp_mid].end()){
                insert_hash(marker[n], temp_prelen, marker_portnum);
            }
            n++;
            start = temp_mid;
            temp_mid = (start + end)/2;
        }
        else{
            end = temp_mid;
            temp_mid = (start + end)/2;
            n--;
        }
    }
}


    
int lookup_ip(int start, int end, uint32_t ip)
{

    uint32_t ip_m;
    int      curr_verdict = -1; 
    uint32_t mask;
    int shift;

    int tmp;
    tmp = (end + start)/2;

    int i = end;
    while(true)
    {
        if (i<-2) 
        {printf("error during look up");
            return curr_verdict;}    
        mask=0xffffffff;
        if (tmp<end)
        {
            tmp = end+start/2;
            end = tmp;
        }
        else
        {
            tmp = tmp+start/2-1;
            end = start;
        }
        shift=32-i;
        mask = mask << shift;
        if (i==0)  
            mask =0x0;
        ip_m = ip & mask;   
        std::unordered_map<uint32_t, int>::const_iterator got = hashtable[i].find (ip_m);
        if ( got ==  hashtable[i].end() )
        {i=tmp;}    
        else
            {
            curr_verdict = got->second;
            return curr_verdict;
            }   
    }
    return curr_verdict;
}

