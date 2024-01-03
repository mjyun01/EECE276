#include<iostream>

using namespace std;
int get_min(int a, int b, int c, int d);

int main(int argc, char** argv)
{
	int test_case;
	int building_num;
	int arr[1000] = {0, } ; 
    int main, l_1, l_2, r_1, r_2 ; 
    int min ; 
	int view ; 
    int tmp ;
	/*
	   여러 개의 테스트 케이스가 주어지므로, 각각을 처리합니다.
	*/
	for( test_case = 0; test_case < 10 ; test_case++)
	{
		cin>>building_num;
		for(int i = 0; i < building_num ; i++ )
		{
            cin >> arr[i]; 
        }
        
        view = 0 ; 
        
        for (int i= 2 ; i< building_num-2 ; i++ ) 
        {
        	main = arr[i]; 
            l_1 = arr[i] - arr[i-1] ; 
            r_1 = arr[i] - arr[i+1] ; 
            l_2 = arr[i] - arr[i-2] ; 
            r_2 = arr[i] - arr[i+2]; 
            min = get_min(l_1, r_1, l_2, r_2) ; 
            tmp = ( min < 0 ) ? 0 : min ; 
        	//cout << tmp ; 
        	view += ( min < 0 ) ? 0 : min ; 
        } 
        cout << '#' << test_case+1 << '\t';
        cout << view << '\n' ; 
	}
	return 0; //정상종료시 반드시 0을 리턴해야합니다.
}

int get_min(int a, int b, int c, int d){
    
	int min, min1, min2 ; 
    
    min1 = ( a < b ) ? a : b ; 
    min2 = ( c < d ) ? c : d ; 
    min = ( min1 < min2 ) ? min1 : min2 ; 
    
    return min ;
}
