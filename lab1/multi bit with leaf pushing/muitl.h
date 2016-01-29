#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>

int stride=2;
int tablelength=pow(2,stride);



struct Node{
    Node  *ptr[4];      
    int     verdict[4];     
    int     prelenJudge[4];
};

Node* init_Mtnode(){
    Node *ret = (Node *)malloc(sizeof(Node));
    
    for (int i=0; i<4; i++){
        ret->ptr[i] = NULL;
        ret->verdict[i] = -1;
        ret->prelenJudge[i] = -1 ;
    }   
    
    return ret;
}

void free_Mt(Node *root){
    
    for (int i=0; i<4; i++){
        if(root->ptr[i] != NULL){
            
            free_Mt(root->ptr[i]);
        }
    }
        
    free(root);
}

void update(Node *curr_node, 
            int temp_array_id,
            int portnum,
            int prelen)
{    Node *following_node ;
     following_node=curr_node->ptr[temp_array_id];
     
    
    if (curr_node->prelenJudge[temp_array_id]<prelen)
    {
        curr_node->verdict[temp_array_id]=portnum;
        curr_node->prelenJudge[temp_array_id]=prelen;
    }
     if(curr_node->ptr[temp_array_id]==NULL){
        return;
        }
    else  {

           
           
            for(int i=0;i<4;i++){
            
            update( following_node,i,curr_node->verdict[temp_array_id],curr_node->prelenJudge[temp_array_id]);
            }

        }
    
        
}


    


/* Insert a rule */
void insert_rule(Node *root, uint32_t prefix, int prelen, int portnum){
    static int     n_rules = 0;
   
#ifdef DEBUG
    uint32_t prefix_r = htonl(prefix);
    fprintf(stderr, "Insert rule: %-15s(%04x)/%d    %d\n", 
            inet_ntoa(*(struct in_addr *)&prefix_r), 
            prefix, prelen, portnum);
#endif

    n_rules ++;

    /* default rule: if packet matches none of the rules, 
     * it will match this default rule, i.e. 0.0.0.0/0 */

    Node   *curr_node = root;
    uint32_t    temp_prefix = prefix;
    int         current_len=prelen;
    int         temp_len=-1;
    int         temp_prtnum=-1;
    int       temp_array_id;

       if( prelen == 0 ){
        for (int i=0; i<4; i++){
            root->verdict[i] = portnum;
        }
        temp_prtnum=portnum;
        return;
    }


   while(current_len>0){
        if (current_len<=2){

            switch(current_len){
               case 1:
                    
                    temp_array_id=(temp_prefix >>30)& 0x00000002 ;
                                        //
                    for (int i=0; i<2; i++){
                        update(curr_node,temp_array_id+i,portnum,prelen);
                       
                    }

                    break;
               
                case 2:
                    
                    temp_array_id=temp_prefix >>30 ;
                    update(curr_node,temp_array_id,portnum,prelen);
                    
                    break;
            }
           
            return;

        }

            /* temp_array_id=(temp_prefix >>30)& 0x00000003 ;
                                        //
                    for (int i=0; i<temp_array_id; i++){
                        update(curr_node,temp_array_id+i,portnum,prelen);
                    }
            return;
        }*/
        else{       
            
            temp_array_id=temp_prefix>>30;
            if (curr_node->ptr[temp_array_id]==NULL){
               
                temp_prtnum=curr_node->verdict[temp_array_id];
                temp_len=curr_node->prelenJudge[temp_array_id];
                curr_node->ptr[temp_array_id] = init_Mtnode();
                
                

                curr_node = curr_node->ptr[temp_array_id];

                for (int i=0; i<4; i++){
                    if (curr_node->prelenJudge[i]<temp_len){
                        curr_node->verdict[i]=temp_prtnum;
                        curr_node->prelenJudge[i]=temp_len;
                    }

                }           
            }
            else{
                
                temp_prtnum=curr_node->verdict[temp_array_id];
                temp_len=curr_node->prelenJudge[temp_array_id];
                curr_node = curr_node->ptr[temp_array_id];

                for (int i=0; i<4; i++){
                    if (curr_node->prelenJudge[i]<temp_len){
                        curr_node->verdict[i]=temp_prtnum;
                        curr_node->prelenJudge[i]=temp_len;
                    }

                }
                
            }
            current_len=current_len-2;
            temp_prefix=temp_prefix<<2; 
            }
        }
}

/* Look up an IP address (represented in a uint32_t) */
int lookup_ip(Node *root, uint32_t ip){
     Node      *curr_node = root;
    int         first2_bit = 0;
    int i = 17 ;
    
    while(i != 0){
        first2_bit = (ip & 0xC0000000)>>30;


        if(curr_node->ptr[ first2_bit] == NULL){

            return curr_node->verdict[first2_bit];
         }     

        if (curr_node->ptr[ first2_bit] != NULL)
    {
        curr_node = curr_node->ptr[ first2_bit];
    }

    ip = ip << 2; 
    i--;
                                   
         
    }


}
