#include "graph.h"
#include <unistd.h>

template<
typename file_vert_t, typename file_index_t, typename file_weight_t,
typename new_vert_t, typename new_index_t, typename new_weight_t>
graph<file_vert_t,file_index_t, file_weight_t,
new_vert_t,new_index_t,new_weight_t>
::graph(
		const char *beg_file,
		const char *csr_file,
		const char *weight_file)
{
	double tm=wtime();
	FILE *file=NULL;
	file_index_t ret;
	
	vert_count=fsize(beg_file)/sizeof(file_index_t) - 1;
	edge_count=fsize(csr_file)/sizeof(file_vert_t);
	
	file=fopen(beg_file, "rb");
	if(file!=NULL)
	{
		file_index_t *tmp_beg_pos=NULL;

		if(posix_memalign((void **)&tmp_beg_pos, getpagesize(),
					sizeof(file_index_t)*(vert_count+1)))
			perror("posix_memalign");

		ret=fread(tmp_beg_pos, sizeof(file_index_t), 
				vert_count+1, file);
		assert(ret==vert_count+1);
		fclose(file);
		edge_count=tmp_beg_pos[vert_count];
		std::cout<<"Expected edge count: "<<tmp_beg_pos[vert_count]<<"\n";

        assert(tmp_beg_pos[vert_count]>0);

		//converting to new type when different 
		if(sizeof(file_index_t)!=sizeof(new_index_t))
		{
			if(posix_memalign((void **)&beg_pos, getpagesize(),
					sizeof(new_index_t)*(vert_count+1)))
			perror("posix_memalign");
			for(new_index_t i=0;i<vert_count+1;++i)
				beg_pos[i]=(new_index_t)tmp_beg_pos[i];
			delete[] tmp_beg_pos;
		}else{beg_pos=(new_index_t*)tmp_beg_pos;}
	}else std::cout<<"beg file cannot open\n";

		file=fopen(csr_file, "rb");
	if(file!=NULL)
	{
		file_vert_t *tmp_csr = NULL;
		if(posix_memalign((void **)&tmp_csr,getpagesize(),
						sizeof(file_vert_t)*edge_count))
			perror("posix_memalign");
		
		ret=fread(tmp_csr, sizeof(file_vert_t), edge_count, file);
        std::cout<<ret<<" "<<edge_count<<"\n";
		assert(ret==edge_count);
		assert(ret==beg_pos[vert_count]);
		fclose(file);
			
		if(sizeof(file_vert_t)!=sizeof(new_vert_t))
		{
			if(posix_memalign((void **)&csr,getpagesize(),
						sizeof(new_vert_t)*edge_count))
				perror("posix_memalign");
			for(new_index_t i=0;i<edge_count;++i)
				csr[i]=(new_vert_t)tmp_csr[i];
			delete[] tmp_csr;
		}else csr=(new_vert_t*)tmp_csr;

	}else std::cout<<"CSR file cannot open\n";

//file=fopen(weight_file, "rb");
//	if(file!=NULL)
//	{
//		file_vert_t *tmp_weight = NULL;
//		if(posix_memalign((void **)&tmp_weight,getpagesize(),
//						sizeof(file_vert_t)*edge_count))
//			perror("posix_memalign");
//		
//		ret=fread(tmp_weight, sizeof(file_vert_t), edge_count, file);
  //      std::cout<<ret<<" "<<edge_count<<"\n";
//		assert(ret==edge_count);
//		assert(ret==beg_pos[vert_count]);
//		fclose(file);
//			
//		if(sizeof(file_vert_t)!=sizeof(new_vert_t))
//		{
//			if(posix_memalign((void **)&weight,getpagesize(),
//						sizeof(new_vert_t)*edge_count))
//				perror("posix_memalign");
//			for(new_index_t i=0;i<edge_count;++i)
//				weight[i]=(new_vert_t)tmp_weight[i];
//			delete[] tmp_weight;
//		}else weight=(new_vert_t*)tmp_weight;
//
//	}else std::cout<<"Weight file cannot open\n";


	file=fopen(weight_file, "rb");
	if(file!=NULL)
	{
		file_weight_t *tmp_weight = NULL;
		if(posix_memalign((void **)&tmp_weight,getpagesize(),
					sizeof(file_weight_t)*edge_count))
			perror("posix_memalign");
		
		ret=fread(tmp_weight, sizeof(file_weight_t), edge_count, file);
		assert(ret==edge_count);
		fclose(file);
	
		if(sizeof(file_weight_t)!=sizeof(new_weight_t))
		{
			if(posix_memalign((void **)&weight,getpagesize(),
						sizeof(new_weight_t)*edge_count))
				perror("posix_memalign");
			for(new_index_t i=0;i<edge_count;++i)
				weight[i]=(new_weight_t)tmp_weight[i];
			delete[] tmp_weight;
		}else weight=(new_weight_t*)tmp_weight;
	}
	else std::cout<<"Weight file cannot open\n";

	std::cout<<"Graph load (success): "<<vert_count<<" verts, "
		<<edge_count<<" edges "<<wtime()-tm<<" second(s)\n";
}

