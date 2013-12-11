// Sukzessive-Kürzeste-Wege-Algorithmus
#include "wgraph.h"

using namespace std;

//Kürzesten Weg mit Dijkstra O(n^2)
void dijkstra(Weighted_Graph &g, int newpot[], vector<Graph::EdgeId> &path, int blocked[])
{
	int n=g.num_nodes();
	int prev[n];
	int visited[n];
	vector<Graph::NodeId> q;

	for(int i=0;i<n-1;i++)
	{
		newpot[i]=2000000000;
		prev[i]=-1;
		visited[i]=0;
	}

	newpot[n-1]=0;
	prev[n-1]=-1;
	q.push_back(n-1);

	while(!q.empty())
	{
		int min=0;
		for(int i=0;i<q.size();i++)
			if(newpot[q[i]]<newpot[q[min]] && !visited[q[i]])
				min=i;

		Graph::NodeId v=q[min];
		q.erase(q.begin()+min);


		visited[v]=1;

		for(int i=0;i<g.get_node(v).out_edges().size();i++)
		{
			Graph::EdgeId e=g.get_node(v).out_edges()[i];
            Graph::NodeId u=g.get_edge(e).get_head();

            int alt = newpot[v]+g._weight[e];

            if(alt<newpot[u] && !blocked[e])
            {
            	newpot[u]=alt;
            	prev[u]=e;
            	if(!visited[u])
            		q.push_back(u);
            }
		}
	}

   //Zusatz für SKWA (alle von s nicht erreichbaren Knoten auf dist[t] setzen)
	for(int i=0;i<n-2;i++)
	{
		if(newpot[i]>newpot[n-2])
			newpot[i]=newpot[n-2];
	}

   //kürzesten Weg zusammenstellen
	int lol=n-2;
	while(1)
	{
		if(prev[lol]!=-1)
		{		
		path.push_back(prev[lol]);
		lol=g.get_edge(prev[lol]).get_tail();
		}
		else
			break;
	}
}

//Residualgraph aktualisieren
void updategres(Weighted_Graph &g, Weighted_Graph &gres, int flow[], int orig[], int blocked[])
{
	gres=g;
	for(int i=0;i<g.num_edges();i++)
	{
      //Für jede Kante mit Fluss, füge Rückkante hinzu
		if(flow[i]>0)
		{
			gres.add_edge(g.get_edge(i).get_head(),g.get_edge(i).get_tail());
			gres._weight.push_back(-1*g._weight[i]);
			orig[gres.num_edges()-1]=i;
			blocked[gres.num_edges()-1]=0;

         //Fluss maximal => Kante blockiert
			if(flow[i]==1)
				blocked[i]=1;
			else
				blocked[i]=0;
		}
	}
}

int main(int argc, char* argv[])
{
    	if (argc > 1) 
      	{
        	   Weighted_Graph g(argv[1],1);
            //Zusatzknoten s,t (s=g.num_edges()-1, t=g.num_edges()-2)
            g.add_node();
            g.add_node();

            int pot[g.num_nodes()],n=g.num_nodes()-2,m=g.num_edges(), f[m];
            vector<double> uweight=g._weight;   //merke urspr. Gewicht (für Output)

            pot[n+1]=0;
            pot[n]=2000000000;        

            //Verbinde s mit allen Knoten in der einen Knotenhälfte
            for(int i=0;i<=n/2-1;i++)
   			{
   				g.add_edge(n+1,i);
            	g._weight.push_back(0);
   				pot[i]=2000000000;
   			}

            //Verbinde t mit der anderen Knotenhälfte
   			for(int i=n/2;i<n;i++)
   			{
   				g.add_edge(i,n);
   				g._weight.push_back(0);
   				pot[i]=2000000000;
   			}

            //speichere Originalkante für Rückkanten und ob Kante blockiert ist
   			int orig[2*g.num_edges()], blocked[2*g.num_edges()];

            //finden eines zul. Potentials mit Moore-Bellman-Ford
	   		for(int i=0;i<g.num_nodes()-1;i++)
	   		{
   				for(Graph::EdgeId e=0;e<g.num_edges();e++)
   				{
   					Graph::NodeId u=g.get_edge(e).get_tail();
   					Graph::NodeId v=g.get_edge(e).get_head();
   					if(pot[u]+g._weight[e]<pot[v])
   						pot[v]=pot[u]+g._weight[e];
   				}
   			}

            //Kosten auf reduzierte Kosten umrechnen
   			for(int e=0;e<g.num_edges();e++)
   			{
   				Graph::NodeId u=g.get_edge(e).get_tail();
   				Graph::NodeId v=g.get_edge(e).get_head();
   				f[e]=0;
   				blocked[e]=0;

   				g._weight[e]=g._weight[e]+pot[u]-pot[v];
   			}

   			Weighted_Graph gres=g;

            //Hauptschleife
   			while(1)
   			{
   				int newpot[g.num_nodes()];
   				vector<Graph::EdgeId> path;

   				//finde kürzesten s-t-Weg
   				dijkstra(gres,newpot,path,blocked);
   				
   				if(path.empty()) //Kein s-t-Weg im Residualgraph
   					break;

               //reduzierte Kosten mit neuem Potential
   				for(int e=0;e<g.num_edges();e++)
   				{
   				Graph::NodeId u=g.get_edge(e).get_tail();
   				Graph::NodeId v=g.get_edge(e).get_head();
               blocked[e]=0;
   				g._weight[e]=g._weight[e]+newpot[u]-newpot[v];
   				}

               //Finde Wert, um den augmentiert wird
   				int min=2000000000;   				
   				for(int i=0;i<path.size();i++)
   				{
   					if(path[i]<n+m) //Vorwärtskante
   					{
   						if(1-f[path[i]]<min) //maximal um Residualkapazität augmentieren
   						{
   							min=1-f[path[i]];
   						}
   					}
   					else   //Rückwärtskante
   					{
   						if(f[orig[path[i]]]<min)
   							min=f[orig[path[i]]];
   					}			
   				}

               //Augmentiere
   				for(int i=0;i<path.size();i++)
   				{
   					if(path[i]<n+m) //Vorwärtskante
   					{
   						f[path[i]]+=min;
   					}
   					else   //Rückwärtskante
   					{
   						f[orig[path[i]]]-=min;
   					}
   				}

               //neuer Residualgraph
   				updategres(g,gres,f,orig,blocked);
   			}


   	stringstream output;
   	int sum=0;
	   for(int i=0;i<g.num_edges();i++)
		{
			Graph::NodeId u=g.get_edge(i).get_tail();
			Graph::NodeId v=g.get_edge(i).get_head();

         //nur die urspr. Kanten mit Fluss=1 sind interessant
			if(u!=g.num_nodes()-1 && v!=g.num_nodes()-2 && f[i]==1)
			{
				sum+=uweight[i];
				output << u << " " << v << "\n";
			}
		}

	cout << sum << "\n" <<output.str();
	}

	return 0;
}