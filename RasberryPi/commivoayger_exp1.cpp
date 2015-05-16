#include <iostream>
#include <queue>
#include <stack>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <vector>
#include <math.h>
 
using namespace std;
#define L 2
#define MAX_COST 2000000000
int exists = 1;
int N;
FILE *inp_file,*out_file;

/*
    Возможное улучшение
    Основная тупость\глупость твоего алгоса заключается в том,
    что последний цикл в Generate с Q.push(..) сохраняет всевозможные 
    варианты продолжения пути, включая самые неоптимальные, которые 
    никогда не будут использованы. В действительности же 
    (если ничего не оптимизировать по скорости) этот цикл должен просто 
    найти и сохранить один путь Q1 с минимальным значением band 
    (или несколько путей с равным band), а также один единственный 
    альтернативный вариант Q2, который равен предыдущему Node с тем 
    же level, но с заблокированным переходом на Q1.site (или несколько 
    site с равным band) с соотв-м пересчетом band и т.п. 
*/

class Node  
{
private:
    int site;
    vector<vector<int>> sign;
    vector<vector<int>> sign1;
    vector<vector<int>> c;
public:
    vector<int> path;
    int layer;
    int band;
 
    Node(int k,vector<vector<int>> &A,int n)
    {
        path.resize(N,0);
        sign.resize(2);
        sign1.resize(2);
        sign[0].resize(N);
        sign[1].resize(N);
        sign1[0].resize(N);
        sign1[1].resize(N);
        layer=n;
        band=0;
        site=k;    
        path[site]=layer+1;
        c = A;
        guiyue();
    }
    Node(int k,Node& A)
    {  
        sign = A.sign;
        sign1 = A.sign1;
        c = A.c;
        layer=A.layer+1;
        site=k;
 
        band=A.band+c[A.site][site];
        path = A.path;
        path[site]=layer+1;
 
        for(int i=0;i<N;i++)
        {  
            if(!c[A.site][i])
            {
                sign[0][A.site]--;
                sign[1][i]--;
            }
            c[A.site][i]=MAX_COST;
        }
        sign1[0][A.site]=1;
 
        for(int i=0;i<N;i++)
        {  
            if(!c[i][site])
            {
                sign[0][i]--;
                sign[1][site]--;
            }
            c[i][site]=MAX_COST;
        }
        sign1[1][site]=1;
 
        if(layer!=(N-1))
        {
            if(!c[site][L-1])
            {
                sign[0][site]--;
                sign[1][L-1]--;
            }
            c[site][L-1]=MAX_COST;
        }
        guiyue();
    }
    void guiyue()
    {
 
        for(int i=0;i<N;i++)
            if(!sign1[0][i]&&!sign[0][i])
            {
                int m=lmin(i,c);
                band=band+m;
                for(int j=0;j<N;j++)
                {
                    c[i][j]=c[i][j]-m;
                    if(!c[i][j])
                    {
                        sign[0][i]++;
                        sign[1][j]++;
                    }
                }
            }
 
            for(int i=0;i<N;i++)
                if(!sign1[1][i]&&!sign[1][i])
                {
                    int m=cmin(i,c);
                    band=band+m;
                    for(int j=0;j<N;j++)
                    {
                        c[j][i]=c[j][i]-m;
                        if(!c[j][i])
                        {
                            sign[0][j]++;
                            sign[1][i]++;
                        }
                    }
                }
    }
 
    void copy(vector<int> &A,vector<int> &B)
    {  
        A = B;
    }
 
    void copy(vector<vector<int>> &A,vector<vector<int>> &B)
    {
        A = B;
    }
 
    int lmin(int i,vector<vector<int>> &A)
    {
        int min=A[i][0];
        for(int j=0;j<N;j++)
            if(min>A[i][j])
                min=A[i][j];
        return min;
    }
 