template<
typename file_vert_t, typename file_index_t, typename file_weight_t,
typename new_vert_t, typename new_index_t, typename new_weight_t>
void graph<file_vert_t,file_index_t, file_weight_t, new_vert_t,new_index_t,new_weight_t>::bfs(new_index_t start_vertex)
{
	if (start_vertex >= vert_count)
	{
		std::cerr << "invalid vertex count" << std::endl;
		return;
	}

	// the standard for a bfs, keep track of visited vertices
	// and use a queue to keep track of the next vertices to visit
	std::vector<bool> visited(vert_count, false);
	std::queue<new_index_t> q;

	visited[start_vertex] = true;	
	q.push(start_vertex);	

	std::cout << "BFS starting from vertex " << start_vertex << ":\n";

	while (!q.empty())
	{
		new_index_t current = q.front();
		q.pop();
		
		std::cout << current << " ";
		
		auto beg = beg_pos[current];
		auto end = beg_pos[current + 1];

		for (auto i = beg; i < end; i++)
		{
			auto neighbor = csr[i];
			if (!visited[neighbor])
			{
				visited[neighbor] = true;
				q.push(neighbor);
			}
		}
	}
	std::cout << std::endl;
}


// the bottom up algorithm requires reversed graph
// but the graph we generate with toy seems to be undirected
// so we do not need to worry about it unless we are playing with reversed graph
// in that case, first inverse every edge, then call this function
template<
typename file_vert_t, typename file_index_t, typename file_weight_t,
typename new_vert_t, typename new_index_t, typename new_weight_t>
void graph<file_vert_t,file_index_t, file_weight_t, new_vert_t,new_index_t,new_weight_t>::bottom_up_bfs(new_index_t start_vertex)
{
	if (start_vertex >= vert_count)
	{
		std::cerr << "invalid vertex count" << std::endl;
		return;
	}

	// the standard for a bfs, keep track of visited vertices
	// and use a queue to keep track of the next vertices to visit
	std::vector<bool> visited(vert_count, false);

	std::set<new_index_t> current_layer;
	current_layer.insert(start_vertex);	

	// std::vector<new_index_t> result;

	visited[start_vertex] = true;	

	std::cout << "BFS bottom up starting from vertex " << start_vertex << ":\n" << start_vertex << " ";

	while (!current_layer.empty())
	{
		std::set<new_index_t> next_layer;
		next_layer.clear();

		for (int i = 0; i < vert_count; i++)
		{
			if (!visited[i]) {
				// std::cout << "checking " << i << std::endl;
				auto beg = beg_pos[i];
				auto end = beg_pos[i + 1];

				for (auto j = beg; j < end; j++)
				{
					auto neighbor = csr[j];
					if (current_layer.find(neighbor) != current_layer.end())
					{
						// std::cout << i << " is a neighbor of " << neighbor << std::endl;
						next_layer.insert(i);
						visited[i] = true;
						std::cout << i << " ";

						break;
						// result.append(i);
					}
				}
					
			}
		}

		current_layer = next_layer;
	}
	std::cout << std::endl;
}


