#include <iostream>
#include "graph.h"

int main(int args, char **argv)
{
	std::cout<<"Input: ./exe beg csr weight\n";
	if(args!=4){std::cout<<"Wrong input\n"; return -1;}
	
	const char *beg_file=argv[1];
	const char *csr_file=argv[2];
	const char *weight_file=argv[3];
	
	//template <file_vertex_t, file_index_t, file_weight_t
	//new_vertex_t, new_index_t, new_weight_t>
	graph<long, long, /*int*/long, long, long,/* char*/long>
	*ginst = new graph
	<long, long, /*int*/long, long, long, /*char*/long>
	(beg_file,csr_file,weight_file);
    
    //**
    //You can implement your single threaded graph algorithm here.
    //like BFS, SSSP, PageRank and etc.

    ginst->bfs(8);
    ginst->bottom_up_bfs(1);
    ginst->top_down_threaded(1, 5);
    // ginst->bottom_up_threaded(1, 5);

    ginst->bottom_up_threaded(8, 5);

    
    for(int i = 0; i < ginst->vert_count; i++)
    {
        int beg = ginst->beg_pos[i];
        int end = ginst->beg_pos[i+1];
        std::cout<<i<<"'s neighor list: ";
     //   std::cout<<i<<"'s outgoing money: ";
        for(int j = beg; j < end; j++)
        {
            std::cout<<ginst->csr[j]<<" ";
            std::cout<<"Money:"; std::cout<<ginst->weight[j]<<" ";
        }
        std::cout<<"\n";
    } 

	return 0;	
}