    int cmin(int i,vector<vector<int>> &A)
    {
        int min=A[0][i];
        for(int j=0;j<N;j++)
            if(min>A[j][i])
                min=A[j][i];
        return min;
    }
 
    bool operator < (const Node &B)const
     {
         return band>B.band;
     }
};
 
 
 
deque< Node > storage;
 
struct init{
    bool two_params;
    int kk;
    int nn;
    vector< vector< int > > *arr;
 
    init(int kkk,int nnn,vector< vector< int > > *a):kk(kkk),nn(nnn),arr(a){two_params = false;};
    init(int kkk,int nnn):kk(kkk),nn(nnn){two_params = true;};
 
    ~init(){};
    Node get() const {
        if(two_params)
            return Node(kk,storage[nn]);
        return Node(kk,*arr,nn);
    }
    bool operator< (const init& b)const{
        return get() < b.get();
    }
};
 
void Generate(int k,vector<vector<int>> &A,int n)
{
    priority_queue <init> Q;
 
    Q.push( init( k, n, &A ) );
    while(!Q.empty())
    {
        Node p( Q.top().get() );
        storage.push_back(p);
 
        if(p.layer==(N-1))
        {
            if (p.band<MAX_COST && p.band >= 0)
                fprintf(out_file,"%d\n",p.band);
            else {
                exists = 0;
                return;
            }
            vector<int> b(N,0);
            for(int i=0;i<N;i++)
                b[p.path[i]-1]=i;
 
            for(int i=0;i<N;i++)
                fprintf(out_file,"%d->",b[i]+1);
 
            fprintf(out_file,"%d",L);
            return;
        }
        Q.pop();
        for(int i=0;i<N;i++)
            if(!p.path[i]){
                Q.push(init( i, storage.size() - 1));
            }
    }
    storage.clear();
}
 
int main(int argc, char *argv[])
{  
    if(argv[1] == NULL || argv[2] == NULL)
        return 2;
    char new_file[] = "inp2.txt";
    int i = 0;
    int k = 0;
    int err_code = 1;
    string str;
    string ins_str = "2000000000";
    char buf = 0;
    vector<vector<int>> A;
    vector<int> tmp;
    out_file = fopen(argv[2],"w");
    inp_file = fopen(argv[1],"r");
    if(inp_file == NULL || out_file == NULL){
        //perror("fopen");
        return 2;
    }
    while(fscanf(inp_file,"%c",&buf)!=EOF) {
        str.push_back(buf);
        i++;
    }
    i = str.find('-');
    if(i == -1){
        //cout << "hello";
        return 2;
    }
 
    while(i != -1 ){
    i = str.find('-');
    str.insert(i," ");
    str.insert(i,ins_str);
    i = str.find('-');
    str.erase(i,1);
    i = str.find('-');
    }
 
    fclose(inp_file);
 
    inp_file = fopen(new_file,"w");
    for(i = 0; i < str.size(); i++)
        fprintf(inp_file,"%c",str.at(i));
    fclose(inp_file);
 
    inp_file = fopen(new_file,"r");
    i = 0;
    while(err_code != EOF ){
        if(err_code == 0)
            return 2;
        err_code = fscanf(inp_file,"%d",&k);
        i++;
    }
    i--;
    fclose(inp_file);
 
    N = (int)sqrt((double)i);
 
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++)
            tmp.push_back(0);
        A.push_back(tmp);
        tmp.clear();
    }
 
    inp_file = fopen(new_file,"r");
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            if(fscanf(inp_file,"%d",&A[i][j]) == 0 ) {
 
                return 2;
            }
    fclose(inp_file);
 
        for (int j=0;j<N;j++)
            if(A[j][j] != MAX_COST)
                return 2;
    remove(new_file);
    try {
    Generate(L-1,A,0);
    }catch(std::bad_alloc ba) {
        cout << ba.what() << endl;
    }
    if(!exists){
        return 1;
    }
    return 0;
}