template<
typename file_vert_t, typename file_index_t, typename file_weight_t,
typename new_vert_t, typename new_index_t, typename new_weight_t>
void graph<file_vert_t, file_index_t, file_weight_t, new_vert_t, new_index_t, new_weight_t>::
process_top_down_layer(std::queue<new_index_t>& q, std::vector<bool>& visited, std::mutex& q_mutex)
{
	while(true) {
		new_index_t current;
		{
			std::lock_guard<std::mutex> lock(q_mutex);
			if(q.empty()) {
				break;
			}
			current = q.front();
			q.pop();
		}

		auto beg = beg_pos[current];
		auto end = beg_pos[current + 1];

		for (auto i = beg; i < end; i++) {
			auto neighbor = csr[i];
			{
				std::lock_guard<std::mutex> lock(q_mutex);
				if (!visited[neighbor]) {
					visited[neighbor] = true;
					q.push(neighbor);
					std::cout << neighbor << " ";
				}
			}
		}
	}
}


template<
typename file_vert_t, typename file_index_t, typename file_weight_t,
typename new_vert_t, typename new_index_t, typename new_weight_t>
void graph<file_vert_t, file_index_t, file_weight_t, new_vert_t, new_index_t, new_weight_t>::
top_down_threaded(new_index_t start_vertex, int num_threads)
{
	if (start_vertex >= vert_count)
	{
		std::cerr << "invalid vertex count" << std::endl;
		return;
	}

	// the standard for a bfs, keep track of visited vertices
	// and use a queue to keep track of the next vertices to visit
	std::vector<bool> visited(vert_count, false);
	std::queue<new_index_t> q;

	visited[start_vertex] = true;	
	q.push(start_vertex);	

	std::cout << "BFS top down threaded starting from vertex " << start_vertex << ":\n" << start_vertex << " ";

	std::vector<std::thread> threads;
	std::mutex q_mutex;

	for (int i = 0; i < num_threads; i++) {
		threads.push_back(std::thread(&graph::process_top_down_layer, this, std::ref(q), std::ref(visited), std::ref(q_mutex)));
	}

	for (int i = 0; i < num_threads; i++) {
		threads[i].join();
	}
	std::cout << std::endl;
}


// threaded bottom up
template<
typename file_vert_t, typename file_index_t, typename file_weight_t,
typename new_vert_t, typename new_index_t, typename new_weight_t>
void graph<file_vert_t, file_index_t, file_weight_t, new_vert_t, new_index_t, new_weight_t>::
process_bottom_up_layer(std::set<new_index_t>& current_layer, std::vector<bool>& visited, std::mutex& q_mutex, std::set<new_index_t>& next_layer, int thread_id, int num_threads)
{
	for (int i = thread_id; i < vert_count; i += num_threads) {
		if (!visited[i]) {
			auto beg = beg_pos[i];
			auto end = beg_pos[i + 1];

			for (auto j = beg; j < end; j++) {
				auto neighbor = csr[j];
				if (current_layer.find(neighbor) != current_layer.end()) {
					{
						std::lock_guard<std::mutex> lock(q_mutex);
						next_layer.insert(i);
						visited[i] = true;
						std::cout << i << " ";
					}
					break;
				}
			}
		}
	}
}


template<
typename file_vert_t, typename file_index_t, typename file_weight_t,
typename new_vert_t, typename new_index_t, typename new_weight_t>
void graph<file_vert_t, file_index_t, file_weight_t, new_vert_t, new_index_t, new_weight_t>::
bottom_up_threaded(new_index_t start_vertex, int num_threads)
{
	if (start_vertex >= vert_count)
	{
		std::cerr << "invalid vertex count" << std::endl;
		return;
	}

	std::vector<bool> visited(vert_count, false);

	std::set<new_index_t> current_layer;
	current_layer.insert(start_vertex);	

	visited[start_vertex] = true;	

	std::cout << "BFS bottom up threaded starting from vertex " << start_vertex << ":\n" << start_vertex << " ";



	while (!current_layer.empty())
	{
		std::set<new_index_t> next_layer;
		next_layer.clear();

		std::vector<std::thread> threads;
		std::mutex q_mutex;

		// std::cout << "start pushing threads..." << std::endl;
		for (int i = 0; i < num_threads; i++) {
			int thread_id = i;	
			threads.push_back(std::thread(&graph::process_bottom_up_layer, this, std::ref(current_layer), std::ref(visited), std::ref(q_mutex), std::ref(next_layer), thread_id, num_threads));
		}

		for (int i = 0; i < num_threads; i++) {
			threads[i].join();
		}

		current_layer = next_layer;
	}
	std::cout << std::endl;
}